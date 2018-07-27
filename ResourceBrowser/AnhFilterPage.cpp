#include "stdafx.h"
#include "ResourceBrowser.h"
#include "AnhFilterPage.h"
#include "Utilities.h"
#include <neuro_routines.h>

typedef struct AnhThreadData {
    AnhItemData *m_item;
    CStatic *m_preview;
} AnhThreadData;

DWORD WINAPI AnhThreadProc(_In_ LPVOID lpParameter)
{
    AnhThreadData data;
    memmove(&data, lpParameter, sizeof(AnhThreadData));
    delete (AnhThreadData*)lpParameter;

    CBitmap bpp8;
    CBitmap bpp32;
    bg_animation_control_table_t anim_ctl[8];
    uint8_t bits[152 * 112 * 2], pic[152 * 112];
    uint8_t anim_bytes[8192];

    uint16_t animations = bg_animation_init_tables(anim_ctl, data.m_item->m_anh);
    memmove(pic, data.m_item->m_pic, 152 * 112);

    while (true)
    {
        bg_animation_update(anim_ctl, animations, anim_bytes, pic);

        for (uint32_t u = 0, k = 0; u < 152 * 112; u++)
        {
            bits[k++] = pic[u] >> 4;
            bits[k++] = pic[u] & 0x0F;
        }
        bpp8.CreateBitmap(304, 112, 1, 8, bits);
        Convert8bppTo32bpp(&bpp8, DosPal, &bpp32);
        data.m_preview->SetBitmap(bpp32);
        bpp8.DeleteObject();
        bpp32.DeleteObject();

        Sleep(55);
    }

    return 0;
}

IMPLEMENT_DYNAMIC(CAnhFilterPage, CDlgFilterPage)

CAnhFilterPage::CAnhFilterPage()
{
    m_hAnhThread = 0;
}

CAnhFilterPage::~CAnhFilterPage()
{
    if (!m_Anhs.empty())
    {
        for (int i = 0; i < m_Anhs.size(); i++)
        {
            free(m_Anhs[i]->m_anh);
            free(m_Anhs[i]->m_pic);
            delete m_Anhs[i];
        }

        m_Anhs.clear();
    }

    if (m_hAnhThread)
    {
        TerminateThread(m_hAnhThread, 0);
        m_hAnhThread = 0;
    }
}

BEGIN_MESSAGE_MAP(CAnhFilterPage, CDlgFilterPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTERTREE, &CAnhFilterPage::OnTvnSelchangedAnh)
    ON_COMMAND(ID_EXPORTSELECTED_EXPORTSELECTED, &CAnhFilterPage::OnExportAnh)
END_MESSAGE_MAP()

void CAnhFilterPage::OnTvnSelchangedAnh(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM hSelected = m_FilterTree.GetSelectedItem();
    DWORD_PTR itemData = m_FilterTree.GetItemData(hSelected);
    
    if (itemData)
    {
        if (m_hAnhThread)
        {
            TerminateThread(m_hAnhThread, 0);
        }

        AnhItemData *itemData = (AnhItemData*)m_FilterTree.GetItemData(hSelected);

        AnhThreadData *threadData;
        assert(threadData = new AnhThreadData);
        threadData->m_item = itemData;
        threadData->m_preview = (CStatic*)this->GetParent()->GetDlgItem(IDC_PREVIEWFRAME);

        m_hAnhThread = CreateThread(NULL, 0, AnhThreadProc, threadData, 0, NULL);
    }

    *pResult = 0;
}

void CAnhFilterPage::OnExportAnh()
{
    MessageBox(L"Not supported Yet :(");
}

void CAnhFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
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

    while (g_res_anh[i].file != -1)
    {
        AnhItemData *itemData;
        assert(itemData = new AnhItemData);

        int len = DecompressResource(fNeuroDat[g_res_anh[i].file], &g_res_anh[i], rsc);
        assert(itemData->m_anh = (uint8_t*)calloc(1, len));
        memmove(itemData->m_anh, rsc, len);

        int n, j = 0;
        char pic[32] = { 0, };
        sscanf_s(g_res_anh[i].name, "R%d.ANH", &n);
        sprintf(pic, "R%d.PIC", n);
        if (strcmp(pic, g_res_pic[j].name))
        {
            while (strcmp(pic, g_res_pic[++j].name));
        }
        len = DecompressResource(fNeuroDat[g_res_pic[j].file], &g_res_pic[j], rsc);
        assert(itemData->m_pic = (uint8_t*)calloc(1, len));
        memmove(itemData->m_pic, rsc, len);

        MultiByteToWideChar(CP_UTF8, 0, g_res_anh[i].name, -1, wName, 32);
        hItem = m_FilterTree.InsertItem(wName, hRootNeuro[g_res_anh[i++].file]);
        m_FilterTree.SetItemData(hItem, (DWORD_PTR)itemData);
        m_Anhs.push_back(itemData);
    }

    m_FilterTree.Expand(hRootNeuro[0], TVE_EXPAND);
    m_FilterTree.Expand(hRootNeuro[1], TVE_EXPAND);
}

void CAnhFilterPage::ChangePageCleanUp()
{
    if (m_hAnhThread)
    {
        TerminateThread(m_hAnhThread, 0);
        m_hAnhThread = 0;
    }
}
