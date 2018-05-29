// DynMenu.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include <ShellAPI.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "DynMenu.h"
#include "st_type.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

/////////////////////////////////////////////////////////////////////////////

int cur_level = 1; // на каком уровне меню находимся
unsigned int count = 0; // указатель на текущий элемент вектора
std::vector<st_type>vk; // вектор, хранящий записи из файла меню

/////////////////////////////////////////////////////////////////////////////

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HMENU				Submenu(int type);
void				LoadMenuFile(char *filename);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.
	// Инициализация глобальных строк

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DYNMENU, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DYNMENU));

    MSG msg;

	// Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DYNMENU));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DYNMENU);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
	
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 250, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	HMENU HMenu;
	HMenu = CreateMenu();

	if (HMenu == NULL)
	{
		MessageBox(NULL, _TEXT("Cannot create menu"), _TEXT("Error"), MB_OK);
		return 0;
	}

	LoadMenuFile("..\\DynMenu\\input.txt");

	HMenu = Submenu(1);

	SetMenu(hWnd, HMenu);
	DrawMenuBar(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			// Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case 1110:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			}
			break;
			case 1200:
				DestroyWindow(hWnd);
				break;
			case 2100:
			{
				
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Добавьте сюда любой код прорисовки, использующий HDC...
			TCHAR greeting[] = TEXT("Win32 application with dynamic menu");
			TextOut(hdc, 5, 5, greeting, _tcslen(greeting));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void LoadMenuFile(char *filename)
{
	cur_level = 1;
	vk.clear();
	// открываем файл *filename
	std::ifstream ifs;
	ifs.open(filename);
	if (!ifs)
	{
		printf("Ошибка при открытии файла.\n");
		exit(1);
	}
	std::string bufer;
	st_type st;
	char *ch_buf = new char();
	// считываем строки из файла
	while (ifs)
	{
		// считываем очередную строку в буфер
		std::getline(ifs, bufer);
		//MessageBox(NULL, std::wstring(bufer.begin(), bufer.end()).c_str(), _TEXT("Error"), MB_OK);
		// если нашли признак окончания меню выходим из цикла
		if (bufer == "end") break;
		//преобразовываем const char* в char*
		ch_buf = const_cast<char *>(bufer.c_str());
		// подсчитываем кол-во знаков '-' перед название пункта
		st.level = strspn(ch_buf, "-");
		// считываем название пункта меню
		while (!bufer.find('-')) {
			bufer.erase(bufer.find('-'),1);
		}
		st.str = std::wstring(bufer.begin(), bufer.end());
		st.used = false;
		// считываем и устанавливаем ID пункта меню
		std::getline(ifs, bufer);
		st.ID = atoi(bufer.c_str());
		// заносим информацию о пункте меню в вектор
		vk.push_back(st);
	}
	// закрываем файл
	ifs.close();
	count = 0;
}

HMENU Submenu(int type) //int type - типменю: =1 (обычное) =2 (popup)
{
	HMENU HMenu;
	// инициализируем очередной пункт меню согласно его типу
	if (type == 1)
		HMenu = CreateMenu();
	else
		HMenu = CreatePopupMenu();
	// в цикле проходим по вектору vk
	while (count != vk.size())
	{
		if (count + 1 >= vk.size())
		{
			AppendMenu(HMenu, MF_STRING, vk.at(count).ID, vk.at(count).str.c_str());
			vk.at(count).used = true;
			return HMenu;
		}
		// если есть подменю
		if (vk.at(count + 1).level > vk.at(count).level)
		{
			int x = count;
			cur_level++;
			count++;
			// вызываем рекурсивно функцию для следующего подуровня
			HMENU HSub = Submenu(type);
			AppendMenu(HMenu, MF_POPUP, (UINT)HSub, vk.at(x).str.c_str());
			vk.at(x).used = true;
		}
		// если считанный пунк меню находится на том же уровне, что и текущий
		else if(vk.at(count + 1).level == vk.at(count).level)
		{
			// добавляем пункт меню
			AppendMenu(HMenu, MF_STRING, vk.at(count).ID, vk.at(count).str.c_str());
			vk.at(count).used = true;
		}
		// если считанный пункт меню на уровень ниже предыдущего
		else if(vk.at(count + 1).level < vk.at(count).level)
		{
			// добавляем пункт меню
			AppendMenu(HMenu, MF_STRING, vk.at(count).ID, vk.at(count).str.c_str());
			vk.at(count).used = true;
			cur_level--;
			return HMenu; // возвращаем сформированное меню
		}
		if ((count + 1) <vk.size())
			if (cur_level> vk.at(count + 1).level) { cur_level--; return HMenu; }
		count++;
	}
	return HMenu;
}
