#include "stdafx.h"
#include "GUILabs.h"
#include "GUILabsDlg.h"
#include "afxdialogex.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

string alf = "abcdefghijklmnopqrstuvwxyz";
CString inFile = NULL;
CString outFile = NULL;
CString keyFile = NULL;
string key;

vector<unsigned char> inputContent;
vector<unsigned char> outputContent;
vector<unsigned char> keyDES;

bool encode;
int shift = 0;

// ���������� ���� CAboutDlg ������������ ��� �������� �������� � ����������

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ������ ����������� ����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

// ����������
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ���������� ���� CGUILabsDlg

CGUILabsDlg::CGUILabsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGUILabsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGUILabsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGUILabsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CGUILabsDlg::DESencode)
    ON_BN_CLICKED(IDC_BUTTON2, &CGUILabsDlg::DESdecode)
    ON_BN_CLICKED(IDC_BUTTON3, &CGUILabsDlg::CaesarEncode)
    ON_BN_CLICKED(IDC_BUTTON4, &CGUILabsDlg::CaesarDecode)
    ON_BN_CLICKED(IDC_BUTTON5, &CGUILabsDlg::VigenereEncode)
    ON_BN_CLICKED(IDC_BUTTON6, &CGUILabsDlg::VigenereDecode)
END_MESSAGE_MAP()

// ����������� ��������� CGUILabsDlg

BOOL CGUILabsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���������� ������ "� ���������..." � ��������� ����.

	// IDM_ABOUTBOX ������ ���� � �������� ��������� �������.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ������ ������ ��� ����� ����������� ����.  ����� ������ ��� �������������,
	//  ���� ������� ���� ���������� �� �������� ����������
	SetIcon(m_hIcon, TRUE);			// ������� ������
	SetIcon(m_hIcon, FALSE);		// ������ ������

	// TODO: �������� �������������� �������������

	return TRUE;  // ������� �������� TRUE, ���� ����� �� ������� �������� ����������
}

void CGUILabsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��� ���������� ������ ����������� � ���������� ���� ����� ��������������� ����������� ���� �����,
//  ����� ���������� ������.  ��� ���������� MFC, ������������ ������ ���������� ��� �������������,
//  ��� ������������� ����������� ������� ��������.

void CGUILabsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �������� ���������� ��� ���������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ������������ ������ �� ������ ����������� ��������������
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ��������� ������
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ������� �������� ��� ������� ��� ��������� ����������� ������� ��� �����������
//  ���������� ����.
HCURSOR CGUILabsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CGUILabsDlg::getFileContent(CString fileName, vector<unsigned char> &fileContent)
{
    ifstream inputStream(fileName, std::ios::binary);

    if (inputStream.fail())
        return false;

    inputStream.seekg(0, std::ios::end);
    int inputSize = inputStream.tellg();
    inputStream.seekg(0, std::ios::beg);

    if (!inputSize)
        return false;

    fileContent.resize(inputSize);
    inputStream.read((char*)&fileContent.front(), inputSize);
    inputStream.close();

    return true;
}

bool CGUILabsDlg::setFileContent(CString fileName, vector<unsigned char> &fileContent)
{
    ofstream outStream(fileName, std::ios::binary);

    outStream.write((char*)&fileContent.front(), fileContent.size());
    outStream.close();

    return true;
}

