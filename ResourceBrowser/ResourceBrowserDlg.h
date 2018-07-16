
// ResourceBrowserDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "DlgFilterPage.h"
#include "afxwin.h"
#include <mmsystem.h>
#include <SFML\Audio.h>

// CResourceBrowserDlg dialog
class CResourceBrowserDlg : public CDialogEx
{
// Construction
public:
    CResourceBrowserDlg(CWnd* pParent = NULL);	// standard constructor
    ~CResourceBrowserDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_RESOURCEBROWSER_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTcnSelchangeFiltertab(NMHDR *pNMHDR, LRESULT *pResult);
    CDlgFilterPage *m_Page[TAB_TOTAL], *m_CurrentPage;
    CTabCtrl m_FilterTab;
    CStatic m_Preview;

    sfMusic *m_sfMusicPlayer;
    HANDLE m_PlayerThread;

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonPlay();
    afx_msg void OnBnClickedButtonStop();
};
