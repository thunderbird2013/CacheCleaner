#define UNICODE
#define _UNICODE
#define _WIN32_IE 0x0500
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <commctrl.h>
#include <sstream>
#include <vector>
#include <string>
#include "about_dialog.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

std::vector<std::wstring> GetAboutText()
{
    std::vector<std::wstring> aboutText;
    aboutText.push_back(L"CacheCleaner v1.0");
    aboutText.push_back(L"Ein einfaches Tool zum Löschen von Caches.");
    aboutText.push_back(L"Entwickelt von Matthias Stoltze");
    aboutText.push_back(L"Copyright © 2025");
    aboutText.push_back(L"--------------------------------------------");
    aboutText.push_back(L"License Information:");
    aboutText.push_back(L"No-Derivatives Open License");
    aboutText.push_back(L"Dieses Werk ist Open Source und darf kostenlos und unverändert weitergegeben werden.");
    aboutText.push_back(L"Jegliche Bearbeitung, Modifikation oder abgeleitete Werke sind untersagt.");
    aboutText.push_back(L"Die Nutzung ist nur für nicht-kommerzielle Zwecke gestattet.");
    aboutText.push_back(L"Github Seite: https://github.com/thunderbird2013");
    return aboutText;
}

void SetAboutText(HWND hDlg)
{
    std::vector<std::wstring> lines = GetAboutText();
    std::wstring fullText;

    for (const auto &line : lines)
    {
        fullText += line + L"\r\n";
    }

    SetDlgItemText(hDlg, IDC_TEXT, fullText.c_str());
    SendDlgItemMessage(hDlg, IDC_TEXT, EM_SETSEL, -1, 0);
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetWindowText(hDlg, L"Über CacheCleaner");
        SetDlgItemText(hDlg, IDC_OBEN, L"CacheCleaner v1.0");
        SetAboutText(hDlg);
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ShowAboutDialog(HWND hWnd)
{
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);
    while (true)
    {
        INT_PTR result = DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_ABOUTDLG), hWnd, AboutDlgProc);
        if (result == IDOK || result == IDCANCEL)
            break;
    }
}