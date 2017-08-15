
// ClientDlg.cpp : 实现文件;
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#include <sstream>

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

// CClientDlg 对话框;
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
	, m_strIP(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IP, m_strIP);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CClientDlg::OnBnClickedBtnConnect)
	ON_EN_CHANGE(IDC_EDIT_IP, &CClientDlg::OnEnChangeEditIp)
END_MESSAGE_MAP()

// CClientDlg 消息处理程序;
BOOL CClientDlg::OnInitDialog()
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
	m_strIP = "192.168.3.230";
	GetDlgItem(IDC_EDIT_IP)->SetWindowTextA(m_strIP.GetBuffer(0));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE;
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标， 显示;
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI clientProc(LPARAM lparam)
{
	CClientApp* pApp = (CClientApp*)lparam;
	CClientDlg* pDlg = (CClientDlg*)pApp->GetMainWnd();

	int bufSizePerFrame = IMAGE_WIDE * IMAGE_HIGH * 1.5 + 8;
	char* szBuf = new char [bufSizePerFrame];
	cv::Mat image;
	int i;
	while(1)
	{
		memset(szBuf, 0, bufSizePerFrame);
		int toltalRecvdSize = 0;
		while (toltalRecvdSize < bufSizePerFrame)
		{
			int recvSizeOneTime = recv(pDlg->m_sockServer, szBuf + toltalRecvdSize, bufSizePerFrame - toltalRecvdSize, 0);
			toltalRecvdSize += recvSizeOneTime;
		}
		image = cv::Mat(IMAGE_WIDE, IMAGE_HIGH, CV_8UC1, (unsigned char*)szBuf + 8);
		cv::imshow("image", image);
		if(cv::waitKey(33) == 0x1b)	break;

		pDlg->SetDlgItemText(IDC_STATIC_RECV, "receive successfully!");
	}
	return 0;
}

void CClientDlg::OnBnClickedBtnConnect()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	// 初始化完成，创建一个TCP的socket;
	m_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//指定连接的服务端信息;
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(14551);
	//客户端只需要连接指定的服务器地址，127.0.0.1是本机的回环地址;
	addrServer.sin_addr.S_un.S_addr = inet_addr(m_strIP.GetBuffer(0));

	// 服务器Bind 客户端是进行连接;
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
	int ret = connect(m_sockServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR));//开始连接;
	while(SOCKET_ERROR == ret)
	{
		Sleep(1000);
		ret = connect(m_sockServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR));
	}
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
	SetDlgItemText(IDC_STATIC_RECV, "server has been connected");

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)clientProc, (LPVOID)(&theApp), 0, 0);
}

void CClientDlg::OnEnChangeEditIp()
{
	GetDlgItem(IDC_EDIT_IP)->GetWindowTextA(m_strIP);
}
