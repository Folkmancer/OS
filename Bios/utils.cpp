#include "stdafx.h"
#include "utils.h"

//
//  FUNCTION: UtilKillTimer(HWND, INT_PTR)
//
//  PURPOSE: Kills specified timer
//
//
BOOL UtilKillTimer(HWND hWnd, INT_PTR pnTimer) {
	BOOL fTimerKilled = FALSE;
	if (NULL != pnTimer) {
		fTimerKilled = KillTimer(hWnd, pnTimer);
		pnTimer = NULL;
	}
	return fTimerKilled;
}