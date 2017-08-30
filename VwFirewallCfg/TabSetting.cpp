// TabSetting.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSetting.h"
#include "process.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSetting dialog


CTabSetting::CTabSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSetting)
	//}}AFX_DATA_INIT

	memset( &m_stConfig, 0, sizeof(m_stConfig) );

	memset( &m_stStatusCache, 0, sizeof(m_stStatusCache) );
	m_stStatusCache.bCoreFwStartCached	= FALSE;
	m_stStatusCache.bStartProtectCached	= FALSE;


	m_hParentWind		= NULL;
	m_pxballoon		= NULL;

	memset( &m_stStatusIcon, 0, sizeof(m_stStatusIcon) );
}

CTabSetting::~CTabSetting()
{
	m_cThSleepDisplayCoreStatus.EndSleep();
	m_cThSleepDisplayCoreStatus.EndThread();

	//	...
	FreeAllIconsResources();
}

VOID CTabSetting::FreeAllIconsResources()
{
	//
	//	free resources
	//
	INT i;
	INT nCount;
	HICON ArrIcons[] = 
	{
		m_stStatusIcon.hIconO,
		m_stStatusIcon.hIconX,
		m_stStatusIcon.hIconTrayRun,
		m_stStatusIcon.hIconTrayRunning,
		m_stStatusIcon.hIconTrayStopped,
		m_stStatusIcon.hIconStatusDomainOk,
		m_stStatusIcon.hIconStatusDomainX,
		m_stStatusIcon.hIconStatusRdpOk,
		m_stStatusIcon.hIconStatusRdpX,
		m_stStatusIcon.hIconStatusFileOk,
		m_stStatusIcon.hIconStatusFileX,
	};
	nCount = sizeof(ArrIcons)/sizeof(ArrIcons[0]);
	for ( i = 0; i < nCount; i ++ )
	{
		if ( ArrIcons[ i ] )
		{
			::DestroyIcon( ArrIcons[ i ] );
		}
	}
}

void CTabSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSetting)
	DDX_Control(pDX, IDC_STATIC_CTLLINK_RDP_SP, m_stcCtlLinkRdpSp);
	DDX_Control(pDX, IDC_STATIC_CTLLINK_FILE_SP, m_stcCtlLinkFileSp);
	DDX_Control(pDX, IDC_STATIC_CTLLINK_DOMAIN_SP, m_stcCtlLinkDomainSp);
	DDX_Control(pDX, IDC_STATIC_PFILE_BLOCKCOUNT, m_stcPFileBlockCount);
	DDX_Control(pDX, IDC_STATIC_STATUS_FILE, m_stcStatusFile);
	DDX_Control(pDX, IDC_STATIC_STATUS_FILE_TXT, m_stcStatusFileTxt);
	DDX_Control(pDX, IDC_STATIC_PRDP_BLOCKCOUNT, m_stcPRdpBlockCount);
	DDX_Control(pDX, IDC_STATIC_STATUS_RDP_TXT, m_stcStatusRdpTxt);
	DDX_Control(pDX, IDC_STATIC_STATUS_RDP, m_stcStatusRdp);
	DDX_Control(pDX, IDC_STATIC_PDOMAIN_BLOCKCOUNT, m_stcPDomainBlockCount);
	DDX_Control(pDX, IDC_STATIC_STATUS_DOMAIN_TXT, m_stcStatusDomainTxt);
	DDX_Control(pDX, IDC_STATIC_STATUS_DOMAIN, m_stcStatusDomain);
	DDX_Control(pDX, IDC_STATIC_FIREWALLSTATUS, m_stcFirewallStatus);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_stcStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSetting, CDialog)
	//{{AFX_MSG_MAP(CTabSetting)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(UM_CTRL_BUTTON, OnCtrlButton)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSetting message handlers

