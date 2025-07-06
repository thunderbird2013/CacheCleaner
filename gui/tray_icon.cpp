#define _WIN32_IE 0x0501
#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>
#include "tray_icon.h"
#include "resource.h"

extern HINSTANCE g_hInstance;

NOTIFYICONDATA nid = {};
HMENU hMenu = nullptr;

bool InitTrayIcon(HINSTANCE hInstance, HWND hWnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    lstrcpy(nid.szTip, TEXT("CacheCleaner"));

    return Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowTrayMenu(HWND hWnd, POINT pt)
{
    if (!hMenu)
    {
        hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, L"🧹 Cache leeren");
        AppendMenu(hMenu, MF_STRING, 2, L"ℹ️ Über CacheCleaner");
        AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(hMenu, MF_STRING, 3, L"❌ Beenden");
    }

    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, nullptr);
}

void RemoveTrayIcon(HWND hWnd)
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
    if (hMenu)
        DestroyMenu(hMenu);
}


void ShowTrayNotification(HWND hWnd, const wchar_t *title, const wchar_t *message, int iconId)
{
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_INFO;

    if (iconId != 0)
    {
        nid.hIcon = (HICON)LoadImageW(g_hInstance, MAKEINTRESOURCE(iconId), IMAGE_ICON, 16, 16, 0);
        nid.uFlags |= NIF_ICON;
        nid.dwInfoFlags = NIIF_USER;
    }
    else
    {
        nid.dwInfoFlags = NIIF_INFO; // Standardsymbol (Info)
    }

    wcsncpy_s(nid.szInfoTitle, title, ARRAYSIZE(nid.szInfoTitle) - 1);
    wcsncpy_s(nid.szInfo, message, ARRAYSIZE(nid.szInfo) - 1);

    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

// The ShowTrayNotification function is commented out as it is not used in the current implementation.