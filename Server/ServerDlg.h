// ServerDlg.h : ͷ�ļ�;
#pragma once

#include "opencv2/opencv.hpp"

// CServerDlg �Ի���;
class CServerDlg : public CDialogEx
{
// ����;
public:
	CServerDlg(CWnd* pParent = NULL);	// ��׼���캯��;

// �Ի�������;
	enum { IDD = IDD_SERVER_DIALOG };

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
	afx_msg void OnBnClickedBtnSend();
	CString m_strBufSend;
	afx_msg void OnUpdateEditSend();
	afx_msg void OnBnClickedBtnOpenServer();

	SOCKET m_sockServer;
	SOCKET m_sockClient;
};