BOOL CTabSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	//	App info
	//
	LoadString( AfxGetInstanceHandle(), IDS_APP_NAME, m_szAppName, sizeof(m_szAppName) );

	//
	//	����״̬
	//
	m_stcFirewallStatus.SetWindowText( "..." );;


	m_LinkAboutMore.SubclassDlgItem( IDC_STATIC_ABOUT_MORE, this );
	m_LinkAboutMore.SetColours( RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF) );
	m_LinkAboutMore.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkAboutMore.SetUnderline( TRUE );
	m_LinkAboutMore.SetVisited( FALSE );
	m_LinkAboutMore.SetURL( "http://rd.vidun.net/?4500" );

	m_CtlLinkFirewall.SubclassDlgItem( IDC_STATIC_CTLLINK_FIREWALL, this );
	m_CtlLinkFirewall.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkFirewall.SetUnderline( TRUE );
	m_CtlLinkFirewall.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FIREWALL, 0 );


	m_CtlLinkFile.SubclassDlgItem( IDC_STATIC_CTLLINK_FILE, this );
	m_CtlLinkFile.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkFile.SetUnderline( TRUE );
	m_CtlLinkFile.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FILE, 0 );

	m_CtlLinkFileSet.SubclassDlgItem( IDC_STATIC_CTLLINK_FILE_SET, this );
	m_CtlLinkFileSet.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkFileSet.SetUnderline( TRUE );
	m_CtlLinkFileSet.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FILE_SET, 0 );


	m_CtlLinkDomain.SubclassDlgItem( IDC_STATIC_CTLLINK_DOMAIN, this );
	m_CtlLinkDomain.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkDomain.SetUnderline( TRUE );
	m_CtlLinkDomain.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_DOMAIN, 0 );

	m_CtlLinkDomainSet.SubclassDlgItem( IDC_STATIC_CTLLINK_DOMAIN_SET, this );
	m_CtlLinkDomainSet.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkDomainSet.SetUnderline( TRUE );
	m_CtlLinkDomainSet.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_DOMAIN_SET, 0 );


	m_CtlLinkRdp.SubclassDlgItem( IDC_STATIC_CTLLINK_RDP, this );
	m_CtlLinkRdp.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkRdp.SetUnderline( TRUE );
	m_CtlLinkRdp.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_RDP, 0 );

	m_CtlLinkRdpSet.SubclassDlgItem( IDC_STATIC_CTLLINK_RDP_SET, this );
	m_CtlLinkRdpSet.SetColours( RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00) );
	m_CtlLinkRdpSet.SetUnderline( TRUE );
	m_CtlLinkRdpSet.SetCallbackMessage( m_hWnd, UM_CTRL_BUTTON, WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_RDP_SET, 0 );


	//
	//
	//
	m_stcCtlLinkFileSp.SetWindowPos( NULL, 0, 0, 8, 12, SWP_NOMOVE );
	m_stcCtlLinkDomainSp.SetWindowPos( NULL, 0, 0, 8, 12, SWP_NOMOVE );
	m_stcCtlLinkRdpSp.SetWindowPos( NULL, 0, 0, 8, 12, SWP_NOMOVE );


	//	��ʾ���ܰ�ť��״̬ 
	DisplayCtrlButtonsStatus();

	//
	//	��ʱ��ȡ ISAPI ��״̬
	//
	m_cThSleepDisplayCoreStatus.m_hThread = (HANDLE)_beginthreadex
	(
		NULL,
		0,
		&_threadDisplayCoreStatus,
		(void*)this,
		0,
		&m_cThSleepDisplayCoreStatus.m_uThreadId
	);
	
	return TRUE;
}


BOOL CTabSetting::PreTranslateMessage(MSG* pMsg) 
{
	//	�����û����µ� ESC ����ǿ���û��� X �˳�
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			return FALSE;
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}


void CTabSetting::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//
	//	�����κ�λ�õ���קЧ��
	//
	HWND hParent = ::GetParent( m_hWnd );
	if ( hParent )
	{
		::PostMessage( hParent, WM_NCLBUTTONDOWN, HTCAPTION,MAKELPARAM( point.x, point.y ) );
	}
	
	//	��ϵͳ���� HTCAPTION ��Ϣ����ϵͳ��Ϊ�����ڱ�������
	CDialog::OnLButtonDown(nFlags, point);
}

