
// MFCApplication5.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication5.h"
#include "MFCApplication5Dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication5App

BEGIN_MESSAGE_MAP(CMFCApplication5App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFCApplication5App 构造

CMFCApplication5App::CMFCApplication5App()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	AllocConsole();
	freopen("CONOUT$", "a+", stdout);
	WSADATA a;
	if (WSAStartup(MAKEWORD(2, 2), &a))//初始化套接字库 
	{
		printf("init fail\n");
		exit(0);
	}

	Client = socket(AF_INET, SOCK_STREAM, 0);//TCP通信
	//SOCKET self = socket(AF_INET, SOCK_DGRAM, 0);//UDP通信 
	//第一个参数：地址族。 AF 是“Address Family”的简写，INET是“Inetnet”的简写。AF_INET 表示 IPv4 地址。 AF_INET6 表示 IPv6 地址
	//第二个参数：数据传输方式/套接字类型。一般取值 SOCK_STREAM（面向连接） 或 SOCK_DGRAM（无连接） 
	//第三个参数：传输协议，常用的有 IPPROTO_TCP（TCP协议） 和 IPPTOTO_UDP（UDP协议）。可填0，操作系统自动推导类型 

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4011);
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.252");

	if (connect(Client, (sockaddr*)&addr, sizeof(addr)))
	{
		printf("connect error, error code: %d\n", WSAGetLastError());
		
		exit(0);
	}
	
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CMFCApplication5App 对象

CMFCApplication5App theApp;
bool g_remember=0, g_auto=0, g_isLogin=0;
SOCKET Client;
CString path="\\\\";

// CMFCApplication5App 初始化

BOOL CMFCApplication5App::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	init();
	login ldlg;
	if (!g_isLogin)
	{
		ldlg.DoModal();
	}
	save();
	if (!g_isLogin)
	{
		if (pShellManager != nullptr)
		{
			delete pShellManager;
		}
		return false;
	}

	CMFCApplication5Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	save();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}



BOOL CMFCApplication5App::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CWinApp::PreTranslateMessage(pMsg);
}
