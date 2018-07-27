// DlgFilterPage.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceBrowser.h"
#include "DlgFilterPage.h"
#include "Utilities.h"
#include <errno.h>
#include <neuro_routines.h>

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

void CDlgFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
{
    switch (tab)
    {
    case TAB_IMH:
    case TAB_PIC:
        BuildBMPTree(fNeuroDat, tab);
        break;

    default:
        break;
    }
}

void CDlgFilterPage::ChangePageCleanUp()
{
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
