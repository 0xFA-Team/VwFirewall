#include "stdafx.h"
#include "SysZwFunc.h"
#include "DrvFunc.h"
#include "ProcConfig.h"
#include "FsApiHook.h"


/**
 *	ԭʼ������ַָ��
 */
DWORD g_dwFsApiHookConfig = FSAPIHOOKFS_CFG_ZWWRITEFILE | 
			FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE | 
			FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT;

PFNZWCREATEFILE g_pfnOrgZwCreateFile				= NULL;
PFNZWOPENFILE g_pfnOrgZwOpenFile				= NULL;
PFNZWREADFILE g_pfnOrgZwReadFile				= NULL;
PFNZWWRITEFILE g_pfnOrgZwWriteFile				= NULL;
PFNZWQUERYINFORMATIONFILE g_pfnOrgZwQueryInformationFile	= NULL;
PFNZWSETINFORMATIONFILE g_pfnOrgZwSetInformationFile		= NULL;
PFNZWCREATEDIRECTORYOBJECT g_pfnOrgZwCreateDirectoryObject	= NULL;



/**
 *	Set Hook Config
 */
VOID fsapihook_set_config( DWORD dwHookConfig )
{
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return;
	}

	__try
	{
		g_dwFsApiHookConfig = dwHookConfig;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: fsapihook_set_config"));
	}
}

/**
 *	Install Hook
 */
