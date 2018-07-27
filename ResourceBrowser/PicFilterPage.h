#pragma once
#include "DlgFilterPage.h"
#include <vector>

class CPicFilterPage : public CDlgFilterPage
{
    DECLARE_DYNAMIC(CPicFilterPage)

public:
    CPicFilterPage();
    virtual ~CPicFilterPage();

    afx_msg void OnTvnSelchangedPic(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnExportPic();

    void BuildTree(FILE *fNeuroDat[2], int tab);

protected:
    DECLARE_MESSAGE_MAP()

private:
    std::vector<CBitmap*> m_PicBitmaps;

    void InsertPicItems(HTREEITEM parent, wchar_t *name, uint8_t *bytes, uint32_t w, uint32_t h);
};