LRESULT CTabSetting::OnCtrlButton( WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FIREWALL:
		{
			//	...
			if ( IsCoreStart() )
			{
				LoadConfig();
				SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_START, m_stConfig.bStart ? 0 : 1 );

				//
				//	ˢ����ʾ״̬
				//
				DisplayCoreStatusSub( FALSE );
				DisplayCtrlButtonsStatus();

				//
				//	֪ͨ��������: װ���µ�������Ϣ
				//
				CTabBaseInfo::NotifyDriverToLoadNewConfig();
			}
			else
			{
				//	���������������°�װ��\r\n�����װ��δ��������������������������
				LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, m_szCaption, sizeof(m_szCaption) );
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ERR_INS1, m_szString, sizeof(m_szString) );
				MessageBox( m_szString, m_szCaption, MB_ICONWARNING );
			}	
		}
		break;
	
	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FILE:
		{
			LoadConfig();
			SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTFILESAFE, m_stConfig.bStartProtectFile ? 0 : 1 );
			DisplayCtrlButtonsStatus();
			
			//
			//	֪ͨ��������: װ���µ�������Ϣ
			//
			CTabBaseInfo::NotifyDriverToLoadNewConfig();
		}
		break;
	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FILE_SET:
		{
			CTabFile * pcTabFile;
			pcTabFile = new CTabFile();
			pcTabFile->Create( IDD_TAB_FILE );
			pcTabFile->ShowWindow( SW_SHOW );
			pcTabFile->UpdateWindow();
		}
		break;

	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_DOMAIN:
		{
			LoadConfig();
			SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTPDOMAIN, m_stConfig.bStartProtectDomain ? 0 : 1 );
			DisplayCtrlButtonsStatus();

			//
			//	֪ͨ��������: װ���µ�������Ϣ
			//
			CTabBaseInfo::NotifyDriverToLoadNewConfig();
		}
		break;
	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_DOMAIN_SET:
		{
			CTabDomain * pcTabDomain;
			pcTabDomain = new CTabDomain();
			pcTabDomain->Create( IDD_TAB_DOMAIN );
			pcTabDomain->ShowWindow( SW_SHOW );
			pcTabDomain->UpdateWindow();
		}
		break;

	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_RDP:
		{
			LoadConfig();
			if ( ! m_stConfig.bStartProtectRdp )
			{
				//
				//	�˲����������ù���
				//
				if ( GetCoreCfgCount( VWFIREWALLDRV_IOCTL_PRDP_CFGCOUNT ) > 0 )
				{
					//	�������ȷ�����ļ���������ڡ�Զ�����氲ȫ����Ԫ���б��ڣ�����������ʣ����򽫵�������Ҳ�޷�ͨ��Զ��������ʸü������\r\n\r\nȷ��Ҫ�����ù�����
					LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_CONFIRM, m_szCaption, sizeof(m_szCaption) );
					LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ALERT_RDP1, m_szString, sizeof(m_szString) );

					if ( IDYES ==
						MessageBox( m_szString, m_szCaption, MB_ICONQUESTION|MB_YESNO ) )
					{
						LoadConfig();
						SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTPRDP, 1 );
						DisplayCtrlButtonsStatus();

						//
						//	֪ͨ��������: װ���µ�������Ϣ
						//
						CTabBaseInfo::NotifyDriverToLoadNewConfig();
					}
				}
				else
				{
					//	�ǳ�Σ�գ�����û������������ʵļ���������⽫��������Ҳ�޷�ͨ��Զ��������ʸü�����������ڡ�Զ�����氲ȫ����Ԫ������ļ��������
					LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, m_szCaption, sizeof(m_szCaption) );
					LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ERR_RDP1, m_szString, sizeof(m_szString) );
					MessageBox( m_szString, m_szCaption, MB_ICONWARNING );
				}
			}
			else
			{
				SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTPRDP, 0 );
				DisplayCtrlButtonsStatus();

				//
				//	֪ͨ��������: װ���µ�������Ϣ
				//
				CTabBaseInfo::NotifyDriverToLoadNewConfig();
			}
		}
		break;
	case WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_RDP_SET:
		{
			CTabRdp * pcTabRdp;
			pcTabRdp = new CTabRdp();
			pcTabRdp->Create( IDD_TAB_RDP );
			pcTabRdp->ShowWindow( SW_SHOW );
			pcTabRdp->UpdateWindow();
		}
		break;
	}
	
	return S_OK;
}

void CTabSetting::OnTimer( UINT nIDEvent ) 
{
	if ( VWFIREWALLCFG_TIMER_UPDATE_LINKTEXT == nIDEvent )
	{
		KillTimer( VWFIREWALLCFG_TIMER_UPDATE_LINKTEXT );
	}	
	
	CDialog::OnTimer(nIDEvent);
}






