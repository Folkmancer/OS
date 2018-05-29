#pragma once

#include "resource.h"
#include <shellapi.h>

#define TRAY_ICONUID 100
#define TRAY_ID_TIMER_CLICK 100
#define TRAY_CLICK_DELAY 10

extern HINSTANCE hInst;

void TrayDrawIcon(HWND hWnd);
void TrayDeleteIcon(HWND hWnd);
void TrayLoadPopupMenu(HWND hWnd);

void TrayStartClickDelayTimer(HWND hWnd);
void TrayKillClickDelayTimer(HWND hWnd);
void CALLBACK TrayOnTimerIconClick(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);