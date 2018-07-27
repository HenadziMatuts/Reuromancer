#pragma once
#include "DlgFilterPage.h"
#include <vector>

typedef struct AnhItemData {
    uint8_t *m_pic;
    uint8_t *m_anh;
} AnhItemData;

class CAnhFilterPage : public CDlgFilterPage
{
    DECLARE_DYNAMIC(CAnhFilterPage)

public:
    CAnhFilterPage();
    virtual ~CAnhFilterPage();

    afx_msg void OnTvnSelchangedAnh(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnExportAnh();

    void BuildTree(FILE *fNeuroDat[2], int tab);
    void ChangePageCleanUp();

protected:
    DECLARE_MESSAGE_MAP()

private:
    std::vector<AnhItemData*> m_Anhs;
    HANDLE m_hAnhThread;
};
