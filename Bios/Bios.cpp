// Bios.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Bios.h"
#include "tray.h"
#include "message.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <WinBase.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

////////////////////////////////////

MEMORYSTATUS MS;
HKEY hKeyResult = 0;
DWORD dwType;
DWORD dwBytes = 256;
DWORD szcomp;
DWORD szuser;
TCHAR BIOSDate[500] = { '\0' };
TCHAR BIOSVer[256] = { '\0' };
TCHAR GPUDate[256] = { '\0' };
TCHAR CPUID[256] = { '\0' };
TCHAR CPUName[256] = { '\0' };
TCHAR CompName[MAX_COMPUTERNAME_LENGTH + 1] = { '\0' };
TCHAR UserName[256 + 1] = { '\0' };

TCHAR CommandLineCalc[] = _T("calc.exe");
TCHAR CommandLinePing[] = _T("ping.exe -t 192.168.1.1");
STARTUPINFO siCalc = { sizeof(siCalc) };
STARTUPINFO siPing = { sizeof(siPing) };
PROCESS_INFORMATION CalcInfo;
PROCESS_INFORMATION PingInfo;

FILETIME lpCreation, lpExit, lpKernel, lpUser;
SYSTEMTIME st, stStart, stEnd, stCreation, stExit, stKernel, stUser;
TCHAR str[500];
static DWORD exitCode;
HWND fWindow, fWindowEx;
DWORD dwPID;
HANDLE htWindow;

