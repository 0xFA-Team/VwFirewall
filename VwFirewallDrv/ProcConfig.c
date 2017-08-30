// ProcConfig.c: implementation of the CProcConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcConfig.h"
#include "DrvFunc.h"
#include "crc32.h"


/**
 *	�����ַ���
 */
//	"GET /"
static CHAR g_szEncryptedString_GET[]			= { -72, -70, -85, -33, -48, 0 };

//	"POST /"
static CHAR g_szEncryptedString_POST[]			= { -81, -80, -84, -85, -33, -48, 0 };

//	"Host: "
static CHAR g_szEncryptedString_Host1[]			= { -73, -112, -116, -117, -59, -33, 0 };

//	"Host:"
static CHAR g_szEncryptedString_Host2[]			= { -73, -112, -116, -117, -59, 0 };

//	"User-Agent: "
static CHAR g_szEncryptedString_UserAgent1[]		= { -86, -116, -102, -115, -46, -66, -104, -102, -111, -117, -59, -33, 0 };

//	"User-Agent:"
static CHAR g_szEncryptedString_UserAgent2[]		= { -86, -116, -102, -115, -46, -66, -104, -102, -111, -117, -59, 0 };

//	"VwPanDmNewCfg"
static CHAR g_szEncryptedString_VwPanDmNewCfg[]		= { -87, -120, -81, -98, -111, -69, -110, -79, -102, -120, -68, -103, -104, 0 };

//	"reg_ip="
static CHAR g_szEncryptedString_reg_ip_[]		= { -115, -102, -104, -96, -106, -113, -62, 0 };

//	"reg_key="
static CHAR g_szEncryptedString_reg_key_[]		= { -115, -102, -104, -96, -108, -102, -122, -62, 0 };

//	"reg_prtype="
static CHAR g_szEncryptedString_reg_prtype_[]		= { -115, -102, -104, -96, -113, -115, -117, -122, -113, -102, -62, 0 };

//	"reg_prtypecs="
static CHAR g_szEncryptedString_reg_prtypecs_[]		= { -115, -102, -104, -96, -113, -115, -117, -122, -113, -102, -100, -116, -62, 0 };

//
//	for prtype format
//	"%s-vwprt-%s-%s"
static TCHAR g_szEncryptedString_PrTypeChecksumFmt[]	= { -38, -116, -46, -119, -120, -113, -115, -117, -46, -38, -116, -46, -38, -116, 0 };

//
//	for prtype
//	"comadv"
static TCHAR g_szEncryptedString_PrType_comadv[]	= { -100, -112, -110, -98, -101, -119, 0 };

//	"comnormal"
static TCHAR g_szEncryptedString_PrType_comnormal[]	= { -100, -112, -110, -111, -112, -115, -110, -98, -109, 0 };

//	"pslunlmt"
static TCHAR g_szEncryptedString_PrType_pslunlmt[]	= { -113, -116, -109, -118, -111, -109, -110, -117, 0 };

//	"psladv"
static TCHAR g_szEncryptedString_PrType_psladv[]	= { -113, -116, -109, -98, -101, -119, 0 };

//	"pslnormal"
static TCHAR g_szEncryptedString_PrType_pslnormal[]	= { -113, -116, -109, -111, -112, -115, -110, -98, -109, 0 };






/**
 *	ȫ�ֱ�������
 */
STCONFIG g_stCfg;
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolDomain;		//	������Ŀ¼��
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolSysHost;
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolRdpClientName;		//	RDP �ͻ��˼������ ��
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDir;		//	������Ŀ¼��
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDirCache;	//	������Ŀ¼Cache��
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolExceptedProcess;	//	����Ľ���
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilename;		//	�ļ����ڴ��
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilenameW;		//	�ļ����ڴ��W

CHAR g_szDecryptedString_GET[ 16 ]			= {0};
CHAR g_szDecryptedString_POST[ 16 ]			= {0};
CHAR g_szDecryptedString_Host1[ 16 ]			= {0};
CHAR g_szDecryptedString_Host2[ 16 ]			= {0};
CHAR g_szDecryptedString_UserAgent1[ 16 ]		= {0};
CHAR g_szDecryptedString_UserAgent2[ 16 ]		= {0};
CHAR g_szDecryptedString_VwPanDmNewCfg[ 16 ]		= {0};
CHAR g_szDecryptedString_reg_ip_[ 16 ]			= {0};
CHAR g_szDecryptedString_reg_key_[ 16 ]			= {0};
CHAR g_szDecryptedString_reg_prtype_[ 16 ]		= {0};
CHAR g_szDecryptedString_reg_prtypecs_[ 16 ]		= {0};

//	for prtypefmt
CHAR g_szDecryptedString_PrTypeChecksumFmt[ 32 ]	= {0};

//	for prtype
CHAR g_szDecryptedString_PrType_comadv[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_comnormal[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_pslunlmt[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_psladv[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_pslnormal[ 32 ]		= {0};






/**
 *	@ Public
 *	��ʼ����������
 */
BOOLEAN procconfig_init_config()
{
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	__try
	{
		//	Init ProcProcess ...
		procprocess_init();

		//	���ܳ����ַ���
		procconfig_decrypt_string();


		RtlZeroMemory( &g_stCfg, sizeof(g_stCfg) );

		g_stCfg.bInited		= FALSE;
		g_stCfg.bLoading	= TRUE;
		g_stCfg.bStart		= FALSE;

		//	��ʼ���ڴ��
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolDomain, NULL, NULL, 0, sizeof(STDOMAIN), 'pdm', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolSysHost, NULL, NULL, 0, sizeof(STSYSHOST), 'pds', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolRdpClientName, NULL, NULL, 0, sizeof(STRDPCLIENTNAME), 'rcn', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, NULL, NULL, 0, sizeof(STPROTECTEDDIR), 'ptd', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache, NULL, NULL, 0, sizeof(STPROTECTEDDIRCACHE), 'ptdc', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess, NULL, NULL, 0, sizeof(STEXCEPTEDPROCESS), 'ecp', 0 );

		//	��ʼ���ڴ�أ�ÿ��Ԫ�ش�С�� MAX_PATH
		//	ʹ�÷�����
		//		���룺pKeyName = (PCHAR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolFilename );
		//		      pKeyName = (PWCHAR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolFilename );
		//		�ͷţ�ExFreeToNPagedLookasideList( &g_npgProcConfigPoolFilename, pKeyName );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolFilename, NULL, NULL, 0, MAX_PATH, 'fnp', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolFilenameW, NULL, NULL, 0, MAX_WPATH, 'fwp', 0 );

		//	��ǰʹ�õ�������������
		g_stCfg.uCurrIndex	= 0;	//	��ǰʹ�õ�������������: 0/1

		//	init fast mutex
		ExInitializeFastMutex( &g_stCfg.stCfgData[0].muxLock );
		ExInitializeFastMutex( &g_stCfg.stCfgData[1].muxLock );

		//	��¼�������Ĵ��̷���
		RtlZeroMemory( g_stCfg.stCfgData[0].wszProtedDisk, sizeof(g_stCfg.stCfgData[0].wszProtedDisk) );
		RtlZeroMemory( g_stCfg.stCfgData[1].wszProtedDisk, sizeof(g_stCfg.stCfgData[1].wszProtedDisk) );


		//	������Ŀ¼����/����
		g_stCfg.stCfgData[0].pstProtectedDirCacheHeader	= NULL;
		g_stCfg.stCfgData[0].uProtectedDirCacheCount	= 0;

		g_stCfg.stCfgData[1].pstProtectedDirCacheHeader	= NULL;
		g_stCfg.stCfgData[1].uProtectedDirCacheCount	= 0;


		//	��ʼ��������Ŀ¼(tree)������
		g_stCfg.stCfgData[0].linkDomainSingle.Next		= NULL;
		g_stCfg.stCfgData[0].uDomainSingleCount			= 0;
		g_stCfg.stCfgData[0].linkSysHostSingle.Next		= NULL;
		g_stCfg.stCfgData[0].uSysHostSingleCount		= 0;
		g_stCfg.stCfgData[0].linkRdpClientNameSingle.Next	= NULL;
		g_stCfg.stCfgData[0].uRdpClientNameSingleCount		= 0;
		g_stCfg.stCfgData[0].linkProtedDirTree.Next		= NULL;
		g_stCfg.stCfgData[0].uProtedDirTreeCount		= 0;
		g_stCfg.stCfgData[0].linkProtedDirSingle.Next		= NULL;
		g_stCfg.stCfgData[0].uProtedDirSingleCount		= 0;

		g_stCfg.stCfgData[1].linkDomainSingle.Next		= NULL;
		g_stCfg.stCfgData[1].uDomainSingleCount			= 0;
		g_stCfg.stCfgData[1].linkSysHostSingle.Next		= NULL;
		g_stCfg.stCfgData[1].uSysHostSingleCount		= 0;
		g_stCfg.stCfgData[1].linkRdpClientNameSingle.Next	= NULL;
		g_stCfg.stCfgData[1].uRdpClientNameSingleCount		= 0;	
		g_stCfg.stCfgData[1].linkProtedDirTree.Next		= NULL;
		g_stCfg.stCfgData[1].uProtedDirTreeCount		= 0;
		g_stCfg.stCfgData[1].linkProtedDirSingle.Next		= NULL;
		g_stCfg.stCfgData[1].uProtedDirSingleCount		= 0;

		//
		//	����Ѿ���ʼ����
		//
		g_stCfg.bInited	= TRUE;

		
		//	��ȡϵͳʱ��
		KeQuerySystemTime( &lnSystemTime );
		ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
		RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

		//
		//	��������ͳ��
		//
		g_stCfg.dwRtspNowDay			= tfTimeFields.Day;
		g_stCfg.llProtectFileBlockCount		= 0;
		g_stCfg.llProtectDomainBlockCount	= 0;
		g_stCfg.llProtectRdpBlockCount		= 0;


		return TRUE;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procconfig_init_config"));
		return FALSE;
	}
}

/**
 *	@ Public
 *	ж��������Ϣ
 */
VOID procconfig_unload_config()
{
	//	ɾ���ڴ��

	__try
	{
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolDomain );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolSysHost );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolRdpClientName );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolProtectedDir );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolFilename );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolFilenameW );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

/**
 *	@ Public
 *	�Ƿ�����Ч�� KEY��������֤�Ƿ�ע��
 */
