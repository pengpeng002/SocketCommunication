#pragma once


// TipDialog 对话框

class TipDialog : public CDialog
{
	DECLARE_DYNAMIC(TipDialog)

public:
	TipDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~TipDialog();
	int delayTime;
	CString tipText;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_SMALL };
#endif
public:
	void setDelayTime(int);
	void setTipText(const CString& s);
	void autoHide();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
