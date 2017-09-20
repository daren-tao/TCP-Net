// ClientDlg.h : ͷ�ļ�;
#pragma once

#include "opencv2/opencv.hpp"


#define IMAGE_WIDE 	240
#define IMAGE_HIGH	320


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


public:
	HANDLE m_hThread;

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
	afx_msg void OnEnChangeEditWide();
	afx_msg void OnEnChangeEditHigh();
	CString m_strWide;
	CString m_strHigh;
	afx_msg void OnBnClickedBtnSave();
	bool m_bSaved;
	int m_iSaved;
	afx_msg void OnBnClickedBtnCalib();
	afx_msg void OnBnClickedBtnStopSave();
};
