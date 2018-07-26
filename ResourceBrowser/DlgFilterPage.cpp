// DlgFilterPage.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceBrowser.h"
#include "DlgFilterPage.h"
#include "afxdialogex.h"
#include <errno.h>

#pragma pack(2)
typedef struct bmp_hdr_t {
    uint16_t type;
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t comp;
    uint32_t size_image;
    uint32_t xppm;
    uint32_t yppm;
    uint32_t colors_used;
    uint32_t colors_important;
} bmp_hdr_t;

typedef struct wav_header_t {
    char riff_header[4];    /* "RIFF" */
    int wav_size;           /* File size - 8 */
    char wave_header[4];    /* "WAVE" */
    char fmt_header[4];     /* "fmt " */
    int fmt_chunk_size;     /* 16 for PCM */
    short audio_format;     /* 1 for PCM */
    short num_channels;
    int sample_rate;
    int byte_rate;
    short sample_alignment; /* num_channels * byte_rate */
    short bit_depth;
    char data_header[4];    /* "data" */
    int data_bytes;
} wav_header_t;

uint8_t DosPal[1024] = {
    0x00, 0x00, 0x00, 0x00, // black
    0x80, 0x00, 0x00, 0x00, // blue
    0x00, 0x80, 0x00, 0x00, // green
    0x80, 0x80, 0x00, 0x00, // cyan
    0x00, 0x00, 0x80, 0x00, // red
    0x80, 0x00, 0x80, 0x00, // magnetta
    0x00, 0x80, 0x80, 0x00, // brown
    0xC0, 0xC0, 0xC0, 0x00, // light gray
    0x80, 0x80, 0x80, 0x00, // dark gray
    0xFF, 0x00, 0x00, 0x00, // light blue
    0x00, 0xFF, 0x00, 0x00, // light green
    0xFF, 0xFF, 0x00, 0x00, // light cyan
    0x00, 0x00, 0xFF, 0x00, // light red
    0xFF, 0x00, 0xFF, 0x00, // light magnetta
    0x00, 0xFF, 0xFF, 0x00, // yellow
    0xFF, 0xFF, 0xFF, 0x00, // white
    0x00,
};

BOOL Convert8bppTo32bpp(CBitmap *src, uint8_t *pal, CBitmap *dst)
{
    BITMAP bm8bpp;
    uint8_t bpp8bits[64000] = { 0, }, *bpp32bits = NULL, *p8 = bpp8bits, *p32 = NULL;
    int bpp8size = 0;

    src->GetBitmap(&bm8bpp);
    bpp8size = bm8bpp.bmWidth * bm8bpp.bmHeight;
    assert(bpp32bits = new uint8_t[bpp8size * 4]);
    if (!bpp32bits)
    {
        return 0;
    }
    src->GetBitmapBits(bpp8size, bpp8bits);

    p32 = bpp32bits;
    for (int i = 0; i < bpp8size; i++)
    {
        int c = *p8++;
        memmove(p32, &pal[c * 4], 4);
        p32 += 4;
    }

    dst->CreateBitmap(bm8bpp.bmWidth, bm8bpp.bmHeight, 1, 32, bpp32bits);
    delete[] bpp32bits;

    return TRUE;
}

int CDlgFilterPage::DecompressResource(FILE *f, resource_t *src, uint8_t *dst)
{
    uint8_t compd[64000];

    if (strstr(src->name, ".PIC") || strstr(src->name, ".IMH"))
    {
        fseek(f, src->offset + 32, SEEK_SET);
    }
    else
    {
        fseek(f, src->offset, SEEK_SET);
    }
    fread(compd, 1, src->size, f);

    if (strstr(src->name, ".IMH"))
    {
        return decompress_imh(compd, dst);
    }
    else
    if (strstr(src->name, ".PIC"))
    {
        return decompress_pic(compd, dst);
    }
    else
    if (strstr(src->name, ".BIH"))
    {
        return decompress_bih(compd, dst);
    }
    else
    {
        return decompress_anh(compd, dst);
    }
}

// CDlgFilterPage dialog

IMPLEMENT_DYNAMIC(CDlgFilterPage, CDialogEx)

CDlgFilterPage::CDlgFilterPage(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_FILTERPAGE, pParent)
{
}

