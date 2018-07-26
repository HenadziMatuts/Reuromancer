#pragma once
#include "afxcmn.h"
#include "stdint.h"
#include <vector>
#include <neuro_routines.h>

extern uint8_t DosPal[1024];
BOOL Convert8bppTo32bpp(CBitmap *src, uint8_t *pal, CBitmap *dst);

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

    int DecompressResource(FILE *f, resource_t *src, uint8_t *dst);

public:
    CTreeCtrl m_FilterTree;

    afx_msg void OnTvnSelchangedFiltertree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnRclickFiltertree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnExportselectedExportselected();

    virtual void BuildTree(FILE *fNeuroDat[2], int tab);
    virtual void ChangePageCleanUp();

    uint8_t* GetWaveform();

private:
    std::vector<CBitmap*> m_Bitmaps;
    std::vector<uint8_t*> m_AudioWaveforms;

    void InsertIMHItems(HTREEITEM parent, char *name, uint8_t *bytes, uint32_t len);
    void InsertBitmapItem(HTREEITEM parent, wchar_t *name, uint8_t *bytes, uint32_t w, uint32_t h);
    
    void StoreBitmap(HTREEITEM item);
    void StoreWave(HTREEITEM item);

    void BuildBMPTree(FILE *fNeuroDat[2], int tab);
    void BuildSoundTree();

};
