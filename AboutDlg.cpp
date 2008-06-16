// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Error.h"

#include "AboutDlg.h"
#include "registry.h"

#include "ParentWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DisplayAboutDlg(CWnd* lpParentWnd)
{
	CAboutDlg AboutDlg(lpParentWnd);
	HRESULT hRes = S_OK;
	INT_PTR iDlgRet = 0;

	EC_D_DIALOG(AboutDlg.DoModal());
}

static TCHAR* CLASS = _T("CAboutDlg");

CAboutDlg::CAboutDlg(
								   CWnd* pParentWnd
								   ):CDialog(IDD_ABOUT,pParentWnd)
{
	TRACE_CONSTRUCTOR(CLASS);
	HRESULT hRes = S_OK;
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	EC_D(m_hIcon,AfxGetApp()->LoadIcon(IDR_MAINFRAME));
}

CAboutDlg::~CAboutDlg()
{
	TRACE_DESTRUCTOR(CLASS);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	HRESULT hRes = S_OK;
	EC_B(CDialog::OnInitDialog());
	TCHAR szFullPath[256];
	DWORD dwVerHnd = 0;
	DWORD dwRet = 0;
	DWORD dwVerInfoSize = 0;
	DWORD dwCheckHeight = GetSystemMetrics(SM_CYMENUCHECK);
	DWORD dwBorder = GetSystemMetrics(SM_CXEDGE);

	int		iRet = 0;
	static TCHAR szProductName[128];

	WC_D(iRet,LoadString(
		GetModuleHandle(NULL),
		ID_PRODUCTNAME,
		szProductName,
		CCH(szProductName)));

	SetWindowText(szProductName);

	CenterWindow();

	CRect MyTextRect;
	CRect MyBarRect;
	CRect MyCheckRect;

	//Find the shape of our client window
	GetClientRect(&MyTextRect);

	//Get Screen coords for the last bar
	CWnd* dlgWnd = GetDlgItem(IDD_BLACKBARLAST);
	if (dlgWnd) dlgWnd->GetWindowRect(&MyBarRect);
	//Convert those to client coords we need
	ScreenToClient(&MyBarRect);
	MyTextRect.DeflateRect(
		dwBorder,
		MyBarRect.bottom+dwBorder*2,
		dwBorder,
		dwBorder+dwCheckHeight+dwBorder*2);

	EC_B(m_HelpText.Create(
		WS_TABSTOP
		| WS_CHILD
		| WS_VISIBLE
		| WS_HSCROLL
		| WS_VSCROLL
		| ES_AUTOHSCROLL
		| ES_AUTOVSCROLL
		| ES_MULTILINE
		| ES_READONLY,
		MyTextRect,
		this,
		IDD_HELP));

	CString szHelpText;
	szHelpText.FormatMessage(IDS_HELPTEXT,szProductName);
	m_HelpText.SetWindowText(szHelpText);
	m_HelpText.SetFont(GetFont());

	MyCheckRect.SetRect(
		MyTextRect.left,
		MyTextRect.bottom+dwBorder,
		MyTextRect.right,
		MyTextRect.bottom+dwCheckHeight+dwBorder*2);

	EC_B(m_DisplayAboutCheck.Create(
		NULL,
		WS_TABSTOP
		| WS_CHILD
		| WS_CLIPSIBLINGS
		| WS_VISIBLE
		| BS_AUTOCHECKBOX,
		MyCheckRect,
		this,
		IDD_DISPLAYABOUT));
	m_DisplayAboutCheck.SetCheck(RegKeys[regkeyDISPLAY_ABOUT_DIALOG].ulCurDWORD?BST_CHECKED:BST_UNCHECKED);
	m_DisplayAboutCheck.SetFont(GetFont());
	CString szDisplayAboutCheck;
	szDisplayAboutCheck.LoadString(IDS_DISPLAYABOUTCHECK);
	m_DisplayAboutCheck.SetWindowText(szDisplayAboutCheck);

	// Get version information from the application.
	EC_D(dwRet,GetModuleFileName(NULL, szFullPath, CCH(szFullPath)));
	EC_D(dwVerInfoSize,GetFileVersionInfoSize(szFullPath, &dwVerHnd));
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it.
		int i = 0;
		size_t cchRoot = 0;

		BYTE* pbData = NULL;
		pbData = new BYTE[dwVerInfoSize];

		if (pbData)
		{
			BOOL bRet = false;
			EC_D(bRet,GetFileVersionInfo(szFullPath,
				dwVerHnd, dwVerInfoSize, (void*)pbData));

			struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate = {0};

			UINT	cbTranslate = 0;
			TCHAR	szGetName[256];

			// Read the list of languages and code pages.
			EC_B(VerQueryValue(
				pbData,
				_T("\\VarFileInfo\\Translation"),// STRING_OK
				(LPVOID*)&lpTranslate,
				&cbTranslate));

			// Read the file description for the first language/codepage
			if (S_OK == hRes && lpTranslate)
			{
				EC_H(StringCchPrintf(
					szGetName,
					CCH(szGetName),
					_T("\\StringFileInfo\\%04x%04x\\"),// STRING_OK
					lpTranslate[0].wLanguage,
					lpTranslate[0].wCodePage));

				EC_H(StringCchLength(szGetName,256,&cchRoot));

				// Walk through the dialog box items that we want to replace.
				if (!FAILED(hRes)) for (i = IDD_ABOUTVERFIRST; i <= IDD_ABOUTVERLAST; i++)
				{
					UINT  cchVer = 0;
					TCHAR*pszVer = NULL;
					TCHAR szResult[256];

					hRes = S_OK;

					UINT uiRet = 0;
					EC_D(uiRet,GetDlgItemText(i, szResult, CCH(szResult)));
					EC_H(StringCchCopy(&szGetName[cchRoot], CCH(szGetName)-cchRoot,szResult));

					EC_B(VerQueryValue(
						(void*)pbData,
						szGetName,
						(void**)&pszVer,
						&cchVer));

					if (S_OK == hRes && cchVer && pszVer)
					{
						// Replace the dialog box item text with version information.
						EC_H(StringCchCopy(szResult, CCH(szResult), pszVer));

						SetDlgItemText(i, szResult);
					}
				}
			}
			delete[] pbData;
		}
	}
	return HRES_TO_BOOL(hRes);
}

LRESULT CAboutDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
//	LRESULT lResult = NULL;

	switch (message)
	{
	case WM_HELP:
		return TRUE;
		break;
	}//end switch
	return CDialog::WindowProc(message,wParam,lParam);
}

void CAboutDlg::OnOK()
{
	CDialog::OnOK();
	int iCheckState = m_DisplayAboutCheck.GetCheck();

	if (BST_CHECKED == iCheckState)
		RegKeys[regkeyDISPLAY_ABOUT_DIALOG].ulCurDWORD = true;
	else
		RegKeys[regkeyDISPLAY_ABOUT_DIALOG].ulCurDWORD = false;
}