BOOLEAN procconfig_is_valid_key( LPCWSTR lpcwszKeyFile )
{
	BOOLEAN bRet		= FALSE;
	DWORD dwFileSize;
	DWORD dwFileValidLength	= 10240;

	CHAR * pszBuffer;
	CHAR * pszSource;	//	len = MAX_PATH
	CHAR * pszMd5;		//	len = 34
	DWORD dwSourceLen	= 260;
	DWORD dwMd5Len		= 34;
	
	DWORD dwSubOffset;
	DWORD dwOffset;

	STREGDATE * pstRegDate;
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	__try
	{
		if ( NULL == lpcwszKeyFile || 0 == wcslen( lpcwszKeyFile ) )
		{
			return FALSE;
		}
		if ( ! drvfunc_is_file_exist( lpcwszKeyFile ) )
		{
			return FALSE;
		}

		dwFileSize = drvfunc_get_filesize( lpcwszKeyFile );
		if ( dwFileValidLength != dwFileSize )
		{
			return FALSE;
		}


		pszBuffer	= (CHAR*)ExAllocatePool( NonPagedPool, dwFileSize );
		pszSource	= (CHAR*)ExAllocatePool( NonPagedPool, dwSourceLen );
		pszMd5		= (CHAR*)ExAllocatePool( NonPagedPool, dwMd5Len );
		if ( pszBuffer && pszSource && pszMd5 )
		{
			RtlZeroMemory( pszBuffer, dwFileSize );
			RtlZeroMemory( pszSource, dwSourceLen );
			RtlZeroMemory( pszMd5, dwMd5Len );

			if ( drvfunc_get_filecontent( lpcwszKeyFile, pszBuffer, dwFileSize ) )
			{
				//	���� MD5 ֵ
				//	g_szDecryptedString_VwPanDmNewCfg = "VwPanDmNewCfg"
				_snprintf( pszSource, dwSourceLen/sizeof(CHAR)-1, "%s,%s,%d,%s", g_stCfg.szRegPrType, g_stCfg.szRegIp, 1, g_stCfg.szRegKey );
				_strlwr( pszSource );
				md5_string( pszSource, strlen(pszSource), pszMd5, dwMd5Len );
				_strlwr( pszMd5 );

				dwSubOffset = atoi(g_stCfg.szRegIp);
				if ( dwSubOffset > 255 )
				{
					dwSubOffset = 255;
				}
				dwOffset = 1024 + dwSubOffset;
				if ( dwOffset < dwFileSize &&
					0 == _strnicmp( pszMd5, pszBuffer+dwOffset, 8 ) )
				{
					//	������ʱ��
					pstRegDate = (STREGDATE*)( pszBuffer + dwOffset + 8 );
					if ( pstRegDate && 
						( 0 != pstRegDate->nExpireYear && 0 != pstRegDate->nExpireMonth && 0 != pstRegDate->nExpireDay )
					)
					{
						//	��ȡϵͳʱ��
						KeQuerySystemTime( &lnSystemTime );
						ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
						RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

						//	�Ƚ�ʱ��
						if ( pstRegDate->nExpireYear > tfTimeFields.Year )
						{
							bRet = TRUE;
						}
						else if ( pstRegDate->nExpireYear == tfTimeFields.Year )
						{
							if ( pstRegDate->nExpireMonth > tfTimeFields.Month )
							{
								bRet = TRUE;
							}
							else if ( pstRegDate->nExpireMonth == tfTimeFields.Month )
							{
								if ( pstRegDate->nExpireDay >= tfTimeFields.Day )
								{
									bRet = TRUE;
								}
							}
						}
					}
					else
					{
						bRet = TRUE;
					}
				}
			}

			ExFreePool( pszBuffer );
			pszBuffer = NULL;

			ExFreePool( pszSource );
			pszSource = NULL;

			ExFreePool( pszMd5 );
			pszMd5 = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procconfig_is_valid_key"));
	}

	return bRet;
}

/**
 *	@ Public
 *	����Ƿ�����Ч�� PrType �� checksum ֵ
 */
BOOLEAN procconfig_is_valid_prtype_checksum( LPCTSTR lpcszPrTypeChecksum )
{
	BOOLEAN  bRet			= FALSE;
	CHAR * pszSource;		//	len = MAX_PATH
	CHAR * pszMd5;			//	len = 34
	DWORD dwSourceLen	= 260;
	DWORD dwMd5Len		= 34;

	__try
	{
		if ( NULL == lpcszPrTypeChecksum || 0 == lpcszPrTypeChecksum[0] )
		{
			return FALSE;
		}
		if ( 0 == g_stCfg.szRegPrType[0] || 0 == g_stCfg.szRegIp[0] || 0 == g_stCfg.szRegKey[0] )
		{
			return FALSE;
		}

		pszSource	= (CHAR*)ExAllocatePool( NonPagedPool, dwSourceLen );
		pszMd5		= (CHAR*)ExAllocatePool( NonPagedPool, dwMd5Len );
		if ( pszSource && pszMd5 )
		{
			RtlZeroMemory( pszSource, dwSourceLen );
			RtlZeroMemory( pszMd5, dwMd5Len );

			//	���� MD5 ֵ
			//	"%s-vwprt-%s-%s"
			_snprintf( pszSource, dwSourceLen/sizeof(CHAR)-1, g_szDecryptedString_PrTypeChecksumFmt, g_stCfg.szRegPrType, g_stCfg.szRegIp, g_stCfg.szRegKey );
			_strlwr( pszSource );
			md5_string( pszSource, strlen(pszSource), pszMd5, dwMd5Len );
			
			if ( 0 == _stricmp( lpcszPrTypeChecksum, pszMd5 ) )
			{
				bRet = TRUE;
			}
			
			ExFreePool( pszSource );
			pszSource = NULL;

			ExFreePool( pszMd5 );
			pszMd5 = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procconfig_is_valid_prtype_checksum"));
	}

	return bRet;
}

/**
 *	@ Public
 *	װ��������Ϣ
 */
BOOLEAN procconfig_load_config( LPCWSTR lpcwszConfigFile )
{
	BOOLEAN bRet		= FALSE;
	UINT uOnlineIndex;

	__try
	{
		if ( ! g_stCfg.bInited )
		{
			return FALSE;
		}

		//
		//	��������� offline ����������
		//
		procconfig_clean_all_offline_config();

		//
		//	װ������
		//
		if ( procconfig_load_allconfig_from_file( lpcwszConfigFile ) )
		{
			//
			//	��ס offline �� online �����������л���������ֵ
			//

			//	��õ�ǰ���ߵ�����������
			//	��Ϊ SwitchIndex() �Ļ��ڣ����Ա����ø����� uOnlineIndex ��¼
			uOnlineIndex = procconfig_stconfig_get_online_index();


			g_stCfg.bLoading	= TRUE;

			//	��ס���ߵ�
			procconfig_lock( uOnlineIndex );

			//	����: ����/����
			procconfig_stconfig_switch_index();

			//	�������ߵ�
			procconfig_unlock( uOnlineIndex );

			g_stCfg.bLoading	= FALSE;


			//	��װ�سɹ�
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_load_config\n" ));
	}	

	return bRet;
}

/**
 *	@ Public
 *	��ʼ����
 */
BOOLEAN procconfig_start()
{
	__try
	{
		procconfig_lock( -1 );

		g_stCfg.bStart	= TRUE;

		procconfig_unlock( -1 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_start\n" ));
	}

	return TRUE;
}

/**
 *	@ Public
 *	ֹͣ����
 */
BOOLEAN procconfig_stop()
{
	__try
	{
		procconfig_lock( -1 );;

		g_stCfg.bStart	= FALSE;

		procconfig_unlock( -1 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_stop\n" ));
	}

	return TRUE;
}

/**
 *	@ Public
 *	�Ƿ���
 */
BOOLEAN procconfig_is_start()
{
	//
	//	��������ӹ��ܶ�δ�������򷵻� FALSE
	//
	if ( FALSE == g_stCfg.bStartFileSafe &&
		FALSE == g_stCfg.bStartProtectDomain &&
		FALSE == g_stCfg.bStartProtectRdp )
	{
		return FALSE;
	}

	//
	//	��������ӹ��ܶ�Ϊ�����κ����ݣ��򷵻� FLASE
	//
	if ( 0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirTreeCount &&
		0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirSingleCount &&
		0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uDomainSingleCount &&
		0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uRdpClientNameSingleCount )
	{
		return FALSE;
	}

	return g_stCfg.bStart;
}

/**
 *	@ Public
 *	�Ƿ�����װ��������Ϣ
 */
BOOLEAN procconfig_is_loading()
{
	return g_stCfg.bLoading;
}

/**
 *	@ Public
 *	����
 */
BOOLEAN procconfig_lock( INT nCurrIndex )
{
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		if ( IS_USER_DEBUG )
		{
			return FALSE;
		}

		//
		//	����/�������������� online ��������Ч
		//
		if ( -1 == nCurrIndex )
		{
			ExAcquireFastMutex( & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].muxLock );
		}
		else if ( 0 == nCurrIndex || 1 == nCurrIndex )
		{
			ExAcquireFastMutex( & g_stCfg.stCfgData[ nCurrIndex ].muxLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_lock\n" ));
	}	

	return TRUE;
}

/**
 *	@ Public
 *	����
 */
BOOLEAN procconfig_unlock( INT nCurrIndex )
{
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		if ( IS_USER_DEBUG )
		{
			return FALSE;
		}

		//
		//	����/�������������� online ��������Ч
		//
		if ( -1 == nCurrIndex )
		{
			ExReleaseFastMutex( & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].muxLock );
		}
		else if ( 0 == nCurrIndex || 1 == nCurrIndex )
		{
			ExReleaseFastMutex( & g_stCfg.stCfgData[ nCurrIndex ].muxLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_unlock\n" ));
	}	

	return TRUE;
}


/**
 *	@ Public
 *	�ۼ����ؼ�����
 */
VOID procconfig_add_block_count( LONGLONG * pllCounter )
{
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	if ( NULL == pllCounter )
	{
		return;
	}

	//	��ȡϵͳʱ��
	KeQuerySystemTime( &lnSystemTime );
	ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
	RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

	if ( g_stCfg.dwRtspNowDay == tfTimeFields.Day )
	{
		//
		//	�ۼӼ�����
		//
		if ( (*pllCounter) >= 0 )
		{
			//	�ۼ����ش���
			(*pllCounter) ++;
		}
		else
		{
			//	������ܵ����
			(*pllCounter) = 0;
		}	
	}
	else
	{
		//
		//	����һ�죬���¿���
		//
		g_stCfg.dwRtspNowDay			= tfTimeFields.Day;
		g_stCfg.llProtectFileBlockCount		= 0;
		g_stCfg.llProtectDomainBlockCount	= 0;
		g_stCfg.llProtectRdpBlockCount		= 0;
	}
}


/**
 *	@ Public
 *	��ȡ topdomain ��������������ʼ��ƫ����
 */
BOOLEAN procconfig_is_matched_domain( LPCSTR lpcszOrgDomain, UINT uOrgDomainLength )
{
	//
	//	lpcszOrgDomain		- [in] HTTP���޸�ǰ��ԭʼ���������磺abc.chinapig.cn
	//	uOrgDomainLength	- [in] lpcszOrgDomain �ĳ��ȣ��� byte Ϊ��λ
	//	RETURN			- topdomain ��������������ʼ��ƫ������ʧ�ܷ��� -1
	//

	BOOLEAN bRet;
	STDOMAIN * pstData;

	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bStart )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}

	if ( NULL == lpcszOrgDomain )
	{
		return FALSE;
	}
	if ( 0 == uOrgDomainLength || uOrgDomainLength >= MAX_PATH )
	{
		return FALSE;
	}


	//	��ʼ��
	bRet = FALSE;

	//
	//	����
	//
	procconfig_lock( -1 );;


	__try
	{
		//
		//	ȷ���Ƿ��� �������ķ�����
		//
		pstData	= procconfig_get_matched_domain( lpcszOrgDomain, uOrgDomainLength );
		if ( pstData )
		{
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_matched_domain\n" ));
	}

	//
	//	����
	//
	procconfig_unlock( -1 );


	return bRet;
}





/**
 *	�������б�����Ŀ¼
 */
BOOLEAN procconfig_clean_all_offline_config()
{
	STDOMAIN * pstDataDomain;
	STSYSHOST * pstDataSysDomain;
	STRDPCLIENTNAME * pstDataRdpClientName;
	STPROTECTEDDIR * pstDataProtectedDir;
	STEXCEPTEDPROCESS * pstExceptedProcess;
	SINGLE_LIST_ENTRY * pEntry;
	UINT uOfflineIndex;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		//
		//	������������ offline ��
		//
		uOfflineIndex = procconfig_stconfig_get_offline_index();


		//	���� offline
		procconfig_lock( uOfflineIndex );

		//////////////////////////////////////////////////////////////////////////
		//	ɾ����������������
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uDomainSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkDomainSingle );
			while( pEntry )
			{
				pstDataDomain = CONTAINING_RECORD( pEntry, STDOMAIN, ListEntry );
				if ( pstDataDomain )
				{
					//	Callers of ExFreeToNPagedLookasideList must be running at IRQL <= DISPATCH_LEVEL.					
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolDomain, pstDataDomain );
					pstDataDomain = NULL;
				}

				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkDomainSingle );
			}

			//	���������
			g_stCfg.stCfgData[ uOfflineIndex ].uDomainSingleCount = 0;
		}



		//////////////////////////////////////////////////////////////////////////
		//	ɾ������ SysHost
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uSysHostSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkSysHostSingle );
			while( pEntry )
			{
				pstDataSysDomain = CONTAINING_RECORD( pEntry, STSYSHOST, ListEntry );
				if ( pstDataSysDomain )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolSysHost, pstDataSysDomain );
					pstDataSysDomain = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkSysHostSingle );
			}

			//	���������
			g_stCfg.stCfgData[ uOfflineIndex ].uSysHostSingleCount = 0;
		}


		//////////////////////////////////////////////////////////////////////////
		//	ɾ������ RDP ClientName
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uRdpClientNameSingleCount )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkRdpClientNameSingle );
			while( pEntry )
			{
				pstDataRdpClientName = CONTAINING_RECORD( pEntry, STRDPCLIENTNAME, ListEntry );
				if ( pstDataRdpClientName )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolRdpClientName, pstDataRdpClientName );
					pstDataRdpClientName = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkRdpClientNameSingle );
			}

			//	���������
			g_stCfg.stCfgData[ uOfflineIndex ].uRdpClientNameSingleCount = 0;
		}


		//	������̱��
		RtlZeroMemory( g_stCfg.stCfgData[ uOfflineIndex ].wszProtedDisk, sizeof(g_stCfg.stCfgData[ uOfflineIndex ].wszProtedDisk) );

		//////////////////////////////////////////////////////////////////////////
		//	��ʼɾ�����еı�����Ŀ¼ for tree type
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirTreeCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirTree );
			while( pEntry )
			{
				pstDataProtectedDir = CONTAINING_RECORD( pEntry, STPROTECTEDDIR, ListEntry );
				if ( pstDataProtectedDir )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, pstDataProtectedDir );
					pstDataProtectedDir = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirTree );
			}
			
			//	���������
			g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirTreeCount = 0;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//	��ʼɾ�����еı�����Ŀ¼ for single type
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirSingle );
			while( pEntry )
			{
				pstDataProtectedDir = CONTAINING_RECORD( pEntry, STPROTECTEDDIR, ListEntry );
				if ( pstDataProtectedDir )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, pstDataProtectedDir );
					pstDataProtectedDir = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirSingle );
			}
			
			//	���������
			g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirSingleCount = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//	��ʼɾ�����еı�����Ŀ¼ CACHE
		//////////////////////////////////////////////////////////////////////////
		procconfig_clean_protected_dir_cache_crc32
			(
				& g_stCfg.stCfgData[ uOfflineIndex ],
				g_stCfg.stCfgData[ uOfflineIndex ].pstProtectedDirCacheHeader
			);
		g_stCfg.stCfgData[ uOfflineIndex ].pstProtectedDirCacheHeader	= NULL;
		g_stCfg.stCfgData[ uOfflineIndex ].uProtectedDirCacheCount	= 0;

		
		//////////////////////////////////////////////////////////////////////////
		//	ɾ������ Excepted Process
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uExceptedProcessSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkExceptedProcessSingle );
			while( pEntry )
			{
				pstExceptedProcess = CONTAINING_RECORD( pEntry, STEXCEPTEDPROCESS, ListEntry );
				if ( pstExceptedProcess )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess, pstExceptedProcess );
					pstExceptedProcess = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkExceptedProcessSingle );
			}
			
			//	���������
			g_stCfg.stCfgData[ uOfflineIndex ].uExceptedProcessSingleCount = 0;
		}

		//	����
		procconfig_unlock( uOfflineIndex );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_clean_all_offline_config\n" ));
	}	


	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
