
// MFCApplication5.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif
#include "mydata.h"
#include "resource.h"		// 主符号
#include "login.h"
#include "TipDialog.h"
#include "SetDialog.h"

#define LISTCLICK 4011
#define ID_MORE   4012
#define WM_TRAY	  WM_USER+123
// CMFCApplication5App:
// 有关此类的实现，请参阅 MFCApplication5.cpp
//
extern bool g_remember, g_auto, g_isLogin;
extern SOCKET Client;
extern CString path;
class CMFCApplication5App : public CWinApp
{
public:
	CMFCApplication5App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CMFCApplication5App theApp;
