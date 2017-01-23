#include "stdafx.h"

Win32Helper::Win32Helper()
{
        valid = InitializeDelayLoading();
}

bool Win32Helper::InitializeDelayLoading()
{
        ntdll = LoadLibrary(L"ntdll");
        if (ntdll == NULL)
                return false;

        _NtQueryInformationProcess = (type_NtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
        if (_NtQueryInformationProcess == NULL)
                return false;
        return true;
}

PCWSTR Win32Helper::GetProcessCommandLine(HANDLE process_handle)
{

        NTSTATUS status;
        BOOL bool_result;
        PROCESS_BASIC_INFORMATION basic_info;
        PEB peb;
        RTL_USER_PROCESS_PARAMETERS process_parameters;
        UNICODE_STRING* command_line;
        static WCHAR buffer[MAX_PATH];
        SIZE_T bytes_read;

        status = (*_NtQueryInformationProcess)(
                        process_handle,
                        ProcessBasicInformation,
                        &basic_info,
                        sizeof(basic_info),
                        NULL);
        if (!NT_SUCCESS(status))
        {
                printf("NtQueryInformationProcess failed with 0x%x\n", status);
                return NULL;
        }

        bool_result = ReadProcessMemory(
                        process_handle,
                        basic_info.PebBaseAddress,
                        &peb,
                        sizeof(peb),
                        &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(Peb) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", sizeof(peb), bytes_read);
                return NULL;
        }

        bool_result = ReadProcessMemory(
                        process_handle,
                        peb.ProcessParameters,
                        &process_parameters,
                        sizeof(process_parameters),
                        &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(ProcessParameters) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", sizeof(peb), bytes_read);
                return NULL;
        }

        command_line = &process_parameters.CommandLine;
        bool_result = ReadProcessMemory(
                        process_handle,
                        command_line->Buffer,
                        buffer,
                        command_line->Length,
                        &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(CmdLine) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", sizeof(peb), bytes_read);
                return NULL;
        }

        return buffer;
}
