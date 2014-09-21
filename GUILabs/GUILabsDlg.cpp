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

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
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

// диалоговое окно CGUILabsDlg

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

// обработчики сообщений CGUILabsDlg

BOOL CGUILabsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
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

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CGUILabsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
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
    // шифрование input шифратором coder
    // для этого необходимо разбить input на блоки по 8 байт
    // а остаток дополнить нулями.
    // количество нулей в конце запишем в начало результата для корректного дешифрования
    vector<unsigned char> output;
    output.push_back(8 - input.size() % 8);

    for (int i = 0; i < input.size() / 8; i++)
    {
        vector<unsigned char> dataPart8; // очередной 8-байтовый кусок
        dataPart8.insert(dataPart8.end(), input.begin() + i * 8, input.begin() + (i + 1) * 8);

        vector<unsigned char> codedPart = coder.encode(dataPart8);                                  // шифруем кусочек из 8 байт

        output.insert(output.end(), codedPart.begin(), codedPart.end());                            // записываем его в результат
    }

    // теперь разберёмся с оставшимся кусочком
    if (input.size() % 8)
    {
        // сформируем кусок из 8 байт,
        // записав незашифрованные байты и дополнив нулями до длины 8
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
    // дешифрование вектора input шифратором coder
    // первым элементом в векторе input находится количество
    // нулей, которые пришлось вставить в конец шифруемого сообщения
    // для выравнивания длинны
    // затем идут несколько блоков по 8 байт - зашифрованные данные.
    // их-то и будем дешифровывать

    vector<unsigned char> output;
    for (int i = 0; i < (input.size() - 1) / 8; i++)
    {
        vector<unsigned char> part8;
        part8.insert(part8.end(), input.begin() + i * 8 + 1, input.begin() + (i + 1) * 8 + 1);     // формируем очередной кусочек из 8 байт

        vector<unsigned char> decodedPart = coder.decode(part8);                                    // дешифруем его

        output.insert(output.end(), decodedPart.begin(), decodedPart.end());                        // записываем к результату
    }

    for (int i = 0; i < input[0]; i++)
        output.pop_back();                                                                          // избавляемся от лишних нулей в конце сообщения

    return output;
}

void CGUILabsDlg::OpenFiles()
{
    CFileDialog dlg1(TRUE);

    dlg1.m_ofn.lpstrTitle = L"Открыть файл для чтения";

    if (dlg1.DoModal() == IDOK)
        inFile = dlg1.m_ofn.lpstrFile;

    CFileDialog dlg2(TRUE);

    dlg2.m_ofn.lpstrTitle = L"Открыть файл с ключом";

    if (dlg2.DoModal() == IDOK)
        keyFile = dlg2.m_ofn.lpstrFile;

    CFileDialog dlg3(TRUE);

    dlg3.m_ofn.lpstrTitle = L"Открыть файл для записи";

    if (dlg3.DoModal() == IDOK)
        outFile = dlg3.m_ofn.lpstrFile;
}

bool CGUILabsDlg::DEScoder(bool encode)
{
    OpenFiles();

    if (!getFileContent(inFile, inputContent))                                                             // считываем входной файл в вектор
    {
        MessageBox(L"Невозможно открыть файл для чтения!", L"Ошибка!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    if (!getFileContent(keyFile, keyDES))                                                                  // считываем ключ в вектор
    {
        MessageBox(L"Невозможно открыть файл с ключом!", L"Ошибка!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    if (keyDES.size() != 8)                                                                                // проверяем, ключ должен быть 8-байтным
    {
        MessageBox(L"Ключ должен быть не меньше 8 байт!", L"Ошибка!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    DESCoder coder(keyDES);

    if (encode)
        outputContent = encodeVector(inputContent, coder);                                                 // шифрование входного фалйа
    else
        outputContent = decodeVector(inputContent, coder);                                                 // дешифрование входного файла
                                                                                                           // первым делом во входном файле записано количество нулей
    if (!setFileContent(outFile, outputContent))
    {
        MessageBox(L"Невозможно открыть файл для записи!", L"Ошибка!", MB_OK | MB_ICONERROR);

        return -1;
    }
        
    return 1;
}

void CGUILabsDlg::DESencode()
{
    encode = true;

    if (DEScoder(encode))
        MessageBox(L"Программа успешно закодировала файл!", L"Успешно!", MB_OK | MB_ICONINFORMATION);
    else
        MessageBox(L"Программа завершилась с ошибкой!", L"Не выполнено!", MB_OK | MB_ICONINFORMATION);
}

void CGUILabsDlg::DESdecode()
{
    encode = false;

    if (DEScoder(encode))
        MessageBox(L"Программа успешно раскодировала файл!", L"Успешно!", MB_OK | MB_ICONINFORMATION);
    else
        MessageBox(L"Программа завершилась с ошибкой!", L"Не выполнено!", MB_OK | MB_ICONINFORMATION);
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

        if (alf.end() == find(alf.begin(), alf.end(), c))                   // если 'c' содержится в алфавите, 
        {                                                                   // то find возвращает итератор на первый элемент
            out << c;                                                       // этого алфавита, если не содержится - на последний, отсюда и условие,
                                                                            // если элемент не соответствует алфавиту,           
            continue;                                                       // то он сразу пишется в файл без изменений
        }

        if ((c + shift) > 122)                                         
            out << alf[alf.find(c + shift - alf.length())];
        else
            out << alf[alf.find(c + shift)];
    }

    MessageBox(L"Программа успешно закодировала файл!", L"Успешно!", MB_OK | MB_ICONINFORMATION);

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

        if (alf.end() == find(alf.begin(), alf.end(), c))                   // если 'c' содержится в алфавите, 
        {                                                                   // то find возвращает итератор на первый элемент
            out << c;                                                       // этого алфавита, если не содержится - на последний, отсюда и условие,
            // если элемент не соответствует алфавиту,           
            continue;                                                       // то он сразу пишется в файл без изменений
        }

        if ((c - shift) < 97)
            out << alf[alf.find(c - shift + alf.length())];
        else
            out << alf[alf.find(c - shift)];
    }

    MessageBox(L"Программа успешно раскодировала файл!", L"Успешно!", MB_OK | MB_ICONINFORMATION);

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

    MessageBox(L"Программа успешно закодировала файл!", L"Успешно!", MB_OK | MB_ICONINFORMATION);

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

    MessageBox(L"Программа успешно раскодировала файл!", L"Успешно!", MB_OK | MB_ICONINFORMATION);

    in.close();
    out.close();
}