vector<unsigned char> CGUILabsDlg::encodeVector(vector<unsigned char>& input, DESCoder& coder)
{
    // ���������� input ���������� coder
    // ��� ����� ���������� ������� input �� ����� �� 8 ����
    // � ������� ��������� ������.
    // ���������� ����� � ����� ������� � ������ ���������� ��� ����������� ������������
    vector<unsigned char> output;
    output.push_back(8 - input.size() % 8);

    for (int i = 0; i < input.size() / 8; i++)
    {
        vector<unsigned char> dataPart8; // ��������� 8-�������� �����
        dataPart8.insert(dataPart8.end(), input.begin() + i * 8, input.begin() + (i + 1) * 8);

        vector<unsigned char> codedPart = coder.encode(dataPart8);                                  // ������� ������� �� 8 ����

        output.insert(output.end(), codedPart.begin(), codedPart.end());                            // ���������� ��� � ���������
    }

    // ������ ��������� � ���������� ��������
    if (input.size() % 8)
    {
        // ���������� ����� �� 8 ����,
        // ������� ��������������� ����� � �������� ������ �� ����� 8
        vector<unsigned char> dataPart;
        dataPart.insert(dataPart.end(), input.end() - input.size() % 8, input.end());

        dataPart.insert(dataPart.end(), 8 - input.size() % 8, 0);

        vector<unsigned char> codedPart = coder.encode(dataPart);
        output.insert(output.end(), codedPart.begin(), codedPart.end());
    }

    return output;
}

vector<unsigned char> CGUILabsDlg::decodeVector(vector<unsigned char>& input, DESCoder& coder)
{
    // ������������ ������� input ���������� coder
    // ������ ��������� � ������� input ��������� ����������
    // �����, ������� �������� �������� � ����� ���������� ���������
    // ��� ������������ ������
    // ����� ���� ��������� ������ �� 8 ���� - ������������� ������.
    // ��-�� � ����� �������������

    vector<unsigned char> output;
    for (int i = 0; i < (input.size() - 1) / 8; i++)
    {
        vector<unsigned char> part8;
        part8.insert(part8.end(), input.begin() + i * 8 + 1, input.begin() + (i + 1) * 8 + 1);     // ��������� ��������� ������� �� 8 ����

        vector<unsigned char> decodedPart = coder.decode(part8);                                    // ��������� ���

        output.insert(output.end(), decodedPart.begin(), decodedPart.end());                        // ���������� � ����������
    }

    for (int i = 0; i < input[0]; i++)
        output.pop_back();                                                                          // ����������� �� ������ ����� � ����� ���������

    return output;
}

void CGUILabsDlg::OpenFiles()
{
    CFileDialog dlg1(TRUE);

    dlg1.m_ofn.lpstrTitle = L"������� ���� ��� ������";

    if (dlg1.DoModal() == IDOK)
        inFile = dlg1.m_ofn.lpstrFile;

    CFileDialog dlg2(TRUE);

    dlg2.m_ofn.lpstrTitle = L"������� ���� � ������";

    if (dlg2.DoModal() == IDOK)
        keyFile = dlg2.m_ofn.lpstrFile;

    CFileDialog dlg3(TRUE);

    dlg3.m_ofn.lpstrTitle = L"������� ���� ��� ������";

    if (dlg3.DoModal() == IDOK)
        outFile = dlg3.m_ofn.lpstrFile;
}