//	Private

//	���ܳ����ַ���
VOID procconfig_decrypt_string()
{
	__try
	{
		//	"GET /"
		RtlMoveMemory( g_szDecryptedString_GET, g_szEncryptedString_GET, sizeof(g_szEncryptedString_GET) );
		drvfunc_xorenc( g_szDecryptedString_GET, 0XFF );
		
		//	"POST /"
		RtlMoveMemory( g_szDecryptedString_POST, g_szEncryptedString_POST, sizeof(g_szEncryptedString_POST) );
		drvfunc_xorenc( g_szDecryptedString_POST, 0XFF );
		
		//	"Host: "
		RtlMoveMemory( g_szDecryptedString_Host1, g_szEncryptedString_Host1, sizeof(g_szEncryptedString_Host1) );
		drvfunc_xorenc( g_szDecryptedString_Host1, 0XFF );
		
		//	"Host:"
		RtlMoveMemory( g_szDecryptedString_Host2, g_szEncryptedString_Host2, sizeof(g_szEncryptedString_Host2) );
		drvfunc_xorenc( g_szDecryptedString_Host2, 0XFF );
		
		//	"User-Agent: "
		RtlMoveMemory( g_szDecryptedString_UserAgent1, g_szEncryptedString_UserAgent1, sizeof(g_szEncryptedString_UserAgent1) );
		drvfunc_xorenc( g_szDecryptedString_UserAgent1, 0XFF );
		
		//	"User-Agent:"
		RtlMoveMemory( g_szDecryptedString_UserAgent2, g_szEncryptedString_UserAgent2, sizeof(g_szEncryptedString_UserAgent2) );
		drvfunc_xorenc( g_szDecryptedString_UserAgent2, 0XFF );

		//	"VwPanDmNewCfg"
		RtlMoveMemory( g_szDecryptedString_VwPanDmNewCfg, g_szEncryptedString_VwPanDmNewCfg, sizeof(g_szEncryptedString_VwPanDmNewCfg) );
		drvfunc_xorenc( g_szDecryptedString_VwPanDmNewCfg, 0XFF );

		//	"reg_ip="
		RtlMoveMemory( g_szDecryptedString_reg_ip_, g_szEncryptedString_reg_ip_, sizeof(g_szEncryptedString_reg_ip_) );
		drvfunc_xorenc( g_szDecryptedString_reg_ip_, 0XFF );
		
		//	"reg_key="
		RtlMoveMemory( g_szDecryptedString_reg_key_, g_szEncryptedString_reg_key_, sizeof(g_szEncryptedString_reg_key_) );
		drvfunc_xorenc( g_szDecryptedString_reg_key_, 0XFF );
		
		//	"reg_prtype="
		RtlMoveMemory( g_szDecryptedString_reg_prtype_, g_szEncryptedString_reg_prtype_, sizeof(g_szEncryptedString_reg_prtype_) );
		drvfunc_xorenc( g_szDecryptedString_reg_prtype_, 0XFF );

		//	"reg_prtypecs="
		RtlMoveMemory( g_szDecryptedString_reg_prtypecs_, g_szEncryptedString_reg_prtypecs_, sizeof(g_szEncryptedString_reg_prtypecs_) );
		drvfunc_xorenc( g_szDecryptedString_reg_prtypecs_, 0XFF );

		//
		//	for prtype format
		//	"%s-vwprt-%s-%s"
		RtlMoveMemory( g_szDecryptedString_PrTypeChecksumFmt, g_szEncryptedString_PrTypeChecksumFmt, sizeof(g_szEncryptedString_PrTypeChecksumFmt) );
		drvfunc_xorenc( g_szDecryptedString_PrTypeChecksumFmt, 0XFF );

		//
		//	for prtype
		//
		//	"comadv"
		RtlMoveMemory( g_szDecryptedString_PrType_comadv, g_szEncryptedString_PrType_comadv, sizeof(g_szEncryptedString_PrType_comadv) );
		drvfunc_xorenc( g_szDecryptedString_PrType_comadv, 0XFF );

		//	"comnormal"
		RtlMoveMemory( g_szDecryptedString_PrType_comnormal, g_szEncryptedString_PrType_comnormal, sizeof(g_szEncryptedString_PrType_comnormal) );
		drvfunc_xorenc( g_szDecryptedString_PrType_comnormal, 0XFF );

		//	"pslunlmt"
		RtlMoveMemory( g_szDecryptedString_PrType_pslunlmt, g_szEncryptedString_PrType_pslunlmt, sizeof(g_szEncryptedString_PrType_pslunlmt) );
		drvfunc_xorenc( g_szDecryptedString_PrType_pslunlmt, 0XFF );

		//	"psladv"
		RtlMoveMemory( g_szDecryptedString_PrType_psladv, g_szEncryptedString_PrType_psladv, sizeof(g_szEncryptedString_PrType_psladv) );
		drvfunc_xorenc( g_szDecryptedString_PrType_psladv, 0XFF );

		//	"pslnormal"
		RtlMoveMemory( g_szDecryptedString_PrType_pslnormal, g_szEncryptedString_PrType_pslnormal, sizeof(g_szEncryptedString_PrType_pslnormal) );
		drvfunc_xorenc( g_szDecryptedString_PrType_pslnormal, 0XFF );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

UINT procconfig_stconfig_get_online_index()
{
	return g_stCfg.uCurrIndex;
}
UINT procconfig_stconfig_get_offline_index()
{
	//	return 0 or 1
	return ( ( g_stCfg.uCurrIndex + 1 ) % 2 );
}
VOID procconfig_stconfig_switch_index()
{
	UINT uOfflineIndex;

	__try
	{
		uOfflineIndex = procconfig_stconfig_get_offline_index();
		g_stCfg.uCurrIndex = uOfflineIndex;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_stconfig_switch_index\n" ));
	}	
}


/**
 *	@ Private
 *	���ݲ�Ʒ���ͣ����������Ҫװ�ز��������ٸ�������
 */
UINT procconfig_get_max_parsed_count()
{
	UINT uRet	= 3;

	__try
	{
		if ( g_stCfg.bReged && g_stCfg.bValidPrType )
		{
			if ( 0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_pslnormal ) )
			{
				//	��ͨ���˰汾����ܽ��� 50
				uRet = 48;
				uRet ++;
			}
			else if ( 0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_psladv ) ||
				0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_pslunlmt ) )
			{
				//	���߼����˰桱�͡�������˰桱
				uRet = 98;
				uRet ++;
			}
			else if ( 0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_comnormal ) )
			{
				//	����ͨ��˾�桱
				uRet = 498;
				uRet ++;
			}
		}
		else
		{
			//	"��������汾ֻ������ 2 ����������Ҫ�������෺�����빺����ʽ�汾��"
			uRet = 1;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_get_max_parsed_count\n" ));
	}	

	return ( uRet + 1 );
}

