#pragma once
#include "DlgFilterPage.h"
#include <vector>

class CImhFilterPage : public CDlgFilterPage
{
    DECLARE_DYNAMIC(CImhFilterPage)

public:
    CImhFilterPage();
    virtual ~CImhFilterPage();

    afx_msg void OnTvnSelchangedImh(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnExportImh();

    void BuildTree(FILE *fNeuroDat[2], int tab);

protected:
    DECLARE_MESSAGE_MAP()

private:
    std::vector<CBitmap*> m_ImhBitmaps;

    void InsertImhItems(HTREEITEM parent, char *name, uint8_t *bytes, uint32_t len);
};