/////////////////////////////////////

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_BIOS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BIOS));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIOS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BIOS);
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
   
   HWND hWnd = CreateWindowW(szWindowClass, L"BiosInfo", WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, CW_USEDEFAULT, 600, 320, nullptr, nullptr, hInstance, nullptr); /*szTitle*/
  
   if (!hWnd)
   {
      return FALSE;
   }

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
			case IDM_TRAY_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_TRAY_SHOW:
				TrayDeleteIcon(hWnd);
				ShowWindow(hWnd, SW_SHOW);
				break;
			case IDM_TRAY_EXIT:
            case IDM_EXIT:
				TrayDeleteIcon(hWnd);
                DestroyWindow(hWnd);
                break;
			case IDM_CALC:
				if (!CreateProcess(NULL, CommandLineCalc, NULL, NULL, FALSE, 0, NULL, NULL, &siCalc, &CalcInfo)) 
									MessageBox(hWnd, _T("Ошибка создания процесса"), _T("Сообщение"), MB_OK);
				else GetLocalTime(&stStart);
				break;
			case IDM_PING:
				if (!CreateProcess(NULL, CommandLinePing, NULL, NULL, FALSE, 0, NULL, NULL, &siPing, &PingInfo)) 
					MessageBox(hWnd, _T("Ошибка создания процесса"), _T("Сообщение"), MB_OK);
				break;
			case IDM_PING_EXIT:
				if (!(TerminateProcess(PingInfo.hProcess, NO_ERROR))) 
					MessageBox(hWnd, _T("Процесс не запущен"), _T("Сообщение"), MB_OK);
				break;
			case IDM_CALC_TIME:
				GetExitCodeProcess(CalcInfo.hProcess, &exitCode);
				if (exitCode == STILL_ACTIVE)
				{
					/*
					if (GetProcessTimes(CalcInfo.hProcess, &lpCreation, &lpExit, &lpKernel, &lpUser))
					{
						FileTimeToSystemTime(&lpCreation, &stCreation);
						FileTimeToSystemTime(&lpExit, &stExit);
						FileTimeToSystemTime(&lpUser, &stUser);
						FileTimeToSystemTime(&lpKernel, &stKernel);
						GetLocalTime(&st);
						st.wDay = st.wDay - stCreation.wDay;
						st.wHour = st.wHour - stCreation.wHour - 5;
						st.wMinute = st.wMinute - stCreation.wMinute;
						st.wSecond = st.wSecond - stCreation.wSecond;
						st.wMilliseconds = st.wMilliseconds - stCreation.wMilliseconds;
						wsprintf(str, _T("Время: %u часы: %u минуты: %u секунды: \n"), st.wHour, st.wMinute, st.wSecond);
						MessageBox(hWnd, str, _T("Сообщение"), MB_OK);
					}*/
					GetLocalTime(&stEnd);
					stEnd.wDay = stEnd.wDay - stStart.wDay;
					stEnd.wHour = stEnd.wHour - stStart.wHour;
					stEnd.wMinute = stEnd.wMinute - stStart.wMinute;
					stEnd.wSecond = stEnd.wSecond - stStart.wSecond;
					stEnd.wMilliseconds = stEnd.wMilliseconds - stStart.wMilliseconds;
					wsprintf(str, _T("Время: %u часы: %u минуты: %u секунды: \n"), stEnd.wHour, stEnd.wMinute, stEnd.wSecond);
						MessageBox(hWnd, str, _T("Сообщение"), MB_OK);
				}
				else if (exitCode != STILL_ACTIVE)
				{/*
					if (GetProcessTimes(CalcInfo.hProcess, &lpCreation, &lpExit, &lpKernel, &lpUser))
					{
						FileTimeToSystemTime(&lpCreation, &stCreation);
						FileTimeToSystemTime(&lpExit, &stExit);
						FileTimeToSystemTime(&lpUser, &stUser);
						FileTimeToSystemTime(&lpKernel, &stKernel);
						GetLocalTime(&st);
						stExit.wDay = stExit.wDay - stCreation.wDay;
						stExit.wHour = stExit.wHour - stCreation.wHour;
						stExit.wMinute = stExit.wMinute - stCreation.wMinute;
						stExit.wSecond = stExit.wSecond - stCreation.wSecond;
						stExit.wMilliseconds = stExit.wMilliseconds - stCreation.wMilliseconds;
						wsprintf(str, _T("Время: %u часы: %u минуты: %u секунды: \n"), stExit.wHour, stExit.wMinute, stExit.wSecond);
						MessageBox(hWnd, str, _T("Message"), MB_OK);
					}*/
					stEnd.wDay = stEnd.wDay - stStart.wDay;
					stEnd.wHour = stEnd.wHour - stStart.wHour;
					stEnd.wMinute = stEnd.wMinute - stStart.wMinute;
					stEnd.wSecond = stEnd.wSecond - stStart.wSecond;
					stEnd.wMilliseconds = stEnd.wMilliseconds - stStart.wMilliseconds;
					wsprintf(str, _T("Время: %u часы: %u минуты: %u секунды: \n"), stEnd.wHour, stEnd.wMinute, stEnd.wSecond);
						MessageBox(hWnd, str, _T("Сообщение"), MB_OK);
				}
				else MessageBox(hWnd, _T("Процесс не запущен"), _T("Сообщение"), MB_OK);
				break;
			case IDM_PING_TIME:
				GetExitCodeProcess(PingInfo.hProcess, &exitCode);
				if (exitCode == STILL_ACTIVE)
				{
					if (GetProcessTimes(PingInfo.hProcess, &lpCreation, &lpExit, &lpKernel, &lpUser))
					{
						FileTimeToSystemTime(&lpCreation, &stCreation);
						FileTimeToSystemTime(&lpExit, &stExit);
						FileTimeToSystemTime(&lpUser, &stUser);
						FileTimeToSystemTime(&lpKernel, &stKernel);
						GetLocalTime(&st);
						st.wDay = st.wDay - stCreation.wDay;
						st.wHour = st.wHour - stCreation.wHour - 5;
						st.wMinute = st.wMinute - stCreation.wMinute;
						st.wSecond = st.wSecond - stCreation.wSecond;
						st.wMilliseconds = st.wMilliseconds - stCreation.wMilliseconds;
						wsprintf(str, _T("Время: %u часы: %u минуты: %u секунды:\n"), st.wHour, st.wMinute, st.wSecond);
						MessageBox(hWnd, str, _T("Сообщение"), MB_OK);
					}
				}
				else if (exitCode != STILL_ACTIVE)
				{
					if (GetProcessTimes(PingInfo.hProcess, &lpCreation, &lpExit, &lpKernel, &lpUser))
					{
						FileTimeToSystemTime(&lpCreation, &stCreation);
						FileTimeToSystemTime(&lpExit, &stExit);
						FileTimeToSystemTime(&lpUser, &stUser);
						FileTimeToSystemTime(&lpKernel, &stKernel);
						GetLocalTime(&st);
						stExit.wDay = stExit.wDay - stCreation.wDay;
						stExit.wHour = stExit.wHour - stCreation.wHour;
						stExit.wMinute = stExit.wMinute - stCreation.wMinute;
						stExit.wSecond = stExit.wSecond - stCreation.wSecond;
						stExit.wMilliseconds = stExit.wMilliseconds - stCreation.wMilliseconds;
						wsprintf(str, _T("Время: %u часы: %u минуты: %u секунды: \n"), stExit.wHour, stExit.wMinute, stExit.wSecond);
						MessageBox(hWnd, str, _T("Сообщение"), MB_OK);
					}
				}
				else MessageBox(hWnd, _T("Процесс не запущен"), _T("Сообщение"), MB_OK);
				break;
			case IDM_CALC_EXIT:
			case IDM_CALC_CLOSE:
				if (fWindow = FindWindow(NULL, _T("Калькулятор"))) {
					fWindowEx = FindWindowEx(fWindow, NULL, NULL, NULL);
					dwPID = NULL;
					GetWindowThreadProcessId(fWindowEx, &dwPID);
					htWindow = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwPID);
					GetExitCodeThread(htWindow, &exitCode);
					TerminateProcess(htWindow, exitCode);
				}
				else MessageBox(hWnd, _T("Процесс не запущен"), _T("Сообщение"), MB_OK);
				break;
			case IDM_PAINT_CLOSE:
				if (fWindow = FindWindow(NULL, _T("Безымянный - Paint"))) {
					fWindowEx = FindWindowEx(fWindow, NULL, NULL, NULL);
					dwPID = NULL;
					GetWindowThreadProcessId(fWindow, &dwPID);					
					htWindow = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwPID);
					GetExitCodeThread(htWindow, &exitCode);
					TerminateProcess(htWindow, exitCode);
				}
				else MessageBox(hWnd, _T("Процесс не запущен"), _T("Сообщение"), MB_OK);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			szcomp = MAX_COMPUTERNAME_LENGTH + 1;
			szuser = sizeof(&UserName);
			
			//////////////////////////////////////

			LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", NULL, KEY_QUERY_VALUE, &hKeyResult);
			lResult = RegQueryValueEx(hKeyResult, L"BiosReleaseDate", NULL, &dwType, (LPBYTE)BIOSDate, &dwBytes);
			lResult = RegQueryValueEx(hKeyResult, L"BiosVersion", NULL, &dwType, (LPBYTE)BIOSVer, &dwBytes);
			RegCloseKey(hKeyResult);
			dwBytes = 256;
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System", NULL, KEY_QUERY_VALUE, &hKeyResult);
			lResult = RegQueryValueEx(hKeyResult, L"VideoBiosDate", NULL, &dwType, (LPBYTE)GPUDate, &dwBytes);
			RegCloseKey(hKeyResult);
			dwBytes = 256;
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", NULL, KEY_QUERY_VALUE, &hKeyResult);
			lResult = RegQueryValueEx(hKeyResult, L"Identifier", NULL, &dwType, (LPBYTE)CPUID, &dwBytes);
			dwBytes = 256;
			lResult = RegQueryValueEx(hKeyResult, L"ProcessorNameString", NULL, &dwType, (LPBYTE)CPUName, &dwBytes);
			RegCloseKey(hKeyResult);

			TextOut(hdc, 5, 5, L"Дата BIOS: ", 10);
			TextOut(hdc, 250, 5, BIOSDate, _tcslen(BIOSDate));
			TextOut(hdc, 5, 25, L"Версия BIOS: ", 14);
			TextOut(hdc, 250, 25, BIOSVer, _tcslen(BIOSVer));
			TextOut(hdc, 5, 45, L"Дата BIOS видеокарты: ", 23);
			TextOut(hdc, 250, 45, GPUDate, _tcslen(GPUDate));
			TextOut(hdc, 5, 65, L"Идентификатор процессора: ", 27);
			TextOut(hdc, 250, 65, CPUID, _tcslen(CPUID));
			TextOut(hdc, 5, 85, L"Имя процессора: ", 17);
			TextOut(hdc, 250, 85, CPUName, _tcslen(CPUName));
			
			//////////////////////////////////////

			GlobalMemoryStatus(&MS);
			
			long x = MS.dwTotalPhys / 1024 / 1024;
			std::string s = std::to_string(x) + " mb";
			std::wstring stemp = std::wstring(s.begin(), s.end());
			LPCWSTR dwTotalPhys = stemp.c_str();
			TextOut(hdc, 5, 105, L"Объем физической памяти: ", 26);
			TextOut(hdc, 250, 105, dwTotalPhys, s.length());

			x = MS.dwMemoryLoad;
			s = std::to_string(x) + "%";
			stemp = std::wstring(s.begin(), s.end());
			LPCWSTR dwMemoryLoad = stemp.c_str();
			TextOut(hdc, 5, 125, L"Памяти занято: ", 16);
			TextOut(hdc, 250, 125, dwMemoryLoad, s.length());
			
			x = MS.dwTotalPageFile / 1024 / 1024;
			s = std::to_string(x) + " mb";
			stemp = std::wstring(s.begin(), s.end());
			LPCWSTR dwTotalPageFile = stemp.c_str();
			TextOut(hdc, 5, 145, L"Объем файла подкачки: ", 23);
			TextOut(hdc, 250, 145, dwTotalPageFile, s.length());
			
			x = MS.dwAvailPageFile / 1024 / 1024;
			s = std::to_string(x) + " mb";
			stemp = std::wstring(s.begin(), s.end());
			LPCWSTR dwAvailPageFile = stemp.c_str();
			TextOut(hdc, 5, 165, L"Свободный объем файла подкачки: ", 33);
			TextOut(hdc, 250, 165, dwAvailPageFile, s.length());
			
			x = MS.dwTotalVirtual / 1024 / 1024 / 1024;
			s = std::to_string(x) + " gb";
			stemp = std::wstring(s.begin(), s.end());
			LPCWSTR dwTotalVirtual = stemp.c_str();
			TextOut(hdc, 5, 185, L"Объем адресного пространства: ", 31);
			TextOut(hdc, 250, 185, dwTotalVirtual, s.length());

			//////////////////////////////////////

			GetUserName(UserName, &szuser);
			GetComputerName(CompName, &szcomp);

			TextOut(hdc, 5, 205, L"Имя компьютера: ", 17);
			TextOut(hdc, 250, 205, CompName, _tcslen(CompName));
			TextOut(hdc, 5, 225, L"Имя пользователя: ", 19);
			TextOut(hdc, 250, 225, UserName, _tcslen(UserName));

			//////////////////////////////////////

            EndPaint(hWnd, &ps);
        }
        break;
	case WM_CLOSE:
		TrayDrawIcon(hWnd);
		ShowWindow(hWnd, SW_HIDE);
		break;
	case WM_TRAYMESSAGE:
		switch (lParam) {
		case WM_LBUTTONDBLCLK:
			TrayKillClickDelayTimer(hWnd);
			TrayDeleteIcon(hWnd);
			ShowWindow(hWnd, SW_SHOW);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			TrayStartClickDelayTimer(hWnd);
			//TrayLoadPopupMenu(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
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