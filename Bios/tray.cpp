#include "stdafx.h"
#include "tray.h"
#include "message.h"
#include "utils.h"

INT_PTR pTimerTrayClick = NULL;	// tray click timer id

//
//  FUNCTION: TrayDrawIcon(HWND)
//
//  PURPOSE:  Draws application icon in a system tray
//
//
void TrayDrawIcon(HWND hWnd) {
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYMESSAGE;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
	LoadString(hInst, IDS_APP_TITLE, nid.szTip, 128);
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &nid);
}

//
//  FUNCTION: TrayDeleteIcon(HWND)
//
//  PURPOSE:  Deletes application icon from system tray
//
//
void TrayDeleteIcon(HWND hWnd) {
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

//
//  FUNCTION: TrayLoadPopupMenu(HWND)
//
//  PURPOSE:  Load tray specific popup menu
//
//
void TrayLoadPopupMenu(HWND hWnd) {
	POINT cursor;
	HMENU hMenu;
	GetCursorPos(&cursor);
	hMenu = (HMENU)GetSubMenu(LoadMenu(hInst, MAKEINTRESOURCE(IDR_TRAY)), 0);
	SetMenuDefaultItem(hMenu, IDM_TRAY_SHOW, false);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, cursor.x, cursor.y, 0, hWnd, NULL);
}


//
//  FUNCTION: TrayStartClickDelayTimer(HWND)
//
//  PURPOSE:  Starts click delay timer which displays a popup menu
//
//  COMMENTS: Needed due to event bubbling when double click occurs
//
//
void TrayStartClickDelayTimer(HWND hWnd) {
	pTimerTrayClick = SetTimer(hWnd, TRAY_ID_TIMER_CLICK, GetDoubleClickTime() + TRAY_CLICK_DELAY, TrayOnTimerIconClick);
}

//
//  FUNCTION: TrayKillClickDelayTimer(HWND)
//
//  PURPOSE:  Kills click delay timer
//
//
void TrayKillClickDelayTimer(HWND hWnd) {
	UtilKillTimer(hWnd, pTimerTrayClick);
}

//
//  FUNCTION: TrayOnTimerIconClick(HWND, UINT, UINT_PTR, DWORD)
//
//  PURPOSE:  Click delay timer callback function which displays tray specific popup menu
//
//
void CALLBACK TrayOnTimerIconClick(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	UtilKillTimer(hWnd, pTimerTrayClick);
	TrayLoadPopupMenu(hWnd);
}