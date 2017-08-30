// ProcConfig.h: interface for the CProcConfig class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCCONFIG_HEADER__
#define __PROCCONFIG_HEADER__

#include "stdafx.h"
#include "ProcessStruct.h"
#include "ProcProcess.h"


/**
 *	�곣������
 */
#define PROCCONFIG_MAX_EXT_LENGTH		6		//	������չ���ĳ��ȱ���С�� 6
#define PROCCONFIG_MAX_EXT_RANGE		0x100		//	������չ�������ַ��ķ�Χ
#define PROCCONFIG_MAX_CONFIGFILESIZE		102400		//	�����ļ��Ĵ�С����С�� 100K
#define PROCCONFIG_MAX_PROTECTEDDIR_CACHE_SIZE	50000		//	��󻺴�������

#define PROCCONFIG_CFGKEY_START			"start="
#define PROCCONFIG_CFGKEY_START_PRDP		"startprdp="
#define PROCCONFIG_CFGKEY_START_PDOMAIN		"startpdomain="
#define PROCCONFIG_CFGKEY_START_FILESAFE	"startfilesafe="	//	�ļ���ȫ
#define PROCCONFIG_CFGKEY_DOMAIN		"domain="
#define PROCCONFIG_CFGKEY_SYSHOST		"syshost="
#define PROCCONFIG_CFGKEY_RDP_CLIENTNAME	"rdpclientname="
#define PROCCONFIG_CFGKEY_PROTECTEDDIR		"protecteddir="
#define PROCCONFIG_CFGKEY_EXCEPTEDPROCESS	"exceptedprocess="
#define PROCCONFIG_CFGKEY_APP_INSDIR		"app_insdir="
#define PROCCONFIG_CFGKEY_APP_LOGDIR		"app_logdir="

#define PROCCONFIG_CFGKEY_REGIP			"reg_ip="
#define PROCCONFIG_CFGKEY_REGKEY		"reg_key="
#define PROCCONFIG_CFGKEY_REGPRTYPE		"reg_prtype="
#define PROCCONFIG_CFGKEY_REGPRTYPECS		"reg_prtypecs="

#define PROCCONFIG_CFGVALNAME_USE		"use:"
#define PROCCONFIG_CFGVALNAME_ITEM		"item:"
#define PROCCONFIG_CFGVALNAME_DM		"dm:"
#define PROCCONFIG_CFGVALNAME_WCD		"wcd:"		//	Wildcard

#define PROCCONFIG_CFGVALNAME_DIR		"dir:"		//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_TREE		"tree:"		//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_ALLEXT		"allext:"	//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_PROTEXT		"protext:"	//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_ECPEXT		"ecpext:"	//	for [ProtectedDir]


//	����������� ����
typedef struct tagExceptedSubDomain
{
	CHAR    szName[ 32 ];
	USHORT  uNameLength;

}STEXCEPTEDSUBDOMAIN, *LPSTEXCEPTEDSUBDOMAIN;

//	������� ����������
typedef struct tagDomain
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOLEAN bUse;						//	�Ƿ�����
	CHAR    szDomain[ MAX_PATH ];				//	��Ҫ��������������磺.abc.com
	USHORT  uDomainLength;					//	��Ҫ����������ĳ���
	BOOLEAN bWildcard;					//	�Ƿ��Ƿ�����

	STEXCEPTEDSUBDOMAIN stArrExceptedSubDomain[ 32 ];	//	����������
	//CHAR    szExceptedSubDomain[ 32 ][ 32 ];		//	����������
	USHORT  uExceptedSubDomainCount;			//	�����������ĸ���

}STDOMAIN, *LPSTDOMAIN;

typedef struct tagSysHost
{
	SINGLE_LIST_ENTRY ListEntry;

	CHAR    szHost[ 128 ];
	USHORT  uHostLength;

}STSYSHOST, *LPSTSYSHOST;

