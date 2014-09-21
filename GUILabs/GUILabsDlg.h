#pragma once
#include "DESCoder.h"

// диалоговое окно CGUILabsDlg
class CGUILabsDlg : public CDialogEx
{
// Создание
public:
	CGUILabsDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_GUILABS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV

// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void DESencode();
    afx_msg void DESdecode();
    afx_msg void CaesarEncode();
    afx_msg void CaesarDecode();
    afx_msg void VigenereEncode();
    afx_msg void VigenereDecode();

    void OpenFiles();

    bool DEScoder(bool encode);

    bool getFileContent(CString fileName, vector<unsigned char> &fileContent);
    bool setFileContent(CString fileName, vector<unsigned char> &fileContent);

    vector<unsigned char> encodeVector(vector<unsigned char>& input, DESCoder& coder);
    vector<unsigned char> decodeVector(vector<unsigned char>& input, DESCoder& coder);
};