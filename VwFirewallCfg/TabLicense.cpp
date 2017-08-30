// TabLicense.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabLicense.h"
#include "DlgExpire.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabLicense dialog


CTabLicense::CTabLicense(CWnd* pParent /*=NULL*/)
	: CDialog(CTabLicense::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabLicense)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	//memset( m_szRegIp, 0, sizeof(m_szRegIp) );
	//memset( m_szRegHost, 0, sizeof(m_szRegHost) );
	//memset( m_szRegKey, 0, sizeof(m_szRegKey) );
	//memset( m_szRegCreateDate, 0, sizeof(m_szRegCreateDate) );
	//memset( m_szRegExpireDate, 0, sizeof(m_szRegExpireDate) );
	memset( m_szMsgCaption, 0, sizeof(m_szMsgCaption) );
	memset( m_szString, 0, sizeof(m_szString) );
}


void CTabLicense::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabLicense)
	DDX_Control(pDX, IDC_BUTTON_ONLINE_CHECK, m_btnOnlineCheck);
	DDX_Control(pDX, IDC_BUTTON_ENTER_KEY, m_btnEnterKey);
	DDX_Control(pDX, IDC_RICHEDIT_INFO, m_richeditInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabLicense, CDialog)
	//{{AFX_MSG_MAP(CTabLicense)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ENTER_KEY, OnButtonEnterKey)
	ON_BN_CLICKED(IDC_BUTTON_ONLINE_CHECK, OnButtonOnlineCheck)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabLicense message handlers

BOOL CTabLicense::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	m_btnOnlineCheck.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_LICENSE_CHECK) ), RGB(0xFF,0x00,0xFF) );
	m_btnOnlineCheck.SetFlat(FALSE);
	
	m_btnEnterKey.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_LICENSE_KEY) ), RGB(0xFF,0x00,0xFF) );
	m_btnEnterKey.SetFlat(FALSE);


	//
	//	����
	//
	m_LinkBuy.SubclassDlgItem( IDC_STATIC_HOWTOBUY, this );
	m_LinkBuy.SetColours( RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF) );
	m_LinkBuy.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkBuy.SetUnderline( TRUE );
	m_LinkBuy.SetVisited( FALSE );
	m_LinkBuy.SetURL( "http://rd.vidun.net/?id=4501" );


	// ..
	if ( LoadConfig() )
	{
		m_btnEnterKey.EnableWindow( TRUE );
		m_btnOnlineCheck.EnableWindow( TRUE );

		//	���ó�ʼע����Ϣ
		SetLicenseContent( TRUE, FALSE, m_stConfig.szRegPrType, m_stConfig.szRegCreateDate, m_stConfig.dwRegExpireType, m_stConfig.szRegExpireDate );
	}


	//
	//	��ʾ�û�����ʱ��
	//
	RenewalNotice();


	SetTimer( VWFIREWALLCFG_TIMER_KILLSEL, 100, NULL );
	
	return TRUE;
}

BOOL CTabLicense::PreTranslateMessage(MSG* pMsg) 
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


void CTabLicense::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CTabLicense::OnTimer(UINT nIDEvent) 
{
	if ( VWFIREWALLCFG_TIMER_KILLSEL == nIDEvent )
	{
		KillTimer( VWFIREWALLCFG_TIMER_KILLSEL );

		//	
		m_richeditInfo.SetSel( -1, -1 );
	}
	else if ( VWFIREWALLCFG_TIMER_ONLINECHECK == nIDEvent )
	{
		KillTimer( VWFIREWALLCFG_TIMER_ONLINECHECK );
		OnlineCheckNow();
	}

	CDialog::OnTimer(nIDEvent);
}




void CTabLicense::OnButtonEnterKey() 
{
	CDlgEnterKey dlg;
	if ( IDOK == dlg.DoModal() )
	{
		LoadConfig();
		SetLicenseContent( TRUE, FALSE, m_stConfig.szRegPrType, "" );
	}
}

void CTabLicense::OnButtonOnlineCheck() 
{
	SetTimer( VWFIREWALLCFG_TIMER_ONLINECHECK, 500, NULL );
	
	//	Please wait while the procedure is connecting to the verification server...
	LoadString( AfxGetInstanceHandle(), IDS_STRING_LICENSE_CONNSRV, m_szString, sizeof(m_szString) );
	m_richeditInfo.SetWindowText( m_szString );
	
	SetRichEditSpecColor( & m_richeditInfo, 0, m_richeditInfo.GetTextLength(), 0, RGB(0x00,0x00,0x00) );

	m_btnEnterKey.EnableWindow( FALSE );
	m_btnOnlineCheck.EnableWindow( FALSE );
}





