// ClientDlg.h : ͷ�ļ�;
#pragma once

#include "opencv2/opencv.hpp"

#define IMAGE_WIDE	960
#define IMAGE_HIGH	1280


// CClientDlg �Ի���;
class CClientDlg : public CDialogEx
{
// ����;
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��;

// �Ի�������;
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��;


// ʵ��;
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConnect();
	SOCKET m_sockServer;
//	DWORD WINAPI clientProc(LPARAM lparam);
	afx_msg void OnEnChangeEditIp();
	CString m_strIP;
};