typedef struct tagRdpClientName
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOLEAN bUse;
	WCHAR   wszClientName[ 16 ];
	USHORT  uLength;

}STRDPCLIENTNAME, *LPSTRDPCLIENTNAME;

//
//	������Ŀ¼����(for ����)
//
typedef struct tagProtectedDir
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOL   bUse;			//	�Ƿ�����
//	UCHAR  uszDir[ MAX_PATH ];	//	������Ŀ¼a
	WCHAR  wszDir[ MAX_PATH ];	//	������Ŀ¼w
	USHORT uDirLength;		//	������Ŀ¼�ĳ���
	ULONG  ulDirCrc32;		//	Ŀ¼�� Crc32 ֵ

	BOOL   bProtSubDir;		//	�Ƿ񱣻�����Ŀ¼

	BOOL   bProtAllExt;		//	�Ƿ�Ҫ�������е���չ����TRUE �ǣ�FALSE ���ǣ��������� uszProtectedExt ��ָ����

	UCHAR  uszProtectedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	����������չ��A
	WCHAR  wszProtectedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	����������չ��W
	UINT   uProtectedExtCount;					//	����������չ���ĸ���

	UCHAR  uszExceptedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	������չ��A
	WCHAR  wszExceptedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	������չ��W
	UINT   uExceptedExtCount;					//	������չ���ĸ���

}STPROTECTEDDIR, *LPSTPROTECTEDDIR;

struct tagProtectedDirCache
{
	ULONG   ulCrc32;
	BOOLEAN bProtectedDir;

	struct tagProtectedDirCache * pstLeft;
	struct tagProtectedDirCache * pstRight;

};
typedef struct tagProtectedDirCache STPROTECTEDDIRCACHE, *LPSTPROTECTEDDIRCACHE;


//
//	����Ľ���
//
typedef struct tagExceptedProcess
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOL   bUse;				//	�Ƿ�����
	UCHAR  uszDosPath[ MAX_PATH ];		//	������̵�·��
	USHORT uDosPathLength;			//	������Ŀ¼�ĳ���
	ULONG  ulDosPathCrc32;			//	Ŀ¼�� Crc32 ֵ

	UCHAR  uszDevicePath[ MAX_PATH ];
	USHORT uDevicePathLengthInBytesU;	//	length of uszDevicePath in bytes
	ULONG  ulDevicePathCrc32U;		//	Crc32 value of uszDevicePath

	WCHAR  wszDevicePath[ MAX_PATH ];	//	�豸���ƣ����硰\Device\HarddiskVolume1\Windows\explorer.exe��
	USHORT uDevicePathLengthInBytesW;	//	length of wszDevicePath in bytes

}STEXCEPTEDPROCESS, *LPSTEXCEPTEDPROCESS;


typedef struct tagConfigData
{
	FAST_MUTEX muxLock;				//	������
	
	SINGLE_LIST_ENTRY linkDomainSingle;		//	��Ҫ���������(single)����
	UINT   uDomainSingleCount;			//	��Ҫ���������(single)����

	SINGLE_LIST_ENTRY linkSysHostSingle;		//	IIS Host(single)����
	UINT   uSysHostSingleCount;			//	IIS Host(single)����

	SINGLE_LIST_ENTRY linkRdpClientNameSingle;	//	RDP ClientName(single)����
	UINT   uRdpClientNameSingleCount;		//	RDP ClientName(single)����

	WCHAR  wszProtedDisk[ 0xFF ];			//	��¼�������Ĵ��̷��ţ��� INDEX 'C' λ�� = 1 ��ʾ�б�����= 0 ��ʾ�ޱ���
	UINT   uProtedDirTotalCount;			//	������Ŀ¼(tree+single)����
	SINGLE_LIST_ENTRY linkProtedDirTree;		//	������Ŀ¼(tree)����
	UINT   uProtedDirTreeCount;			//	������Ŀ¼(tree)����

	//	������Ŀ¼����/����
	STPROTECTEDDIRCACHE * pstProtectedDirCacheHeader;
	UINT   uProtectedDirCacheCount;

	SINGLE_LIST_ENTRY linkProtedDirSingle;		//	������Ŀ¼(single)����
	UINT   uProtedDirSingleCount;			//	������Ŀ¼(single)����

	SINGLE_LIST_ENTRY linkExceptedProcessSingle;	//	����Ľ���(single)����
	UINT   uExceptedProcessSingleCount;		//	����Ľ���(single)����

}STCONFIGDATA, *LPSTCONFIGDATA;