CDlgFilterPage::~CDlgFilterPage()
{
    if (!m_Bitmaps.empty())
    {
        for (int i = 0; i < m_Bitmaps.size(); i++)
        {
            delete m_Bitmaps[i];
        }

        m_Bitmaps.clear();
    }

    if (!m_AudioWaveforms.empty())
    {
        for (int i = 0; i < m_AudioWaveforms.size(); i++)
        {
            free(m_AudioWaveforms[i]);
        }

        m_AudioWaveforms.clear();
    }
}

void CDlgFilterPage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILTERTREE, m_FilterTree);
}


BEGIN_MESSAGE_MAP(CDlgFilterPage, CDialogEx)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTERTREE, &CDlgFilterPage::OnTvnSelchangedFiltertree)
    ON_WM_SIZE()
    ON_NOTIFY(NM_RCLICK, IDC_FILTERTREE, &CDlgFilterPage::OnRclickFiltertree)
    ON_COMMAND(ID_EXPORTSELECTED_EXPORTSELECTED, &CDlgFilterPage::OnExportselectedExportselected)
END_MESSAGE_MAP()


// CDlgFilterPage message handlers


void CDlgFilterPage::OnTvnSelchangedFiltertree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    HTREEITEM hSelected = m_FilterTree.GetSelectedItem();
    DWORD_PTR itemData = m_FilterTree.GetItemData(hSelected);

    this->GetParent()->GetDlgItem(IDC_SLIDER_TRACKDUR)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_STATIC_TRACKPOS)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_STATIC_TRACKLEN)->ShowWindow(SW_HIDE);

    if (!itemData)
    {
        return;
    }

    if (m_FilterTree.GetItemText(hSelected).Find(L".BMP") != -1)
    {
        CBitmap *bpp8 = (CBitmap*)itemData;
        CBitmap bpp32;

        if (Convert8bppTo32bpp(bpp8, DosPal, &bpp32))
        {
            CStatic *preview = (CStatic*)this->GetParent()->GetDlgItem(IDC_PREVIEWFRAME);
            preview->SetBitmap(bpp32);
            bpp32.DeleteObject();
        }
    }
    else if (m_FilterTree.GetItemText(hSelected).Find(L".WAV") != -1)
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


void CDlgFilterPage::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (m_FilterTree.m_hWnd)
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        m_FilterTree.MoveWindow(&rcClient);
    }
}

void CDlgFilterPage::InsertBitmapItem(HTREEITEM parent, wchar_t *name, uint8_t *bytes, uint32_t w, uint32_t h)
{
    uint8_t bits[64000] = { 0, }, *p = bytes;
    uint32_t size = w * h * 2;
    HTREEITEM item;
    CBitmap *bmp = NULL;

    for (uint32_t u = 0, i = 0; u < size / 2; u++)
    {
        bits[i++] = p[u] >> 4;
        bits[i++] = p[u] & 0x0F;
    }

    item = m_FilterTree.InsertItem(name, parent);

    /* That is a 8bpp bitmaps */
    assert(bmp = new CBitmap());
    bmp->CreateBitmap(w * 2, h, 1, 8, bits);
    m_FilterTree.SetItemData(item, (DWORD_PTR)bmp);
    m_Bitmaps.push_back(bmp);
}

void CDlgFilterPage::InsertIMHItems(HTREEITEM parent, char *name, uint8_t *bytes, uint32_t _len)
{
    uint8_t bits[64000] = { 0, }, *p = bytes;
    uint32_t bm_size = 0, len = 0;
    imh_hdr_t *rle = NULL;
    char label[2048] = { 0, };
    wchar_t wLabel[64] = { 0, };
    CBitmap *bmp = NULL;
    HTREEITEM item;

    while (len != _len)
    {
        rle = (imh_hdr_t*)p;
        bm_size = rle->height * rle->width * 2;

        p += sizeof(imh_hdr_t);

        for (uint32_t u = 0, i = 0; u < bm_size / 2; u++)
        {
            bits[i++] = p[u] >> 4;
            bits[i++] = p[u] & 0x0F;
        }

        sprintf(label, "%s_%d.BMP", name, len);
        MultiByteToWideChar(CP_UTF8, 0, label, -1, wLabel, 64);
        item = m_FilterTree.InsertItem(wLabel, parent);

        /* That is a 8bpp bitmaps */
        assert(bmp = new CBitmap());
        bmp->CreateBitmap(rle->width * 2, rle->height, 1, 8, bits);
        m_FilterTree.SetItemData(item, (DWORD_PTR)bmp);
        m_Bitmaps.push_back(bmp);

        len += (sizeof(imh_hdr_t) + (bm_size / 2));
        p += (bm_size / 2);
    }
}