void CTabLicense::OnlineCheckNow()
{
	TCHAR szPrName[ MAX_PATH ];
	TCHAR szParameter[ MAX_URL ]	= {0};
	TCHAR szMessage[ MAX_PATH ]	= {0};
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};

	STVWONLINELICENSECHECKDATA stLicenseData;

	// ..
	LoadConfig();

	memset( szPrName, 0, sizeof(szPrName) );
	memcpy( szPrName, g_szPrName_VwFirewall, min( sizeof(g_szPrName_VwFirewall), sizeof(szPrName) ) );
	delib_xorenc( szPrName );

	_sntprintf
	(
		szParameter, sizeof(szParameter)-sizeof(TCHAR),
		"pr=%s&ipaddr=%s&host=%s&regkey=%s&bdhost=%s",
		szPrName,	//	"vwpandomain"
		m_stConfig.szRegIp, m_stConfig.szRegHost, m_stConfig.szRegKey, ""
	);

	//	�����������ע������֤����
	memset( &stLicenseData, 0, sizeof(stLicenseData) );
	CVwOnlineLicenseCheck::PostVwOnlineLicenseCheckWithServer( szParameter, &stLicenseData );

	if ( 1 == stLicenseData.nResult )
	{
		//
		//	��ʽ�汾
		//
		SetLicenseContent( FALSE, TRUE, m_stConfig.szRegPrType, stLicenseData.szVwCdate, stLicenseData.nVwExpireType, stLicenseData.szVwExpireDate );
	}
	else if ( 0 == stLicenseData.nResult )
	{
		//
		//	����ʽ�����ߵ����û�
		//
		SetLicenseContent( FALSE, FALSE, m_stConfig.szRegPrType, stLicenseData.szVwCdate, stLicenseData.nVwExpireType, stLicenseData.szVwExpireDate );
	}
	else
	{
		//
		//	��������
		//

		//	Sorry, we were unable to connect to the verification server at this time. Please try again later.
		LoadString( AfxGetInstanceHandle(), IDS_STRING_LICENSE_FAILEDCONN, m_szString, sizeof(m_szString) );
		m_richeditInfo.SetWindowText( m_szString );
	}

	//
	//	�������ݿ��¼
	//
	if ( 1 == stLicenseData.nResult )
	{
		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGIP, m_stConfig.szRegIp );
		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGKEY, m_stConfig.szRegKey );

		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("%d"), stLicenseData.nVwExpireType );
		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIRETYPE, szTemp );

		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPE, stLicenseData.szVwPrType );
		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPECS, stLicenseData.szVwPrTypeChecksum );
		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGCREATEDATE, stLicenseData.szVwCdate );
		CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIREDATE, stLicenseData.szVwExpireDate );
	}

	//	֪ͨ�ײ��������¼�������
	//	...
	CTabBaseInfo::NotifyDriverToLoadNewConfig();

	//
	//	...
	//
	m_btnEnterKey.EnableWindow( TRUE );
	m_btnOnlineCheck.EnableWindow( TRUE );
}

BOOL CTabLicense::LoadConfig()
{
	BOOL bRet = FALSE;
	
	memset( &m_stConfig, 0, sizeof(m_stConfig) );
	if ( CVwFirewallConfigFile::LoadConfig( &m_stConfig ) )
	{
		//_sntprintf( m_szRegIp, sizeof(m_szRegIp)-sizeof(TCHAR), "%s", m_stConfig.szRegIp );
		//_sntprintf( m_szRegHost, sizeof(m_szRegHost)-sizeof(TCHAR), "%s", m_stConfig.szRegHost );
		//_sntprintf( m_szRegKey, sizeof(m_szRegKey)-sizeof(TCHAR), "%s", m_stConfig.szRegKey );
		//_sntprintf( m_szRegPrType, sizeof(m_szRegPrType)-sizeof(TCHAR), "%s", m_stConfig.szRegPrType );

		//m_dwRegExpireType	= m_stConfig.dwRegExpireType;
		//_sntprintf( m_szRegCreateDate, sizeof(m_szRegCreateDate)-sizeof(TCHAR), "%s", m_stConfig.szRegCreateDate );
		//_sntprintf( m_szRegExpireDate, sizeof(m_szRegExpireDate)-sizeof(TCHAR), "%s", m_stConfig.szRegExpireDate );
	}

	return TRUE;
}

