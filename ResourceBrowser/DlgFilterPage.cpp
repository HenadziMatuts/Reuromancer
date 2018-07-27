// DlgFilterPage.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceBrowser.h"
#include "DlgFilterPage.h"
#include <errno.h>

// CDlgFilterPage dialog

IMPLEMENT_DYNAMIC(CDlgFilterPage, CDialogEx)

CDlgFilterPage::CDlgFilterPage(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_FILTERPAGE, pParent)
{
}

CDlgFilterPage::~CDlgFilterPage()
{
}

void CDlgFilterPage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILTERTREE, m_FilterTree);
}

BEGIN_MESSAGE_MAP(CDlgFilterPage, CDialogEx)
    ON_WM_SIZE()
    ON_NOTIFY(NM_RCLICK, IDC_FILTERTREE, &CDlgFilterPage::OnRclickFiltertree)
END_MESSAGE_MAP()

// CDlgFilterPage message handlers

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

void CDlgFilterPage::BuildTree(FILE *fNeuroDat[2], int tab)
{
}

void CDlgFilterPage::ChangePageCleanUp()
{
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