BOOLEAN procconfig_load_allconfig_from_file( LPCWSTR lpcwszConfigFile )
{
	BOOLEAN bRet;
	NTSTATUS ntStatus;
	DWORD dwFileSize;
	CHAR * pszBuffer;
	DWORD dwBuffSize;
	CHAR * pszHead;
	DWORD  dwLineSize;
	CHAR * pszLine;
	CHAR * pszTemp;
	CHAR * pszDrive;	//	szDrive[3] = TEXT(" :");
	STDRIVERINFO * pstDriverInfo;

	STDOMAIN * pstNewDomain;
	STSYSHOST * pstNewSysHost;
	STRDPCLIENTNAME * pstNewRdpClientName;
	STPROTECTEDDIR * pstNewProtectedDir;
	STEXCEPTEDPROCESS * pstNewExceptedProcess;

	UINT i;
	


	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		if ( NULL == lpcwszConfigFile )
		{
			return FALSE;
		}
		if ( 0 == wcslen( lpcwszConfigFile ) )
		{
			return FALSE;
		}
		if ( ! drvfunc_is_file_exist( lpcwszConfigFile ) )
		{
			return FALSE;
		}

		//	...
		dwFileSize = drvfunc_get_filesize( lpcwszConfigFile );
		if ( 0 == dwFileSize )
		{
			//	���ļ�Ҳ��Ϊ�ǳɹ�
			return TRUE;
		}
		if ( dwFileSize >= PROCCONFIG_MAX_CONFIGFILESIZE )
		{
			return FALSE;
		}

		//
		//	��ʼ������ֵ
		//
		bRet = FALSE;

		//
		//	�����ڴ�
		//
		pstNewDomain		= (LPSTDOMAIN)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolDomain );
		pstNewSysHost		= (LPSTSYSHOST)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolSysHost );
		pstNewRdpClientName	= (LPSTRDPCLIENTNAME)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolRdpClientName );
		pstNewProtectedDir	= (LPSTPROTECTEDDIR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolProtectedDir );
		pstNewExceptedProcess	= (LPSTEXCEPTEDPROCESS)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess );

		dwLineSize	= 1024;
		pszLine		= (CHAR*)ExAllocatePool( NonPagedPool, dwLineSize );
		pszTemp		= (CHAR*)ExAllocatePool( NonPagedPool, MAX_PATH );
		pszDrive	= (CHAR*)ExAllocatePool( NonPagedPool, 3 );
		pstDriverInfo	= (STDRIVERINFO*)ExAllocatePool( NonPagedPool, sizeof(STDRIVERINFO) );

		dwBuffSize	= dwFileSize + sizeof(CHAR);
		pszBuffer	= (CHAR*)ExAllocatePool( NonPagedPool, dwBuffSize );

		if ( pstNewDomain && pstNewSysHost && pstNewRdpClientName && pstNewProtectedDir && pstNewExceptedProcess &&
			pszLine && pszTemp && pszDrive && pstDriverInfo &&
			pszBuffer )
		{
			RtlZeroMemory( pstNewDomain, sizeof(STDOMAIN) );
			RtlZeroMemory( pstNewSysHost, sizeof(STSYSHOST) );
			RtlZeroMemory( pstNewRdpClientName, sizeof(STRDPCLIENTNAME) );
			RtlZeroMemory( pstNewProtectedDir, sizeof(STPROTECTEDDIR) );
			RtlZeroMemory( pstNewExceptedProcess, sizeof(STEXCEPTEDPROCESS) );

			RtlZeroMemory( pszLine, dwLineSize );
			RtlZeroMemory( pszTemp, MAX_PATH );
			RtlZeroMemory( pszDrive, 3 );
			RtlZeroMemory( pstDriverInfo, sizeof(STDRIVERINFO) );
			RtlZeroMemory( pszBuffer, dwBuffSize );

			//	��ȡ���������ļ�������
			if ( drvfunc_get_filecontent( lpcwszConfigFile, pszBuffer, dwBuffSize ) )
			{
				//	����ֵ
				bRet = TRUE;

				//
				//	��ȡ "start=" ������
				//
				g_stCfg.bStart = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START );

				//
				//	��ȡ "startprdp=" ������
				//
				g_stCfg.bStartProtectRdp = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START_PRDP );

				//
				//	��ȡ "startpdomain=" ������
				//
				g_stCfg.bStartProtectDomain = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START_PDOMAIN );

				//
				//	��ȡ "startfilesafe=" ������
				//
				g_stCfg.bStartFileSafe = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START_FILESAFE );

				//
				//	��ȡ AppInfo
				//	Ŀ¼�ļ����ƣ�L"\\??\\C:\\DeAntiHack.log"
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, PROCCONFIG_CFGKEY_APP_INSDIR, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					wcscpy( g_stCfg.wszAppDir, L"\\??\\" );
					drvfunc_c2w( pszLine, g_stCfg.wszAppDir+4, sizeof(g_stCfg.wszAppDir)-8 );
					g_stCfg.uAppDirLength = wcslen( g_stCfg.wszAppDir );
				}
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, PROCCONFIG_CFGKEY_APP_LOGDIR, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					wcscpy( g_stCfg.wszLogDir, L"\\??\\" );
					drvfunc_c2w( pszLine, g_stCfg.wszLogDir+4, sizeof(g_stCfg.wszLogDir)-8 );
					g_stCfg.uLogDirLength = wcslen( g_stCfg.wszLogDir );
				}


				//
				//	��ȡ "reg_ip=" ������
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_ip_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegIp, sizeof(g_stCfg.szRegIp)/sizeof(CHAR)-1, "%s", pszLine );
				}

				//
				//	��ȡ "reg_key=" ������
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_key_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegKey, sizeof(g_stCfg.szRegKey)/sizeof(CHAR)-1, "%s", pszLine );
				}
				
				//
				//	��ȡ "reg_prtype=" ������
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_prtype_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegPrType, sizeof(g_stCfg.szRegPrType)/sizeof(CHAR)-1, "%s", pszLine );
				}

				//
				//	��ȡ "reg_prtypecs=" ������
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_prtypecs_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegPrTypeChecksum, sizeof(g_stCfg.szRegPrTypeChecksum)/sizeof(CHAR)-1, "%s", pszLine );
				}

				//
				//	��ȡ "domain=..." ������
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_DOMAIN, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewDomain, sizeof(STDOMAIN) );

						//	��ȡ����
						drvfunc_get_casecookie_value
							(
								pszLine, PROCCONFIG_CFGVALNAME_DM, ";",
								pstNewDomain->szDomain, sizeof(pstNewDomain->szDomain), TRUE
							);
						pstNewDomain->uDomainLength = (USHORT)strlen(pstNewDomain->szDomain);
						if ( pstNewDomain->uDomainLength )
						{
							//	Ŀ¼��ͳͳСд
							_strlwr( pstNewDomain->szDomain );

							//	��ȡ�Ƿ�ʹ��
							pstNewDomain->bUse = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );

							//	��ȡ�Ƿ�ʹ��
							pstNewDomain->bWildcard = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_WCD, ";" );

							//
							//	��ӵ�������
							//
							if ( pstNewDomain->bUse )
							{
								procconfig_addnew_domain( pstNewDomain );
							}
						}
					}

					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}


				//
				//	��ȡ "rdpclientname=..." ������
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_RDP_CLIENTNAME, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewRdpClientName, sizeof(STRDPCLIENTNAME) );

						//	��ȡ ClientName
						RtlZeroMemory( pszTemp, MAX_PATH );
						drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_ITEM, ";", pszTemp, MAX_PATH, TRUE );
						if ( strlen(pszTemp) )
						{
							ntStatus = drvfunc_c2w( pszTemp, pstNewRdpClientName->wszClientName, sizeof(pstNewRdpClientName->wszClientName) );
							if ( NT_SUCCESS(ntStatus) )
							{
								pstNewRdpClientName->uLength = (USHORT)( wcslen( pstNewRdpClientName->wszClientName ) * sizeof(WCHAR) );
								if ( pstNewRdpClientName->uLength )
								{
									_wcslwr( pstNewRdpClientName->wszClientName );

									//	��ȡ�Ƿ�ʹ��
									pstNewRdpClientName->bUse = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );

									//
									//	��ӵ�������
									//
									if ( pstNewRdpClientName->bUse )
									{
										procconfig_addnew_rdp_clientname( pstNewRdpClientName );
									}
								}
							}
						}
					}

					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}//	end of reading rdpclientname


				//
				//	[ProtectedDir]
				//	��ȡ "protecteddir=..." ������
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_PROTECTEDDIR, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewProtectedDir, sizeof(STPROTECTEDDIR) );

						//	get dir
						RtlZeroMemory( pszTemp, MAX_PATH );
						drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_DIR, ";", pszTemp, MAX_PATH, TRUE );
						pstNewProtectedDir->uDirLength = (USHORT)strlen(pszTemp);
						if ( pstNewProtectedDir->uDirLength )
						{
							//	ȥ������ġ�/�����ߡ�\\��
							if ( '\\' == pszTemp[ pstNewProtectedDir->uDirLength - 1 ] ||
								'/' == pszTemp[ pstNewProtectedDir->uDirLength - 1 ] )
							{
								pszTemp[ pstNewProtectedDir->uDirLength - 1 ] = 0;
								pstNewProtectedDir->uDirLength = (USHORT)strlen(pszTemp);
							}
							//	Ŀ¼��ͳͳСд
							_strlwr( pszTemp );

							//	convert to wchar
							drvfunc_c2w( pszTemp, pstNewProtectedDir->wszDir, sizeof(pstNewProtectedDir->wszDir) );
							pstNewProtectedDir->uDirLength = (USHORT)( wcslen( pstNewProtectedDir->wszDir ) * sizeof(WCHAR) );
							pstNewProtectedDir->ulDirCrc32 = crc32_autolwr_w( pstNewProtectedDir->wszDir, pstNewProtectedDir->uDirLength );


							//	��ȡ�Ƿ�ʹ��
							pstNewProtectedDir->bUse = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );

							
							//	get "tree:"
							pstNewProtectedDir->bProtSubDir = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_TREE, ";" );
							
							//	�Ƿ񱣻�������չ�� "allext:"
							pstNewProtectedDir->bProtAllExt = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_ALLEXT, ";" );
							
							//	get protected "protext:"
							RtlZeroMemory( pszTemp, MAX_PATH );
							drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_PROTEXT, ";", pszTemp, MAX_PATH, TRUE );
							pstNewProtectedDir->uProtectedExtCount = procconfig_split_string_to_array
									(
										pszTemp,
										'.',
										pstNewProtectedDir->uszProtectedExt,
										sizeof(pstNewProtectedDir->uszProtectedExt)/sizeof(pstNewProtectedDir->uszProtectedExt[0])
									);
							//	convert uchar to wchar
							RtlZeroMemory( pstNewProtectedDir->wszProtectedExt, sizeof(pstNewProtectedDir->wszProtectedExt) );
							for ( i = 0; i < pstNewProtectedDir->uProtectedExtCount; i ++ )
							{
								drvfunc_c2w
								(
									pstNewProtectedDir->uszProtectedExt[ i ],
									pstNewProtectedDir->wszProtectedExt[ i ],
									sizeof( pstNewProtectedDir->wszProtectedExt[ i ] )
								);
							}

							//	get excepted "ecpext:" 
							RtlZeroMemory( pszTemp, MAX_PATH );
							drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_ECPEXT, ";", pszTemp, MAX_PATH, TRUE );
							pstNewProtectedDir->uExceptedExtCount = procconfig_split_string_to_array
									(
										pszTemp,
										'.',
										pstNewProtectedDir->uszExceptedExt,
										sizeof(pstNewProtectedDir->uszExceptedExt)/sizeof(pstNewProtectedDir->uszExceptedExt[0])
									);
							//	convert uchar to wchar
							RtlZeroMemory( pstNewProtectedDir->wszExceptedExt, sizeof(pstNewProtectedDir->wszExceptedExt) );
							for ( i = 0; i < pstNewProtectedDir->uExceptedExtCount; i ++ )
							{
								drvfunc_c2w
								(
									pstNewProtectedDir->uszExceptedExt[ i ],
									pstNewProtectedDir->wszExceptedExt[ i ],
									sizeof( pstNewProtectedDir->wszExceptedExt[ i ] )
								);
							}

							//
							//	��ӵ�������
							//
							if ( pstNewProtectedDir->bUse )
							{
								procconfig_addnew_protected_dir( pstNewProtectedDir );
							}
						}
					}
					
					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}//	end of reading protecteddir


				//
				//	[ExceptedProcess]
				//	��ȡ "exceptedprocess=..." ������
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_EXCEPTEDPROCESS, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewExceptedProcess, sizeof(STEXCEPTEDPROCESS) );
						
						//	get dir
						drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_ITEM, ";", pstNewExceptedProcess->uszDosPath, sizeof(pstNewExceptedProcess->uszDosPath), TRUE );
						pstNewExceptedProcess->uDosPathLength = (USHORT)strlen(pstNewExceptedProcess->uszDosPath);
						if ( pstNewExceptedProcess->uDosPathLength > 2 )
						{
							//	Ŀ¼��ͳͳСд
							_strlwr( pstNewExceptedProcess->uszDosPath );
							pstNewExceptedProcess->ulDosPathCrc32	= crc32( pstNewExceptedProcess->uszDosPath, pstNewExceptedProcess->uDosPathLength );

							//	��ȡ�Ƿ�ʹ��
							pstNewExceptedProcess->bUse	= drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );
							
							if ( pstNewExceptedProcess->bUse )
							{
								//	make device volume path
								RtlZeroMemory( pstDriverInfo, sizeof(STDRIVERINFO) );

								RtlZeroMemory( pszDrive, 3 );
								RtlZeroMemory( pszTemp, MAX_PATH );
								RtlMoveMemory( pstDriverInfo->uszDosDriverLetter, pstNewExceptedProcess->uszDosPath, 2 );

								if ( drvfunc_query_device_name( pstDriverInfo ) )
								{
									//	join the string "\Device\HarddiskVolume1" and "\Windows\explorer.exe"
									//	pstNewExceptedProcess->uszDevicePath like "\Device\HarddiskVolume1\windows\explorer.exe"
									_snprintf( pstNewExceptedProcess->uszDevicePath, sizeof(pstNewExceptedProcess->uszDevicePath)/sizeof(UCHAR)-1,
										"%s%s", pstDriverInfo->uszDeviceName, pstNewExceptedProcess->uszDosPath + 2 );
									pstNewExceptedProcess->uDevicePathLengthInBytesU = (USHORT)strlen(pstNewExceptedProcess->uszDevicePath);
									_strlwr( pstNewExceptedProcess->uszDevicePath );
									pstNewExceptedProcess->ulDevicePathCrc32U = crc32_autolwr_a( pstNewExceptedProcess->uszDevicePath, pstNewExceptedProcess->uDevicePathLengthInBytesU );

									ntStatus = drvfunc_c2w( pstNewExceptedProcess->uszDevicePath, pstNewExceptedProcess->wszDevicePath, sizeof(pstNewExceptedProcess->wszDevicePath) );
									if ( NT_SUCCESS( ntStatus ) )
									{
										pstNewExceptedProcess->uDevicePathLengthInBytesW = wcslen(pstNewExceptedProcess->wszDevicePath) * sizeof(WCHAR);
										//
										//	��ӵ�������
										//
										procconfig_addnew_excepted_process( pstNewExceptedProcess );
									}
								}
							}
						}
					}
					
					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}//	end of reading exceptedprocess
			}

			//
			//	free memory
			//
			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolDomain, pstNewDomain );
			pstNewDomain = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolSysHost, pstNewSysHost );
			pstNewSysHost = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolRdpClientName, pstNewRdpClientName );
			pstNewRdpClientName = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, pstNewProtectedDir );
			pstNewProtectedDir = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess, pstNewExceptedProcess );
			pstNewExceptedProcess = NULL;

			ExFreePool( pszLine );
			pszLine = NULL;

			ExFreePool( pszTemp );
			pszTemp = NULL;

			ExFreePool( pszDrive );
			pszDrive = NULL;

			ExFreePool( pstDriverInfo );
			pstDriverInfo = NULL;

			ExFreePool( pszBuffer );
			pszBuffer = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_load_allconfig_from_file\n" ));
	}	

	return bRet;
}


