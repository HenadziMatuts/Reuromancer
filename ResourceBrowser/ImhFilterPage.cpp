#include "stdafx.h"
#include "ResourceBrowser.h"
#include "ImhFilterPage.h"
#include "Utilities.h"
#include <neuro_routines.h>

IMPLEMENT_DYNAMIC(CImhFilterPage, CDlgFilterPage)

CImhFilterPage::CImhFilterPage()
{
}

CImhFilterPage::~CImhFilterPage()
{
    if (!m_ImhBitmaps.empty())
    {
        for (int i = 0; i < m_ImhBitmaps.size(); i++)
        {
            delete m_ImhBitmaps[i];
        }

        m_ImhBitmaps.clear();
    }
}

BEGIN_MESSAGE_MAP(CImhFilterPage, CDlgFilterPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTERTREE, &CImhFilterPage::OnTvnSelchangedImh)
    ON_COMMAND(ID_EXPORTSELECTED_EXPORTSELECTED, &CImhFilterPage::OnExportImh)
END_MESSAGE_MAP()

void CImhFilterPage::OnTvnSelchangedImh(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM hSelected = m_FilterTree.GetSelectedItem();
    DWORD_PTR itemData = m_FilterTree.GetItemData(hSelected);

    if (itemData)
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

void CImhFilterPage::OnExportImh()
{
    HTREEITEM item = m_FilterTree.GetSelectedItem();
    CBitmap *bmp = (CBitmap*)m_FilterTree.GetItemData(item);

    StoreBitmapAsFile(bmp, m_FilterTree.GetItemText(item));
}

void CImhFilterPage::InsertImhItems(HTREEITEM parent, char *name, uint8_t *bytes, uint32_t _len)
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
        m_ImhBitmaps.push_back(bmp);

        len += (sizeof(imh_hdr_t) + (bm_size / 2));
        p += (bm_size / 2);
    }
}

void CImhFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
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

    while (g_res_imh[i].file != -1)
    {
        MultiByteToWideChar(CP_UTF8, 0, g_res_imh[i].name, -1, wName, 32);
        hItem = m_FilterTree.InsertItem(wName, hRootNeuro[g_res_imh[i].file]);
        m_FilterTree.SetItemData(hItem, (DWORD_PTR)0);
        if (len = DecompressResource(fNeuroDat[g_res_imh[i].file], &g_res_imh[i], rsc))
        {
            InsertImhItems(hItem, g_res_imh[i++].name, rsc, len);
        }
    }

    m_FilterTree.Expand(hRootNeuro[0], TVE_EXPAND);
    m_FilterTree.Expand(hRootNeuro[1], TVE_EXPAND);
}
