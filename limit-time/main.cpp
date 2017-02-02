/*
 * Description:
 *     When the target app is started, this app is run first, check whether 
 *     the date/time is allowed, and the allow or deny the target to continue
 */

// graphedt.exe
//
// C:\Program Files (x86)\Windows Kits\8.1\bin\x86\graphedt.exe
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\graphedt.exe
//    Debugger = "C:\Windows\system32\limit-time.exe"

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

bool is_weekend(SYSTEMTIME* now)
{
        return now->wDayOfWeek == 0 || now->wDayOfWeek == 6;
}

bool is_weekday(SYSTEMTIME* now)
{
        return !is_weekend(now);
}

bool is_too_late(SYSTEMTIME* now)
{
        return now->wHour >= 21 || now->wHour <= 9;
}

bool shall_it_be_denied()
{
        SYSTEMTIME now;
        GetLocalTime(&now);

        return is_weekday(&now) || is_too_late(&now);
}

PCWSTR get_deny_reason()
{
        return L"Only allowed on weekend";
}

PCWSTR get_cmd_line()
{
        PWSTR cmd_line = GetCommandLine();
        printf("%S\n", cmd_line);

        PCWSTR end;
        if (cmd_line[0] != L'\"')
                end = wcschr(cmd_line, L' ');
        else
                end = wcschr(cmd_line + 1, L'\"');

        if (end == NULL)
                return NULL;

        end++;
        while (*end == L' ')
                end++;

        printf("%S\n", end);
        return end;
}

void start_application(PCWSTR cmd_line)
{
        STARTUPINFO         si;
        PROCESS_INFORMATION pi;
        PWSTR               cmd_line_buf;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        cmd_line_buf = _wcsdup(cmd_line);
        if (cmd_line_buf == NULL)
                return;

        if (!CreateProcess(NULL,          // No module name (use command line)
                           cmd_line_buf,  // Command line
                           NULL,          // Process handle not inheritable
                           NULL,          // Thread handle not inheritable
                           FALSE,         // Set handle inheritance to FALSE
                           DEBUG_PROCESS, // avoid invoking debugger again
                           NULL,          // Use parent's environment block
                           NULL,          // Use parent's starting directory
                           &si,           // Pointer to STARTUPINFO structure
                           &pi))          // Pointer to PROCESS_INFORMATION structure
        {
                printf("CreateProcess failed with 0x%x\n", GetLastError());
        }

        DebugSetProcessKillOnExit(FALSE);

        free(cmd_line_buf);
}

int CALLBACK WinMain(
        _In_ HINSTANCE /*hInstance*/,
        _In_ HINSTANCE /*hPrevInstance*/,
        _In_ LPSTR /*lpCmdLine*/,
        _In_ int /*nCmdShow*/)
{
        // FILE* ignored;
        // AllocConsole();
        // freopen_s(&ignored, "CON", "w", stdout);

        if (shall_it_be_denied())
        {
                printf("denied\n");
                MessageBox(NULL, get_deny_reason(), L"Warning", MB_ICONWARNING | MB_OK);
                return 0;
        }

        PCWSTR cmd_line = get_cmd_line();
        start_application(cmd_line);
        return 0;
}