/**
 *	��������� ����������
 */
BOOLEAN procconfig_addnew_domain( LPSTDOMAIN pstDomain )
{
	//
	//	pstDomain		- [in] ������Ŀ¼��������Ϣ
	//	RETURN			- TRUE / FLASE
	//	ע�⣺
	//		������ò�����Զ�������� offline ������
	//		���Բ���Ҫ����
	//

	BOOLEAN  bRet;
	STDOMAIN * pstNewItem;
	UINT  uOfflineIndex;
	
	BOOLEAN bExistItem;
	STDOMAIN * pstDataTemp;
	STCONFIGDATA * pstOfflineCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstDomain )
	{
		return FALSE;
	}

	//
	//	����ֵ��ʼ��
	//
	bRet = FALSE;

	//
	//	��� offline ��������
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();


	//	����
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	�жϸ�Ԫ���Ƿ����
		//
		bExistItem		= FALSE;
		pstOfflineCfgData	= & g_stCfg.stCfgData[ uOfflineIndex ];

		if ( pstOfflineCfgData && 
			pstOfflineCfgData->linkDomainSingle.Next )
		{
			//
			//	ȷ���Ѿ���������
			//

			pLinkMove = pstOfflineCfgData->linkDomainSingle.Next;
			while( pLinkMove )
			{
				//
				//	����������: .chinapig.cn ע��ǰ������е�
				//
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STDOMAIN, ListEntry );

				//
				//	�Ӻ�����ǰ��ƥ��
				//	����Խ��������Խ�����������������ǰ��
				//
				if ( pstDataTemp &&
					pstDomain->uDomainLength == pstDataTemp->uDomainLength &&
					0 == _strnicmp( pstDomain->szDomain, pstDataTemp->szDomain, pstDataTemp->uDomainLength ) )
				{
					bExistItem = TRUE;
					break;
				}
				
				pLinkMove = pLinkMove->Next;
			}
		}

		//
		//	########################################
		//	���ݲ�Ʒ���͡��Ƿ���ʽ�汾�Ĳ�ͬ
		//	�������װ�ض��ٸ�������������
		//
		//if ( pstOfflineCfgData->uDomainSingleCount > procconfig_get_max_parsed_count() )
		//{
		//	bExistItem = TRUE;
		//}

		if ( ! bExistItem )
		{
			pstNewItem = (LPSTDOMAIN)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolDomain );
			if ( pstNewItem )
			{
				//	����
				RtlMoveMemory( pstNewItem, pstDomain, sizeof(STDOMAIN) );

				//
				//	��������
				//	PushEntryList sets ListHead->Next to Entry,
				//	and Entry->Next to point to the old first entry of the list.			
				//
				//	insert into tree ptotected dir
				//
				PushEntryList( & pstOfflineCfgData->linkDomainSingle, &pstNewItem->ListEntry );
				pstOfflineCfgData->uDomainSingleCount ++;

				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_domain\n" ));
	}

	//	����
	procconfig_unlock( uOfflineIndex );

	return bRet;
}


/**
 *	����� RDP ClientName ����������
 */
BOOLEAN procconfig_addnew_rdp_clientname( LPSTRDPCLIENTNAME pstRdpClientName )
{
	//
	//	pstDomain		- [in] ������Ŀ¼��������Ϣ
	//	RETURN			- TRUE / FLASE
	//	ע�⣺
	//		������ò�����Զ�������� offline ������
	//		���Բ���Ҫ����
	//

	BOOLEAN  bRet;
	STRDPCLIENTNAME * pstNewItem;
	UINT  uOfflineIndex;

	BOOLEAN bExistItem;
	STRDPCLIENTNAME * pstDataTemp;
	STCONFIGDATA * pstOfflineCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstRdpClientName )
	{
		return FALSE;
	}

	//
	//	����ֵ��ʼ��
	//
	bRet = FALSE;

	//
	//	��� offline ��������
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();


	//	����
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	�жϸ�Ԫ���Ƿ����
		//
		bExistItem		= FALSE;
		pstOfflineCfgData	= & g_stCfg.stCfgData[ uOfflineIndex ];

		if ( pstOfflineCfgData && 
			pstOfflineCfgData->linkRdpClientNameSingle.Next )
		{
			//
			//	ȷ���Ѿ���������
			//

			pLinkMove = pstOfflineCfgData->linkRdpClientNameSingle.Next;
			while( pLinkMove )
			{
				//
				//	����������: .chinapig.cn ע��ǰ������е�
				//
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STRDPCLIENTNAME, ListEntry );

				//
				//	�Ӻ�����ǰ��ƥ��
				//	����Խ��������Խ�����������������ǰ��
				//
				if ( pstDataTemp &&
					pstRdpClientName->uLength == pstDataTemp->uLength &&
					0 == _wcsnicmp( pstRdpClientName->wszClientName, pstDataTemp->wszClientName, pstDataTemp->uLength/sizeof(WCHAR) ) )
				{
					bExistItem = TRUE;
					break;
				}

				pLinkMove = pLinkMove->Next;
			}
		}

		if ( ! bExistItem )
		{
			pstNewItem = (LPSTRDPCLIENTNAME)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolRdpClientName );
			if ( pstNewItem )
			{
				//	����
				RtlMoveMemory( pstNewItem, pstRdpClientName, sizeof(STRDPCLIENTNAME) );

				//
				//	��������
				//	PushEntryList sets ListHead->Next to Entry,
				//	and Entry->Next to point to the old first entry of the list.			
				//
				//	insert into tree ptotected dir
				//
				PushEntryList( & pstOfflineCfgData->linkRdpClientNameSingle, &pstNewItem->ListEntry );
				pstOfflineCfgData->uRdpClientNameSingleCount ++;

				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_rdp_clientname\n" ));
	}

	//	����
	procconfig_unlock( uOfflineIndex );

	return bRet;
}


/**
 *	@ Private
 *	�Ƿ�����ڴ����������б���
 */
LPSTDOMAIN procconfig_get_matched_domain( LPCSTR lpcszOrgDomain, UINT uOrgDomainLength )
{
	//
	//	lpcszOrgDomain		- [in] HTTP ���޸�ǰ��������ԭʼ���������磺abc.chinapig.cn
	//	uOrgDomainLength	- [in] ԭʼ�����ĳ���
	//	RETURN			- ָ��ȫ��������Ϣ��ƥ���ϵĽڵ�ָ�� ���� NULL
	//
	STDOMAIN * pstRet;

	STDOMAIN * pstDataTemp;
	STCONFIGDATA * pstCurrCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;


	if ( NULL == lpcszOrgDomain )
	{
		return NULL;
	}
	if ( 0 == uOrgDomainLength || uOrgDomainLength >= MAX_PATH )
	{
		return NULL;
	}


	//	����ֵ
	pstRet	= NULL;


	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData && 
			pstCurrCfgData->linkDomainSingle.Next )
		{
			//
			//	��ѯ�����Ƿ����б���
			//
			pLinkMove = pstCurrCfgData->linkDomainSingle.Next;
			while( pLinkMove )
			{
				//
				//	���룺abc.chinapig.cn
				//	���ã�   .chinapig.cn
				//
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STDOMAIN, ListEntry );

				//
				//	�Ӻ�����ǰ��ƥ��
				//	����Խ��������Խ�����������������ǰ��
				//
				if ( pstDataTemp &&
					uOrgDomainLength >= pstDataTemp->uDomainLength &&
					0 == _strnicmp( lpcszOrgDomain + ( uOrgDomainLength - pstDataTemp->uDomainLength ),
							pstDataTemp->szDomain,
							pstDataTemp->uDomainLength )
				)
				{
					pstRet = pstDataTemp;
					break;
				}

				pLinkMove = pLinkMove->Next;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_get_matched_domain\n" ));
	}	

	return pstRet;
}

/**
 *	@ Private
 *	��� RDP clientname �Ƿ�����ڰ�����
 */
