#pragma once
#include <afxcmn.h>
#include <stdint.h>

// CDlgFilterPage dialog

class CDlgFilterPage : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgFilterPage)

public:
    CDlgFilterPage(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgFilterPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FILTERPAGE };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

public:
    CTreeCtrl m_FilterTree;

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnRclickFiltertree(NMHDR *pNMHDR, LRESULT *pResult);

    virtual void BuildTree(FILE *fNeuroDat[2], int tab);
    virtual void ChangePageCleanUp();
};
