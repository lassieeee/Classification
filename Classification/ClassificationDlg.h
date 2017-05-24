
// PPDlg.h : 头文件
//

#pragma once
#include "ini.h"


// CPPDlg 对话框
class CPPDlg : public CDialogEx
{
// 构造
public:
	CPPDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	// 富文本框内的消息显示
	void appendInfo(CString info, int flag = 0);
	void appendInfo(char* info, int flag = 0);
	afx_msg void OnEnChangePathOpen();
	afx_msg void OnEnChangePathSave();
	CListBox m_info;
};
