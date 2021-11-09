
// MFCApplication5Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication5.h"
#include "MFCApplication5Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication5Dlg 对话框



CMFCApplication5Dlg::CMFCApplication5Dlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFCAPPLICATION5_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_setDialog = new SetDialog(this);
}

void CMFCApplication5Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_fileList);
	DDX_Control(pDX, IDC_SHOW, m_editShow);
}

BEGIN_MESSAGE_MAP(CMFCApplication5Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_NOTIFY(NM_CLICK, IDC_LIST1, &CMFCApplication5Dlg::OnNMClickList1)
	ON_MESSAGE(LISTCLICK, &CMFCApplication5Dlg::OnListClick)
	ON_COMMAND(ID_UPLOAD, &CMFCApplication5Dlg::OnUpload)
	ON_COMMAND(ID_NEW_DIR, &CMFCApplication5Dlg::OnNewDir)
	ON_COMMAND(ID_DOWNLOAD, &CMFCApplication5Dlg::OnDownload)
	ON_COMMAND(ID_DELETE, &CMFCApplication5Dlg::OnDelete)
	ON_COMMAND(ID_RENAME, &CMFCApplication5Dlg::OnRename)
	ON_COMMAND(ID_COPY, &CMFCApplication5Dlg::OnCopy)
	ON_COMMAND(ID_CUT, &CMFCApplication5Dlg::OnCut)
	ON_COMMAND(ID_OPEN, &CMFCApplication5Dlg::OnOpen)
	ON_COMMAND(ID_REFRESH, &CMFCApplication5Dlg::OnRefresh)
	ON_COMMAND(ID_PASTE, &CMFCApplication5Dlg::OnPaste)
	ON_MESSAGE(WM_TRAY, &CMFCApplication5Dlg::OnTray)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication5Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication5Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication5Dlg::OnBnClickedButton2)
	ON_EN_KILLFOCUS(IDC_SHOW, &CMFCApplication5Dlg::OnEnKillfocusShow)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETTING, &CMFCApplication5Dlg::OnBnClickedSetting)
	ON_COMMAND(ID_OPEN_WINDOW, &CMFCApplication5Dlg::OnOpenWindow)
	ON_COMMAND(ID_OPEN_SETTING, &CMFCApplication5Dlg::OnOpenSetting)
	ON_COMMAND(ID_EXIT, &CMFCApplication5Dlg::OnExit)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &CMFCApplication5Dlg::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// CMFCApplication5Dlg 消息处理程序

BOOL CMFCApplication5Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 3, 3);
	// TODO: 在此添加额外的初始化代码
	CRect rect;
	m_fileList.GetClientRect(&rect);
	m_fileList.ModifyStyle(0, WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS);
	m_fileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	m_fileList.SetImageList(&m_imageList, LVSIL_SMALL);
	m_fileList.SetBkColor(RGB(252, 252, 252));
	m_fileList.InsertColumn(0, "name", 0, 200, 0);
	m_fileList.InsertColumn(1, "type", 0, 100, 0);
	m_fileList.InsertColumn(2, "date", 0, 150, 0);
	m_fileList.InsertColumn(3, "size", LVCFMT_RIGHT, 100, 0);

	g_refresh();
	g_showList(&m_fileList);
	GetDlgItem(IDC_EDIT1)->SetWindowTextA(path);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(0);
	SetTimer(0, 100, 0);

	m_haccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	m_setDialog->Create(IDD_PROPPAGE_MEDIUM);
	AddShellIcon();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication5Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication5Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




afx_msg LRESULT CMFCApplication5Dlg::OnListClick(WPARAM wParam, LPARAM lParam)
{
	int num = 0;
	POSITION pos = m_fileList.GetFirstSelectedItemPosition();
	while (pos)
	{
		num++;
		if (num > 1) break;
		m_fileList.GetNextSelectedItem(pos);
	}

	CMenu* menu = GetMenu();
	if (num == 0)
	{
		if (menu->RemoveMenu(ID_OPEN, MF_BYCOMMAND))
		{
			menu->RemoveMenu(ID_DOWNLOAD, MF_BYCOMMAND);
			menu->RemoveMenu(ID_DELETE, MF_BYCOMMAND);
			menu->DeleteMenu(0, MF_BYPOSITION);//删除 "更多" 菜单，注意此处postion为0，因为前面两个已经被删除了
			menu->AppendMenuA(MF_STRING, ID_UPLOAD, "上传");
			menu->AppendMenuA(MF_STRING, ID_NEW_DIR, "新建文件夹");
			DrawMenuBar();
		}
	}
	else
	{
		if (menu->RemoveMenu(ID_UPLOAD, MF_BYCOMMAND))
		{
			menu->RemoveMenu(ID_NEW_DIR, MF_BYCOMMAND);
			menu->AppendMenuA(MF_STRING, ID_OPEN, "打开");
			menu->AppendMenuA(MF_STRING, ID_DOWNLOAD, "下载");
			menu->AppendMenuA(MF_STRING, ID_DELETE, "删除");
			CMenu more;
			more.CreatePopupMenu();
			if (num == 1)
			{
				more.AppendMenuA(MF_STRING, ID_RENAME, "重命名");
			}
			more.AppendMenuA(MF_STRING, ID_COPY, "复制");
			more.AppendMenuA(MF_STRING, ID_CUT, "剪切");
			if (option != -1)
			{
				more.AppendMenuA(MF_STRING, ID_PASTE, "粘贴");
			}
			menu->AppendMenuA(MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT)more.m_hMenu, "更多");
			more.Detach();
		}
		else
		{
			CMenu* subMenu = menu->GetSubMenu(3);
			VERIFY(subMenu);
			if (num > 1)
			{
				subMenu->RemoveMenu(ID_RENAME, MF_BYCOMMAND);
			}
			else
			{
				CString str;
				subMenu->GetMenuStringA(ID_RENAME, str, MF_BYCOMMAND);
				if (str == "")
				{
					subMenu->InsertMenuA(0, MF_STRING | MF_BYPOSITION, ID_RENAME, "重命名");
				}				
			}
		}
		DrawMenuBar();
	}
	//printf("get a click, num = %d\n", num);
	return 0;
}


