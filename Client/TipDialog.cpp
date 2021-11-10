// TipDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication5.h"
#include "thread"
#include "TipDialog.h"
#include "afxdialogex.h"


// TipDialog 对话框

IMPLEMENT_DYNAMIC(TipDialog, CDialog)

TipDialog::TipDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_SMALL, pParent)
{
	delayTime = 5000;
	tipText = "默认提示信息";
}

TipDialog::~TipDialog()
{
}

void TipDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TipDialog, CDialog)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void TipDialog::setDelayTime(int x)
{
	delayTime = x;
}

void TipDialog::setTipText(const CString& s)
{
	tipText = s;
	GetDlgItem(IDC_TIP)->SetWindowTextA(s);
}

// TipDialog 消息处理程序

void TipDialog::autoHide()
{
	Sleep(delayTime);
	this->ShowWindow(SW_HIDE);
}

//BOOL TipDialog::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	BOOL ret = CDialog::Create(lpszTemplateName, pParentWnd);
//	std::thread th(autoHide);
//	return ret;
//}


void TipDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	std::thread th(&TipDialog::autoHide, this);
	th.detach();
	// TODO: 在此处添加消息处理程序代码
}