bool CGUILabsDlg::DEScoder(bool encode)
{
    OpenFiles();

    if (!getFileContent(inFile, inputContent))                                                             // ��������� ������� ���� � ������
    {
        MessageBox(L"���������� ������� ���� ��� ������!", L"������!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    if (!getFileContent(keyFile, keyDES))                                                                  // ��������� ���� � ������
    {
        MessageBox(L"���������� ������� ���� � ������!", L"������!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    if (keyDES.size() != 8)                                                                                // ���������, ���� ������ ���� 8-�������
    {
        MessageBox(L"���� ������ ���� �� ������ 8 ����!", L"������!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    DESCoder coder(keyDES);

    if (encode)
        outputContent = encodeVector(inputContent, coder);                                                 // ���������� �������� �����
    else
        outputContent = decodeVector(inputContent, coder);                                                 // ������������ �������� �����
                                                                                                           // ������ ����� �� ������� ����� �������� ���������� �����
    if (!setFileContent(outFile, outputContent))
    {
        MessageBox(L"���������� ������� ���� ��� ������!", L"������!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    return 1;
}

void CGUILabsDlg::DESencode()
{
    encode = true;

    if (DEScoder(encode))
        MessageBox(L"��������� ������� ������������ ����!", L"�������!", MB_OK | MB_ICONINFORMATION);
    else
        MessageBox(L"��������� ����������� � �������!", L"�� ���������!", MB_OK | MB_ICONINFORMATION);
}

void CGUILabsDlg::DESdecode()
{
    encode = false;

    if (DEScoder(encode))
        MessageBox(L"��������� ������� ������������� ����!", L"�������!", MB_OK | MB_ICONINFORMATION);
    else
        MessageBox(L"��������� ����������� � �������!", L"�� ���������!", MB_OK | MB_ICONINFORMATION);
}

void CGUILabsDlg::CaesarEncode()
{
    OpenFiles();

    ifstream k(keyFile);
    ifstream in(inFile);
    ofstream out(outFile);

    k >> shift;
    
    if (shift > 26)
        shift = shift % 26;

    for (int i = 0;; i++)
    {
        char c = in.get();

        if (in.eof())
            break;

        if (alf.end() == find(alf.begin(), alf.end(), c))                   // ���� 'c' ���������� � ��������, 
        {                                                                   // �� find ���������� �������� �� ������ �������
            out << c;                                                       // ����� ��������, ���� �� ���������� - �� ���������, ������ � �������,
                                                                            // ���� ������� �� ������������� ��������,           
            continue;                                                       // �� �� ����� ������� � ���� ��� ���������
        }

        if ((c + shift) > 122)                                         
            out << alf[alf.find(c + shift - alf.length())];
        else
            out << alf[alf.find(c + shift)];
    }

    MessageBox(L"��������� ������� ������������ ����!", L"�������!", MB_OK | MB_ICONINFORMATION);

    in.close();
    out.close();
}

void CGUILabsDlg::CaesarDecode()
{
    OpenFiles();

    ifstream k(keyFile);
    ifstream in(inFile);
    ofstream out(outFile);

    k >> shift;

    if (shift > 26)
        shift = shift % 26;

    for (int i = 0;; i++)
    {
        char c = in.get();

        if (in.eof())
            break;

        if (alf.end() == find(alf.begin(), alf.end(), c))                   // ���� 'c' ���������� � ��������, 
        {                                                                   // �� find ���������� �������� �� ������ �������
            out << c;                                                       // ����� ��������, ���� �� ���������� - �� ���������, ������ � �������,
            // ���� ������� �� ������������� ��������,           
            continue;                                                       // �� �� ����� ������� � ���� ��� ���������
        }

        if ((c - shift) < 97)
            out << alf[alf.find(c - shift + alf.length())];
        else
            out << alf[alf.find(c - shift)];
    }

    MessageBox(L"��������� ������� ������������� ����!", L"�������!", MB_OK | MB_ICONINFORMATION);

    in.close();
    out.close();
}


void CGUILabsDlg::VigenereEncode()
{
    OpenFiles();

    ifstream k(keyFile);
    ifstream in(inFile);
    ofstream out(outFile);

    k >> key;

    for (int i = 0;; i++)
    {
        i %= key.length();

        char c = in.get();

        if (in.eof())
            break;

        if (alf.end() == find(alf.begin(), alf.end(), c))
        {
            out << c;

            continue;
        }
        
        out << alf[(c - 2 * alf[0] + key[i]) % alf.length()];
    }

    MessageBox(L"��������� ������� ������������ ����!", L"�������!", MB_OK | MB_ICONINFORMATION);

    in.close();
    out.close();
}

void CGUILabsDlg::VigenereDecode()
{
    OpenFiles();

    ifstream k(keyFile);
    ifstream in(inFile);
    ofstream out(outFile);

    k >> key;

    for (int i = 0;; i++)
    {
        i %= key.length();

        char c = in.get();

        if (in.eof())
            break;

        if (alf.end() == find(alf.begin(), alf.end(), c))
        {
            out << c;

            continue;
        }
        
        out << alf[(c + alf.length() - key[i]) % alf.length()];
    }

    MessageBox(L"��������� ������� ������������� ����!", L"�������!", MB_OK | MB_ICONINFORMATION);

    in.close();
    out.close();
}