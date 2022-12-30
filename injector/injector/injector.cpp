#include <windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <iostream>
#include <WinBase.h>


DWORD GetProcId(const char* proc_name)
{
    DWORD procId = 0;
    const HANDLE h_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (h_snap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(h_snap, &procEntry))
        {
            do
            {
                if (!_stricmp(procEntry.szExeFile, proc_name))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            }
            while (Process32Next(h_snap, &procEntry));
        }
    }
    CloseHandle(h_snap);
    return procId;
}

int main(int argc, char* argv[])
{
    const char* dllPath = R"(C:\Users\Gabriel Marques\RiderProjects\soulstone_hks\x64\Debug\soulstone_hks.dll)";
    const char* procName = "Soulstone Survivors.exe";
    DWORD procId = 0;

    while (!procId)
    {
        procId = GetProcId(procName);
        Sleep(30);
    }

    const HANDLE h_proc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (h_proc && h_proc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(h_proc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if(loc)
        {
            WriteProcessMemory(h_proc, loc, dllPath, strlen(dllPath) + 1, 0);            
        }


        const HANDLE h_thread = CreateRemoteThread(h_proc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
        if (h_thread)
        {
            CloseHandle(h_thread);
        }
    }

    if (h_proc)
    {
        CloseHandle(h_proc);
    }

    return 0;
}