//	��������ǽ
VOID CTabSetting::StartFirewall()
{
	LONG lnOp	= DELIBDRV_ERROR_UNKNOWN;
	TCHAR szTemp[ MAX_PATH ];

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		//
		//	������������������
		//
		lnOp = delib_drv_write_io( DEVIE_NAMEA, VWFIREWALLDRV_IOCTL_START, NULL, 0 );
		//lnOp = delib_drv_start( DEVIE_NAMEA );
		if ( DELIBDRV_ERROR_SUCC == lnOp )
		{
			//
			//	�������õ� INI �����ļ�
			//
			CVwFirewallConfigFile::SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_START, 1 );
		}
		else
		{
			//	����ʧ�ܣ��������: %l\r\n�����װ��δ��������������������������
			LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, m_szCaption, sizeof(m_szCaption) );
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ERR_START, m_szString, sizeof(m_szString) );
			_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, m_szString, lnOp );
			MessageBox( szTemp, m_szCaption, MB_ICONWARNING );
		}
	}
	else
	{
		//	���������������°�װ��\r\n�����װ��δ��������������������������
		LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, m_szCaption, sizeof(m_szCaption) );
		LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ERR_INS1, m_szString, sizeof(m_szString) );
		MessageBox( m_szString, m_szCaption, MB_ICONWARNING );
	}
}

//	ֹͣ����ǽ
VOID CTabSetting::StopFirewall()
{
	LONG lnOp	= DELIBDRV_ERROR_UNKNOWN;
	TCHAR szTemp[ MAX_PATH ];

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		//
		//	����������ֹͣ����
		//
		//lnOp = delib_drv_stop( DEVIE_NAMEA );
		lnOp = delib_drv_write_io( DEVIE_NAMEA, VWFIREWALLDRV_IOCTL_STOP, NULL, 0 );
		if ( DELIBDRV_ERROR_SUCC == lnOp )
		{
			//
			//	�������õ� INI �����ļ�
			//
			CVwFirewallConfigFile::SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_START, 0 );
		}
		else
		{
			//	ֹͣʧ�ܣ��������: %l\r\n�����װ��δ��������������������������
			LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, m_szCaption, sizeof(m_szCaption) );
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ERR_STOP, m_szString, sizeof(m_szString) );
			_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, m_szString, lnOp );
			MessageBox( szTemp, m_szCaption, MB_ICONWARNING );
		}
	}
	else
	{
		//	���������������°�װ��\r\n�����װ��δ��������������������������
		LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, m_szCaption, sizeof(m_szCaption) );
		LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_ERR_INS1, m_szString, sizeof(m_szString) );
		MessageBox( m_szString, m_szCaption, MB_ICONWARNING );
	}
}





unsigned __stdcall CTabSetting::_threadDisplayCoreStatus( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSetting * pThis = (CTabSetting*)arglist;
		if ( pThis )
		{
			pThis->DisplayCoreStatusProc( TRUE );
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CTabSetting::DisplayCoreStatusProc( BOOL bWarningIsapi, BOOL bFlashIcon /* = TRUE */ )
{
	TCHAR szString[ MAX_PATH ]	= {0};
	TCHAR szFilterPath[ MAX_PATH ]	= {0};

	__try
	{
		//
		//	load icon
		//
		if ( NULL == m_stStatusIcon.hIconO )
		{
			m_stStatusIcon.hIconO = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_OK), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR|LR_SHARED );
		}
		if ( NULL == m_stStatusIcon.hIconX )
		{
			m_stStatusIcon.hIconX = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_X), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR|LR_SHARED );
		}
		
		if ( NULL == m_stStatusIcon.hIconTrayRun )
		{
			m_stStatusIcon.hIconTrayRun = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SYSTRAY_RUN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
		}
		if ( NULL == m_stStatusIcon.hIconTrayRunning )
		{
			m_stStatusIcon.hIconTrayRunning = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SYSTRAY_RUNNING), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
		}
		if ( NULL == m_stStatusIcon.hIconTrayStopped )
		{
			m_stStatusIcon.hIconTrayStopped = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SYSTRAY_STOPPED), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	while( TRUE )
	{
		__try
		{
			//	��ʾ����ǽ״̬
			DisplayCoreStatusSub( bFlashIcon );

			//	��ʾ���ܰ�ť��״̬ 
			DisplayCtrlButtonsStatus();

			//	��ʾ����ͳ��
			DisplayCoreCountInfo();
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}

		//	...
		Sleep( 3000 );
	}
}

