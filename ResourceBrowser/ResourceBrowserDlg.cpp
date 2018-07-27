
// ResourceBrowserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceBrowser.h"
#include "ResourceBrowserDlg.h"
#include "ImhFilterPage.h"
#include "PicFilterPage.h"
#include "AnhFilterPage.h"
#include "SoundFilterPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct PlayerThreadData {
    sfMusic *m_player;
    CSliderCtrl *m_slider;
    CStatic *m_time;
} PlayerThreadData;

DWORD WINAPI PlayerThreadProc(_In_ LPVOID lpParameter)
{
    PlayerThreadData threadData;
    
    memmove(&threadData, (PlayerThreadData*)lpParameter, sizeof(PlayerThreadData));
    delete lpParameter;

    while (true)
    {
        sfTime totalUsec = sfMusic_getDuration(threadData.m_player);
        sfTime currPosUsec = sfMusic_getPlayingOffset(threadData.m_player);
        uint32_t currPosSec = (uint32_t)(currPosUsec.microseconds / 1000000);
        wchar_t wsztime[8];

        swprintf_s(wsztime, L"%02d:%02d", currPosSec / 60, currPosSec % 60);
        threadData.m_time->SetWindowTextW(wsztime);

        float pos = ((float)currPosUsec.microseconds / totalUsec.microseconds) * 100;
        threadData.m_slider->SetPos((int)pos);

        Sleep(500);
    }

    return 0;
}

// CResourceBrowserDlg dialog

CResourceBrowserDlg::CResourceBrowserDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_RESOURCEBROWSER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_sfMusicPlayer = NULL;
    m_PlayerThread = 0;
}

CResourceBrowserDlg::~CResourceBrowserDlg()
{
}

void CResourceBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILTERTAB, m_FilterTab);
    DDX_Control(pDX, IDC_PREVIEWFRAME, m_Preview);
}

BEGIN_MESSAGE_MAP(CResourceBrowserDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TCN_SELCHANGE, IDC_FILTERTAB, &CResourceBrowserDlg::OnTcnSelchangeFiltertab)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDOK, &CResourceBrowserDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CResourceBrowserDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_PLAY, &CResourceBrowserDlg::OnBnClickedButtonPlay)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CResourceBrowserDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CResourceBrowserDlg message handlers

BOOL CResourceBrowserDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    m_FilterTab.InsertItem(TAB_IMH, L"IMH");
    m_FilterTab.InsertItem(TAB_PIC, L"PIC");
    m_FilterTab.InsertItem(TAB_ANH, L"ANH");
    m_FilterTab.InsertItem(TAB_SOUND, L"SOUND");

    CRect rcClient, rcWindow;

    m_FilterTab.GetClientRect(&rcClient);
    m_FilterTab.AdjustRect(FALSE, &rcClient);

    m_FilterTab.GetWindowRect(&rcWindow);
    ScreenToClient(&rcWindow);
    rcClient.OffsetRect(rcWindow.left, rcWindow.top);

    // TODO: Add extra initialization here
    FILE *fNeuroDat[2] = { NULL, NULL };

    for (int i = 0; i < 2; i++)
    {
        fNeuroDat[i] = fopen((i == 0) ? "NEURO1.DAT" : "NEURO2.DAT", "rb");
        while (!fNeuroDat[i])
        {
            wchar_t wDat[16] = { 0, }, wMsg[256] = { 0, };
            int ret = 0;

            MultiByteToWideChar(CP_UTF8, 0, (i == 0) ? "NEURO1.DAT" : "NEURO2.DAT", -1, wDat, 16);

            swprintf(wMsg, 255, L"\"%s\" is not present in executable directory."
                " Specify it manually or die trying.", wDat);
            ret = MessageBox(wMsg, NULL, MB_OKCANCEL | MB_ICONERROR);

            if (ret != IDOK)
            {
                ExitProcess(0);
            }

            CFileDialog openDlg(TRUE, NULL, wDat, OFN_OVERWRITEPROMPT, NULL);
            if (openDlg.DoModal() == IDOK)
            {
                char path[2048];
                CT2A fileName(openDlg.GetFileName(), CP_UTF8);
                CT2A filePath(openDlg.GetFolderPath(), CP_UTF8);

                sprintf(path, "%s\\%s", filePath.m_psz, fileName.m_psz);
                fNeuroDat[i] = fopen(path, "rb");
            }
        }
    }

    for (int i = 0; i < TAB_TOTAL; i++)
    {
        if (i == TAB_IMH)
        {
            m_Page[i] = new CImhFilterPage();
        }
        else if (i == TAB_PIC)
        {
            m_Page[i] = new CPicFilterPage();
        }
        else if (i == TAB_ANH)
        {
            m_Page[i] = new CAnhFilterPage();
        }
        else if (i == TAB_SOUND)
        {
            m_Page[i] = new CSoundFilterPage();
        }

        m_Page[i]->Create(IDD_FILTERPAGE, m_FilterTab.GetWindow(IDD_FILTERPAGE));
        m_Page[i]->MoveWindow(&rcClient);
        m_Page[i]->BuildTree(fNeuroDat, i);
    }
    m_CurrentPage = m_Page[TAB_IMH];
    m_CurrentPage->ShowWindow(SW_SHOW);

    fclose(fNeuroDat[0]);
    fclose(fNeuroDat[1]);

    this->GetDlgItem(IDC_SLIDER_TRACKDUR)->ShowWindow(SW_HIDE);
    this->GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(SW_HIDE);
    this->GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_HIDE);
    this->GetDlgItem(IDC_STATIC_TRACKPOS)->ShowWindow(SW_HIDE);
    this->GetDlgItem(IDC_STATIC_TRACKLEN)->ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CResourceBrowserDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CResourceBrowserDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CResourceBrowserDlg::OnTcnSelchangeFiltertab(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    EFilterTab sel = (EFilterTab)m_FilterTab.GetCurSel();

    if (m_CurrentPage)
    {
        m_CurrentPage->ChangePageCleanUp();
        OnBnClickedButtonStop();

        m_CurrentPage->ShowWindow(SW_HIDE);
        m_CurrentPage = m_Page[sel];
        m_CurrentPage->ShowWindow(SW_SHOW);
    }
}


void CResourceBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}


void CResourceBrowserDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    for (int i = 0; i < TAB_TOTAL; i++)
    {
        delete m_Page[i];
    }

    if (m_sfMusicPlayer)
    {
        sfMusic_destroy(m_sfMusicPlayer);
    }

    if (m_PlayerThread)
    {
        TerminateThread(m_PlayerThread, 0);
        m_PlayerThread = 0;
    }

    CDialogEx::OnOK();
}

void CResourceBrowserDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    for (int i = 0; i < TAB_TOTAL; i++)
    {
        delete m_Page[i];
    }

    if (m_sfMusicPlayer)
    {
        sfMusic_destroy(m_sfMusicPlayer);
    }

    if (m_PlayerThread)
    {
        TerminateThread(m_PlayerThread, 0);
        m_PlayerThread = 0;
    }

    CDialogEx::OnCancel();
}

void CResourceBrowserDlg::OnBnClickedButtonPlay()
{
    uint8_t *wf = ((CSoundFilterPage*)m_CurrentPage)->GetWaveform();
    if (!wf)
    {
        return;
    }
    uint32_t len = *(uint32_t*)(wf + 4) + 8;

    OnBnClickedButtonStop();

    m_sfMusicPlayer = sfMusic_createFromMemory(wf, len);
    if (!m_sfMusicPlayer)
    {
        MessageBox(L"Bad luck opening audio output device :(");
        return;
    }

    PlayerThreadData *threadData;
    assert(threadData = new PlayerThreadData);
    threadData->m_player = m_sfMusicPlayer;
    threadData->m_slider = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_TRACKDUR);
    threadData->m_time = (CStatic*)this->GetDlgItem(IDC_STATIC_TRACKPOS);

    m_PlayerThread = CreateThread(NULL, 0, PlayerThreadProc, threadData, 0, NULL);

    sfMusic_play(m_sfMusicPlayer);
}

void CResourceBrowserDlg::OnBnClickedButtonStop()
{
    if (m_sfMusicPlayer)
    {
        sfMusic_stop(m_sfMusicPlayer);
        sfMusic_destroy(m_sfMusicPlayer);
        m_sfMusicPlayer = NULL;
    }
    if (m_PlayerThread)
    {
        TerminateThread(m_PlayerThread, 0);
        m_PlayerThread = 0;
    }

    CSliderCtrl *slider = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_TRACKDUR);
    slider->SetPos(0);
    
    wchar_t szwTrackLen[8];
    swprintf_s(szwTrackLen, L"00:00");
    this->GetDlgItem(IDC_STATIC_TRACKPOS)->SetWindowTextW(szwTrackLen);
}
