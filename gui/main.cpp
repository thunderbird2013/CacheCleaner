#define _WIN32_WINNT 0x0501
#define WINVER 0x0501

#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <wchar.h>
#include "tray_icon.h"
#include "ipc_client.h"
#include "about_dialog.h"
#include "resource.h"

HINSTANCE g_hInstance = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            ShowTrayMenu(hWnd, pt);           
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: {

            std::wstring antwort = SendeLeerenBefehlMitAntwort();
            if (antwort.empty()) {
                MessageBoxW(hWnd, L"Dienst nicht erreichbar", L"Fehler", MB_OK | MB_ICONERROR);
            } else {
                NOTIFYICONDATA nid = {};
                nid.cbSize = sizeof(nid);
                nid.hWnd = hWnd;
                nid.uID = 1;
                nid.uFlags = NIF_INFO | NIF_ICON;
                nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); 
                wcscpy_s(nid.szInfoTitle, L"CacheCleaner");
                wcscpy_s(nid.szInfo, antwort.c_str());
                nid.dwInfoFlags = NIIF_USER;
                Shell_NotifyIcon(NIM_MODIFY, &nid);
            }
            break;
        }
        case 2:
            ShowAboutDialog(hWnd);
            break;
        case 3:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
       // StoppeDienst();
        RemoveTrayIcon(hWnd);           
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    g_hInstance = hInstance;

   SC_HANDLE hSCM = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCM) {
        MessageBoxW(nullptr, L"SCManager konnte nicht geöffnet werden.", L"Dienstfehler", MB_OK | MB_ICONERROR);
        return 0;
    }

    SC_HANDLE hService = OpenService(hSCM, L"CacheCleanerService", SERVICE_QUERY_STATUS);
    if (!hService) {
        MessageBoxW(nullptr, L"Dienst 'CacheCleanerService' konnte nicht geöffnet werden.", L"Dienstfehler", MB_OK | MB_ICONERROR);
        CloseServiceHandle(hSCM);
        return 0;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded = 0;
    BOOL ok = QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &bytesNeeded);

    if (!ok) {
        MessageBoxW(nullptr, L"Dienststatus konnte nicht abgefragt werden.", L"Dienstfehler", MB_OK | MB_ICONERROR);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
        return 0;
    }

    if (ssp.dwCurrentState != SERVICE_RUNNING) {
        MessageBoxW(nullptr, L"Der Dienst 'CacheCleanerService' ist nicht gestartet.", L"Dienstfehler", MB_OK | MB_ICONERROR);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
        return 0;
    }

    // Erfolg: Handles schließen
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    HANDLE hMutex = CreateMutex(nullptr, TRUE, L"CacheCleanerSingletonMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(nullptr, L"CacheCleaner läuft bereits.", L"Hinweis", MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    const wchar_t CLASS_NAME[] = L"TrayWnd";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(CLASS_NAME, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);
    if (!hWnd) return 1;

    InitTrayIcon(hInstance, hWnd);

            NOTIFYICONDATA nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = hWnd;
            nid.uID = 1;
            nid.uFlags = NIF_INFO | NIF_ICON;
            nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); // dein eigenes Icon
            nid.hIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 16, 16, 0);
            wcscpy_s(nid.szInfoTitle, L"CacheCleaner");
            wcscpy_s(nid.szInfo, L"CacheCleaner ist aktiv jetzt.");
            nid.dwInfoFlags = NIIF_USER;
            Shell_NotifyIcon(NIM_MODIFY, &nid);


    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseHandle(hMutex);
    return 0;
}
