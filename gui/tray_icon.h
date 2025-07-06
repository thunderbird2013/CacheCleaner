#ifndef TRAY_ICON_H
#define TRAY_ICON_H

#include <windows.h>

bool InitTrayIcon(HINSTANCE hInstance, HWND hWnd);
void ShowTrayMenu(HWND hWnd, POINT pt);
void RemoveTrayIcon(HWND hWnd);
void ShowTrayNotification(HWND hWnd, const wchar_t* title, const wchar_t* message, int iconId = 0);

#define WM_TRAYICON (WM_USER + 1)

#endif
