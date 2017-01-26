#include "precomp.h"

FindApp::FindApp(HWND hwnd)
        : logger(CHECK_INTERNVAL_IN_SECONDS), timer(hwnd, CHECK_INTERNVAL_IN_SECONDS, TimerCallback, this)
{
}

void FindApp::TimerCallback(PVOID context)
{
        FindApp* _this = (FindApp*)context;
        _this->TimerCallback();
}

void FindApp::TimerCallback()
{
        window_title[0] = L'\0';
        command_line[0] = L'\0';

        timer.OutputTimeStamp();

        if (GetForegroundApp())
                logger.AddEntry(window_title, command_line);
}

HANDLE FindApp::OpenProcess(DWORD process_id)
{
        return ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, NULL, process_id);
}

bool FindApp::GetForegroundApp()
{
        // ISSUE:
        //      Unicode from window title, e.g. "楼宇 - Notepad", is not correctly displayed or saved

        HWND window_handle = GetForegroundWindow();
        if (window_handle == NULL)
                return false;

        DWORD thread_id;
        DWORD process_id;
        thread_id = GetWindowThreadProcessId(window_handle, &process_id);
        if (thread_id == 0)
                return false;

        HANDLE process_handle = OpenProcess(process_id);
        if (process_handle == NULL)
                return false;

        // Note: for universal app, the title is always fixed e.g. "Microsoft Edge". That is not very interesting.
        // Instead, we'd rather get the title from the wwahost app, which tells us which web page is opened
        //
        int result = GetWindowText(window_handle, window_title, sizeof(window_title) / sizeof(window_title[0]));
        if (result != 0)
        {
                printf("Title:   %S", window_title);
                printf("\n"); // unicode output is not working 100%. move endline out so that it wont get eaten
        }
        else
                printf("GetWindowText failed with 0x%x\n", GetLastError());

        if (IsImmersiveProcess(process_handle))
        {
                CloseHandle(process_handle);

                if (!helper.GetUniversalApp(&window_handle, &process_id))
                        return false;

                process_handle = OpenProcess(process_id);
                if (process_handle == NULL)
                        return false;
        }

        bool bool_result = helper.GetProcessCommandLine(process_handle, command_line, sizeof(command_line) / sizeof(command_line[0]));
        if (bool_result)
        {
                printf("CmdLine: %S", command_line);
                printf("\n");
        }

        CloseHandle(process_handle);
        return true;
}