VOID CTabSetting::DisplayCoreStatusSub( BOOL bFlashIcon )
{
	TCHAR szTemp[ MAX_PATH ];
	TCHAR szFirewallStatus[ MAX_PATH ];
	TCHAR szNewSysTitle[ MAX_PATH ];
	BOOL  bCoreFwStart;

	bCoreFwStart = IsCoreStart() && IsFwStart();

	if ( ! m_stStatusCache.bCoreFwStartCached ||
		m_stStatusCache.bCoreFwStart != bCoreFwStart )
	{
		//	���浽����
		m_stStatusCache.bCoreFwStart		= bCoreFwStart;
		m_stStatusCache.bCoreFwStartCached	= TRUE;

		if ( bCoreFwStart )
		{
	//		m_btnStart.EnableWindow( FALSE );
	//		m_btnStop.EnableWindow( TRUE );
			
			//if ( ::IsWindowVisible( m_stcStatus.m_hWnd ) )
			{
				m_stcStatus.SetIcon( m_stStatusIcon.hIconO );
			}

			//	�رշ���ǽ
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_STOPFW, m_szString, sizeof(m_szString) );
			m_CtlLinkFirewall.SetWindowText( m_szString );
			m_CtlLinkFirewall.SetHint( m_szString );

			//	��������
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_RUNNING, m_szString, sizeof(m_szString) );
			_sntprintf( szNewSysTitle, sizeof(szNewSysTitle)/sizeof(TCHAR)-1, _T("%s - %s"), m_szAppName, m_szString );

			//	%s�������У�ϵͳ���ڱ�������
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_INFO1, szTemp, sizeof(szTemp) );
			_sntprintf( szFirewallStatus, sizeof(szFirewallStatus)/sizeof(TCHAR)-1, szTemp, m_szAppName );
			m_stcFirewallStatus.SetWindowText( szFirewallStatus );

			//	for tray icon	
			m_pxballoon->ModifyTrayIcon( m_stStatusIcon.hIconTrayRunning, szNewSysTitle );
			
			if ( bFlashIcon )
			{
				//	�Ƿ���˸��ʾ Icon �������е�Ч��
				Sleep( 1000 );
			}
			
			//	for tray icon
			m_pxballoon->ModifyTrayIcon( m_stStatusIcon.hIconTrayRun, szNewSysTitle );
		}
		else
		{
	//		m_btnStart.EnableWindow( TRUE );
	//		m_btnStop.EnableWindow( FALSE );
			
			//if ( ::IsWindowVisible( m_stcStatus.m_hWnd ) )
			{
				m_stcStatus.SetIcon( m_stStatusIcon.hIconX );
			}

			//	��������ǽ
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_STARTFW, m_szString, sizeof(m_szString) );
			m_CtlLinkFirewall.SetWindowText( m_szString );
			m_CtlLinkFirewall.SetHint( m_szString );

			//	��ֹͣ
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_STOPPED, m_szString, sizeof(m_szString) );
			_sntprintf( szNewSysTitle, sizeof(szNewSysTitle)/sizeof(TCHAR)-1, _T("%s - %s"), m_szAppName, m_szString );

			//	%s��ֹͣ��ϵͳ����Σ���У��뾡�쿪������ǽ
			LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_INFO2, szTemp, sizeof(szTemp) );
			_sntprintf( szFirewallStatus, sizeof(szFirewallStatus)/sizeof(TCHAR)-1, szTemp, m_szAppName );
			m_stcFirewallStatus.SetWindowText( szFirewallStatus );

			//	for tray icon
			m_pxballoon->ModifyTrayIcon( m_stStatusIcon.hIconTrayStopped, szNewSysTitle );
		}

		Invalidate();
	}
}

/**
 *	��ʾ���ܰ�ť�����ֺ�״̬
 */
