
// PPDlg.h : ͷ�ļ�
//

#pragma once
#include "ini.h"


// CPPDlg �Ի���
class CPPDlg : public CDialogEx
{
// ����
public:
	CPPDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSift();
	afx_msg void OnBnClickedBtnKmeans();
	afx_msg void OnBnClickedBtnWordf();
	afx_msg void OnBnClickedBtnLda();
private:
	CIni ini;
public:
	int m_kmK;
	int m_ldaK;
	CString m_pathOpen;
	CString m_pathSave;
	BOOL m_isSiftDraw;
//	CRichEditCtrl m_info;
	// ���ı����ڵ���Ϣ��ʾ
	void appendInfo(CString info, int flag = 0);
	void appendInfo(char* info, int flag = 0);
	afx_msg void OnEnChangePathOpen();
	afx_msg void OnEnChangePathSave();
	CListBox m_info;
};