typedef struct tagRegDate
{
	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;
	
	INT nExpireYear;			//	���ڵ���
	INT nExpireMonth;			//	���ڵ���
	INT nExpireDay;				//	���ڵ���
	
}STREGDATE, *LPSTREGDATE;

typedef struct tagConfig
{
	BOOL   bInited;				//	�Ƿ񱻳�ʼ����
	BOOL   bStart;				//	�Ƿ�ʼ�ɻ�
	BOOL   bStartProtectRdp;		//	�������� RDP
	BOOL   bStartProtectDomain;		//	��������������
	BOOL   bStartFileSafe;			//	�����ļ���ȫ
	BOOL   bLoading;			//	�Ƿ�����װ��������Ϣ
	
	//
	BOOL   bReged;				//	�Ƿ���ע��汾
	BOOL   bValidPrType;			//	�Ƿ�����Ч�� PrType
	CHAR   szRegIp[ 32 ];			//	ע�� IP ��ַ
	CHAR   szRegKey[ 64 ];			//	ע����
	CHAR   szRegPrType[ 32 ];		//	��Ʒ����
	CHAR   szRegPrTypeChecksum[ 64 ];	//	��Ʒ���͵�У��ֵ

	//	AppInfo
	WCHAR  wszAppDir[ MAX_PATH ];		//	Ӧ�ó�������Ŀ¼��һ���� "C:\VirtualWall\VwFirewall"
	UINT   uAppDirLength;
	WCHAR  wszLogDir[ MAX_PATH ];		//	��־�ļ�����Ŀ¼��һ���� "C:\VirtualWall\VwFirewall\Logs"
	UINT   uLogDirLength;

	//
	UINT   uCurrIndex;			//	��ǰʹ�õ������������� = 0/1
	STCONFIGDATA stCfgData[ 2 ];		//	����Ϊ���ܶ�̬װ�أ��������������ã�һ�� online����һ�� offline

	//
	DWORD  dwRtspNowDay;			//	���ؽ������ڼ�¼������ÿ��ͳ��
	LONGLONG llProtectFileBlockCount;	//	��������ͳ�ƣ��ļ���
	LONGLONG llProtectDomainBlockCount;	//	��������ͳ�ƣ�����������
	LONGLONG llProtectRdpBlockCount;	//	��������ͳ�ƣ�Զ������

}STCONFIG, *LPSTCONFIG;


/**
 *	extern ȫ�ֱ���
 */
extern STCONFIG g_stCfg;
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolDomain;			//	��Ҫ��������� ��
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolSysHost;		//	��Ҫ��������� ��
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolRdpClientName;		//	RDP �ͻ��˼������ ��
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDir;		//	������Ŀ¼��
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDirCache;	//	������Ŀ¼Cache��
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolExceptedProcess;	//	����Ľ���
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilename;		//	�ļ����ڴ��
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilenameW;		//	�ļ����ڴ��W

extern CHAR g_szDecryptedString_GET[ 16 ];
extern CHAR g_szDecryptedString_POST[ 16 ];
extern CHAR g_szDecryptedString_Host1[ 16 ];
extern CHAR g_szDecryptedString_Host2[ 16 ];
extern CHAR g_szDecryptedString_UserAgent1[ 16 ];
extern CHAR g_szDecryptedString_UserAgent2[ 16 ];
extern CHAR g_szDecryptedString_VwPanDmNewCfg[ 16 ];
extern CHAR g_szDecryptedString_reg_ip_[ 16 ];
extern CHAR g_szDecryptedString_reg_key_[ 16 ];
extern CHAR g_szDecryptedString_reg_prtype_[ 16 ];
extern CHAR g_szDecryptedString_reg_prtypecs_[ 16 ];

