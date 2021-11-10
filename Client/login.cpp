// login.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication5.h"
#include "login.h"



// login 对话框

IMPLEMENT_DYNAMIC(login, CDialogEx)

login::login(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN, pParent)
{

}

login::~login()
{
}

void login::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER_NAME, m_userName);//用户名
	DDX_Control(pDX, IDC_PASSWORD, m_password);//密码
	DDX_Control(pDX, IDC_AUTO, m_auto);//自动登录
	DDX_Control(pDX, IDC_REMEMBER, m_remember);//记住密码
}


BEGIN_MESSAGE_MAP(login, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &login::OnBnClickedButton1)//登录按钮
	ON_BN_CLICKED(IDC_BUTTON2, &login::OnBnClickedButton2)//隐藏按钮
END_MESSAGE_MAP()


// login 消息处理程序

//点击登录按钮
void login::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString name, pass;
	
	m_userName.GetWindowText(name);//获取用户输入的用户名
	m_password.GetWindowText(pass);//获取用户输入的密码
	printf("name = %s, pass = %s\n", name.GetBuffer(), pass.GetBuffer());
	if (checkPassword(name.GetBuffer(), pass.GetBuffer()))
	//if (name == "admin" && pass == "admin")//用户与密码匹配
	{
		g_isLogin = 1;//表示登录成功
	}
	else//用户名与密码不匹配，比如没有输入用户名或密码
	{
		GetDlgItem(IDC_BUTTON2)->ShowWindow(1);//显示 隐藏按钮
		GetDlgItem(IDC_TIPS)->ShowWindow(1);//显示原本隐藏的提示文本框
		GetDlgItem(IDC_TIPS)->SetWindowTextA("密码错误");//设置文本内容
	}
	g_auto = m_auto.GetCheck();//是否勾选自动登录
	g_remember = m_remember.GetCheck();//是否勾选记住密码
	if (g_auto || g_remember)//如果不记住密码也不自动登录，则没有必要保存用户输入的用户名和密码
	{
		memcpy(::name, name, name.GetLength());
		memcpy(::pass, pass, pass.GetLength());
	}

	if (g_isLogin)//如果登录成功，则关闭登录对话框
	{
		CDialogEx::OnOK();
	}
	
}


BOOL login::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)//如果按下的回车键
	{
		if (GetFocus() == GetDlgItem(IDC_USER_NAME))
		{
			GetDlgItem(IDC_PASSWORD)->SetFocus();
		}
		else if (GetFocus() == GetDlgItem(IDC_PASSWORD) || GetFocus() == GetDlgItem(IDC_BUTTON1))
		{
			OnBnClickedButton1();
		}
		return 1;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void login::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	
}


void login::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TIPS)->ShowWindow(SW_HIDE);
}


BOOL login::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (strcmp(::name, "null"))
	{
		m_userName.SetWindowTextA(::name);
	}
	if (g_remember && strcmp(::pass, "null"))
	{
		m_password.SetWindowTextA(::pass);
	}
	m_remember.SetCheck(g_remember);
	m_auto.SetCheck(g_auto);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
