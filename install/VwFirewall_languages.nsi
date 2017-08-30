;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; �����԰�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LangString g_sMlProductName ${LANG_ENGLISH} "VirtualWall Firewall"
LangString g_sMlProductName ${LANG_SIMPCHINESE} "΢�ܷ���ǽ"

LangString g_sMlUninstall ${LANG_ENGLISH} "Uninstall"
LangString g_sMlUninstall ${LANG_SIMPCHINESE} "ж��"

LangString g_sMlHomepage ${LANG_ENGLISH} "VirtualWall Homepage"
LangString g_sMlHomepage ${LANG_SIMPCHINESE} "΢�ܹٷ���վ"

LangString g_sMlHelpDoc ${LANG_ENGLISH} "Help Documents"
LangString g_sMlHelpDoc ${LANG_SIMPCHINESE} "�����ĵ�"

LangString g_sMlError_OSMust2003 ${LANG_ENGLISH} "Sorry, this software must be installed on the Windows 2003 operating system!"
LangString g_sMlError_OSMust2003 ${LANG_SIMPCHINESE} "�Բ��𣬸�������밲װ�� Windows 2003 ����ϵͳ�ϣ�"

LangString g_sMlError_TestSys32Dir ${LANG_ENGLISH} "We cann't continue to install, make sure that the $SYSDIR directory can be written into the DLL files."
LangString g_sMlError_TestSys32Dir ${LANG_SIMPCHINESE} "��װ�޷���������ȷ�ϡ�$SYSDIR��Ŀ¼����д�� DLL �ļ���"

LangString g_sMlError_TestSysDriverDir ${LANG_ENGLISH} "We cann't continue to install, make sure that the $SYSDIR\drivers directory can be written into the DLL files."
LangString g_sMlError_TestSysDriverDir ${LANG_SIMPCHINESE} "��װ�޷���������ȷ�ϡ�$SYSDIR\drivers��Ŀ¼����д�� DLL �ļ���"

LangString g_sMlError_TestSysDriverDir2 ${LANG_ENGLISH} "We cann't continue to install, make sure that the $SYSDIR\drivers directory can be written into the .sys files."
LangString g_sMlError_TestSysDriverDir2 ${LANG_SIMPCHINESE} "��װ�޷���������ȷ�ϡ�$SYSDIR\drivers��Ŀ¼����д�� .sys �ļ���"

LangString g_sMlInsRestartIISNote ${LANG_ENGLISH} "Note: We will stop IIS Service, Are you ready?"
LangString g_sMlInsRestartIISNote ${LANG_SIMPCHINESE} "ע��:��װ���̽�ֹͣ IIS �����Ƿ�׼�����ˣ�"

LangString g_sMlInsSuccess ${LANG_ENGLISH} "Congratulations! The software was installed successfully. You must reboot this computer after using it. $\r$\nAre you want to reboot this computer now?"
LangString g_sMlInsSuccess ${LANG_SIMPCHINESE} "��ϲ�������װ��ɣ�������Ҫ��������ϵͳ��������ʹ�á�$\r$\n�Ƿ����ھ������������"

LangString g_sMlUninsCompleted ${LANG_ENGLISH} "$(g_sMlProductName) ${PRODUCT_VERSION} was uninstalled completed! "
LangString g_sMlUninsCompleted ${LANG_SIMPCHINESE} "$(g_sMlProductName) ${PRODUCT_VERSION} ж����ɣ�"

LangString g_sMlUninsConfirm ${LANG_ENGLISH} "Are you sure you want to uninstall $(g_sMlProductName) ${PRODUCT_VERSION}?"
LangString g_sMlUninsConfirm ${LANG_SIMPCHINESE} "ȷ��Ҫ��ȫж�ء�$(g_sMlProductName) ${PRODUCT_VERSION}��"

LangString g_sMlUninsRestartIISNote ${LANG_ENGLISH} "Note: We will stop IIS Service, Are you ready?"
LangString g_sMlUninsRestartIISNote ${LANG_SIMPCHINESE} "ע��:ж�ع��̽�ֹͣ IIS �����Ƿ�׼�����ˣ�"

LangString g_sMlDllUpgradeMustReboot ${LANG_ENGLISH} "In order to make the program work correctly, we need to upgrade some shared DLL files, but the DLL files is being used by other programs.$\r$\nWe must restart this computer to complete the upgrade, Are you want to reboot this computer now?"
LangString g_sMlDllUpgradeMustReboot ${LANG_SIMPCHINESE} "���ʹ�õĹ��� DLL �ļ���Ҫ��������ܱ�֤�����������С����ڸ� DLL �ļ����ڱ���������ʹ�ã��޷����£�$\r$\n���������������������������Ƿ����ھ������������"

LangString g_sMlDllUpgradeMustReboot2 ${LANG_ENGLISH} "Please rerun this installation program after reboot!"
LangString g_sMlDllUpgradeMustReboot2 ${LANG_SIMPCHINESE} "����������������ٴ����иð�װ����"



;
;   ������ѡ��Ի���
;
!macro ShowMultiLanguageSelectionDialog

   ;Language selection dialog

   Push ""
   Push ${LANG_ENGLISH}
   Push English
   Push ${LANG_SIMPCHINESE}
   Push "Simplified Chinese"
   Push A ; A means auto count languages
          ; for the auto count to work the first empty push (Push "") must remain
   LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

   Pop $LANGUAGE
   StrCmp $LANGUAGE "cancel" 0 +2
          Abort

!macroend
