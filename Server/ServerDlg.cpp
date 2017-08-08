// ServerDlg.cpp : 实现文件;
#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据;
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持;

	// 实现;
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

// CServerDlg 对话框;
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


// CServerDlg 消息处理程序;

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中;

	// IDM_ABOUTBOX 必须在系统命令范围内;
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动;
	//  执行此操作;
	SetIcon(m_hIcon, TRUE);			// 设置大图标;
	SetIcon(m_hIcon, FALSE);		// 设置小图标;

	// TODO: 在此添加额外的初始化代码;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE;
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

// 如果向对话框添加最小化按钮，则需要下面的代码;
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序;
//  这将由框架自动完成;

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文;

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中;
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标, 显示;
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码;
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
	//	进行网络环境的初始化操作;
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	//	初始化完成，创建一个TCP的socket;
	m_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//	进行绑定操作;
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;				//	协议簇为IPV4的;
	addrServer.sin_port = htons(14551);				//	端口;
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;	// ip地址，INADDR_ANY表示绑定电脑上所有网卡IP;
	//	完成绑定操作;
	int ret = bind(m_sockServer, (sockaddr *)&addrServer, sizeof(sockaddr));
	//	绑定成功，进行监听;
	ret = listen(m_sockServer, 10);

	sockaddr_in addrClient; //	用于保存客户端的网络节点的信息;
	int addrClientLen = sizeof(sockaddr_in);

	GetDlgItem(IDC_BTN_OPEN_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
	//	等待客户端的连接;
	m_sockClient = accept(m_sockServer, (sockaddr*)&addrClient, &addrClientLen);

	GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
}