void CDlgFilterPage::BuildBMPTree(FILE *fNeuroDat[2], int tab)
{
    int i = 0, len = 0;
    HTREEITEM hRootNeuro[2], hItem;
    wchar_t wName[32] = { 0, };
    char name[32] = { 0, };
    uint8_t rsc[64000];

    hRootNeuro[0] = m_FilterTree.InsertItem(L"NEURO1.DAT");
    hRootNeuro[1] = m_FilterTree.InsertItem(L"NEURO2.DAT");
    m_FilterTree.SetItemData(hRootNeuro[0], (DWORD_PTR)0);
    m_FilterTree.SetItemData(hRootNeuro[1], (DWORD_PTR)0);

    switch (tab)
    {
    case TAB_IMH:
        while (g_res_imh[i].file != -1)
        {
            MultiByteToWideChar(CP_UTF8, 0, g_res_imh[i].name, -1, wName, 32);
            hItem = m_FilterTree.InsertItem(wName, hRootNeuro[g_res_imh[i].file]);
            m_FilterTree.SetItemData(hItem, (DWORD_PTR)0);
            if (len = DecompressResource(fNeuroDat[g_res_imh[i].file], &g_res_imh[i], rsc))
            {
                InsertIMHItems(hItem, g_res_imh[i++].name, rsc, len);
            }
        }
        break;

    case TAB_PIC:
        while (g_res_pic[i].file != -1)
        {
            sprintf(name, "%s.BMP", g_res_pic[i].name);
            MultiByteToWideChar(CP_UTF8, 0, name, -1, wName, 32);
            len = DecompressResource(fNeuroDat[g_res_pic[i].file], &g_res_pic[i], rsc);
            if (len = DecompressResource(fNeuroDat[g_res_pic[i].file], &g_res_pic[i], rsc))
            {
                InsertBitmapItem(hRootNeuro[g_res_pic[i++].file], wName, rsc, 152, 112);
            }
        }
        break;

    default:
        break;
    }

    m_FilterTree.Expand(hRootNeuro[0], TVE_EXPAND);
    m_FilterTree.Expand(hRootNeuro[1], TVE_EXPAND);
}

void CDlgFilterPage::BuildSoundTree()
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
        strcpy(hdr->riff_header, "RIFF");
        hdr->wav_size = sizeof(wav_header_t) + bytes - 8;
        strcpy(hdr->wave_header, "WAVE");
        strcpy(hdr->fmt_header, "fmt ");
        hdr->fmt_chunk_size = 16;
        hdr->audio_format = 1;
        hdr->num_channels = 1;
        hdr->sample_rate = 44100;
        hdr->byte_rate = 44100;
        hdr->sample_alignment = 1;
        hdr->bit_depth = 8;
        strcpy(hdr->data_header, "data");
        hdr->data_bytes = bytes;

        swprintf_s(itemName, L"TRACK_%02d.WAV", i);
        HTREEITEM item = m_FilterTree.InsertItem(itemName);
        m_FilterTree.SetItemData(item, (DWORD_PTR)wf);
        m_AudioWaveforms.push_back(wf);
    }

    free(waveform);
}

void CDlgFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
{
    switch (tab)
    {
    case TAB_IMH:
    case TAB_PIC:
        BuildBMPTree(fNeuroDat, tab);
        break;

    case TAB_SOUND:
        BuildSoundTree();
        break;

    default:
        break;
    }
}

uint8_t* CDlgFilterPage::GetWaveform()
{
    HTREEITEM hSelected = m_FilterTree.GetSelectedItem();
    DWORD_PTR itemData = m_FilterTree.GetItemData(hSelected);

    if (!itemData || m_FilterTree.GetItemText(hSelected).Find(L".WAV") == -1)
    {
        return nullptr;
    }
    else
    {
        return (uint8_t*)itemData;
    }
}

void CDlgFilterPage::ChangePageCleanUp()
{

    this->GetParent()->GetDlgItem(IDC_SLIDER_TRACKDUR)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_STATIC_TRACKPOS)->ShowWindow(SW_HIDE);
    this->GetParent()->GetDlgItem(IDC_STATIC_TRACKLEN)->ShowWindow(SW_HIDE);
}