BOOLEAN procconfig_is_exist_rdp_clientname( LPCWSTR lpcwszClientName, UINT uLength )
{
	//
	//	lpcwszClientName	- [in] ClientName
	//	uLength			- [in] ԭʼ�����ĳ���
	//	RETURN			- TRUE / FALSE
	//
	BOOLEAN bRet;
	STRDPCLIENTNAME * pstDataTemp;
	STCONFIGDATA * pstCurrCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;


	if ( NULL == lpcwszClientName )
	{
		return FALSE;
	}
	if ( 0 == uLength || uLength >= 16 )
	{
		return FALSE;
	}


	//	����ֵ
	bRet = FALSE;

	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData )
		{
			if ( pstCurrCfgData->uRdpClientNameSingleCount > 0 )
			{
				if ( pstCurrCfgData->linkRdpClientNameSingle.Next )
				{
					//
					//	��ѯ�����Ƿ����б���
					//
					pLinkMove = pstCurrCfgData->linkRdpClientNameSingle.Next;
					while( pLinkMove )
					{
						pstDataTemp = CONTAINING_RECORD( pLinkMove, STRDPCLIENTNAME, ListEntry );

						//
						//	�Ӻ�����ǰ��ƥ��
						//	����Խ��������Խ�����������������ǰ��
						//
						if ( pstDataTemp &&
							uLength == pstDataTemp->uLength &&
							0 == _wcsnicmp( lpcwszClientName, pstDataTemp->wszClientName, pstDataTemp->uLength/sizeof(WCHAR) ) )
						{
							bRet = TRUE;
							break;
						}

						pLinkMove = pLinkMove->Next;
					}
				}
				else
				{
					//	������ �û�δ����κμ���������Ź�
					bRet = TRUE;
				}
			}
			else
			{
				//	�û�δ����κμ���������Ź�
				bRet = TRUE;
			}
		}
		else
		{
			//	�������ݿ���󣬷Ź�
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_exist_rdp_clientname\n" ));
	}	

	return bRet;
}


/**
 *	����ĳ���ַ��ָ������ַ������Դ˷ֿ���������
 */
UINT procconfig_split_string_to_array( IN LPCSTR lpcszStringList, IN UCHAR uChrSpliter, OUT BYTE pbtArray[][PROCCONFIG_MAX_EXT_LENGTH], IN DWORD dwArrayCount )
{
	//
	//	lpcszStringList	- [in]  ��չ���б����磺".gif.jpg.bmp."��Ҫ�� lpcszStringList ������ '.' ��β
	//	pbtArray	- [out] Ҫ������������ڴ��ַ��������һ��ָ������ָ���Ǹ� 2 ά������
	//	dwArrayCount	- [in]  ���� 1 ά�ĸ���
	//	RETURN		- �ɹ��ָ�����е���ĸ���
	//

	UINT   uRet;
	LPCSTR lpcszHead;
	LPCSTR lpcszMove;
	UINT   uPosStart;
	UINT   uPosEnd;
	INT    nExtLength;
	UINT   i;
	UCHAR  uChar;

	if ( NULL == lpcszStringList || NULL == pbtArray || 0 == dwArrayCount )
	{
		return 0;
	}
	if ( 0 == lpcszStringList[0] )
	{
		return 0;
	}

	//
	//	��ʼ��
	//
	uRet = 0;

	//
	//	Ҫ�� lpcszStringList ������ '.' ��β
	//
	if ( uChrSpliter == lpcszStringList[ strlen(lpcszStringList) -sizeof(CHAR) ] )
	{
		//	Callers of RtlZeroMemory can be running at any IRQL
		RtlZeroMemory( pbtArray, sizeof(pbtArray[0])*dwArrayCount );
		
		lpcszHead = lpcszStringList;
		while( TRUE )
		{
			lpcszHead = strchr( lpcszHead, uChrSpliter );
			if ( lpcszHead )
			{
				lpcszHead += sizeof(CHAR);
				if ( lpcszHead )
				{
					lpcszMove = strchr( lpcszHead, uChrSpliter );
					if ( lpcszMove )
					{
						//
						//
						//
						uPosStart	= (UINT)( lpcszHead - lpcszStringList );
						uPosEnd		= (UINT)( lpcszMove - lpcszStringList );
						nExtLength	= (INT)( uPosEnd - uPosStart );
						
						if ( nExtLength > 0 && nExtLength < PROCCONFIG_MAX_EXT_LENGTH )
						{
							//
							//	���Ӽ����������ش������չ������
							//
							if ( uRet < dwArrayCount )
							{
								_snprintf( pbtArray[ uRet ], nExtLength, "%s", lpcszStringList + uPosStart );
								uRet ++;
							}
							else
							{
								break;
							}
						}
						
						lpcszHead = lpcszMove;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	return uRet;
}


/**
 *	����µı�����Ŀ¼
 */
BOOLEAN procconfig_addnew_protected_dir( LPSTPROTECTEDDIR pstProtectedDir )
{
	//
	//	pstProtectedDir		- [in] ������Ŀ¼��������Ϣ
	//	RETURN			- TRUE / FLASE
	//	ע�⣺
	//		������ò�����Զ�������� offline ������
	//		���Բ���Ҫ����
	//

	BOOL  bRet;
	STCONFIGDATA * pstOfflineCfgData;
	STPROTECTEDDIR * pstNewItem;
	STPROTECTEDDIR * pstDataTemp;
	SINGLE_LIST_ENTRY * plinkTarget;
	SINGLE_LIST_ENTRY * pLinkMove;
	UINT  * puProtedDirCount;

	UINT  uOfflineIndex;
	BOOL  bAddThis;
	BOOL  bExistDirInDisk;
	BOOL  bExistItem;
	PWCHAR pwszNewDir	= NULL;
	WCHAR  wcDiskLwr;
	NTSTATUS ntStatus;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstProtectedDir )
	{
		return FALSE;
	}

	//
	//	����ֵ��ʼ��
	//
	bRet = FALSE;


	//
	//	��� offline ��������
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();
	
	
	//	����
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	�������������ϣ���Ŀ¼�Ƿ����
		//
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			//	���� drvfunc_is_dir_exist_w ���������� PASSIVE_LEVEL ����
			bExistDirInDisk = drvfunc_is_dir_exist_w( pstProtectedDir->wszDir );
		}
		else
		{
			//	��������� PASSIVE_LEVEL ����Ļ���
			//	����ΪĿ¼���ڰɣ�ʵ��û��ʲô�취
			bExistDirInDisk = TRUE;
		}

		/*
		bExistDirInDisk	= FALSE;
		//	Callers of ExAllocatePool must be executing at IRQL <= DISPATCH_LEVEL
		pwszNewDir = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
		if ( pwszNewDir )
		{
			RtlZeroMemory( pwszNewDir, MAX_WPATH );

			ntStatus = drvfunc_c2w( pstProtectedDir->uszDir, pwszNewDir, MAX_WPATH );
			if ( NT_SUCCESS(ntStatus) )
			{
				bExistDirInDisk = drvfunc_is_dir_exist_w( pwszNewDir );
			}

			ExFreePool( pwszNewDir );
			pwszNewDir = NULL;
		}
		*/

		if ( bExistDirInDisk )
		{
			//
			//	�жϸ�Ԫ���Ƿ����
			//
			pstOfflineCfgData = & g_stCfg.stCfgData[ uOfflineIndex ];
			
			//
			//	���һ�¸ô��̱�������
			//
			wcDiskLwr = pstProtectedDir->wszDir[ 0 ];
			if ( L'A' <= wcDiskLwr && wcDiskLwr <= L'Z' )
			{
				wcDiskLwr += ( L'a' - L'A' );
			}
			pstOfflineCfgData->wszProtedDisk[ wcDiskLwr ]	= 1;

			//
			//	...
			//
			if ( pstProtectedDir->bProtSubDir )
			{
				//	����Ŀ¼������Ŀ¼�µ��ļ�
				plinkTarget = & ( pstOfflineCfgData->linkProtedDirTree );
				puProtedDirCount = & ( pstOfflineCfgData->uProtedDirTreeCount );
			}
			else
			{
				//	��������Ŀ¼���ļ�
				plinkTarget = & ( pstOfflineCfgData->linkProtedDirSingle );
				puProtedDirCount = & ( pstOfflineCfgData->uProtedDirSingleCount );
			}
			if ( plinkTarget && puProtedDirCount )
			{
				bExistItem = FALSE;

				//	...
				pLinkMove = (*plinkTarget).Next;
				while( pLinkMove )
				{
					//
					//	������Ŀ¼
					//
					pstDataTemp = CONTAINING_RECORD( pLinkMove, STPROTECTEDDIR, ListEntry );
					
					//
					//	�Ӻ�����ǰ��ƥ��
					//	����Խ��������Խ�����������������ǰ��
					//
					if ( pstDataTemp &&
						pstProtectedDir->uDirLength == pstDataTemp->uDirLength &&
						0 == _wcsnicmp( pstProtectedDir->wszDir, pstDataTemp->wszDir, pstDataTemp->uDirLength/sizeof(WCHAR) ) )
					{
						bExistItem = TRUE;
						break;
					}
					pLinkMove = pLinkMove->Next;
				}

				if ( ! bExistItem )
				{
					pstNewItem = (LPSTPROTECTEDDIR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolProtectedDir );
					if ( pstNewItem )
					{
						//	����
						RtlMoveMemory( pstNewItem, pstProtectedDir, sizeof(STPROTECTEDDIR) );

						//
						//	��������
						//	PushEntryList sets ListHead->Next to Entry,
						//	and Entry->Next to point to the old first entry of the list.			
						//
						//	insert into tree ptotected dir
						//
						PushEntryList( plinkTarget, &pstNewItem->ListEntry );
						(*puProtedDirCount) ++;

						bRet = TRUE;
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_protected_dir\n" ));
	}

	//	����
	procconfig_unlock( uOfflineIndex );

	//	...
	return bRet;
}

/**
 *	�Ƿ��Ǳ������Ĵ���
 */
BOOLEAN	procconfig_is_protected_disk( WCHAR wszDisk )
{
	BOOLEAN  bRet;
	STCONFIGDATA * pstCurrCfgData;
	WCHAR wcDiskLwr;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( ! procconfig_is_start() )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bStartFileSafe )
	{
		return FALSE;
	}

	//	��ʼ��
	bRet = FALSE;

	//
	//	����
	//
	procconfig_lock( -1 );


	__try
	{
		//
		//	���һ�¸ô��̱�������
		//
		wcDiskLwr = wszDisk;
		if ( L'A' <= wcDiskLwr && wcDiskLwr <= L'Z' )
		{
			wcDiskLwr += ( L'a' - L'A' );
		}

		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData )
		{
			if ( 1 == pstCurrCfgData->wszProtedDisk[ wcDiskLwr ] )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_protected_disk\n" ));
	}

	//
	//	����
	//
	procconfig_unlock( -1 );

	return bRet;
}

/**
 *	�Ƿ��Ǳ�����Ŀ¼
 */
BOOLEAN procconfig_is_protected_dir( IN WCHAR * lpwszFilePath, IN UINT uFilePathLength, IN BOOLEAN bCheckAllParentDirs, IN HANDLE hFileHandle )
{
	//
	//	lpuszFilePath		- [in] �����ļ�����Ҫ��֤�������� DOS ��ʽ�����豸��ʽ������������ "\??\" ��ͷ
	//	uFilePathLength		- [in] lpuszFilePath �ĳ��ȣ��� byte Ϊ��λ
	//	bCheckAllParentDirs	- [in] �Ƿ�Ҳ���������м���Ŀ¼�ļ�⣬Ĭ���� FLASE��ֻ����Ŀ¼�������ж�ʱ�õ�
	//	RETURN			- TRUE / FALSE
	//

	BOOLEAN  bRet;
	STPROTECTEDDIR * pstDataProtectedDir;
	ULONG ulFilePathCrc32;
	ULONG ulDirCrc32;
	STCONFIGDATA * pstCurrCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;
	INT   nDirLen;
	INT   i;
	BOOLEAN bPathIsDir;
	//UCHAR uChar;

	NTSTATUS ntStatus;
	IO_STATUS_BLOCK stIoStatus;
	FILE_STANDARD_INFORMATION stStandardInfo;


	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( ! procconfig_is_start() )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bStartFileSafe )
	{
		return FALSE;
	}
	if ( NULL == lpwszFilePath )
	{
		return FALSE;
	}
	if ( 0 == uFilePathLength || uFilePathLength >= MAX_PATH )
	{
		return FALSE;
	}

	//	��ʼ��
	bRet = FALSE;
	bPathIsDir = FALSE;

	//
	//	����
	//
	procconfig_lock( -1 );


	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData &&
			( pstCurrCfgData->uProtedDirTreeCount > 0 || pstCurrCfgData->uProtedDirSingleCount > 0 ) )
		{
			bPathIsDir = FALSE;
			if ( hFileHandle )
			{
				//
				ntStatus = ZwQueryInformationFile( hFileHandle, &stIoStatus, &stStandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
				if ( NT_SUCCESS( ntStatus ) )
				{
					bPathIsDir = stStandardInfo.Directory;
				}
			}

			//
			//	�ҵ��ļ�ȫ·���У�Ŀ¼�Ľ�����
			//	nDirLen = bytes
			//
			if ( bPathIsDir )
			{
				nDirLen = uFilePathLength;
			}
			else
			{
				nDirLen = drvfunc_get_dir_length_w( lpwszFilePath, uFilePathLength );

				//	�ó��ȵķ�������顰�����·�����Ƿ���Ŀ¼
				bPathIsDir = ( nDirLen == uFilePathLength ? TRUE : FALSE );
			}
			if ( nDirLen > 0 )
			{
				//
				//	��ȡ�Զ�ȡСд����ַ����� Crc32 ��ֵ
				//
				ulDirCrc32 = crc32_autolwr_w( lpwszFilePath, nDirLen );
				if ( bPathIsDir )
				{
					ulFilePathCrc32 = ulDirCrc32;
				}
				else
				{
					ulFilePathCrc32 = crc32_autolwr_w( lpwszFilePath, uFilePathLength );
				}

				if ( procconfig_is_protected_dir_by_cache_crc32( pstCurrCfgData, ulFilePathCrc32 ) )
				{
					//
					//	�� cache ���ҵ��� ��·�� �Ǳ�����Ŀ¼
					//
					bRet = TRUE;
				}
				else
				{
					//
					//	������Ŀ¼�� ���ȴ���
					//
					pLinkMove = pstCurrCfgData->linkProtedDirTree.Next;
					while( pLinkMove )
					{
						pstDataProtectedDir = CONTAINING_RECORD( pLinkMove, STPROTECTEDDIR, ListEntry );
						if ( pstDataProtectedDir )
						{
							if ( nDirLen >= pstDataProtectedDir->uDirLength )
							{
								//
								//	���������£�
								//	lpuszFilePath=
								//	"C:\Inetpub\wwwroot\images"
								//	"C:\Inetpub\wwwroot\"
								//	"C:\Inetpub\wwwroot"
								//
								//	pstDataProtectedDir->uszDir=
								//	"C:\Inetpub\wwwroot"	(����·������ġ�\����ǿ��ȥ��)
								//

								//
								//	���������� ���� �Ǵ����Ŀ¼��������Ŀ¼
								//
								if ( 0 == _wcsnicmp( lpwszFilePath, pstDataProtectedDir->wszDir, pstDataProtectedDir->uDirLength/sizeof(WCHAR) ) )
								{
									if ( bPathIsDir )
									{
										//	�����·����Ŀ¼��û�б�Ҫ�ټ������չ��
										//	��ֱ�ӷ��� TRUE
										bRet = TRUE;
										break;
									}
									else
									{
										//	�����·�����ļ�����Ҫ�����ж�����չ��...
										if ( procconfig_is_protected_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
										{
											//	�Ƿ�������
											if ( ! procconfig_is_excepted_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
											{
												bRet = TRUE;
												break;
											}
										}
									}
								}
							}
							else
							{
								//
								//	���������£�
								//	lpuszFilePath=
								//	"C:\Inetpub\"
								//	"C:\Inetpub"
								//
								//	pstDataProtectedDir->uszDir=
								//	"C:\Inetpub\wwwroot"	(����·������ġ�\����ǿ��ȥ��)
								//

								//
								//	�����Ŀ¼���������ݵ�Ŀ¼��
								//
								if ( bCheckAllParentDirs && bPathIsDir )
								{
									//
									//	��Ҫ��ֹ������Ŀ¼��������
									//
									//	���ָ��Ҫ�������Ŀ¼�����м���Ŀ¼�Ļ���
									//	
									//
									if ( 0 == _wcsnicmp( pstDataProtectedDir->wszDir, lpwszFilePath, nDirLen/sizeof(WCHAR) ) )
									{
										if ( '\\' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] || '/' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] )
										{
											//	lpuszFilePath="C:\Inetpub\"
											bRet = TRUE;
											break;
										}
										else if ( '\\' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] || '/' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] )
										{
											//	lpuszFilePath="C:\Inetpub"
											bRet = TRUE;
											break;
										}
									}
								}
							}
						}

						pLinkMove = pLinkMove->Next;
					}
					
					//
					//	�ڲ��ң���������Ŀ¼��
					//
					if ( ! bRet )
					{
						pLinkMove = pstCurrCfgData->linkProtedDirSingle.Next;
						while( pLinkMove )
						{
							pstDataProtectedDir = CONTAINING_RECORD( pLinkMove, STPROTECTEDDIR, ListEntry );
							if ( pstDataProtectedDir )
							{
								if ( nDirLen == pstDataProtectedDir->uDirLength )
								{
									//
									//	���������£�
									//	lpuszFilePath=
									//	"C:\Inetpub\wwwroot\"
									//	"C:\Inetpub\wwwroot"
									//
									//	pstDataProtectedDir->uszDir=
									//	"C:\Inetpub\wwwroot"	(����·������ġ�\����ǿ��ȥ��)
									//
									
									//
									//	���������� ���� �Ǵ����Ŀ¼
									//
									if ( ulDirCrc32 == pstDataProtectedDir->ulDirCrc32 || 
										0 == _wcsnicmp( pstDataProtectedDir->wszDir, lpwszFilePath, pstDataProtectedDir->uDirLength/sizeof(WCHAR) ) )
									{
										if ( bPathIsDir )
										{
											//	�����·����Ŀ¼��û�б�Ҫ�ټ������չ��
											//	��ֱ�ӷ��� TRUE
											bRet = TRUE;
											break;
										}
										else
										{
											//	�����·�����ļ�����Ҫ�����ж�����չ��...
											if ( procconfig_is_protected_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
											{
												//	�Ƿ�������
												if ( ! procconfig_is_excepted_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
												{
													bRet = TRUE;
													break;
												}
											}
										}
									}
								}
								else if ( nDirLen < pstDataProtectedDir->uDirLength )
								{
									//
									//	���������£�
									//	lpuszFilePath=
									//	"C:\Inetpub\"
									//	"C:\Inetpub"
									//
									//	pstDataProtectedDir->uszDir=
									//	"C:\Inetpub\wwwroot"	(����·������ġ�\����ǿ��ȥ��)
									//
									
									//
									//	�����Ŀ¼���������ݵ�Ŀ¼��
									//
									if ( bCheckAllParentDirs && bPathIsDir )
									{
										//
										//	��Ҫ��ֹ������Ŀ¼��������
										//
										//	���ָ��Ҫ�������Ŀ¼�����м���Ŀ¼�Ļ���
										//	
										//
										if ( 0 == _wcsnicmp( pstDataProtectedDir->wszDir, lpwszFilePath, nDirLen/sizeof(WCHAR) ) )
										{
											if ( '\\' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] || '/' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] )
											{
												//	lpuszFilePath="C:\Inetpub\"
												bRet = TRUE;
												break;
											}
											else if ( '\\' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] || '/' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] )
											{
												//	lpuszFilePath="C:\Inetpub"
												bRet = TRUE;
												break;
											}
										}
									}
								}
							}

							pLinkMove = pLinkMove->Next;
						}
					}

					//
					//	�� ulFilePathCrc32 ��ӵ�������
					//
					procconfig_addnew_protected_dir_cache_crc32( pstCurrCfgData, ulFilePathCrc32, bRet );
				}

			} // end of if ( nDirLen > 0 )
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_protected_dir\n" ));
	}


	//
	//	����
	//
	procconfig_unlock( -1 );


	return bRet;
}


