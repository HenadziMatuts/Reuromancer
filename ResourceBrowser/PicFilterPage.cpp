#include "stdafx.h"
#include "ResourceBrowser.h"
#include "PicFilterPage.h"
#include "Utilities.h"
#include <neuro_routines.h>

IMPLEMENT_DYNAMIC(CPicFilterPage, CDlgFilterPage)

CPicFilterPage::CPicFilterPage()
{
}

CPicFilterPage::~CPicFilterPage()
{
    if (!m_PicBitmaps.empty())
    {
        for (int i = 0; i < m_PicBitmaps.size(); i++)
        {
            delete m_PicBitmaps[i];
        }

        m_PicBitmaps.clear();
    }
}

BEGIN_MESSAGE_MAP(CPicFilterPage, CDlgFilterPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTERTREE, &CPicFilterPage::OnTvnSelchangedPic)
    ON_COMMAND(ID_EXPORTSELECTED_EXPORTSELECTED, &CPicFilterPage::OnExportPic)
END_MESSAGE_MAP()

void CPicFilterPage::OnTvnSelchangedPic(NMHDR *pNMHDR, LRESULT *pResult)
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

void CPicFilterPage::OnExportPic()
{
    HTREEITEM item = m_FilterTree.GetSelectedItem();
    CBitmap *bmp = (CBitmap*)m_FilterTree.GetItemData(item);

    StoreBitmapAsFile(bmp, m_FilterTree.GetItemText(item));
}

void CPicFilterPage::InsertPicItems(HTREEITEM parent, wchar_t *name, uint8_t *bytes, uint32_t w, uint32_t h)
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
    m_PicBitmaps.push_back(bmp);
}

void CPicFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
{
    int i = 0, len = 0;
    HTREEITEM hRootNeuro[2];
    wchar_t wName[32] = { 0, };
    char name[32] = { 0, };
    uint8_t rsc[64000];

    hRootNeuro[0] = m_FilterTree.InsertItem(L"NEURO1.DAT");
    hRootNeuro[1] = m_FilterTree.InsertItem(L"NEURO2.DAT");
    m_FilterTree.SetItemData(hRootNeuro[0], (DWORD_PTR)0);
    m_FilterTree.SetItemData(hRootNeuro[1], (DWORD_PTR)0);

    while (g_res_pic[i].file != -1)
    {
        sprintf(name, "%s.BMP", g_res_pic[i].name);
        MultiByteToWideChar(CP_UTF8, 0, name, -1, wName, 32);
        len = DecompressResource(fNeuroDat[g_res_pic[i].file], &g_res_pic[i], rsc);
        if (len = DecompressResource(fNeuroDat[g_res_pic[i].file], &g_res_pic[i], rsc))
        {
            InsertPicItems(hRootNeuro[g_res_pic[i++].file], wName, rsc, 152, 112);
        }
    }

    m_FilterTree.Expand(hRootNeuro[0], TVE_EXPAND);
    m_FilterTree.Expand(hRootNeuro[1], TVE_EXPAND);
}
