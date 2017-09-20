
// ClientDlg.cpp : ʵ���ļ�;
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#include <sstream>

#include "StereoCalib.h"

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

// CClientDlg �Ի���;
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
	ON_EN_CHANGE(IDC_EDIT_WIDE, &CClientDlg::OnEnChangeEditWide)
	ON_EN_CHANGE(IDC_EDIT_HIGH, &CClientDlg::OnEnChangeEditHigh)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CClientDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CALIB, &CClientDlg::OnBnClickedBtnCalib)
	ON_BN_CLICKED(IDC_BTN_STOP_SAVE, &CClientDlg::OnBnClickedBtnStopSave)
END_MESSAGE_MAP()

// CClientDlg ��Ϣ�������;
BOOL CClientDlg::OnInitDialog()
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
	m_strIP = "192.168.2.220";
	GetDlgItem(IDC_EDIT_IP)->SetWindowText(m_strIP.GetBuffer(0));

	m_strWide = "320";
	GetDlgItem(IDC_EDIT_WIDE)->SetWindowText(m_strWide.GetBuffer(0));
	m_strHigh = "240";
	GetDlgItem(IDC_EDIT_HIGH)->SetWindowText(m_strHigh.GetBuffer(0));

	m_bSaved = false;
	m_iSaved = 0;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE;
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

// �����Ի��������С����ť������Ҫ����Ĵ���;
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���;
//  �⽫�ɿ���Զ����;

void CClientDlg::OnPaint()
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù�꣬ ��ʾ;
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



static uchar* gDisparity;
static double detectDist;

cv::Mat rotateImage(cv::Mat img, int degree)
{
	double sintheta = sin(degree  * CV_PI / 180.0), costheta = cos(degree  * CV_PI / 180.0);
	int wide = img.cols, high = img.rows;
	int rdWide	= int(high * fabs(sintheta) + wide * fabs(costheta));
	int rdHigh	= int(wide * fabs(sintheta) + high * fabs(costheta));
	float map[6];
	cv::Mat rMatrix = cv::Mat(2, 3, CV_32F, map);
	CvPoint2D32f center = cvPoint2D32f(wide / 2, high / 2);//	��ת����;

	CvMat map_matrix2 = rMatrix;
	cv2DRotationMatrix(center, degree, 1.0, &map_matrix2);//�����ά��ת�ķ���任����;

	map[2] += (rdWide - wide) / 2;
	map[5] += (rdHigh - high) / 2;

	cv::Mat img_rotate;//	cv::warpAffine()Ĭ�ϵ���ת��������ʱ�룬���ԼӸ��ű�ʾת��Ϊ˳ʱ��;
	cv::warpAffine(img, img_rotate, rMatrix, cv::Size(rdWide, rdHigh), 1, 0, 0);
	return img_rotate;
}

#define PREVIEW_WIDTH 320
#define PREVIEW_HIGHT 240
int wait_time_ms = 33;


