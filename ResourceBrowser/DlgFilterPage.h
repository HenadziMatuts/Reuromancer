#pragma once
#include <afxcmn.h>
#include <stdint.h>
#include <vector>

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

    afx_msg void OnTvnSelchangedFiltertree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnRclickFiltertree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnExportselectedExportselected();

    virtual void BuildTree(FILE *fNeuroDat[2], int tab);
    virtual void ChangePageCleanUp();

private:
    std::vector<CBitmap*> m_Bitmaps;

    void InsertIMHItems(HTREEITEM parent, char *name, uint8_t *bytes, uint32_t len);
    void InsertBitmapItem(HTREEITEM parent, wchar_t *name, uint8_t *bytes, uint32_t w, uint32_t h);
    
    void StoreBitmap(HTREEITEM item);

    void BuildBMPTree(FILE *fNeuroDat[2], int tab);

};
