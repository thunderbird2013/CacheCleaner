#include "ipc_client.h"
#include <string>
#include <sstream>
#include <windows.h>


bool SendeLeerenBefehl() {
    HANDLE hPipe = INVALID_HANDLE_VALUE;

    // Retry-Mechanismus
    for (int i = 0; i < 5; ++i) {
        hPipe = CreateFileW(
            L"\\\\.\\pipe\\CacheCleanerPipe",
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, 0, nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_FILE_NOT_FOUND)
            break;

        Sleep(300);
    }

    if (hPipe == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();

        std::wstringstream ss;
        ss << L"Verbindung zur Named Pipe fehlgeschlagen.\n\nFehlercode: " << err;

        if (err == ERROR_FILE_NOT_FOUND)
            ss << L"\n(Der Dienst läuft vermutlich nicht oder hat die Pipe nicht erstellt)";
        else if (err == ERROR_ACCESS_DENIED)
            ss << L"\n(Zugriff verweigert – evtl. Pipe bereits exklusiv offen oder Rechteproblem)";
        else
            ss << L"\n(Unbekannter Fehler)";

        MessageBoxW(nullptr, ss.str().c_str(), L"Verbindungsfehler", MB_OK | MB_ICONERROR);
        return false;
    }

    DWORD mode = PIPE_READMODE_BYTE;
    SetNamedPipeHandleState(hPipe, &mode, nullptr, nullptr);

    const char* msg = "LEERE";
    DWORD written;
    BOOL result = WriteFile(hPipe, msg, 5, &written, nullptr);

    if (!result || written != 5) {
        DWORD err = GetLastError();
        std::wstringstream ss;
        ss << L"Schreiben in die Pipe fehlgeschlagen.\n\nFehlercode: " << err;
        MessageBoxW(nullptr, ss.str().c_str(), L"Schreibfehler", MB_OK | MB_ICONERROR);
        CloseHandle(hPipe);
        return false;
    }

    // Antwort lesen
    char response[32] = {};
    DWORD read = 0;
    BOOL readOk = ReadFile(hPipe, response, sizeof(response) - 1, &read, nullptr);
    CloseHandle(hPipe);

    response[read] = '\0';
    if (!readOk || strcmp(response, "OK") != 0) {
        MessageBoxW(nullptr, L"Dienst antwortete mit Fehler.", L"Antwort", MB_OK | MB_ICONWARNING);
        return false;
    }

    MessageBoxW(nullptr, L"Standby-Cache erfolgreich geleert.", L"Erfolg", MB_OK | MB_ICONINFORMATION);
    return true;
}

std::wstring SendeLeerenBefehlMitAntwort() {
    HANDLE hPipe = INVALID_HANDLE_VALUE;

    for (int i = 0; i < 5; ++i) {
        hPipe = CreateFileW(
            L"\\\\.\\pipe\\CacheCleanerPipe",
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, 0, nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_FILE_NOT_FOUND)
            break;

        Sleep(300);
    }

    if (hPipe == INVALID_HANDLE_VALUE) {
        return L"";
    }

    DWORD mode = PIPE_READMODE_BYTE;
    SetNamedPipeHandleState(hPipe, &mode, nullptr, nullptr);

    const char* msg = "LEERE";
    DWORD written;
    BOOL result = WriteFile(hPipe, msg, 5, &written, nullptr);

    if (!result || written != 5) {
        CloseHandle(hPipe);
        return L"";
    }

    char response[128] = {};
    DWORD read = 0;
    BOOL readOk = ReadFile(hPipe, response, sizeof(response) - 1, &read, nullptr);
    CloseHandle(hPipe);

    if (!readOk || read == 0) return L"";

    response[read] = '\0';

    std::string responseStr(response);
    return std::wstring(responseStr.begin(), responseStr.end());
    }
