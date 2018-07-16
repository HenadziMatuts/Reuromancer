#pragma once
#include "afxcmn.h"
#include "stdint.h"
#include <vector>

typedef struct AnhItemData {
    uint8_t *m_pic;
    uint8_t *m_anh;
} AnhItemData;

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

    void BuildTree(FILE *fNeuroDat[2], int tab);
    uint8_t* GetWaveform();
    void ChangePageCleanUp();

private:
    std::vector<CBitmap*> m_Bitmaps;
    std::vector<uint8_t*> m_AudioWaveforms;
    std::vector<AnhItemData*> m_Anhs;

    HANDLE m_hAnhThread;

    void InsertIMHItems(HTREEITEM parent, char *name, uint8_t *bytes, uint32_t len);
    void InsertBitmapItem(HTREEITEM parent, wchar_t *name, uint8_t *bytes, uint32_t w, uint32_t h);
    
    void StoreBitmap(HTREEITEM item);
    void StoreWave(HTREEITEM item);

    void BuildBMPTree(FILE *fNeuroDat[2], int tab);
    void BuildANHTree(FILE *fNeuroDat[2]);
    void BuildSoundTree();
};