DWORD WINAPI clientProc(LPARAM lparam)
{
	CClientApp* pApp = (CClientApp*)lparam;
	CClientDlg* pDlg = (CClientDlg*)pApp->GetMainWnd();
	
	int bufSizePerFrame = 2 * atoi(pDlg->m_strWide.GetBuffer(0)) * atoi(pDlg->m_strHigh.GetBuffer(0)) * 3 / 2 + 1;
	char* szBuf;
	cv::Mat image1, image2, stereoDisparity;
	int i;

	cv::FileStorage fs("./image/calib_paras.xml", cv::FileStorage::READ);
	cv::Mat remapX1, remapY1, remapX2, remapY2;
	fs["remapX1"] >> remapX1;
	fs["remapY1"] >> remapY1;
	fs["remapX2"] >> remapX2;
	fs["remapY2"] >> remapY2;


	char *file1, *file2;
	while(1)
	{
		szBuf = new char [bufSizePerFrame];
		memset(szBuf, 0, bufSizePerFrame);
		int toltalRecvdSize = 0;
		while (toltalRecvdSize < bufSizePerFrame)
		{
			int recvSizeOneTime = recv(pDlg->m_sockServer, szBuf + toltalRecvdSize, bufSizePerFrame - toltalRecvdSize, 0);
			toltalRecvdSize += recvSizeOneTime;
		}
		// ��������Ϊ�������ͼ��;
		if(szBuf[0] == 97)
		{
			image1 = cv::Mat(atoi(pDlg->m_strHigh.GetBuffer(0)), atoi(pDlg->m_strWide.GetBuffer(0)), CV_8UC1, (unsigned char*)szBuf + 1);
			image2 = cv::Mat(atoi(pDlg->m_strHigh.GetBuffer(0)), atoi(pDlg->m_strWide.GetBuffer(0)), CV_8UC1, 
				(unsigned char*)szBuf + atoi(pDlg->m_strWide.GetBuffer(0)) * atoi(pDlg->m_strHigh.GetBuffer(0)) * 3 / 2 + 1);
		}
		//	��������Ϊ�Ӳ�ͼ;
		if(szBuf[0] == 65)
			stereoDisparity = cv::Mat(240, 320, CV_8UC1, (unsigned char*)(szBuf + 1));

		if(!image1.empty() && !image2.empty())
		{
			pDlg->m_iSaved++;
			if(pDlg->m_bSaved && pDlg->m_iSaved % 60 == 0)
			{
				file1 = new char[256];		sprintf(file1, "image/left_%d.jpg", pDlg->m_iSaved / 60);
				file2 = new char[256];		sprintf(file2, "image/right_%d.jpg", pDlg->m_iSaved / 60);
				cv::imwrite(file1, image2);	cv::imwrite(file2, image1);

				cv::bitwise_not(image1, image1);	cv::bitwise_not(image2, image2);
				wait_time_ms = 300;
				
				delete file1;	delete file2;
			}
			cv::remap(image2, image2, remapX1, remapY1, cv::INTER_LINEAR);
			cv::remap(image1, image1, remapX2, remapY2, cv::INTER_LINEAR);

#if 0
			cv::Mat kernel(1, 3, CV_32FC1, cv::Scalar(0));
			kernel.at<float>(0, 0) = -1;
			kernel.at<float>(0, 2) = 1;
			cv::filter2D(image1, image1, image1.depth(), kernel);
			cv::filter2D(image2, image2, image2.depth(), kernel);
//#endif
			cv::equalizeHist(image1, image1);
			cv::equalizeHist(image2, image2);
#endif

			cv::Mat stitch(image1.rows, image1.cols * 2, image1.type());
			image2.copyTo(stitch.colRange(0, image1.cols));
			image1.copyTo(stitch.colRange(image1.cols, stitch.cols));
			cv::cvtColor(stitch, stitch, cv::COLOR_GRAY2RGB);

			for(int i = 10; i < stitch.rows; i+= 20)
				cv::line(stitch, cv::Point(0, i), cv::Point(stitch.cols, i), cv::Scalar(0, 0, 255));
			
		//	cv::imshow("video", stitch);
		//	cv::resize(stitch, stitch, cv::Size(), 2.0, 2.0);
			cv::imshow("image", stitch);
		}
		if(!stereoDisparity.empty())
		{
			cv::cvtColor(stereoDisparity, stereoDisparity, cv::COLOR_GRAY2RGB);
			cv::imshow("disparity", stereoDisparity);
		}

		if(cv::waitKey(wait_time_ms) == 0x1b)	{
			ExitProcess(-1);	break;
		}
		wait_time_ms = 33;

		pDlg->SetDlgItemText(IDC_STATIC_RECV, "receive successfully!");

		delete szBuf;
	}
	return 0;
}

void CClientDlg::OnBnClickedBtnConnect()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	// ��ʼ����ɣ�����һ��TCP��socket;
	m_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//ָ�����ӵķ������Ϣ;
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(14551);
	//�ͻ���ֻ��Ҫ����ָ���ķ�������ַ��127.0.0.1�Ǳ����Ļػ���ַ;
	addrServer.sin_addr.S_un.S_addr = inet_addr(m_strIP.GetBuffer(0));

	// ������Bind �ͻ����ǽ�������;
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
	int ret = connect(m_sockServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR));//��ʼ����;
	while(SOCKET_ERROR == ret)
	{
		Sleep(1000);
		ret = connect(m_sockServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR));
	}
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
	SetDlgItemText(IDC_STATIC_RECV, "server has been connected");

	m_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)clientProc, (LPVOID)(&theApp), 0, 0);
}

void CClientDlg::OnEnChangeEditIp()
{
	GetDlgItem(IDC_EDIT_IP)->GetWindowText(m_strIP);
}


