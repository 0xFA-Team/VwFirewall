// DlgProtectedDirAdd.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgProtectedDirAdd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProtectedDirAdd dialog


CDlgProtectedDirAdd::CDlgProtectedDirAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProtectedDirAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProtectedDirAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bUse	= TRUE;
	m_bTree	= TRUE;
	memset( m_szDir, 0, sizeof(m_szDir) );
	memset( m_szEcpExt, 0, sizeof(m_szEcpExt) );
}


void CDlgProtectedDirAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProtectedDirAdd)
	DDX_Control(pDX, IDC_CHECK_TREE, m_chkTree);
	DDX_Control(pDX, IDC_CHECK_USE, m_chkUse);
	DDX_Control(pDX, IDC_EDIT_ECPEXT, m_editEcpExt);
	DDX_Control(pDX, IDC_EDIT_DIR, m_editDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProtectedDirAdd, CDialog)
	//{{AFX_MSG_MAP(CDlgProtectedDirAdd)
	ON_BN_CLICKED(IDC_BUTTON_BROWSERDIR, OnButtonBrowserDir)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProtectedDirAdd message handlers

BOOL CDlgProtectedDirAdd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_chkUse.SetCheck( m_bUse );
	m_chkTree.SetCheck( m_bTree );
	m_editDir.SetWindowText( m_szDir );
	m_editEcpExt.SetWindowText( m_szEcpExt );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProtectedDirAdd::OnButtonBrowserDir() 
{
	CProcFunc cProcFunc;
	TCHAR szDir[ MAX_PATH ]	= {0};

	if ( cProcFunc.BrowseForFolder( "", szDir, sizeof(szDir) ) )
	{
		m_editDir.SetWindowText( szDir );
	}
}

void CDlgProtectedDirAdd::OnButtonSave() 
{
	TCHAR szWindowsDir[ MAX_PATH ];
	CProcFunc cProcFunc;
	m_bUse	= m_chkUse.GetCheck();
	m_bTree	= m_chkTree.GetCheck();

	memset( szWindowsDir, 0, sizeof(szWindowsDir) );
	memset( m_szDir, 0, sizeof(m_szDir) );
	memset( m_szEcpExt, 0, sizeof(m_szEcpExt) );


	GetWindowsDirectory( szWindowsDir, sizeof(szWindowsDir) );

	m_editDir.GetWindowText( m_szDir, sizeof(m_szDir) );
	StrTrim( m_szDir, _T(" \r\n\t") );

	m_editEcpExt.GetWindowText( m_szEcpExt, sizeof(m_szEcpExt) );
	StrTrim( m_szEcpExt, _T(" \r\n\t") );
	delib_replace( m_szEcpExt, _T(" "), _T("") );
	m_editEcpExt.SetWindowText( m_szEcpExt );

	if ( ! cProcFunc.IsValidProtectedDir( m_szDir ) )
	{
		MessageBox( "��������ȷ��Ŀ¼����Ҫ�������Ļ������������ַ���", "����", MB_ICONWARNING );
		m_editDir.SetSel( 0, -1 );
		m_editDir.SetFocus();
		return;
	}
	if ( 0 == _tcsnicmp( szWindowsDir, m_szDir, _tcslen(szWindowsDir) ) )
	{
		MessageBox( "ǿ�ҽ�������Ҫ����ϵͳĿ¼Ϊ������Ŀ¼���������𲻱�Ҫ���鷳������ϵͳ�޷��������С�", "����", MB_ICONWARNING );
		m_editDir.SetSel( 0, -1 );
		m_editDir.SetFocus();
		return;
	}

	if ( _tcslen( m_szEcpExt ) )
	{
		if ( cProcFunc.IsValidExceptedExt( m_szEcpExt ) )
		{
			if ( '.' != m_szEcpExt[ _tcslen(m_szEcpExt) -1 ] )
			{
				_tcscat( m_szEcpExt, "." );
			}
		}
		else
		{
			MessageBox( "��������ȷ��������չ����ÿ����չ�����Ȳ�Ҫ���� 6 ���ַ������ 127 ����չ����", "����", MB_ICONWARNING );
			m_editEcpExt.SetSel( 0, -1 );
			m_editEcpExt.SetFocus();
			return;
		}
	}

	OnOK();
}
