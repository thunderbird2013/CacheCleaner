#define UNICODE
#define _UNICODE

#include <windows.h>
#include <thread>
#include <string>
#include <cstring>
#include <sstream>

#include "memory_cleaner.h"

SERVICE_STATUS_HANDLE gStatusHandle = NULL;
SERVICE_STATUS gServiceStatus = {};
bool running = true;

void ReportStatus(DWORD state) {
    gServiceStatus.dwCurrentState = state;
    SetServiceStatus(gStatusHandle, &gServiceStatus);
}

void WINAPI ServiceControlHandler(DWORD control) {
    if (control == SERVICE_CONTROL_STOP) {
        running = false;
        ReportStatus(SERVICE_STOP_PENDING);
    }
}

void ServiceWorkerThread() {
    while (running) {
        SECURITY_ATTRIBUTES sa = {};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = FALSE;

        PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
        InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(pSD, TRUE, nullptr, FALSE);
        sa.lpSecurityDescriptor = pSD;

        HANDLE hPipe = CreateNamedPipeW(
            L"\\\\.\\pipe\\CacheCleanerPipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_WAIT,
            1,
            1024,
            1024,
            0,
            &sa
        );

        LocalFree(pSD);

        if (hPipe == INVALID_HANDLE_VALUE) break;

        if (ConnectNamedPipe(hPipe, nullptr) || GetLastError() == ERROR_PIPE_CONNECTED) {
            char buffer[64] = {};
            DWORD bytesRead = 0;

            if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
                buffer[bytesRead] = '\0';

                MEMORYSTATUSEX memBefore = { sizeof(memBefore) };
                MEMORYSTATUSEX memAfter  = { sizeof(memAfter) };

                GlobalMemoryStatusEx(&memBefore);
                bool success = false;

                if (strncmp(buffer, "LEERE", 5) == 0) {
                    success = LeereStandbyList();
                }

                if (strncmp(buffer, "LOW", 3) == 0) {
                    success = LeereLowPriorityStandbyList();
                    }

                GlobalMemoryStatusEx(&memAfter);

                std::ostringstream oss;
                if (success) {
                    DWORD beforeMB = (DWORD)(memBefore.ullAvailPhys >> 20);
                    DWORD afterMB  = (DWORD)(memAfter.ullAvailPhys >> 20);
                    oss << "Cache: " << beforeMB << " MB -> " << afterMB << " MB";
                } else {
                    oss << "FEHLER";
                }

                std::string msg = oss.str();
                DWORD bytesWritten = 0;
                WriteFile(hPipe, msg.c_str(), (DWORD)msg.size(), &bytesWritten, nullptr);
            }
        }

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }
}

void WINAPI ServiceMain(DWORD, LPWSTR*) {
    gStatusHandle = RegisterServiceCtrlHandlerW(L"CacheCleanerService", ServiceControlHandler);
    if (!gStatusHandle) return;

    gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    gServiceStatus.dwWin32ExitCode = NO_ERROR;
    gServiceStatus.dwWaitHint = 1000;
    gServiceStatus.dwControlsAccepted = 0;
    SetServiceStatus(gStatusHandle, &gServiceStatus);

    std::thread(ServiceWorkerThread).detach();

    gServiceStatus.dwCurrentState = SERVICE_RUNNING;
    gServiceStatus.dwWaitHint = 0;
    gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SetServiceStatus(gStatusHandle, &gServiceStatus);

    while (running) Sleep(1000);

    gServiceStatus.dwCurrentState = SERVICE_STOPPED;
    gServiceStatus.dwWaitHint = 0;
    gServiceStatus.dwControlsAccepted = 0;
    SetServiceStatus(gStatusHandle, &gServiceStatus);
}

wchar_t serviceName[] = L"CacheCleanerService";

int main() {
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { serviceName, ServiceMain },
        { nullptr, nullptr }
    };

    if (!StartServiceCtrlDispatcher(serviceTable)) {
        MessageBoxW(nullptr, L"Nicht als Dienst gestartet!", L"Fehler", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