VOID CTabLicense::SetLicenseContent( BOOL bInit, BOOL bEnterpriseVersion, LPCTSTR lpcszPrType, LPCTSTR lpcszCDate, LONG lnExpireType, TCHAR * lpszExpireDate )
{
	TCHAR szString[ 1024 ]		= {0};
	TCHAR szContent[ 1024 ]		= {0};
	TCHAR szStrNoLmt[ MAX_PATH ]	= {0};
	TCHAR szStrExpire[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	TCHAR szTempFmt[ MAX_PATH ]	= {0};
	TCHAR szPrTypeName[ 64 ]	= {0};
	LPSTR lpszMov			= NULL;
	INT nStart			= 0;
	INT nEnd			= 0;

	if ( ! bInit )
	{
		if ( bEnterpriseVersion )
		{
			LoadString( AfxGetInstanceHandle(), IDS_STRING_LICENSE_OK, szString, sizeof(szString) );
			strcat( szString, "\n\n" );
		}
		else
		{
			LoadString( AfxGetInstanceHandle(), IDS_STRING_LICENSE_EVALUATE, szString, sizeof(szString) );
			strcat( szString, "\n\n" );
		}	
	}

	// ..
	m_richeditInfo.SetWindowText( "" );
	nStart = strlen( szString );
	
	LoadString( AfxGetInstanceHandle(), IDS_STRING_REGNOLMT, szStrNoLmt, sizeof(szStrNoLmt) );
	LoadString( AfxGetInstanceHandle(), IDS_STRING_REGCONTENTS, szTemp, sizeof(szTemp) );
	
	if ( 0 == lnExpireType )
	{
		//	0 ��ʾ������
		_sntprintf( szStrExpire, sizeof(szStrExpire)-sizeof(TCHAR), "%s", szStrNoLmt );
	}
	else
	{
		//	1 ��ʾ������
		_sntprintf( szStrExpire, sizeof(szStrExpire)-sizeof(TCHAR), "%s", ( lpszExpireDate ? lpszExpireDate : "" ) );
	}
	
	//	��ȡ��Ʒ��������
	CVwLicence::GetPrTypeNameByCode( lpcszPrType, szPrTypeName, sizeof(szPrTypeName) );

	_sntprintf( szTempFmt, sizeof(szTempFmt)-sizeof(TCHAR), szTemp, m_stConfig.szRegIp, m_stConfig.szRegKey, szPrTypeName, lpcszCDate, szStrExpire );
	lpszMov = strstr( szTempFmt, "\n" );
	if ( lpszMov )
	{
		nEnd = nStart + lpszMov - szTempFmt;
		
		_sntprintf( szContent, sizeof(szContent)-sizeof(TCHAR), "%s%s", szString, szTempFmt );
		m_richeditInfo.SetWindowText( szContent );
		SetRichEditSpecBold( & m_richeditInfo, nStart, nEnd );
	}
}

//////////////////////////////////////////////////////////////////////////
// ��������Ϊ����
VOID CTabLicense::SetRichEditSpecBold( CRichEditCtrl * pRichedit, int nStart, int nEnd )
{
	if ( NULL == pRichedit )
		return;
	
	CHARFORMAT cf;
	memset( &cf, 0, sizeof(cf) );
	cf.cbSize = sizeof( cf );
	pRichedit->GetDefaultCharFormat( cf );
	
	cf.dwMask |= CFM_BOLD | CFM_COLOR | CFM_PROTECTED;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.dwEffects |= CFE_PROTECTED;
	cf.dwEffects |= CFE_BOLD;
	
	pRichedit->SetSel( nStart, nEnd );
	pRichedit->SetSelectionCharFormat( cf );
	pRichedit->SetSel( -1, -1 );
}

//////////////////////////////////////////////////////////////////////////
// ����������ɫ
VOID CTabLicense::SetRichEditSpecColor( CRichEditCtrl * pRichedit, int nStart, int nEnd, DWORD dwFontStyle, COLORREF crTextColor )
{
	if ( NULL == pRichedit )
		return;
	
	CHARFORMAT cf;
	memset( &cf, 0, sizeof(cf) );
	cf.cbSize = sizeof( cf );
	pRichedit->GetDefaultCharFormat( cf );
	
	cf.dwMask |= CFM_BOLD | CFM_COLOR | CFM_PROTECTED;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.dwEffects |= CFE_PROTECTED;
	cf.dwEffects |= dwFontStyle;
	cf.crTextColor = crTextColor;
	
	pRichedit->SetSel( nStart, nEnd );
	pRichedit->SetSelectionCharFormat( cf );
	pRichedit->SetSel( -1, -1 );
}




/**
*	�߷�����
*/
VOID CTabLicense::RenewalNotice()
{
	CDlgExpire dlg;
	SYSTEMTIME st;
	COleDateTime cOldTmRegExpireDate;
	COleDateTime cOldTmNow;
	COleDateTimeSpan cOldTmSpan;
	
	//
	//	�������û�
	//	m_szRegExpireDate ��ʽ���� "2016-1-1"
	//	m_dwRegExpireType = 1 ��ʾ�������û�
	//
	if ( _tcslen( m_stConfig.szRegExpireDate ) && 1 == m_stConfig.dwRegExpireType )
	{
		//	��ȡ����ʱ��
		GetLocalTime( &st );
		cOldTmNow.SetDateTime( st.wYear, st.wMonth, st.wDay, 0, 0, 0 );
		
		cOldTmRegExpireDate.ParseDateTime( m_stConfig.szRegExpireDate );
		if ( COleDateTime::valid == cOldTmRegExpireDate.GetStatus() )
		{
			//VariantTimeToSystemTime( cOldTmRegExpireDate, &st );
			
			//	�������� COleDateTimes ʱ��Ĳ�ֵ
			cOldTmSpan = cOldTmRegExpireDate - cOldTmNow;
			
			if ( cOldTmSpan.GetTotalDays() < 30 )
			{
				_sntprintf( dlg.m_szRegExpireDate, sizeof(dlg.m_szRegExpireDate)-sizeof(TCHAR), "%s", m_stConfig.szRegExpireDate );
				dlg.DoModal();
			}
		}
	}
}

