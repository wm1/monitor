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

        DWORD process_id = GetForegroundWindowProcessId();
        if (process_id == 0)
                return;

        bool result = GetProcessCommandLine(process_id);
        if (result)
                printf("CmdLine: %S\n", command_line);

        logger.AddEntry(window_title, command_line);
}

DWORD FindApp::GetForegroundWindowProcessId()
{
        // Give me a break. I want to set focus on a different app and test how it works
        //
        // PASS:
        //      C:\WINDOWS\Explorer.EXE
        //      "C:\WINDOWS\system32\cmd.exe"
        //      "C:\WINDOWS\system32\notepad.exe"
        //      "C:\Program Files (x86)\Microsoft Office\root\Office16\OUTLOOK.EXE"
        //      "C:\Program Files (x86)\Microsoft Office\root\Office16\WINWORD.EXE"
        //      "C:\Downloads\Applications\GitForWindows\git-cmd.exe"
        //      "C:\Downloads\Applications\Firefox\firefox.exe"
        //      "C:\Users\<name>\AppData\Local\Apps\2.0\C59CNL51.D6L\P471YA68.WKO\rese..tion_898c62c7805c90ea_0002.0004_5b78ad25d2c5f8af\reSearch v2.exe"
        //
        // PASS (need run findapp.exe as admin)
        //      "C:\WINDOWS\system32\taskmgr.exe"
        //
        // FAILED:
        //      "C:\Program Files\WindowsApps\Microsoft.WindowsCalculator_10.1612.3341.0_x64__8wekyb3d8bbwe\Calculator.exe"
        //              ==> (Displayed as a different process C:\WINDOWS\system32\ApplicationFrameHost.exe)
        //      "C:\Windows\SystemApps\Microsoft.MicrosoftEdge_8wekyb3d8bbwe\MicrosoftEdge.exe"
        //
        // ISSUE:
        //      Unicode from window title, e.g. "楼宇 - Notepad", is not correctly displayed or saved

        HWND window_handle = GetForegroundWindow();
        if (window_handle == NULL)
                return 0;

        int result = GetWindowText(window_handle, window_title, sizeof(window_title) / sizeof(window_title[0]));
        if (result != 0)
        {
                printf("Title:   %S", window_title);
                printf("\n"); // unicode output is not working 100%. move endline out so that it wont get eaten
        }
        else
                printf("GetWindowText failed with 0x%x\n", GetLastError());

        DWORD thread_id;
        DWORD process_id;
        thread_id = GetWindowThreadProcessId(window_handle, &process_id);
        if (thread_id == 0)
                return 0;

        return process_id;
}

bool FindApp::GetProcessCommandLine(DWORD process_id)
{
        bool   result         = false;
        HANDLE process_handle = OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                NULL,
                process_id);
        if (process_handle != NULL)
        {
                result = helper.GetProcessCommandLine(process_handle, command_line, sizeof(command_line) / sizeof(command_line[0]));
                CloseHandle(process_handle);
        }
        return result;
}
