// SetDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication5.h"
#include "SetDialog.h"
#include "afxdialogex.h"


// SetDialog 对话框

IMPLEMENT_DYNAMIC(SetDialog, CDialogEx)

SetDialog::SetDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPPAGE_MEDIUM, pParent)
{

}

SetDialog::~SetDialog()
{
}

void SetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BOOTRUN, m_bootRun);
	DDX_Control(pDX, IDC_BUBBLETIP, m_bubbleTip);
	DDX_Control(pDX, IDC_DEFAULTPATH, m_useDefault);
	DDX_Control(pDX, IDC_EDIT_PATH, m_downloadPath);
	DDX_Control(pDX, IDC_BROWSE, m_browse);
	DDX_Control(pDX, IDC_USESET, m_saveSetting);
}


BEGIN_MESSAGE_MAP(SetDialog, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_OK, &SetDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, &SetDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_USESET, &SetDialog::OnBnClickedUseset)
	ON_BN_CLICKED(IDC_BOOTRUN, &SetDialog::OnBnClickedBootrun)
	ON_BN_CLICKED(IDC_BUBBLETIP, &SetDialog::OnBnClickedBubbletip)
	ON_BN_CLICKED(IDC_DEFAULTPATH, &SetDialog::OnBnClickedDefaultpath)
	ON_BN_CLICKED(IDC_BROWSE, &SetDialog::OnBnClickedBrowse)
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// SetDialog 消息处理程序


void SetDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedUseset();
	ShowWindow(SW_HIDE);
}


void SetDialog::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	initDialog();
	ShowWindow(SW_HIDE);
}


void SetDialog::OnBnClickedUseset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_saveSetting.EnableWindow(0);
	g_autoRun = m_bootRun.GetCheck();
	g_useBubble = m_bubbleTip.GetCheck();
	g_useDefaultPath = m_useDefault.GetCheck();
	m_downloadPath.GetWindowTextA(g_savePath);
}


void SetDialog::OnBnClickedBootrun()
{
	// TODO: 在此添加控件通知处理程序代码
	m_saveSetting.EnableWindow(1);
}


void SetDialog::OnBnClickedBubbletip()
{
	// TODO: 在此添加控件通知处理程序代码
	m_saveSetting.EnableWindow(1);
}


void SetDialog::OnBnClickedDefaultpath()
{
	// TODO: 在此添加控件通知处理程序代码
	m_saveSetting.EnableWindow(1);
	m_downloadPath.EnableWindow(m_useDefault.GetCheck());
	m_browse.EnableWindow(m_useDefault.GetCheck());
}


void SetDialog::OnBnClickedBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	CFolderPickerDialog fpd;
	fpd.m_ofn.lpstrTitle = "请选择下载路径";
	if (fpd.DoModal() == TRUE)
	{
		m_saveSetting.EnableWindow(1);
		m_downloadPath.SetWindowTextA(fpd.GetFolderPath()+"\\");
	}
}


BOOL SetDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	initDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void SetDialog::initDialog()
{
	m_bootRun.SetCheck(g_autoRun);
	m_bubbleTip.SetCheck(g_useBubble);
	m_useDefault.SetCheck(g_useDefaultPath);
	m_downloadPath.SetWindowTextA(g_savePath);
	m_downloadPath.EnableWindow(g_useDefaultPath);
	m_browse.EnableWindow(g_useDefaultPath);
	m_saveSetting.EnableWindow(0);
	printf("init ok!\n");
}


void SetDialog::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::PostNcDestroy();
	printf("this is deleted!\n");
	//delete this;
}


void SetDialog::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedCancel();
	//CDialogEx::OnClose();
}
