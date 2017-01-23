#include "precomp.h"

class FindApp
{
public:
        void Test();

private:
        Win32Helper helper;
        DWORD       GetForegroundWindowProcessId();
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
                return;

        printf("process_id %d\n", process_id);
        PCWSTR command_line = GetProcessCommandLine(process_id);
        if (command_line != NULL)
                printf("command_line: %S\n", command_line);
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

#define SECONDS_TO_MILLISECOND(x) ((x)*1000)
        Sleep(SECONDS_TO_MILLISECOND(3));

        HWND window_handle = GetForegroundWindow();
        if (window_handle == NULL)
                return 0;

        WCHAR buffer[MAX_PATH];
        int   result = GetWindowText(window_handle, buffer, sizeof(buffer) / sizeof(buffer[0]));
        if (result != 0)
                printf("Title: %S\n", buffer);
        else
                printf("GetWindowText failed with 0x%x\n", GetLastError());

        DWORD thread_id, process_id;
        thread_id = GetWindowThreadProcessId(window_handle, &process_id);
        if (thread_id == 0)
                return 0;

        return process_id;
}

PCWSTR FindApp::GetProcessCommandLine(DWORD process_id)
{
        PCWSTR command_line   = NULL;
        HANDLE process_handle = OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                NULL,
                process_id);
        if (process_handle != NULL)
        {
                command_line = helper.GetProcessCommandLine(process_handle);
                CloseHandle(process_handle);
        }
        return command_line;
}