VOID CTabSetting::DisplayCtrlButtonsStatus()
{
	TCHAR szStatusFile[ MAX_PATH ];
	TCHAR szStatusDomain[ MAX_PATH ];
	TCHAR szStatusRdp[ MAX_PATH ];
	TCHAR szProtectChecksum[ MAX_PATH ];
	DWORD dwRdpCfgCount;

	//
	//	load icon
	//
	if ( NULL == m_stStatusIcon.hIconStatusDomainOk )
	{
		m_stStatusIcon.hIconStatusDomainOk = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_DOMAIN_OK), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED );
	}
	if ( NULL == m_stStatusIcon.hIconStatusDomainX )
	{
		m_stStatusIcon.hIconStatusDomainX = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_DOMAIN_X), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED );
	}
	if ( NULL == m_stStatusIcon.hIconStatusRdpOk )
	{
		m_stStatusIcon.hIconStatusRdpOk = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_RDP_OK), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED );
	}
	if ( NULL == m_stStatusIcon.hIconStatusRdpX )
	{
		m_stStatusIcon.hIconStatusRdpX = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_RDP_X), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED );
	}
	if ( NULL == m_stStatusIcon.hIconStatusFileOk )
	{
		m_stStatusIcon.hIconStatusFileOk = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_FILE_OK), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED );
	}
	if ( NULL == m_stStatusIcon.hIconStatusFileX )
	{
		m_stStatusIcon.hIconStatusFileX = (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STATUS_FILE_X), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED );
	}

	if ( LoadConfig() )
	{
		dwRdpCfgCount = GetCoreCfgCount( VWFIREWALLDRV_IOCTL_PRDP_CFGCOUNT );
		_sntprintf
		(
			szProtectChecksum, sizeof(szProtectChecksum)/sizeof(TCHAR)-1, 
			_T("corefwstart=%d;startfilesafe=%d;startdm=%d;startrdp=%d;rdpcfgcount=%d"),
			( m_stStatusCache.bCoreFwStart ? 1 : 0 ),
			( m_stConfig.bStartProtectFile ? 1 : 0 ),
			( m_stConfig.bStartProtectDomain ? 1 : 0 ),
			( m_stConfig.bStartProtectRdp ? 1: 0 ),
			dwRdpCfgCount
		);

		if ( ! m_stStatusCache.bStartProtectCached ||
			0 != _tcsicmp( m_stStatusCache.szProtectChecksum, szProtectChecksum ) )
		{
			//	���浽 Cache
			_sntprintf( m_stStatusCache.szProtectChecksum, sizeof(m_stStatusCache.szProtectChecksum)/sizeof(TCHAR)-1, _T("%s"), szProtectChecksum );
			m_stStatusCache.bStartProtectCached	= TRUE;

			if ( m_stStatusCache.bCoreFwStart &&
				m_stConfig.bStartProtectFile )
			{
				//	�ļ�����ȫϵͳ�ѿ���
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_FILE_RUNNING, szStatusFile, sizeof(szStatusFile) );
				//	ֹͣ����
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_STOP_PROTECT, m_szString, sizeof(m_szString) );
				m_CtlLinkFile.SetWindowText( m_szString );
				m_CtlLinkFile.SetHint( m_szString );
				m_stcStatusFile.SetIcon( m_stStatusIcon.hIconStatusFileOk );
			}
			else
			{
				//	�ļ�����ȫϵͳ�ѹر�
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_FILE_STOPPED, szStatusFile, sizeof(szStatusFile) );
				//	��������
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_START_PROTECT, m_szString, sizeof(m_szString) );
				m_CtlLinkFile.SetWindowText( m_szString );
				m_CtlLinkFile.SetHint( m_szString );
				m_stcStatusFile.SetIcon( m_stStatusIcon.hIconStatusFileX );
			}


			if ( m_stStatusCache.bCoreFwStart &&
				m_stConfig.bStartProtectDomain )
			{
				//	����������ϵͳ�ѿ���
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_DM_RUNNING, szStatusDomain, sizeof(szStatusDomain) );
				//	ֹͣ����
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_STOP_PROTECT, m_szString, sizeof(m_szString) );
				m_CtlLinkDomain.SetWindowText( m_szString );
				m_CtlLinkDomain.SetHint( m_szString );
				m_stcStatusDomain.SetIcon( m_stStatusIcon.hIconStatusDomainOk );
			}
			else
			{
				//	����������ϵͳ�ѹر�
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_DM_STOPPED, szStatusDomain, sizeof(szStatusDomain) );
				//	��������
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_START_PROTECT, m_szString, sizeof(m_szString) );
				m_CtlLinkDomain.SetWindowText( m_szString );
				m_CtlLinkDomain.SetHint( m_szString );
				m_stcStatusDomain.SetIcon( m_stStatusIcon.hIconStatusDomainX );
			}

			if ( m_stStatusCache.bCoreFwStart &&
				m_stConfig.bStartProtectRdp && dwRdpCfgCount > 0 )
			{
				//	Զ�����氲ȫ�ѿ���
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_RDP_RUNNING, szStatusRdp, sizeof(szStatusRdp) );
				//	ֹͣ����
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_STOP_PROTECT, m_szString, sizeof(m_szString) );
				m_CtlLinkRdp.SetWindowText( m_szString );
				m_CtlLinkRdp.SetHint( m_szString );
				m_stcStatusRdp.SetIcon( m_stStatusIcon.hIconStatusRdpOk );
			}
			else
			{
				//	Զ�����氲ȫ�ѹر�
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_RDP_STOPPED, szStatusRdp, sizeof(szStatusRdp) );
				//	��������
				LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_START_PROTECT, m_szString, sizeof(m_szString) );
				m_CtlLinkRdp.SetWindowText( m_szString );
				m_CtlLinkRdp.SetHint( m_szString );
				m_stcStatusRdp.SetIcon( m_stStatusIcon.hIconStatusRdpX );
			}

			m_stcStatusFileTxt.SetWindowText( szStatusFile );
			m_stcStatusDomainTxt.SetWindowText( szStatusDomain );
			m_stcStatusRdpTxt.SetWindowText( szStatusRdp );

			Invalidate();
		}	
	}
}

