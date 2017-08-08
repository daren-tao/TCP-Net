// ServerDlg.cpp : ʵ���ļ�;
#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������;
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��;

	// ʵ��;
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CServerDlg �Ի���;
CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strBufSend = _T("");
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strBufSend);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEND, &CServerDlg::OnBnClickedBtnSend)
	ON_EN_CHANGE(IDC_EDIT_SEND, &CServerDlg::OnUpdateEditSend)
	ON_BN_CLICKED(IDC_BTN_OPEN_SERVER, &CServerDlg::OnBnClickedBtnOpenServer)
END_MESSAGE_MAP()


// CServerDlg ��Ϣ�������;

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵���;

	// IDM_ABOUTBOX ������ϵͳ���Χ��;
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�;
	//  ִ�д˲���;
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��;
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��;

	// TODO: �ڴ���Ӷ���ĳ�ʼ������;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE;
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���;
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���;
//  �⽫�ɿ���Զ����;

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������;

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���;
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��, ��ʾ;
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnBnClickedBtnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������;
	unsigned char* y = new unsigned char [960 * 1280 * 1.5 + 8];
	for (int i = 250; i <= 280; i++)
	{
		std::stringstream sstr;
		sstr << "E:/testDemoCodes/cameraserver/0_1843200_" << i << ".yuv";
		FILE* fp = fopen(sstr.str().c_str(), "rb");
		fread(y + 8, 1.5 * 960 * 1280 * sizeof(unsigned char), 1, fp);

		int ret = send(m_sockClient, (char*)y, 1.5 * 960 * 1280 * sizeof(uchar) + 8, 0);

		Sleep(1000);
	}
}

void CServerDlg::OnUpdateEditSend()
{
	GetDlgItem(IDC_EDIT_SEND)->GetWindowTextA(m_strBufSend);
}

void CServerDlg::OnBnClickedBtnOpenServer()
{
	//	�������绷���ĳ�ʼ������;
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	//	��ʼ����ɣ�����һ��TCP��socket;
	m_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//	���а󶨲���;
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;				//	Э���ΪIPV4��;
	addrServer.sin_port = htons(14551);				//	�˿�;
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;	// ip��ַ��INADDR_ANY��ʾ�󶨵�������������IP;
	//	��ɰ󶨲���;
	int ret = bind(m_sockServer, (sockaddr *)&addrServer, sizeof(sockaddr));
	//	�󶨳ɹ������м���;
	ret = listen(m_sockServer, 10);

	sockaddr_in addrClient; //	���ڱ���ͻ��˵�����ڵ����Ϣ;
	int addrClientLen = sizeof(sockaddr_in);

	GetDlgItem(IDC_BTN_OPEN_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
	//	�ȴ��ͻ��˵�����;
	m_sockClient = accept(m_sockServer, (sockaddr*)&addrClient, &addrClientLen);

	GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
}
