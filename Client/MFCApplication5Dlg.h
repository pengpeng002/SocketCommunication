
// MFCApplication5Dlg.h: 头文件
//

#pragma once
#include "CMListCtrl.h"

// CMFCApplication5Dlg 对话框
class CMFCApplication5Dlg : public CDialog
{
// 构造
public:
	CMFCApplication5Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION5_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTray(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CMListCtrl m_fileList;
	CImageList m_imageList;
	//TipDialog m_tipDialog;
	SetDialog *m_setDialog;
	HACCEL m_haccel;
	//CToolTipCtrl m_bubbleTip;
	//afx_msg void OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	void AddShellIcon();
protected:
	afx_msg LRESULT OnListClick(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnUpload();
	afx_msg void OnNewDir();
	afx_msg void OnDownload();
	afx_msg void OnDelete();
	afx_msg void OnRename();
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnOpen();
	afx_msg void OnRefresh();
	afx_msg void OnPaste();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CEdit m_editShow;
	afx_msg void OnEnKillfocusShow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSetting();
	afx_msg void OnOpenWindow();
	afx_msg void OnOpenSetting();
	afx_msg void OnExit();
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
};