void CClientDlg::OnEnChangeEditWide()
{
	GetDlgItem(IDC_EDIT_WIDE)->GetWindowText(m_strWide);
}


void CClientDlg::OnEnChangeEditHigh()
{
	GetDlgItem(IDC_EDIT_HIGH)->GetWindowText(m_strHigh);
}


void CClientDlg::OnBnClickedBtnSave()
{
	m_bSaved = true;
}


void CClientDlg::OnBnClickedBtnStopSave()
{
	m_bSaved = false;
}

#include <io.h>
std::vector<std::string> get_files_in_folder(std::string folder_path, std::string filter)
{
	std::vector<std::string> files;
	long hfiles = 0;
	_finddata_t findData;
	hfiles = _findfirst(folder_path.append(filter/*"\\*"*/).c_str(), &findData);
	do{
		if ( !(findData.attrib & _A_SUBDIR))
			files.push_back(findData.name);

	} while(_findnext(hfiles, &findData) == 0);

	return files;
}

void CClientDlg::OnBnClickedBtnCalib()
{
	cv::Size image_size(320, 240);
	cv::Size corner_size(8, 6);
	float square_size = 26;

	std::vector<cv::Mat> l_frames, r_frame;

	std::vector<std::string> l_files = get_files_in_folder("./image", "/left_*");
	std::vector<std::string> r_files = get_files_in_folder("./image", "/right_*");
	if(l_files.size() != r_files.size())
	{
		SetDlgItemText(IDC_STATIC_RECV, "left images are not as the name as right ones");
		return;
	}
	int boards_n = l_files.size();

	StereoCalib::CornerDatas cornerDatas;
	StereoCalib::StereoParams stereoParams;
	StereoCalib::RemapMatrixs remapMatrixs;

	StereoCalib m_stereoCalibrator;
	//	1) ��ʼ���ǵ����ݼ�;
	m_stereoCalibrator.initCornerData(boards_n, image_size, corner_size, square_size, cornerDatas);

	int boards_found_n = 0;
	cv::Mat image1, image2;
	//	2) ���̸�ǵ���;
	for (int index = 0; index < l_files.size(); index++)
	{
		image1 = cv::imread("./image/" + l_files[index]);
		image2 = cv::imread("./image/" + r_files[index]);

		if ( m_stereoCalibrator.detectCorners(image1, image2, cornerDatas, boards_found_n))
			boards_found_n++;
	}
	if (boards_found_n < 4) {
		SetDlgItemText(IDC_STATIC_RECV, "the boards detected corners successfully are less than 4, exit calibration !");
		return;
	} else if (boards_found_n != boards_n) {
		boards_n = boards_found_n;
		m_stereoCalibrator.resizeCornerData(boards_n, cornerDatas);
	}
	
	//	3) ˫Ŀ�궨;
	bool single_calib_needed = true;
	stereoParams.cameraParams1.flags = CV_CALIB_FIX_K3 | CV_CALIB_FIX_PRINCIPAL_POINT | CV_CALIB_FIX_ASPECT_RATIO | CV_CALIB_ZERO_TANGENT_DIST;
	stereoParams.cameraParams2.flags = CV_CALIB_FIX_K3 | CV_CALIB_FIX_PRINCIPAL_POINT | CV_CALIB_FIX_ASPECT_RATIO | CV_CALIB_ZERO_TANGENT_DIST;
	stereoParams.flags = CV_CALIB_FIX_PRINCIPAL_POINT | CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_FIX_ASPECT_RATIO | CV_CALIB_SAME_FOCAL_LENGTH;
	stereoParams.alpha = -1;
	m_stereoCalibrator.calibrateStereoCamera(cornerDatas, stereoParams, single_calib_needed);
	double avgErr = 0;
	m_stereoCalibrator.getStereoCalibrateError(cornerDatas, stereoParams, avgErr);
	
	//	4) ִ��˫ĿУ��;
	StereoCalib::RECTIFYMETHOD method = StereoCalib::RECTIFY_BOUGUET;
	m_stereoCalibrator.rectifyStereoCamera(cornerDatas, stereoParams, remapMatrixs, method);

	//	5) ��������ͷ�������;
	m_stereoCalibrator.saveCalibrationDatas("./image/calib_paras.xml", method, cornerDatas, stereoParams, remapMatrixs);
}



