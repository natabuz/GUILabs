
// GUILabs.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CGUILabsApp:
// О реализации данного класса см. GUILabs.cpp
//

class CGUILabsApp : public CWinApp
{
public:
	CGUILabsApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CGUILabsApp theApp;