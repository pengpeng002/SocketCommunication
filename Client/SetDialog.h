#pragma once


// SetDialog 对话框

class SetDialog : public CDialogEx
{
	DECLARE_DYNAMIC(SetDialog)

public:
	SetDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SetDialog();
	void initDialog();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_MEDIUM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_bootRun;
	CButton m_bubbleTip;
	CButton m_useDefault;
	CEdit m_downloadPath;
	CButton m_browse;
	CButton m_saveSetting;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedUseset();
	afx_msg void OnBnClickedBootrun();
	afx_msg void OnBnClickedBubbletip();
	afx_msg void OnBnClickedDefaultpath();
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnClose();
};