/**
 *	�Ƿ��Ǳ�������չ��
 */
BOOLEAN	procconfig_is_protected_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength )
{
	//
	//	pstProtectedDir		- protected dir
	//	lpcwszFilePath		- wide chars
	//	uFilePathLength		- in bytes
	//
	BOOL  bRet;
	INT   nExtPos;
	INT   nExtCount;
	UINT  i;
	//UCHAR uChar;

	if ( NULL == pstProtectedDir || NULL == lpcwszFilePath || 0 == uFilePathLength )
	{
		return FALSE;
	}

	if ( pstProtectedDir->bProtAllExt )
	{
		//	�������������չ������ô�Ͳ����ж���
		
		//	ֱ�ӷ��� TRUE
		return TRUE;
	}
	else
	{
		//	��������ָ����չ��
		
		if ( 0 == pstProtectedDir->uProtectedExtCount )
		{
			//
			//	��������������
			//	1��δ���ñ���������չ�����
			//	2��ָ����������չ������Ҳ��0
			//	��ôֱ�ӷ��� FALSE
			//
			return FALSE;
		}
	}

	bRet		= FALSE;
	nExtCount	= 0;

	for ( nExtPos = ( uFilePathLength/sizeof(WCHAR) - 1 ); nExtPos >= 0; nExtPos -- )
	{
		if ( '.' == lpcwszFilePath[ nExtPos ] )
		{
			//	���� "." ��λ�ã����Ųһ��λ��
			//	���磺��exe������gif��
			nExtPos ++;

			//	������չ���ĳ��� nExtCount in chars
			nExtCount = uFilePathLength/sizeof(WCHAR) - nExtPos;

			//	����
			break;
		}
	}

	if ( nExtCount > 0 && nExtCount < PROCCONFIG_MAX_EXT_LENGTH )
	{
		for ( i = 0; i < pstProtectedDir->uProtectedExtCount; i ++ )
		{
			if ( 0 == _wcsnicmp( pstProtectedDir->wszProtectedExt[ i ], lpcwszFilePath + nExtPos, nExtCount ) )
			{
				//	���˷��ֵ�ȷ���û����ñ���������չ����
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}



/**
 *	�Ƿ����������չ��
 */
BOOLEAN	procconfig_is_excepted_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength )
{
	BOOL  bRet;
	INT   nExtPos;
	INT   nExtCount;
	UINT  i;
	//UCHAR uChar;

	if ( NULL == pstProtectedDir || NULL == lpcwszFilePath || 0 == uFilePathLength )
	{
		return FALSE;
	}
	if ( 0 == pstProtectedDir->uExceptedExtCount )
	{
		return FALSE;
	}

	bRet		= FALSE;
	nExtCount	= 0;

	for ( nExtPos = ( uFilePathLength/sizeof(WCHAR) - 1 ); nExtPos >= 0; nExtPos -- )
	{
		if ( '.' == lpcwszFilePath[ nExtPos ] )
		{
			//	���� "." ��λ�ã����Ųһ��λ��
			//	���磺��exe������gif��
			nExtPos ++;
			
			//	������չ���ĳ���
			nExtCount = uFilePathLength/sizeof(WCHAR) - nExtPos;
			
			//	����
			break;
		}
	}

	if ( nExtCount > 1 && nExtCount < PROCCONFIG_MAX_EXT_LENGTH )
	{
		for ( i = 0; i < pstProtectedDir->uExceptedExtCount; i ++ )
		{
			if ( 0 == _wcsnicmp( pstProtectedDir->wszExceptedExt[ i ], lpcwszFilePath + nExtPos, nExtCount ) )
			{
				//	���˷��ֵ�ȷ��������Ŷ��
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}




/**
 *	@ Private
 *	����������(������Ŀ¼�� Crc32 ֵ������)
 */
BOOLEAN procconfig_addnew_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32, BOOLEAN bProtectedDir )
{
	BOOLEAN bRet;
	STPROTECTEDDIRCACHE * pstMove;
	STPROTECTEDDIRCACHE * pstLast;
	STPROTECTEDDIRCACHE * pstNewItem;
	BOOLEAN bFoundItem;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( NULL == pstCurrCfgData )
	{
		return FALSE;
	}
	if ( pstCurrCfgData->uProtectedDirCacheCount > PROCCONFIG_MAX_PROTECTEDDIR_CACHE_SIZE )
	{
		//	�������������Ԫ�ظ�������
		return FALSE;
	}

	//	��ʼ��
	bRet = FALSE;
	
	
	__try
	{
		pstMove		= NULL;
		pstLast		= NULL;
		bFoundItem	= FALSE;

		pstMove = pstCurrCfgData->pstProtectedDirCacheHeader;
		while ( pstMove )
		{
			if ( ulCrc32 > pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstLeft;
			}
			else if ( ulCrc32 < pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstRight;
			}
			else if ( ulCrc32 == pstMove->ulCrc32 )
			{
				bFoundItem = TRUE;
				break;
			}
		}

		if ( bFoundItem )
		{
			//	����Ѿ����ڣ�����Ϊ��ӳɹ�
			bRet = TRUE;
		}
		else
		{
			pstNewItem = (STPROTECTEDDIRCACHE*)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache );
			if ( pstNewItem )
			{
				RtlZeroMemory( pstNewItem, sizeof(STPROTECTEDDIRCACHE) );
				
				pstNewItem->ulCrc32		= ulCrc32;
				pstNewItem->bProtectedDir	= bProtectedDir;
				pstNewItem->pstLeft		= NULL;
				pstNewItem->pstRight		= NULL;

				//	�����������Ŀռ����
				if ( NULL == pstCurrCfgData->pstProtectedDirCacheHeader )
				{
					// ������һ�����
					pstCurrCfgData->pstProtectedDirCacheHeader = pstNewItem;
				}
				else
				{
					//	�����������
					if ( ulCrc32 > pstLast->ulCrc32 )
					{
						pstLast->pstLeft = pstNewItem;
					}
					else if ( ulCrc32 < pstLast->ulCrc32 )
					{
						pstLast->pstRight = pstNewItem;
					}
				}

				//
				//	Use following code to free the memony used by cache item
				//	--------------------------------------------------------------------------------
				//	ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache, pstNewItem );
				//	pstNewItem = NULL;
				//

				//	���Ӽ���
				pstCurrCfgData->uProtectedDirCacheCount ++;

				//
				//	��ӳɹ�
				//
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_protected_dir_cache_crc32\n" ));
	}

	return bRet;
}

/**
 *	@ Private
 *	�Ӷ����������м������������Ƿ��Ǳ���������(������Ŀ¼�� Crc32 ֵ������)
 */
BOOLEAN procconfig_is_protected_dir_by_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32 )
{
	BOOLEAN bRet;
	STPROTECTEDDIRCACHE * pstMove;
	STPROTECTEDDIRCACHE * pstLast;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( NULL == pstCurrCfgData )
	{
		return FALSE;
	}
	
	//	��ʼ��
	bRet = FALSE;

	__try
	{
		pstMove		= NULL;
		pstLast		= NULL;

		pstMove = pstCurrCfgData->pstProtectedDirCacheHeader;
		while ( pstMove )
		{
			if ( ulCrc32 > pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstLeft;
			}
			else if ( ulCrc32 < pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstRight;
			}
			else if ( ulCrc32 == pstMove->ulCrc32 )
			{
				//	�Ƿ��Ǳ���������
				bRet = pstMove->bProtectedDir;
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_protected_dir_by_cache_crc32\n" ));
	}

	return bRet;
}
BOOLEAN procconfig_clean_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, STPROTECTEDDIRCACHE * pstProtectedDirCache )
{
	BOOLEAN bRet;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	//if ( procconfig_is_loading() )
	//{
	//	return FALSE;
	//}
	if ( NULL == pstCurrCfgData )
	{
		return FALSE;
	}

	//	��ʼ��
	bRet = FALSE;

	__try
	{
		if ( pstProtectedDirCache )
		{
			//	���� �������Һ���
			procconfig_clean_protected_dir_cache_crc32( pstCurrCfgData, pstProtectedDirCache->pstLeft );
			procconfig_clean_protected_dir_cache_crc32( pstCurrCfgData, pstProtectedDirCache->pstRight );
			
			//	���ټ���
			pstCurrCfgData->uProtectedDirCacheCount --;

			//	�ͷ��ڴ�
			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache, pstProtectedDirCache );
			pstProtectedDirCache = NULL;

			//	...
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_clean_protected_dir_cache_crc32\n" ));
	}
	
	return bRet;
}





/**
 *	����µ��������
 */
BOOLEAN procconfig_addnew_excepted_process( LPSTEXCEPTEDPROCESS pstExceptedProcess )
{
	//
	//	pstExceptedProcess	- [in] ���������Ϣ
	//	RETURN			- TRUE / FLASE
	//	ע�⣺
	//		������ò�����Զ�������� offline ������
	//		���Բ���Ҫ����
	//

	BOOL  bRet;
	STCONFIGDATA * pstOfflineCfgData;
	STEXCEPTEDPROCESS * pstNewItem;
	STEXCEPTEDPROCESS * pstDataTemp;
	//SINGLE_LIST_ENTRY * plinkTarget;
	SINGLE_LIST_ENTRY * pLinkMove;

	BOOL  bExistDirInDisk;

	UINT  uOfflineIndex;
	//BOOL  bAddThis;
	BOOL  bExistItem;
	//PWCHAR pwszNewDir	= NULL;
	//NTSTATUS ntStatus;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstExceptedProcess )
	{
		return FALSE;
	}

	//
	//	����ֵ��ʼ��
	//
	bRet = FALSE;


	//
	//	��� offline ��������
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();
	
	
	//	����
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	�������������ϣ���Ŀ¼�Ƿ����
		//
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			//	���� drvfunc_is_file_exist_a ���������� PASSIVE_LEVEL ����
			bExistDirInDisk = drvfunc_is_file_exist_a( pstExceptedProcess->uszDosPath );
		}
		else
		{
			//	��������� PASSIVE_LEVEL ����Ļ���
			//	����ΪĿ¼���ڰɣ�ʵ��û��ʲô�취
			bExistDirInDisk = TRUE;
		}

		if ( bExistDirInDisk )
		{
			//
			//	�жϸ�Ԫ���Ƿ����
			//
			bExistItem		= FALSE;
			pstOfflineCfgData	= & g_stCfg.stCfgData[ uOfflineIndex ];

			if ( pstOfflineCfgData && 
				pstOfflineCfgData->linkExceptedProcessSingle.Next )
			{
				//
				//	ȷ���Ѿ���������
				//
				
				pLinkMove = pstOfflineCfgData->linkExceptedProcessSingle.Next;
				while( pLinkMove )
				{
					pstDataTemp = CONTAINING_RECORD( pLinkMove, STEXCEPTEDPROCESS, ListEntry );

					//
					//	�Ӻ�����ǰ��ƥ��
					//
					if ( pstDataTemp &&
						pstExceptedProcess->uDosPathLength == pstDataTemp->uDosPathLength &&
						0 == _strnicmp( pstExceptedProcess->uszDosPath, pstDataTemp->uszDosPath, pstDataTemp->uDosPathLength ) )
					{
						bExistItem = TRUE;
						break;
					}
					
					pLinkMove = pLinkMove->Next;
				}
			}

			if ( ! bExistItem )
			{
				pstNewItem = (LPSTEXCEPTEDPROCESS)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess );
				if ( pstNewItem )
				{
					//	����
					RtlMoveMemory( pstNewItem, pstExceptedProcess, sizeof(STEXCEPTEDPROCESS) );
					
					//
					//	��������
					//	PushEntryList sets ListHead->Next to Entry,
					//	and Entry->Next to point to the old first entry of the list.
					//
					PushEntryList( & pstOfflineCfgData->linkExceptedProcessSingle, &pstNewItem->ListEntry );
					pstOfflineCfgData->uExceptedProcessSingleCount ++;
					
					bRet = TRUE;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_excepted_process\n" ));
	}

	//	����
	procconfig_unlock( uOfflineIndex );

	//	...
	return bRet;
}

/**
 *	@ Public for <FsApiHook Action Checking>
 *	check if excepted process for accessing the files
 */
BOOLEAN procconfig_is_excepted_process()
{
	BOOLEAN bRet;
	HANDLE ProcessId;
	ULONG ulProcessIdTmp;
	WCHAR * pwszFullImagePath;
	USHORT uFullImagePathLength;
	ULONG ulFullImagePathCrc32U;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		ProcessId	= PsGetCurrentProcessId();
		ulProcessIdTmp	= 0;
		RtlMoveMemory( &ulProcessIdTmp, &ProcessId, sizeof(ProcessId) );

		if ( procprocess_get_fullimagepath_by_pid( ulProcessIdTmp, &pwszFullImagePath, &uFullImagePathLength, &ulFullImagePathCrc32U ) )
		{
			if ( procconfig_is_excepted_process_by_devicepath( pwszFullImagePath, uFullImagePathLength ) )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_excepted_process\n" ));
	}

	return bRet;
}
BOOLEAN procconfig_is_excepted_process_by_crc32u( ULONG ulFullImagePathCrc32U )
{
	//
	//	ulFullImagePathCrc32U	- [in] crc32 value of uszFullImagePath in UCHAR
	//	RETURN			- TRUE / FALSE
	//

	BOOLEAN bRet;
	STCONFIGDATA * pstCurrCfgData;
	STEXCEPTEDPROCESS * pstDataTemp;
	SINGLE_LIST_ENTRY * pLinkMove;
	
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;
	
	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData && 
			pstCurrCfgData->linkExceptedProcessSingle.Next )
		{
			//
			//	��ѯ�����Ƿ����б���
			//
			pLinkMove = pstCurrCfgData->linkExceptedProcessSingle.Next;
			while( pLinkMove )
			{
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STEXCEPTEDPROCESS, ListEntry );
				
				//
				//	�Ӻ�����ǰ��ƥ��
				//
				if ( pstDataTemp &&
					ulFullImagePathCrc32U == pstDataTemp->ulDevicePathCrc32U )
				{
					bRet = TRUE;
					break;
				}
				
				pLinkMove = pLinkMove->Next;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_excepted_process_by_crc32u\n" ));
	}
	
	return bRet;
}
BOOLEAN procconfig_is_excepted_process_by_devicepath( PWSTR pwszDevicePath, USHORT uLengthInBytes )
{
	//
	//	pwszDevicePath	- [in] like "\Device\HarddiskVolume1\windows\explorer.exe"
	//	uLengthInBytes	- [in] length of the string pointed to by pwszDevicePath, in bytes.
	//	RETURN		- TRUE / FALSE
	//

	BOOLEAN bRet;
	STCONFIGDATA * pstCurrCfgData;
	STEXCEPTEDPROCESS * pstDataTemp;
	SINGLE_LIST_ENTRY * pLinkMove;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pwszDevicePath || 0 == uLengthInBytes )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData && 
			pstCurrCfgData->linkExceptedProcessSingle.Next )
		{
			//
			//	��ѯ�����Ƿ����б���
			//
			pLinkMove = pstCurrCfgData->linkExceptedProcessSingle.Next;
			while( pLinkMove )
			{
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STEXCEPTEDPROCESS, ListEntry );
				
				//
				//	�Ӻ�����ǰ��ƥ��
				//
				if ( pstDataTemp &&
					uLengthInBytes == pstDataTemp->uDevicePathLengthInBytesW &&
					0 == _wcsnicmp( pwszDevicePath, pstDataTemp->wszDevicePath, pstDataTemp->uDevicePathLengthInBytesW/sizeof(WCHAR) ) )
				{
					bRet = TRUE;
					break;
				}
				
				pLinkMove = pLinkMove->Next;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_excepted_process_by_devicepath\n" ));
	}
	
	return bRet;
}