VOID fsapihook_hook_install()
{
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return;
	}

	__try
	{
		//	Hook ZwCreateFile()
		if ( FSAPIHOOKFS_CFG_ZWCREATEFILE == ( FSAPIHOOKFS_CFG_ZWCREATEFILE & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwCreateFile = (PFNZWCREATEFILE)SYSCALL( ZwCreateFile );
			SYSCALL( ZwCreateFile ) = (PVOID)NewZwCreateFile;
		}

		//	Hook ZwOpenFile()
		if ( FSAPIHOOKFS_CFG_ZWOPENFILE == ( FSAPIHOOKFS_CFG_ZWOPENFILE & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwOpenFile = (PFNZWOPENFILE)SYSCALL( ZwOpenFile );
			SYSCALL( ZwOpenFile ) = (PVOID)NewZwOpenFile;
		}

		//	Hook ZwReadFile()
		if ( FSAPIHOOKFS_CFG_ZWREADFILE == ( FSAPIHOOKFS_CFG_ZWREADFILE & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwReadFile = (PFNZWREADFILE)SYSCALL( ZwReadFile );
			SYSCALL( ZwReadFile ) = (PVOID)NewZwReadFile;
		}

		//	Hook ZwWriteFile()
		if ( FSAPIHOOKFS_CFG_ZWWRITEFILE == ( FSAPIHOOKFS_CFG_ZWWRITEFILE & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwWriteFile = (PFNZWWRITEFILE)SYSCALL( ZwWriteFile );
			SYSCALL( ZwWriteFile ) = (PVOID)NewZwWriteFile;
		}

		//	Hook ZwQueryInformationFile()
		if ( FSAPIHOOKFS_CFG_ZWQUERYINFORMATIONFILE == ( FSAPIHOOKFS_CFG_ZWQUERYINFORMATIONFILE & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwQueryInformationFile = (PFNZWQUERYINFORMATIONFILE)SYSCALL( ZwQueryInformationFile );
			SYSCALL( ZwQueryInformationFile ) = (PVOID)NewZwQueryInformationFile;
		}

		//	Hook ZwSetInformationFile()
		if ( FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE == ( FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwSetInformationFile = (PFNZWSETINFORMATIONFILE)SYSCALL( ZwSetInformationFile );
			SYSCALL( ZwSetInformationFile ) = (PVOID)NewZwSetInformationFile;
		}

		//	Hook ZwCreateDirectoryObject()
		if ( FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT == ( FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT & g_dwFsApiHookConfig ) )
		{
			g_pfnOrgZwCreateDirectoryObject = (PFNZWCREATEDIRECTORYOBJECT)SYSCALL( ZwCreateDirectoryObject );
			SYSCALL( ZwCreateDirectoryObject ) = (PVOID)NewZwCreateDirectoryObject;
		}

		KdPrint(("HookFsInstall: Install.\n"));
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: fsapihook_hook_install"));
	}
}

/**
 *	UnInstall Hook
 */
VOID fsapihook_hook_uninstall()
{
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return;
	}

	__try
	{
		//	Unload Hook ZwCreateFile
		if ( FSAPIHOOKFS_CFG_ZWCREATEFILE == ( FSAPIHOOKFS_CFG_ZWCREATEFILE & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwCreateFile )
			{
				SYSCALL( ZwCreateFile ) = (PVOID)g_pfnOrgZwCreateFile;
			}
		}

		//	Unload Hook ZwOpenFile
		if ( FSAPIHOOKFS_CFG_ZWOPENFILE == ( FSAPIHOOKFS_CFG_ZWOPENFILE & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwOpenFile )
			{
				SYSCALL( ZwOpenFile ) = (PVOID)g_pfnOrgZwOpenFile;
			}
		}

		//	Unload Hook ZwReadFile
		if ( FSAPIHOOKFS_CFG_ZWREADFILE == ( FSAPIHOOKFS_CFG_ZWREADFILE & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwReadFile )
			{
				SYSCALL( ZwReadFile ) = (PVOID)g_pfnOrgZwReadFile;
			}
		}

		//	Unload Hook ZwWriteFile
		if ( FSAPIHOOKFS_CFG_ZWWRITEFILE == ( FSAPIHOOKFS_CFG_ZWWRITEFILE & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwWriteFile )
			{
				SYSCALL( ZwWriteFile ) = (PVOID)g_pfnOrgZwWriteFile;
			}
		}

		//	Unload Hook ZwQueryInformationFile
		if ( FSAPIHOOKFS_CFG_ZWQUERYINFORMATIONFILE == ( FSAPIHOOKFS_CFG_ZWQUERYINFORMATIONFILE & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwQueryInformationFile )
			{
				SYSCALL( ZwQueryInformationFile ) = (PVOID)g_pfnOrgZwQueryInformationFile;
			}
		}

		//	Unload Hook ZwSetInformationFile
		if ( FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE == ( FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwSetInformationFile )
			{
				SYSCALL( ZwSetInformationFile ) = (PVOID)g_pfnOrgZwSetInformationFile;
			}
		}

		//	Unload Hook ZwCreateDirectoryObject
		if ( FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT == ( FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT & g_dwFsApiHookConfig ) )
		{
			if ( g_pfnOrgZwCreateDirectoryObject )
			{
				SYSCALL( ZwCreateDirectoryObject ) = (PVOID)g_pfnOrgZwCreateDirectoryObject;
			}
		}

		KdPrint(("HookFsUnInstall: UnInstall.\n"));
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: fsapihook_hook_uninstall"));
	}
}

/**
 *	����û����ļ��Ĳ���Ȩ��
 *	for ZwCreateFile
 */
DWORD fsapihook_check_action_for_zwcreatefile( IN POBJECT_ATTRIBUTES pObjectAttributes )
{
	//
	//	pObjectAttributes	- [in] ��Ϊ���� ZwCreateFile ʱ���ļ���Ϣ������������ṹ���ڣ����Ա��봦�������õ��ļ���
	//	RETURN			- ������
	//

	DWORD dwRet;
	BOOLEAN bIsProtectedDisk;
	WCHAR  wszFullFilePath[ MAX_PATH ];
	WCHAR  wszFileName[ MAX_PATH ];
	UINT   uFileNameLength;
	WCHAR * lpwszRealFileName;
	UINT  uFullFilePathLength;
	NTSTATUS status;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( UserMode != ExGetPreviousMode() )
	{
		//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( NULL == pObjectAttributes )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( ! procconfig_is_start() )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( ! g_stCfg.bStartFileSafe )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}

	//
	//	Ĭ���Ǹ��� PASS
	//
	dwRet = FSAPIHOOKFS_ACTION_PASS;

	__try
	{
		//
		//	pObjectAttributes->ObjectName->Length �ĵ�λ�� byte ���ԣ��䳤��Ҫ���� 2������ 24 / 2 = 12 ���ַ�
		//
		if ( pObjectAttributes->ObjectName->Length >= 24 &&
			L'\\' == pObjectAttributes->ObjectName->Buffer[0] &&
			L'?'  == pObjectAttributes->ObjectName->Buffer[1] &&
			L'?'  == pObjectAttributes->ObjectName->Buffer[2] &&
			L'\\' == pObjectAttributes->ObjectName->Buffer[3] &&
			L':' == pObjectAttributes->ObjectName->Buffer[5] )
		{
			if ( procconfig_is_protected_disk( pObjectAttributes->ObjectName->Buffer[ 4 ] ) )
			{
				//
				//	û�����Ŀ¼�ģ�����·���������� pObjectAttributes->ObjectName
				//

				//
				//	pObjectAttributes->ObjectName =
				//		"\Device\Tcp"
				//		"\Device\NetBT_Tcpip_{F831F3FE-B2AA-4D0A-8C57-063DD51455F2}"
				//		"\Device\Tcp6"
				//		"\Device\RasAcd"
				//		"\??\C:\WINDOWS\debug\UserMode\ChkAcc.log"
				//		"\??\C:\WINDOWS\system32\spool\PRINTERS"
				//

				//	ȥ�� Device ·����ǰ��Ŀ�ͷ "\??\"
				lpwszRealFileName	= pObjectAttributes->ObjectName->Buffer + 4;
				uFileNameLength		= pObjectAttributes->ObjectName->Length - 4 * sizeof(WCHAR);

				//	����Ƿ��Ǳ�����Ŀ¼
				if ( procconfig_is_protected_dir( lpwszRealFileName, uFileNameLength, FALSE, NULL ) )
				{
					//	Check if excepted process
					if ( ! procconfig_is_excepted_process() )
					{
						//	ͳ�����ش˴���
						procconfig_add_block_count( & g_stCfg.llProtectFileBlockCount );

						//	�ܾ�����
						dwRet = FSAPIHOOKFS_ACTION_DENIED;
					}
				}
			}
		}
		else if ( pObjectAttributes->RootDirectory )
		{
			//
			//	�����Ŀ¼��
			//

			//
			//	pObjectAttributes.RootDirectory �Ǹ���ֵĳ�Ա�������ǹ������Ľ���
			//	Optional handle to the root object directory for the path name specified by the ObjectName member.
			//	If RootDirectory is NULL, ObjectName must point to a fully-qualified object name that includes the full path to the target object.
			//	If RootDirectory is non-NULL, ObjectName specifies an object name relative to the RootDirectory directory.
			//	The RootDirectory handle can refer to a file system directory or an object directory in the object manager namespace. 
			//
			RtlZeroMemory( wszFullFilePath, sizeof(wszFullFilePath) );
			uFullFilePathLength = drvfunc_get_widefilename_by_handle( pObjectAttributes->RootDirectory, wszFullFilePath, sizeof(wszFullFilePath) );
			if ( uFullFilePathLength > 0 && 
				uFullFilePathLength < sizeof(wszFullFilePath) )
			{
				bIsProtectedDisk = FALSE;

				if ( uFullFilePathLength >= 12 &&
					L'\\' == wszFullFilePath[0] &&
					L'?'  == wszFullFilePath[1] &&
					L'?'  == wszFullFilePath[2] &&
					L'\\' == wszFullFilePath[3] &&
					L':' == wszFullFilePath[5] )
				{
					//	"\??\c:\windows"
					bIsProtectedDisk = procconfig_is_protected_disk( wszFullFilePath[4] );
				}
				else if ( uFullFilePathLength >= 6 &&
					L':' == wszFullFilePath[1] )
				{
					//	"c:\"
					bIsProtectedDisk = procconfig_is_protected_disk( wszFullFilePath[0] );
				}

				//
				//	ȷ���Ǳ������Ĵ���
				//
				if ( bIsProtectedDisk )
				{
					//
					//	��֤ wszFullFilePath �� "\" ��β
					//
					if ( L'\\' != wszFullFilePath[ uFullFilePathLength - 1 ] )
					{
						wcscat( wszFullFilePath, L"\\" );
						uFullFilePathLength += sizeof(WCHAR);
					}

					KdPrint(("fsapihook_check_action_for_zwcreatefile wszFullFilePath=%s\n", wszFullFilePath));

					//
					//	�� UNICODE_STRING ���ļ���ת��Ϊ ASIC
					//
					//RtlZeroMemory( wszFileName, sizeof(wszFileName) );
					if ( pObjectAttributes->ObjectName->Length < sizeof(wszFileName) )
					{
						RtlCopyMemory( wszFileName, pObjectAttributes->ObjectName->Buffer, pObjectAttributes->ObjectName->Length );
						wszFileName[ pObjectAttributes->ObjectName->Length/sizeof(WCHAR) ] = 0;
						uFileNameLength = pObjectAttributes->ObjectName->Length;
					}
					else
					{
						RtlCopyMemory( wszFileName, pObjectAttributes->ObjectName->Buffer, sizeof(wszFileName) - sizeof(WCHAR) );
						wszFileName[ sizeof(wszFileName)/sizeof(WCHAR) - 1 ] = 0;
						uFileNameLength = sizeof(wszFileName) - sizeof(WCHAR);
					}

					if ( ( uFullFilePathLength + uFileNameLength ) < sizeof(wszFullFilePath) )
					{
						//
						//	ƴ������·��
						//
						wcscat( wszFullFilePath, wszFileName );

						lpwszRealFileName	= wszFullFilePath;
						uFileNameLength		= uFullFilePathLength + uFileNameLength;

						KdPrint( ( "fsapihook_check_action_for_zwcreatefile lpwszRealFileName=%s\n", lpwszRealFileName ) );

						//	����Ƿ��Ǳ�����Ŀ¼
						if ( procconfig_is_protected_dir( lpwszRealFileName, uFileNameLength, FALSE, NULL ) )
						{
							//	Check if excepted process
							if ( ! procconfig_is_excepted_process() )
							{
								//	ͳ�����ش˴���
								procconfig_add_block_count( & g_stCfg.llProtectFileBlockCount );

								//	�ܾ�����
								dwRet = FSAPIHOOKFS_ACTION_DENIED;
							}
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: fsapihook_check_action_for_zwcreatefile"));
	}

	return dwRet;
}

/**
 *	����û����ļ��Ĳ���Ȩ��
 */
DWORD fsapihook_check_action( IN HANDLE hFileHandle, BOOLEAN bCheckAllParentDirs )
{
	//
	//	hFileHandle		- [in] �ļ���������ڻ���ļ���ȫ·��
	//	bCheckAllParentDirs	- [in] �Ƿ��������и�·��
	//	RETURN			- ������
	//

	DWORD dwRet;
	BOOLEAN bIsProtectedDisk;
	WCHAR * pwszNewFilename;
	UINT  uFileNameLength;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( UserMode != ExGetPreviousMode() )
	{
		//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( NULL == hFileHandle )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( ! procconfig_is_start() )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( ! g_stCfg.bStartFileSafe )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}

	//
	//	Ĭ���Ǹ��� PASS
	//
	dwRet = FSAPIHOOKFS_ACTION_PASS;


	__try
	{
		//	
		//	��ȡ�ļ������ж��Ƿ��Ǳ�����Ŀ¼
		//
		pwszNewFilename = (PWCHAR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolFilenameW );
		if ( pwszNewFilename )
		{
			RtlZeroMemory( pwszNewFilename, MAX_WPATH );
			uFileNameLength = drvfunc_get_widefilename_by_handle( hFileHandle, pwszNewFilename, MAX_WPATH );
			if ( uFileNameLength )
			{
				bIsProtectedDisk = FALSE;

				if ( uFileNameLength >= 12 &&
					L'\\' == pwszNewFilename[0] &&
					L'?'  == pwszNewFilename[1] &&
					L'?'  == pwszNewFilename[2] &&
					L'\\' == pwszNewFilename[3] &&
					L':' == pwszNewFilename[5] )
				{
					//	"\??\c:\windows"
					bIsProtectedDisk = procconfig_is_protected_disk( pwszNewFilename[4] );
				}
				else if ( uFileNameLength >= 6 && 
					L':' == pwszNewFilename[1] )
				{
					//	"c:\"
					bIsProtectedDisk = procconfig_is_protected_disk( pwszNewFilename[0] );
				}

				if ( bIsProtectedDisk )
				{
					if ( procconfig_is_protected_dir( pwszNewFilename, uFileNameLength, bCheckAllParentDirs, hFileHandle ) )
					{
						//	Check if excepted process
						if ( ! procconfig_is_excepted_process() )
						{
							//	ͳ�����ش˴���
							procconfig_add_block_count( & g_stCfg.llProtectFileBlockCount );

							//	�ܾ�����
							dwRet = FSAPIHOOKFS_ACTION_DENIED;
						}
					}
				}
			}

			//	free memory
			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolFilenameW, pwszNewFilename );
			pwszNewFilename = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: fsapihook_check_action"));
	}

	return dwRet;
}

DWORD fsapihook_check_action_by_filename( IN PWCHAR pwszFilename, IN USHORT uFileNameLength )
{
	//
	//	pwszFilename	- [in] �ļ�ȫ·�������磺
	//				"C:\DeFirewall.txt"
	//				"\??\C:\DeDir\2.bat" Ҳ�п�����������
	//	uFileNameLength	- [in] the length of pwszFilename in bytes
	//	RETURN		- ������
	//
	DWORD dwRet;
	WCHAR * pwszNewFilename;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( UserMode != ExGetPreviousMode() )
	{
		//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.
		return FSAPIHOOKFS_ACTION_PASS;
	}
	if ( NULL == pwszFilename || 0 == uFileNameLength )
	{
		return FSAPIHOOKFS_ACTION_PASS;
	}
	
	//
	//	Ĭ���Ǹ��� PASS
	//
	dwRet = FSAPIHOOKFS_ACTION_PASS;

	__try
	{
		if ( pwszFilename )
		{
			if ( uFileNameLength > 4 )
			{
				if ( 0 == wcsncmp( pwszFilename, L"\\??\\", 4 ) )
				{
					pwszNewFilename	= pwszFilename + 4;
					uFileNameLength	-= 4;
				}
				else
				{
					pwszNewFilename	= pwszFilename;
				}

				if ( procconfig_is_protected_dir( pwszNewFilename, uFileNameLength, FALSE, NULL ) )
				{
					//	Check if excepted process
					if ( ! procconfig_is_excepted_process() )
					{
						//	ͳ�����ش˴���
						procconfig_add_block_count( & g_stCfg.llProtectFileBlockCount );

						//	�ܾ�����
						dwRet = FSAPIHOOKFS_ACTION_DENIED;
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: fsapihook_check_action_by_filename"));
	}

	return dwRet;
}


//////////////////////////////////////////////////////////////////////////





/**
 *	New process function
 *	Hook API ZwCreateFile
 */
NTSTATUS NewZwCreateFile
	(
		OUT PHANDLE FileHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes,
		OUT PIO_STATUS_BLOCK IoStatusBlock,
		IN PLARGE_INTEGER AllocationSize OPTIONAL,
		IN ULONG FileAttributes,
		IN ULONG ShareAccess,
		IN ULONG CreateDisposition,
		IN ULONG CreateOptions,
		IN PVOID EaBuffer OPTIONAL,
		IN ULONG EaLength
	)
{
//	CHAR szFullProcessName[ MAX_PATH ];
//	USHORT uFullnameLength;

	__try
	{
		//
		//	ExGetPreviousMode() return one of UserMode or KernelMode
		//
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.

				//	ֻ������ں˵��û�����
				//	TODO:
				//		1, ������Դ��������������Ҽ� > �½� > �½��ĵ�.txt �����ж�����
				//

			//	memset( szFullProcessName, 0, sizeof(szFullProcessName) );
			//	if ( drvfunc_get_processfullname( szFullProcessName, sizeof(szFullProcessName), &uFullnameLength ) )
			//	{
			//		DbgPrint( "NewZwCreateFile::processfullname=%s,len=%d", szFullProcessName, uFullnameLength );
			//	}

				if ( 1 || FILE_WRITE_DATA == ( FILE_WRITE_DATA & DesiredAccess ) ||
					FILE_APPEND_DATA == ( FILE_APPEND_DATA & DesiredAccess ) )
				{
					if ( FILE_SUPERSEDE == CreateDisposition || FILE_CREATE == CreateDisposition ||
						FILE_OPEN_IF == CreateDisposition || FILE_OVERWRITE_IF == CreateDisposition )
					{
						if ( FSAPIHOOKFS_ACTION_DENIED == fsapihook_check_action_for_zwcreatefile( ObjectAttributes ) )
						{
							//	�ܾ�����
							KdPrint(("NewZwCreateFile:: denied.\n"));

							//	�ܾ�����
							if ( FileHandle )
							{
								*FileHandle = (HANDLE)-1;	//INVALID_HANDLE_VALUE;
							}
							IoStatusBlock->Information	= FILE_EXISTS;
							IoStatusBlock->Status		= STATUS_ACCESS_DENIED;
							return STATUS_ACCESS_DENIED;
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwCreateFile"));
	}

	return g_pfnOrgZwCreateFile
	(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength
	);
}



/**
 *	New process function
 *	Hook API ZwOpenFile
 */
NTSTATUS NewZwOpenFile
	(
		OUT PHANDLE FileHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes,
		OUT PIO_STATUS_BLOCK IoStatusBlock,
		IN ULONG ShareAccess,
		IN ULONG OpenOptions
	)
{
	__try
	{
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.

				//	..
		//		KdPrint(("NewZwOpenFile:: not Kernel mode.\n"));

				/*
				if ( FILE_WRITE_DATA == ( FILE_WRITE_DATA & DesiredAccess ) ||
					FILE_APPEND_DATA == ( FILE_APPEND_DATA & DesiredAccess ) )
				{
					if ( FSAPIHOOKFS_ACTION_DENIED == fsapihook_check_action( FileHandle, FALSE ) )
					{
						//	�ܾ�����
						return STATUS_ACCESS_DENIED;
					}
				}
				*/

				/*
				//	ע�⣺wcsstr�����ǲ���ȫ�ģ����������е��ַ����ǲ�Ҫ��\0��β�ġ� 	
				if ( ObjectAttributes->ObjectName->Buffer && wcsstr( ObjectAttributes->ObjectName->Buffer, L"111.txt" ) )
				{
					//	�ܾ�����
					*FileHandle			= NULL;
					IoStatusBlock->Information	= 0;
					IoStatusBlock->Status		= STATUS_UNSUCCESSFUL;
					
					//return STATUS_UNSUCCESSFUL;
					return STATUS_ACCESS_DENIED;
				}
				*/
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwOpenFile"));
	}

	return g_pfnOrgZwOpenFile
	(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		ShareAccess,
		OpenOptions
	);
}



/**
 *	New process function
 *	Hook API ZwReadFile
 */
NTSTATUS NewZwReadFile
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
)
{
	__try
	{
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.

				//	..
				KdPrint(("NewZwReadFile:: not Kernel mode.\n"));
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwReadFile"));
	}

	return g_pfnOrgZwReadFile
	(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		ByteOffset,
		Key
	);
}

/**
 *	New process function
 *	Hook API ZwWriteFile
 */
NTSTATUS NewZwWriteFile
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
)
{
	__try
	{
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.
				
				if ( FSAPIHOOKFS_ACTION_DENIED == fsapihook_check_action( FileHandle, FALSE ) )
				{
					//	�ܾ�����
					KdPrint(("NewZwWriteFile:: denied.\n"));
					return STATUS_ACCESS_DENIED;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwWriteFile"));
	}

	return g_pfnOrgZwWriteFile
	(
		FileHandle,
		Event OPTIONAL,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		ByteOffset,
		Key
	);
}


/**
 *	New process function
 *	Hook API NewZwQueryInformationFile
 */
NTSTATUS NewZwQueryInformationFile
(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
)
{
//	KdPrint(("NewZwQueryInformationFile:: start\n"));

	__try
	{
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwQueryInformationFile"));
	}

	return g_pfnOrgZwQueryInformationFile
	(
		FileHandle,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass
	);
}

/**
 *	New process function
 *	Hook API NewZwSetInformationFile
 */
NTSTATUS NewZwSetInformationFile
(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
)
{
	DWORD dwAction;
	BOOLEAN bIsProtectedDisk;
	FILE_RENAME_INFORMATION * pFileRenameInfo;
	UCHAR * puszNewFilename;
	NTSTATUS status;

	IO_STATUS_BLOCK stIoStatus;
	FILE_STANDARD_INFORMATION stStandardInfo;
	BOOLEAN bIsDirectory;

	__try
	{
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.

				//
				//	action = pass
				//
				dwAction = FSAPIHOOKFS_ACTION_PASS;


				if ( FileRenameInformation == FileInformationClass )
				{
					//
					//	�ļ�����������
					//
					
					//
					//	���ȣ��жϱ��������� Դ�ļ� �Ƿ��Ǳ������ļ�
					//
					status = ZwQueryInformationFile( FileHandle, &stIoStatus, &stStandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
					if ( NT_SUCCESS( status ) )
					{
						bIsDirectory = stStandardInfo.Directory;
					}
					else
					{
						bIsDirectory = FALSE;
					}

					dwAction = fsapihook_check_action( FileHandle, TRUE );

					if ( FSAPIHOOKFS_ACTION_PASS == dwAction )
					{
						//
						//	���ж� ����������Ŀ���ļ� �Ƿ��Ǳ������ļ�
						//
						if ( FileInformation )
						{
							pFileRenameInfo = (FILE_RENAME_INFORMATION*)FileInformation;
							if ( pFileRenameInfo &&
								pFileRenameInfo->FileNameLength >= 6 )
							{
								bIsProtectedDisk = FALSE;
								if ( pFileRenameInfo->FileNameLength >= 12 &&
									L'\\' == pFileRenameInfo->FileName[0] &&
									L'?'  == pFileRenameInfo->FileName[1] &&
									L'?'  == pFileRenameInfo->FileName[2] &&
									L'\\' == pFileRenameInfo->FileName[3] &&
									L':' == pFileRenameInfo->FileName[5] )
								{
									//	"\??\c:\windows"
									bIsProtectedDisk = procconfig_is_protected_disk( pFileRenameInfo->FileName[4] );
								}
								else if ( pFileRenameInfo->FileNameLength >= 6 &&
									L':' == pFileRenameInfo->FileName[1] )
								{
									//	"c:\"
									bIsProtectedDisk = procconfig_is_protected_disk( pFileRenameInfo->FileName[0] );
								}

								//
								//	ȷ���Ǳ������Ĵ��̺��ٽ�һ����֤
								//
								if ( bIsProtectedDisk )
								{
									dwAction = fsapihook_check_action_by_filename
										(
											(PWCHAR)( pFileRenameInfo->FileName ),
											(USHORT)( pFileRenameInfo->FileNameLength )
										);

									/*
									puszNewFilename = (PUCHAR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolFilename );
									if ( puszNewFilename )
									{
										RtlZeroMemory( puszNewFilename, MAX_PATH );

										status = drvfunc_w2c_ex( pFileRenameInfo->FileName, (USHORT)(pFileRenameInfo->FileNameLength), puszNewFilename, MAX_PATH );
										if ( NT_SUCCESS(status) )
										{
											//
											//	�ж�֮
											//
											dwAction = fsapihook_check_action_by_filename( puszNewFilename, (USHORT)( pFileRenameInfo->FileNameLength / sizeof(WCHAR) ) );
										}

										//	free memory
										ExFreeToNPagedLookasideList( &g_npgProcConfigPoolFilename, puszNewFilename );
										puszNewFilename = NULL;
									}
									*/
								}
							}
						}
					}
				}
				else if ( FileDispositionInformation == FileInformationClass )
				{
					//
					//	�ļ�ɾ������
					//
					dwAction = fsapihook_check_action( FileHandle, FALSE );
				}

				if ( FSAPIHOOKFS_ACTION_DENIED == dwAction )
				{
					//	�ܾ�����
					KdPrint(("NewZwSetInformationFile:: FileRenameInformation denied.\n"));
					return STATUS_ACCESS_DENIED;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwSetInformationFile"));
	}

	return g_pfnOrgZwSetInformationFile
	(
		FileHandle,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass
	);
}

/**
 *	New process function
 *	Hook API ZwCreateDirectoryObject
 */
NTSTATUS NewZwCreateDirectoryObject
(
	OUT PHANDLE DirectoryHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes
)
{
	__try
	{
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			if ( UserMode == ExGetPreviousMode() )
			{
				//	Callers of ExGetPreviousMode must be running at IRQL <= DISPATCH_LEVEL.

				if ( FSAPIHOOKFS_ACTION_DENIED == fsapihook_check_action( DirectoryHandle, FALSE ) )
				{
					//	�ܾ�����
					KdPrint(("NewZwSetInformationFile:: NewZwCreateDirectoryObject denied.\n"));
					return STATUS_ACCESS_DENIED;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: NewZwCreateDirectoryObject"));
	}

	return g_pfnOrgZwCreateDirectoryObject
	(
		DirectoryHandle,
		DesiredAccess,
		ObjectAttributes
	);
}