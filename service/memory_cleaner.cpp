#include <windows.h>
#include "memory_cleaner.h"

typedef enum _SYSTEM_MEMORY_LIST_COMMAND {
    MemoryPurgeStandbyList = 4,
    MemoryPurgeLowPriorityStandbyList = 5
} SYSTEM_MEMORY_LIST_COMMAND;

bool LeereStandbyList() {
    SYSTEM_MEMORY_LIST_COMMAND cmd = MemoryPurgeStandbyList;

    typedef NTSTATUS (NTAPI *pNtSetSystemInformation)(int, PVOID, ULONG);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return false;

    pNtSetSystemInformation NtFunc = (pNtSetSystemInformation)
        GetProcAddress(hNtdll, "NtSetSystemInformation");

    if (!NtFunc) return false;

    NTSTATUS status = NtFunc(80, &cmd, sizeof(cmd));
    return status == 0;
}

bool LeereLowPriorityStandbyList() {
 
    SYSTEM_MEMORY_LIST_COMMAND cmd = MemoryPurgeLowPriorityStandbyList;

    typedef NTSTATUS (NTAPI *pNtSetSystemInformation)(int, PVOID, ULONG);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return false;

    pNtSetSystemInformation NtFunc = (pNtSetSystemInformation)
        GetProcAddress(hNtdll, "NtSetSystemInformation");

    if (!NtFunc) return false;

    NTSTATUS status = NtFunc(80, &cmd, sizeof(cmd));
    return status == 0;
}


