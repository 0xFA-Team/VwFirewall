// ProcPacket.cpp: implementation of the ProcPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcPacket.h"





/**
 *	������������
 **/
BOOLEAN procpacket_ip_is_allowed_packet( UCHAR * puszPacket, UINT uSize )
{
	//
	//	puszPacket	- [in] IP ���ݰ�
	//	uSize		- [in] IP ���ݰ���С
	//	RETURN		- TRUE �Ź���FALSE ����
	//

	BOOLEAN bRet		= TRUE;
	UCHAR * lpuszTcpContent;
	UINT uTcpContentLen;
	UINT uIPHeaderLen;
	UINT uTcpHeaderLen;
	STIPHEADER * pstIPPacket;
	STTCPHEADER * pstTCPPacket;
	UCHAR * pszTemp;
	TS_UD_CS_CORE * pstTsUdCsCore;
	CHAR szClientName[ 32 ];
	CHAR szRemoteIpAddr[ 16 ];

	if ( NULL == puszPacket || 0 == uSize )
	{
		return TRUE;
	}

	//	�����ܷŹ���ֻ�м�����ȷ���Ƿ�����
	bRet = TRUE;


	__try
	{
		pstIPPacket = (STIPHEADER*)puszPacket;
		if ( pstIPPacket && IPPROTO_TCP == pstIPPacket->ucProtocol )
		{
			//
			//	parse ip packet
			//
			uIPHeaderLen	= pstIPPacket->ucHeaderLength * 4;
			pstTCPPacket	= (STTCPHEADER*)( puszPacket + uIPHeaderLen );
			if ( pstTCPPacket )
			{
				//
				//	parse tcp packet
				//
				uTcpHeaderLen	= ( pstTCPPacket->offset ? pstTCPPacket->offset : pstTCPPacket->unused ) * 4;
				lpuszTcpContent	= (UCHAR*)( puszPacket + uIPHeaderLen + uTcpHeaderLen );
				uTcpContentLen	= uSize - ( uIPHeaderLen + uTcpHeaderLen );
				if ( lpuszTcpContent )
				{
					//
					//	����������
					//
					if ( bRet &&
						g_stCfg.bStartProtectDomain )
					{
						if ( 0 == _strnicmp( lpuszTcpContent, g_szDecryptedString_GET, 5 ) ||
							0 == _strnicmp( lpuszTcpContent, g_szDecryptedString_POST, 6 ) )
						{
							if ( ! procpacket_http_is_allowed_domain( lpuszTcpContent ) )
							{
								//	���ص��˰�
								bRet = FALSE;

								//	ͳ�����ش˴���
								procconfig_add_block_count( & g_stCfg.llProtectDomainBlockCount );
							}

							//procpacket_write_templog( (BYTE*)lpuszTcpContent, uTcpContentLen, L"DM", bRet );

							procpacket_inet_ntoa( pstIPPacket->ulSourceAddress, szRemoteIpAddr, sizeof(szRemoteIpAddr) );
							KdPrint(( "procpacket_ip_is_allowed_packet:[%s-%d]{%s} %s\n", "DM", ( bRet ? 1 : 0 ), szRemoteIpAddr, lpuszTcpContent ));
						}
					}


					//
					//	RDP Զ�����氲ȫ
					//
					if ( bRet &&
						g_stCfg.bStartProtectRdp )
					{
						//	����Ƿ��Ƿ����� RDP ����˿ڵİ�
						if ( procrdp_isexist_rdp_port( procpacket_get_valid_tcpport( pstTCPPacket->destinationPort ) ) )
						{
							procpacket_inet_ntoa( pstIPPacket->ulSourceAddress, szRemoteIpAddr, sizeof(szRemoteIpAddr) );
							KdPrint( ( "procpacket_ip_is_allowed_packet:is rdp port [%s]{%s}\n", "RDP", szRemoteIpAddr ) );

							if ( procrdp_parse_packet_mcscipdu( (BYTE*)lpuszTcpContent, uTcpContentLen, &pstTsUdCsCore ) )
							{
								if ( ! procconfig_is_exist_rdp_clientname( pstTsUdCsCore->clientName, wcslen(pstTsUdCsCore->clientName)*sizeof(WCHAR) ) )
								{
									//	���ص��˰�
									bRet = FALSE;

									//	ͳ�����ش˴���
									procconfig_add_block_count( & g_stCfg.llProtectRdpBlockCount );
								}

								//procpacket_write_templog( (BYTE*)lpuszTcpContent, uTcpContentLen, L"RDP", bRet );

								drvfunc_w2c( pstTsUdCsCore->clientName, szClientName, sizeof(szClientName) );
								KdPrint(( "procpacket_ip_is_allowed_packet:[%s-%d]{%s} %s\n", "RDP", ( bRet ? 1 : 0 ), szRemoteIpAddr, szClientName ));
							}
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_ip_is_allowed_packet\n" ));
	}	

	return bRet;
}

BOOLEAN procpacket_write_templog( BYTE * pbtPacket, UINT uPacketLength, WCHAR * lpwszType, BOOLEAN bBlocked )
{
	BOOLEAN bRet;
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;
	WCHAR wszLogFile[ MAX_PATH ];
	STDRVTLLOG stLog;

	if ( NULL == pbtPacket || 0 == uPacketLength || NULL == lpwszType )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	KeQuerySystemTime( &lnSystemTime );
	ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
	RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

	if ( g_stCfg.uLogDirLength > 4 )
	{
		memset( wszLogFile, 0, sizeof(wszLogFile) );
		_snwprintf
		(
			wszLogFile, sizeof(wszLogFile)/sizeof(WCHAR)-1,
			L"%s[%s-%d]%04d%02d%02d-%02d%02d%02d-%d.log",
			g_stCfg.wszLogDir,
			lpwszType,
			( bBlocked ? 1 : 0 ),
			tfTimeFields.Year, tfTimeFields.Month, tfTimeFields.Day,
			tfTimeFields.Hour, tfTimeFields.Minute, tfTimeFields.Second, tfTimeFields.Milliseconds
		);

		//bRet = drvfunc_save_filecontent( wszLogFile, (PVOID)pbtPacket, uPacketLength );

		stLog.lpcwszFilename	= wszLogFile;
		stLog.lpvContent	= (PVOID)pbtPacket;
		stLog.dwSize		= uPacketLength;

		if ( NT_SUCCESS( drvthreadlogger_execute_task( drvthreadlogger_worker, (PVOID)(&stLog) ) ) )
		{
			bRet = TRUE;
		}	
	}

	return bRet;
}

/**
 *	�����ַ�������
 */
BOOLEAN procpacket_swap_string( UCHAR * lpuszString, INT nLen, INT nLeftStrLen )
{
	//
	//	lpuszString	- [in] ����Դ�ַ���
	//	uLen		- [in] ����Դ�ַ����ĳ���
	//	uLeftStrLen	- [in] �����Ҫ�ƶ����ַ����ĳ���
	//	
	INT i, j, k;
	UCHAR cTemp;

	if ( NULL == lpuszString || 0 == nLen || nLeftStrLen >= nLen )
	{
		return FALSE;
	}	

	__try
	{
		for ( i = nLeftStrLen, k = 1;
			i < nLen;
			i++, k++ )
		{
			for ( j = i; j >= k ; j -- )
			{
				cTemp			= lpuszString[ j ];
				lpuszString[ j ]	= lpuszString[ j - 1 ];
				lpuszString[ j - 1 ]	= cTemp;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_swap_string\n" ));
	}	

	return TRUE;
}

/**
 *	�� HTTP �������Ƿ���������ʵ� ����
 */
BOOLEAN procpacket_http_is_allowed_domain( UCHAR * lpuszHttp )
{
	//
	//	lpuszHttp	- [in] http ����ʼ��λ��
	//
	//	HTTP�����ݸ�ʽ����
	//	------------------------------------------------------------
	//	GET http://runonce.msn.com/runonce3.aspx HTTP/1.1
	//	Accept: */*
	//	Accept-Language: zh-cn
	//	UA-CPU: x86
	//	Accept-Encoding: gzip, deflate
	//	User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727; .NET CLR 1.1.4322)
	//	Host: runonce.msn.com
	//	Connection: Keep-Alive
	//

	BOOLEAN bRet;
	UCHAR * lpuszHost;
	UCHAR * lpuszHostNew;
	UCHAR * lpuszTopDomain;
	UCHAR * lpuszUserAgent;
	UCHAR * lpuszTail;
	UCHAR * lpuszTailCrlf;
	UCHAR * lpuszTailColon;

	UINT	uHttpHostDLen;
	UINT	uHttpUserAgentDLen;
	UINT	uFullDomainLen;
	UINT	uTopDomainLen;
	UINT	uTopDomainOffset;

	UCHAR * lpuszSubDomain;
	UINT	uSubDomainLen;
	BOOLEAN bDomainMatched;

	if ( NULL == lpuszHttp )
	{
		return FALSE;
	}
	if ( 0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uDomainSingleCount )
	{
		//	���������Ϊ�գ�Ҳ��û���κ��������Է��ʣ��������ص�
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		uHttpHostDLen		= PROCPACKET_HTTP_HOST_DLEN;

		//	find "Host:"
		lpuszHost = strstr( lpuszHttp, g_szDecryptedString_Host1 );
		if ( NULL == lpuszHost )
		{
			uHttpHostDLen --;
			lpuszHost = strstr( lpuszHttp, g_szDecryptedString_Host2 );
		}

		if ( lpuszHost )
		{
			//
			//	Ҫ����������ļ��е���������
			//
			uTopDomainLen	= 0;
			lpuszTopDomain	= NULL;
			lpuszTailCrlf	= strstr( lpuszHost, "\r\n" );
			lpuszTailColon	= strstr( lpuszHost + uHttpHostDLen, ":" );
			if ( lpuszTailCrlf && lpuszTailColon )
			{
				lpuszTail = min( lpuszTailCrlf, lpuszTailColon );
			}
			else
			{
				lpuszTail = lpuszTailCrlf;
			}
			if ( lpuszTail )
			{
				uFullDomainLen	= (UINT)( lpuszTail - lpuszHost - uHttpHostDLen );
				if ( uFullDomainLen >= 4 && uFullDomainLen < 128 )
				{
					//	4 ~ 128 �ĳ�����Ϊ�ǱȽϺ��ʵ���������
					bRet = procconfig_is_matched_domain( lpuszHost + uHttpHostDLen, uFullDomainLen );
				}
				else
				{
					//	�Ź� - �������Ȳ���ȷ
					bRet = TRUE;
				}	
			}
			else
			{
				//	�Ź� - δ�ҵ� HOST �ڵ���ȷ��β
				bRet = TRUE;
			}	
		}
		else
		{
			//
			//	����δ�ҵ� HOST ��
			//

			//	ȷ�ϵ�ȷ�� HTTP ��
			if ( strstr( lpuszHttp, " HTTP/1." ) || strstr( lpuszHttp, " http/1." ) )
			{
				//	find "User-Agent:"
				lpuszUserAgent = strstr( lpuszHttp, g_szDecryptedString_UserAgent1 );
				if ( NULL == lpuszUserAgent )
				{
					lpuszUserAgent = strstr( lpuszHttp, g_szDecryptedString_UserAgent2 );
				}

				if ( lpuszUserAgent )
				{
					//	���ں��� " HTTP/1." �� "User-Agent: " �ڣ��϶��� HTTP
					//	�����İ�Ҫ����
					bRet = FALSE;
				}
				else
				{
					//	δ�ҵ� "User-Agent: " �е�Σ�գ��Ź�
					bRet = TRUE;
				}	
			}
			else
			{
				//	δ�ҵ� " HTTP/1." �е�Σ�գ��Ź�
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_http_is_allowed_domain\n" ));
	}	

	return bRet;
}


/**
 *	�������� TCP/UDP ��У��ͣ�����䵽����
 */
BOOLEAN procpacket_fill_packet_checksum( UCHAR * puszPacket )
{
	//
	//	puszPacket	- [in] IP ���ݰ�
	//	RETURN		�ɹ����㲢��дУ��� TRUE
	//			OR FALSE
	//

	BOOLEAN bRet				= FALSE;
	STIPHEADER * pstIPHeader		= NULL;		//	IPͷָ��
	UINT uIpHeaderLen;					//	IPͷ����
	USHORT uIpDataLen			= 0;		//	IP���ݲ������峤��

	STTCPHEADER * pstTCPHeader		= NULL;		//	TCPͷָ��
	STTCPPSDHEADER * pstTCPPsdHeader	= NULL;

	STIPHEADER * pstIPHeaderTemp;		//	��ʱ��� IP ͷ


	if ( NULL == puszPacket )
	{
		return FALSE;
	}

	__try
	{
		pstIPHeader = (STIPHEADER*)puszPacket;
		if ( pstIPHeader )
		{
			if ( IPPROTO_TCP == pstIPHeader->ucProtocol )
			{
				////////////////////////////////////////
				//	FOR TCP ��
				////////////////////////////////////////

				//	IPͷ����
				uIpHeaderLen	= pstIPHeader->ucHeaderLength * 4;	//	( ( pstIPHeader->ucHeaderLength ) & 15 ) * 4;
				if ( uIpHeaderLen >= sizeof(STIPHEADER) )
				{
					pstTCPHeader	= (STTCPHEADER*)( puszPacket + uIpHeaderLen );

					//	�����Э��ͷ�Լ��������ݵ��ܳ���
					//	TCP ͷ�� + TCP ������������
					uIpDataLen	= procpacket_ntohs( pstIPHeader->uTotalLength ) - uIpHeaderLen;

					//
					//	1, ���� IP ͷ
					//	2, ��� TCP αͷ��
					//	3, ���� TCP ���� checksum
					//	4, ��ԭ IP ͷ
					//	5, ���� IP ͷ���� checksum
					//
					pstIPHeaderTemp	= (STIPHEADER*)ExAllocatePool( NonPagedPool, uIpHeaderLen );
					if ( pstIPHeaderTemp )
					{
						memset( pstIPHeaderTemp, 0, uIpHeaderLen );

						//	���� IP ͷ
						memcpy( pstIPHeaderTemp, pstIPHeader, uIpHeaderLen );

						//	���α TCP ͷ
						//	��Ҫ���������ݣ����� IP ͷ��ƫ��
						pstTCPPsdHeader = (STTCPPSDHEADER*)( (UCHAR*)pstIPHeader + ( uIpHeaderLen - sizeof(STTCPPSDHEADER) ) );
						memset( pstTCPPsdHeader, 0, sizeof(STTCPPSDHEADER) );

						pstTCPPsdHeader->destip		= pstIPHeaderTemp->ulDestinationAddress;
						pstTCPPsdHeader->sourceip	= pstIPHeaderTemp->ulSourceAddress;
						pstTCPPsdHeader->mbz		= 0;
						pstTCPPsdHeader->ptcl		= IPPROTO_TCP;
						pstTCPPsdHeader->tcpl		= procpacket_htons( uIpDataLen );

						//
						//	���� TCP У��ͣ���д��ԭʼ���ڴ�
						//	TCP �ײ� + TCP ���ݲ����������У���
						//
						pstTCPHeader->checksum	= 0;
						pstTCPHeader->checksum	= procpacket_get_checksum( (USHORT*)pstTCPPsdHeader, uIpDataLen + sizeof(STTCPPSDHEADER) );

						//	��ԭ IP ͷ
						memcpy( pstIPHeader, pstIPHeaderTemp, uIpHeaderLen );

						//
						//	����ipͷ��У��ͣ���д��ԭʼ���ڴ�
						//	IP �ײ�У����Ƕ� IP ͷ��У���
						//
						pstIPHeader->uChecksum	= 0;
						pstIPHeader->uChecksum	= procpacket_get_checksum( (USHORT*)pstIPHeader, uIpHeaderLen );


						//	free memory
						ExFreePool( pstIPHeaderTemp );
						pstIPHeaderTemp = NULL;
					}
				}
			}
			else if ( IPPROTO_UDP == pstIPHeader->ucProtocol )
			{
				////////////////////////////////////////
				//	FOR UDP ��
				////////////////////////////////////////
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_fill_packet_checksum\n" ));
	}


	return bRet;
}


/**
 *	��ȡ TCP �� checksum
 */
USHORT procpacket_get_checksum( USHORT * puszBuffer, int nSize )
{
	ULONG ulRet = 0;

	__try
	{
		while ( nSize > 1 )
		{
			ulRet += *puszBuffer++;
			nSize  -= sizeof(USHORT);
		}

		if ( nSize )
		{
			ulRet += *(UCHAR*)puszBuffer;
		}

		ulRet	= ( ulRet >> 16 ) + ( ulRet & 0xffff );
		ulRet	+= ( ulRet >>16 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_get_checksum\n" ));
	}	

	return (USHORT)( ~ulRet );
}


/**
 *	��ȡ��ȷ�� TCP �˿�
 */
USHORT procpacket_get_valid_tcpport( USHORT uPort )
{
	//	�������� 8 λ��� 8 λ�Ե�
	return MAKEWORD( HIBYTE(uPort), LOBYTE(uPort) );
}

/**
 *	һЩ NET ������
 **/
USHORT procpacket_ntohs( USHORT x )
{
	#if BYTE_ORDER == LITTLE_ENDIAN
		UCHAR * s = (UCHAR*)&x;
		return (USHORT)( s[0] << 8 | s[1] );
	#else
		return x;
	#endif
}
USHORT procpacket_htons( USHORT x )
{
	#if BYTE_ORDER == LITTLE_ENDIAN
		UCHAR *s = (UCHAR *)&x;
		return (USHORT)( s[0] << 8 | s[1] );
	#else
		return x;
	#endif
}
BOOLEAN procpacket_inet_ntoa( ULONG ulAddr, PCHAR pszAddr, UINT uSize )
{
	CHAR * pTemp;

	if ( NULL == pszAddr || uSize < 16 )
	{
		return FALSE;
	}

	pTemp = (CHAR*)&ulAddr;

	_snprintf
	(
		pszAddr, uSize/sizeof(CHAR)-1,
		"%d.%d.%d.%d",
		(INT)( ulAddr & 0x000000FF ),
		(INT)( ( ulAddr & 0x0000FF00 ) >> 8 ),
		(INT)( ( ulAddr & 0x00FF0000 ) >> 16 ),
		(INT)( ( ulAddr & 0xFF000000 ) >> 24 )
	);

	return TRUE;
}