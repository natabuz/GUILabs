
// GUILabs.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CGUILabsApp:
// � ���������� ������� ������ ��. GUILabs.cpp
//

class CGUILabsApp : public CWinApp
{
public:
	CGUILabsApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CGUILabsApp theApp;