// TabAbout.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabAbout dialog


CTabAbout::CTabAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CTabAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabAbout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTabAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabAbout)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabAbout, CDialog)
	//{{AFX_MSG_MAP(CTabAbout)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabAbout message handlers

BOOL CTabAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	//
	//	����
	//
	m_LinkAboutMore.SubclassDlgItem( IDC_STATIC_ABOUT_MORE, this );
	m_LinkAboutMore.SetColours( RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF) );
	m_LinkAboutMore.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkAboutMore.SetUnderline( TRUE );
	m_LinkAboutMore.SetVisited( FALSE );
	m_LinkAboutMore.SetURL( "http://rd.vidun.net/?4500" );



	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabAbout::OnLButtonDown(UINT nFlags, CPoint point) 
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

/**
 *	װ��������Ϣ
 */
BOOL CTabAbout::LoadConfig()
{
	return TRUE;
}

/**
 *	����������Ϣ
 */
BOOL CTabAbout::SaveConfig()
{
	return TRUE;
}