//	for prtypefmt
extern CHAR g_szDecryptedString_PrTypeChecksumFmt[ 32 ];

//	for prtype
extern CHAR g_szDecryptedString_PrType_comadv[ 32 ];
extern CHAR g_szDecryptedString_PrType_comnormal[ 32 ];
extern CHAR g_szDecryptedString_PrType_pslunlmt[ 32 ];
extern CHAR g_szDecryptedString_PrType_psladv[ 32 ];
extern CHAR g_szDecryptedString_PrType_pslnormal[ 32 ];


//
//	public
//
BOOLEAN procconfig_init_config();
VOID	procconfig_unload_config();
BOOLEAN procconfig_is_valid_key( LPCWSTR lpcwszKeyFile );
BOOLEAN procconfig_is_valid_prtype_checksum( LPCTSTR lpcszPrTypeChecksum );
BOOLEAN procconfig_load_config( LPCWSTR lpcwszConfigFile );

BOOLEAN procconfig_start();
BOOLEAN procconfig_stop();
BOOLEAN procconfig_is_start();
BOOLEAN procconfig_is_loading();
BOOLEAN procconfig_lock( INT nCurrIndex );
BOOLEAN procconfig_unlock( INT nCurrIndex );

VOID    procconfig_add_block_count( LONGLONG * pllCounter );
BOOLEAN procconfig_is_matched_domain( LPCSTR lpcszDomain, UINT uDomainLength );
BOOLEAN procconfig_clean_all_offline_config();

//
//	private
//
VOID    procconfig_decrypt_string();
UINT	procconfig_stconfig_get_online_index();
UINT	procconfig_stconfig_get_offline_index();
VOID	procconfig_stconfig_switch_index();

UINT    procconfig_get_max_parsed_count();
BOOLEAN procconfig_load_allconfig_from_file( LPCWSTR lpcwszConfigFile );
BOOLEAN procconfig_addnew_domain( LPSTDOMAIN pstDomain );
BOOLEAN procconfig_addnew_rdp_clientname( LPSTRDPCLIENTNAME pstRdpClientName );
LPSTDOMAIN procconfig_get_matched_domain( LPCSTR lpcszOrgDomain, UINT uOrgDomainLength );
BOOLEAN procconfig_is_exist_rdp_clientname( LPCWSTR lpcwszClientName, UINT uLength );

UINT	procconfig_split_string_to_array( IN LPCSTR lpcszStringList, IN UCHAR uChrSpliter, OUT BYTE pbtArray[][PROCCONFIG_MAX_EXT_LENGTH], IN DWORD dwArrayCount );



BOOLEAN	procconfig_addnew_protected_dir( LPSTPROTECTEDDIR pstProtectedDir );

BOOLEAN	procconfig_is_protected_dir( IN WCHAR * lpwszFilePath, IN UINT uFilePathLength, IN BOOLEAN bCheckAllParentDirs, IN HANDLE hFileHandle );

BOOLEAN	procconfig_is_protected_disk( WCHAR wszDisk );
BOOLEAN	procconfig_is_protected_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength );
BOOLEAN	procconfig_is_excepted_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength );


BOOLEAN procconfig_addnew_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32, BOOLEAN bProtectedDir );
BOOLEAN procconfig_is_protected_dir_by_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32 );
BOOLEAN procconfig_clean_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, STPROTECTEDDIRCACHE * pstProtectedDirCache );



BOOLEAN procconfig_addnew_excepted_process( LPSTEXCEPTEDPROCESS pstExceptedProcess );
BOOLEAN procconfig_is_excepted_process();
BOOLEAN procconfig_is_excepted_process_by_devicepath( PWSTR pwszDevicePath, USHORT uLengthInBytes );




#endif // __PROCCONFIG_HEADER__