/**
 *	��ʾ��������ͳ��
 */
VOID CTabSetting::DisplayCoreCountInfo()
{
	TCHAR szTemp[ MAX_PATH ];
	TCHAR szCountInfoChecksum[ MAX_PATH ];

	DWORD dwPFileCfgCount;
	DWORD dwPFileBlockCount;

	DWORD dwPDomainCfgCount;
	DWORD dwPDomainBlockCount;
	
	DWORD dwPRdpCfgCount;
	DWORD dwPRdpBlockCount;

	dwPFileCfgCount		= GetCoreCfgCount( VWFIREWALLDRV_IOCTL_PFILE_CFGCOUNT );
	dwPFileBlockCount	= GetCoreBlockCount( VWFIREWALLDRV_IOCTL_PFILE_BLOCKCOUNT );	

	dwPDomainCfgCount	= GetCoreCfgCount( VWFIREWALLDRV_IOCTL_PDOMAIN_CFGCOUNT );
	dwPDomainBlockCount	= GetCoreBlockCount( VWFIREWALLDRV_IOCTL_PDOMAIN_BLOCKCOUNT );

	dwPRdpCfgCount		= GetCoreCfgCount( VWFIREWALLDRV_IOCTL_PRDP_CFGCOUNT );
	dwPRdpBlockCount	= GetCoreBlockCount( VWFIREWALLDRV_IOCTL_PRDP_BLOCKCOUNT );

	_sntprintf
	(
		szCountInfoChecksum, sizeof(szCountInfoChecksum)/sizeof(TCHAR)-1, 
		_T("pfile_cfgcount=%d;pfile_blockcount=%d;pdomain_cfgcount=%d;pdomain_blockcount=%d;prdp_cfgcount=%d;prdp_blockcount=%d;"),
		dwPFileCfgCount, dwPFileBlockCount,
		dwPDomainCfgCount, dwPDomainBlockCount,
		dwPRdpCfgCount, dwPRdpBlockCount
	);
	if ( ! m_stStatusCache.bCountInfoCached ||
		0 != _tcsicmp( m_stStatusCache.szCountInfoChecksum, szCountInfoChecksum ) )
	{
		//	���浽 Cache
		_sntprintf( m_stStatusCache.szCountInfoChecksum, sizeof(m_stStatusCache.szCountInfoChecksum)/sizeof(TCHAR)-1, _T("%s"), szCountInfoChecksum );
		m_stStatusCache.bCountInfoCached = TRUE;

		//	������ %d ��,���������� %d ��
		LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_INFO3, m_szString, sizeof(m_szString) );
		_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, m_szString, dwPFileCfgCount, dwPFileBlockCount );
		m_stcPFileBlockCount.SetWindowText( szTemp );

		//	������ %d ��,���������� %d ��
		LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_INFO3, m_szString, sizeof(m_szString) );
		_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, m_szString, dwPDomainCfgCount, dwPDomainBlockCount );
		m_stcPDomainBlockCount.SetWindowText( szTemp );

		//	������ %d ��,���������� %d ��
		LoadString( AfxGetInstanceHandle(), IDS_TABSETTING_STR_INFO3, m_szString, sizeof(m_szString) );
		_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, m_szString, dwPRdpCfgCount, dwPRdpBlockCount );
		m_stcPRdpBlockCount.SetWindowText( szTemp );
	}
}

