
// shanxun.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CshanxunApp:
// 有关此类的实现，请参阅 shanxun.cpp
//

class CshanxunApp : public CWinApp
{
public:
	CshanxunApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CshanxunApp theApp;