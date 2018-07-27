#pragma once
#include "DlgFilterPage.h"
#include <vector>

class CSoundFilterPage : public CDlgFilterPage
{
    DECLARE_DYNAMIC(CSoundFilterPage)

public:
    CSoundFilterPage();
    virtual ~CSoundFilterPage();

    afx_msg void OnTvnSelchangedSound(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnExportSound();

    void BuildTree(FILE *fNeuroDat[2], int tab);
    void ChangePageCleanUp();

    uint8_t* GetWaveform();

protected:
    DECLARE_MESSAGE_MAP()

private:
    std::vector<uint8_t*> m_AudioWaveforms;
};