BOOL CTabSetting::IsFwStart()
{
	BOOL  bRet = FALSE;

	if ( LoadConfig() )
	{
		bRet = m_stConfig.bStart;
	}

	return bRet;
}

BOOL CTabSetting::IsCoreRunning()
{
	BOOL  bRet		= FALSE;
	ULONG ulStart		= 0;
	DWORD dwBytesReturned	= 0;

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		if ( SERVICE_RUNNING == delib_drv_get_status( DEVIE_NAMEA ) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CTabSetting::IsCoreStart()
{
	BOOL  bRet		= FALSE;
	ULONG ulStart		= 0;
	DWORD dwBytesReturned	= 0;

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		if ( SERVICE_RUNNING == delib_drv_get_status( DEVIE_NAMEA ) )
		{
			if ( DELIBDRV_ERROR_SUCC == 
				delib_drv_read_io( DEVIE_NAMEA, VWFIREWALLDRV_IOCTL_ISSTART, &ulStart, sizeof(ulStart), & dwBytesReturned )
			)
			{
				if ( sizeof(ulStart) == dwBytesReturned )
				{
					bRet = ( 1 == ulStart ? TRUE : FALSE );
				}
			}
		}
	}

	return bRet;
}

LONGLONG CTabSetting::GetCoreBlockCount( DWORD dwCountType )
{
	if ( VWFIREWALLDRV_IOCTL_PFILE_BLOCKCOUNT != dwCountType &&
		VWFIREWALLDRV_IOCTL_PDOMAIN_BLOCKCOUNT != dwCountType &&
		VWFIREWALLDRV_IOCTL_PRDP_BLOCKCOUNT != dwCountType )
	{
		return 0;
	}

	LONGLONG llRet	= 0;
	DWORD dwBytesReturned	= 0;

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		if ( SERVICE_RUNNING == delib_drv_get_status( DEVIE_NAMEA ) )
		{
			if ( DELIBDRV_ERROR_SUCC == 
				delib_drv_read_io( DEVIE_NAMEA, dwCountType, &llRet, sizeof(llRet), & dwBytesReturned ) )
			{
				if ( sizeof(llRet) != dwBytesReturned )
				{
					//	���󣬾ͷ��� 0
					llRet = 0;
				}
			}
		}
	}

	return llRet;
}

UINT CTabSetting::GetCoreCfgCount( DWORD dwCountType )
{
	if ( VWFIREWALLDRV_IOCTL_PFILE_CFGCOUNT != dwCountType &&
		VWFIREWALLDRV_IOCTL_PDOMAIN_CFGCOUNT != dwCountType &&
		VWFIREWALLDRV_IOCTL_PRDP_CFGCOUNT != dwCountType )
	{
		return 0;
	}

	UINT uRet	= 0;
	DWORD dwBytesReturned	= 0;

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		if ( SERVICE_RUNNING == delib_drv_get_status( DEVIE_NAMEA ) )
		{
			if ( DELIBDRV_ERROR_SUCC == 
				delib_drv_read_io( DEVIE_NAMEA, dwCountType, &uRet, sizeof(uRet), & dwBytesReturned ) )
			{
				if ( sizeof(uRet) != dwBytesReturned )
				{
					//	���󣬾ͷ��� 0
					uRet = 0;
				}
			}
		}
	}

	return uRet;
}

BOOL CTabSetting::LoadConfig()
{
	return CVwFirewallConfigFile::LoadConfig( &m_stConfig );
}

BOOL CTabSetting::SaveConfig()
{
	return FALSE;
	//return CVwFirewallConfigFile::SaveConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_START, ( m_bStart ? 1 : 0 ) );
}


