#include "stdafx.h"
#include "ResourceBrowser.h"
#include "SoundFilterPage.h"
#include "Utilities.h"
#include <neuro_routines.h>

IMPLEMENT_DYNAMIC(CSoundFilterPage, CDlgFilterPage)

CSoundFilterPage::CSoundFilterPage()
{
}

CSoundFilterPage::~CSoundFilterPage()
{
    if (!m_AudioWaveforms.empty())
    {
        for (int i = 0; i < m_AudioWaveforms.size(); i++)
        {
            free(m_AudioWaveforms[i]);
        }

        m_AudioWaveforms.clear();
    }
}

BEGIN_MESSAGE_MAP(CSoundFilterPage, CDlgFilterPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTERTREE, &CSoundFilterPage::OnTvnSelchangedSound)
    ON_COMMAND(ID_EXPORTSELECTED_EXPORTSELECTED, &CSoundFilterPage::OnExportSound)
END_MESSAGE_MAP()

void CSoundFilterPage::OnTvnSelchangedSound(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM hSelected = m_FilterTree.GetSelectedItem();
    DWORD_PTR itemData = m_FilterTree.GetItemData(hSelected);

    if (itemData)
    {
        this->GetParent()->GetDlgItem(IDC_SLIDER_TRACKDUR)->ShowWindow(SW_SHOW);
        this->GetParent()->GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(SW_SHOW);
        this->GetParent()->GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_SHOW);
        this->GetParent()->GetDlgItem(IDC_STATIC_TRACKPOS)->ShowWindow(SW_SHOW);
        this->GetParent()->GetDlgItem(IDC_STATIC_TRACKLEN)->ShowWindow(SW_SHOW);

        /* stop if something is playing */
        this->GetParent()->GetDlgItem(IDC_BUTTON_STOP)->SendMessage(BM_CLICK);

        wchar_t szwTrackLen[8];
        swprintf_s(szwTrackLen, L"00:00");
        this->GetParent()->GetDlgItem(IDC_STATIC_TRACKPOS)->SetWindowTextW(szwTrackLen);

        wav_header_t *hdr = (wav_header_t*)m_FilterTree.GetItemData(hSelected);
        int samples = hdr->data_bytes;
        int trackLen_sec = samples / 44100;
        swprintf_s(szwTrackLen, L"/ %02d:%02d", trackLen_sec / 60, trackLen_sec % 60);
        this->GetParent()->GetDlgItem(IDC_STATIC_TRACKLEN)->SetWindowTextW(szwTrackLen);

        CSliderCtrl *slider = (CSliderCtrl*)this->GetParent()->GetDlgItem(IDC_SLIDER_TRACKDUR);
        slider->SetPos(0);
    }

    *pResult = 0;
}

void CSoundFilterPage::OnExportSound()
{
    HTREEITEM item = m_FilterTree.GetSelectedItem();
    uint8_t *wf = (uint8_t*)m_FilterTree.GetItemData(item);

    StoreWaveAsFile(wf, m_FilterTree.GetItemText(item));
}

void CSoundFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
{
    uint8_t *waveform = NULL;
    wchar_t itemName[16];
    int bytes = 0;

    /* 100 seconds of 44100hz, 8 bit, mono PCM */
    assert(waveform = (uint8_t*)calloc(100, 44100));

    for (int i = 1; i <= 14; i++)
    {
        int bytes = build_track_waveform(i, waveform, 44100 * 100);
        uint8_t *wf;
        assert(wf = (uint8_t*)calloc(1, bytes + sizeof(wav_header_t)));
        memmove(wf + sizeof(wav_header_t), waveform, bytes);

        wav_header_t *hdr = (wav_header_t*)wf;
        memmove(hdr, &WavHdrTemplate, sizeof(wav_header_t));
        hdr->wav_size = sizeof(wav_header_t) + bytes - 8;
        hdr->data_bytes = bytes;

        swprintf_s(itemName, L"TRACK_%02d.WAV", i);
        HTREEITEM item = m_FilterTree.InsertItem(itemName);
        m_FilterTree.SetItemData(item, (DWORD_PTR)wf);
        m_AudioWaveforms.push_back(wf);
    }

    free(waveform);
}

void CSoundFilterPage::ChangePageCleanUp()
{
    this->GetParent()->GetDlgItem(IDC_SLIDER_TRACKDUR)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_STATIC_TRACKPOS)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_STATIC_TRACKLEN)->ShowWindow(SW_HIDE);
}

uint8_t* CSoundFilterPage::GetWaveform()
{
    HTREEITEM hSelected = m_FilterTree.GetSelectedItem();
    DWORD_PTR itemData = m_FilterTree.GetItemData(hSelected);

    return itemData ? (uint8_t*)itemData : nullptr;
}