void CDlgFilterPage::StoreBitmap(HTREEITEM item)
{
    CFileDialog saveDlg(FALSE, NULL, m_FilterTree.GetItemText(item), OFN_OVERWRITEPROMPT, NULL);
    CBitmap *bmp = (CBitmap*)m_FilterTree.GetItemData(item);
    BITMAP bm;
    bmp_hdr_t hdr;
    uint8_t *bits = NULL;

    bmp->GetBitmap(&bm);
    assert(bits = new uint8_t[bm.bmWidth * bm.bmHeight]);

    memset(&hdr, 0, sizeof(bmp_hdr_t));
    hdr.type = 0x4D42; /* "BM" */
    hdr.size = (sizeof(hdr) + 1024 + (bm.bmWidth * bm.bmHeight));
    hdr.offset = 0x0436;
    hdr.header_size = 40;
    hdr.width = bm.bmWidth;
    hdr.height = bm.bmHeight;
    hdr.planes = 1;
    hdr.bpp = 8;
    hdr.size_image = bm.bmWidth * bm.bmHeight;
    hdr.colors_used = 256;
    hdr.colors_important = 256;

    if (saveDlg.DoModal() == IDOK)
    {
        FILE *f = NULL;
        char path[2048];
        CT2A fileName(saveDlg.GetFileName(), CP_UTF8);
        CT2A filePath(saveDlg.GetFolderPath(), CP_UTF8);

        sprintf(path, "%s\\%s", filePath.m_psz, fileName.m_psz);
        bmp->GetBitmapBits(bm.bmWidth * bm.bmHeight, bits);

        assert(f = fopen(path, "wb"));
        fwrite(&hdr, 1, sizeof(bmp_hdr_t), f);
        fwrite(DosPal, 1, 1024, f);

        for (int i = bm.bmHeight - 1; i >= 0; i--)
        {
            fwrite(&bits[i * bm.bmWidth], 1, bm.bmWidth, f);
        }
        fclose(f);
    }

    delete[] bits;
}

void CDlgFilterPage::StoreWave(HTREEITEM item)
{
    CFileDialog saveDlg(FALSE, NULL, m_FilterTree.GetItemText(item), OFN_OVERWRITEPROMPT, NULL);
    uint8_t *wf = (uint8_t*)m_FilterTree.GetItemData(item);
    wav_header_t *hdr = (wav_header_t*)wf;

    if (saveDlg.DoModal() == IDOK)
    {
        FILE *f = NULL;
        char path[2048];
        CT2A fileName(saveDlg.GetFileName(), CP_UTF8);
        CT2A filePath(saveDlg.GetFolderPath(), CP_UTF8);

        sprintf(path, "%s\\%s", filePath.m_psz, fileName.m_psz);

        assert(f = fopen(path, "wb"));
        fwrite(wf, 1, sizeof(wav_header_t) + hdr->data_bytes, f);
        fclose(f);
    }
}

void CDlgFilterPage::OnExportselectedExportselected()
{
    // TODO: Add your command handler code here
    HTREEITEM item = m_FilterTree.GetSelectedItem();

    if (!item || !m_FilterTree.GetItemData(item))
    {
        return;
    }
    m_FilterTree.GetItemText(item);
    if (m_FilterTree.GetItemText(item).Find(L".BMP") != -1)
    {
        StoreBitmap(item);
    }
    else if (m_FilterTree.GetItemText(item).Find(L".WAV") != -1)
    {
        StoreWave(item);
    }
    else
    {
        MessageBox(L"Not supported Yet :(");
    }
}

void CDlgFilterPage::OnRclickFiltertree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    HTREEITEM item;
    POINT p;
    UINT flags;

    GetCursorPos(&p);
    ScreenToClient(&p);

    item = m_FilterTree.HitTest(p, &flags);
    if (item && (flags & TVHT_ONITEM) && m_FilterTree.GetItemData(item))
    {
        m_FilterTree.SelectItem(item);
        CMenu menu;
        menu.LoadMenuW(IDR_POPUP);

        CMenu *popup = menu.GetSubMenu(0);
        ClientToScreen(&p);
        popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, this);
    }

    *pResult = 0;
}