void CMFCApplication5Dlg::OnUpload()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnUpload();
}


void CMFCApplication5Dlg::OnNewDir()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnNewDir();
}


void CMFCApplication5Dlg::OnDownload()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnDownload();
}


void CMFCApplication5Dlg::OnDelete()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnDelete();
}


void CMFCApplication5Dlg::OnRename()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnRename();
}


void CMFCApplication5Dlg::OnCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnCopy();
}


void CMFCApplication5Dlg::OnCut()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnCut();
}


void CMFCApplication5Dlg::OnOpen()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnOpen();
}


void CMFCApplication5Dlg::OnPaste()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnPaste();
}


void CMFCApplication5Dlg::OnRefresh()
{
	// TODO: 在此添加命令处理程序代码
	m_fileList.OnRefresh();
}

//logout
void CMFCApplication5Dlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_fileList.showTipMessage("抱歉，暂不提供登出功能");
	ShowWindow(SW_HIDE);
	g_isLogin = 0;
	login dlg;
	dlg.DoModal();
	if (!g_isLogin)
	{
		OnExit();
	}
	ShowWindow(SW_SHOW);
	m_fileList.OnRefresh();
}


static int findNext(int now, int add=1)
{
	char* cp = lastPath.GetBuffer();
	int len = lastPath.GetLength();
	for (int i = now+add; i > 0 && i < len; i+=add)
	{
		if (cp[i] == '\\') return i;
	}
	return 1;
}

//<
void CMFCApplication5Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	goforward = 1;
	int next = findNext(path.GetLength()-1, -1);
	path = lastPath.Left(next+1);
	if (path.GetLength() <= 3) goback = 0;
	g_refresh();
	g_showList(&m_fileList);
}

//>
void CMFCApplication5Dlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	goback = 1;
	int next = findNext(path.GetLength()-1);
	path = lastPath.Left(next+1);
	if (path == lastPath) goforward = 0;
	g_refresh();
	g_showList(&m_fileList);
}


void CMFCApplication5Dlg::OnEnKillfocusShow()
{
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CMFCApplication5Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (::TranslateAccelerator(GetSafeHwnd(), m_haccel, pMsg)) return true;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

//note: do nothing!
void CMFCApplication5Dlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//do nothing!!!
}


void CMFCApplication5Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnExit();
	//ShowWindow(SW_HIDE);
	//CDialog::OnClose();
}

//update user path
void CMFCApplication5Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	GetDlgItem(IDC_EDIT1)->SetWindowTextA(path);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(goback);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(goforward);
	CDialog::OnTimer(nIDEvent);
}


void CMFCApplication5Dlg::OnBnClickedSetting()
{
	// TODO: 在此添加控件通知处理程序代码
	m_setDialog->ShowWindow(SW_SHOW);
}


void CMFCApplication5Dlg::AddShellIcon()
{
	// TODO:  在此添加控件通知处理程序代码

	NOTIFYICONDATA  data;

	data.cbSize = sizeof(NOTIFYICONDATA);//结构体的大小
	data.hWnd = m_hWnd;//接收托盘消息的窗口句柄
	data.uID = IDR_MAINFRAME;//定义的托盘图标ID
	data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;//设置属性，下面的三个属性是否有效
	data.uCallbackMessage = WM_TRAY;//自定义的消息，使用#define WM_ONTRAY WM_USER + 123
	data.hIcon = m_hIcon;//显示在系统托盘上的图标
	memcpy(data.szTip, "Client", 6);
	data.szTip[6] = 0;

	Shell_NotifyIcon(NIM_ADD, &data);// 向任务栏的状态栏发送添加托盘图标的消息
}


LRESULT CMFCApplication5Dlg::OnTray(WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDR_MAINFRAME) return 1;
	switch (lParam)
	{
	case WM_RBUTTONUP: {
		CPoint point;
		::GetCursorPos(&point);
		CMenu menu;
		menu.LoadMenu(IDR_MENU4);
		CMenu* pSubMenu = menu.GetSubMenu(0);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	case WM_LBUTTONDBLCLK: {
		ShowWindow(SW_SHOW);
	}
	}
	return 0;
}


void CMFCApplication5Dlg::OnOpenWindow()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_SHOW);
}


void CMFCApplication5Dlg::OnOpenSetting()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedSetting();
}


void CMFCApplication5Dlg::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	//ShowWindow(SW_HIDE);
	int opt = 0;
	send(Client, (char*)&opt, 4, 0);
	NOTIFYICONDATA data;
	data.cbSize = sizeof(NOTIFYICONDATA);
	data.hWnd = m_hWnd;
	data.uID = IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_DELETE, &data);
	EndDialog(IDCANCEL);//很奇怪的事，在这里只调用CDialog::OnClose(); 似乎没有任何用，对话框和进程都还在正常运行。但如果是放在OnClose里调用又可以
	CDialog::OnClose();
}


void CMFCApplication5Dlg::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int x = pNMLV->iSubItem;
	if (x != sortColumn)
	{
		sortColumn = x;
		isDesc = 1;
	}
	else
	{
		isDesc = !isDesc;
	}
	m_fileList.SortItems(CompareFunc, (DWORD_PTR)&m_fileList);
	*pResult = 0;
}
