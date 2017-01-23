#include "stdafx.h"

class FindApp
{
public:
        void Test();

private:
        Win32Helper helper;
        DWORD GetForegroundWindowProcessId();
        PCWSTR GetProcessCommandLine(DWORD process_id);
};

void wmain()
{
        FindApp find;
        find.Test();
}

void FindApp::Test()
{
        DWORD process_id = GetForegroundWindowProcessId();
        if (process_id == 0)
        {
            return;
        }
        printf("process_id %d\n", process_id);
        PCWSTR command_line = GetProcessCommandLine(process_id);
        if (command_line != NULL)
        {
            printf("command_line: %ws\n", command_line);
        }
}

DWORD FindApp::GetForegroundWindowProcessId()
{
        HWND window_handle = GetForegroundWindow();
        if (window_handle == NULL)
        {
            return 0;
        }

        DWORD thread_id, process_id;
        thread_id = GetWindowThreadProcessId(window_handle, &process_id);
        if (thread_id == 0)
        {
            return 0;
        }

        return process_id;
}

PCWSTR FindApp::GetProcessCommandLine(DWORD process_id)
{
        PCWSTR command_line = NULL;
        HANDLE process_handle = OpenProcess(
                                        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                        NULL,
                                        process_id);
        DWORD error = GetLastError();
        if (process_handle != NULL)
        {
                command_line = helper.GetProcessCommandLine(process_handle);
                error = GetLastError();
                CloseHandle(process_handle);
        }
        return command_line;
}

