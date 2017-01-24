#include "precomp.h"

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

#if !defined(_WIN64)
#error To simplify the code, we only want to build for x64
#endif

bool Win32Helper::GetProcessCommandLine(HANDLE process_handle, _Out_ PWSTR command_line_buffer, size_t buffer_size_in_wchar)
{
        BOOL is_wow64;
        if (!IsWow64Process(process_handle, &is_wow64))
        {
                printf("IsWow64Process failed with 0x%x\n", GetLastError());
                return false;
        }

        NTSTATUS                  status;
        PROCESS_BASIC_INFORMATION basic_info;
        PVOID                     peb32_address;

        if (!is_wow64)
                status = (*_NtQueryInformationProcess)(
                        process_handle,
                        ProcessBasicInformation,
                        &basic_info,
                        sizeof(basic_info),
                        NULL);
        else
                status = (*_NtQueryInformationProcess)(
                        process_handle,
                        ProcessWow64Information,
                        &peb32_address,
                        sizeof(peb32_address),
                        NULL);
        if (!NT_SUCCESS(status))
        {
                printf("NtQueryInformationProcess failed with 0x%x\n", status);
                return false;
        }

        PEB    peb;
        BOOL   bool_result;
        size_t bytes_read;

#pragma pack(push, 1)
        typedef __int32 POINTER32;
        struct PEB32
        {
                BYTE      Reserved1[2];
                BYTE      BeingDebugged;
                BYTE      Reserved2[1];
                POINTER32 Reserved3[2];
                POINTER32 Ldr;
                POINTER32 ProcessParameters;
        } peb32;
#pragma pack(pop)

        PVOID  address;
        PVOID  buffer;
        size_t size;

        if (!is_wow64)
        {
                address = basic_info.PebBaseAddress;
                buffer  = &peb;
                size    = sizeof(peb);
        }
        else
        {
                address = peb32_address;
                buffer  = &peb32;
                size    = sizeof(peb32);
        }

        bool_result = ReadProcessMemory(process_handle, address, buffer, size, &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(Peb) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", size, bytes_read);
                return false;
        }

        RTL_USER_PROCESS_PARAMETERS process_parameters;

#pragma pack(push, 1)
        struct UNICODE32
        {
                USHORT    Length;
                USHORT    MaximumLength;
                POINTER32 Buffer;
        };
        struct RTL_USER_PROCESS_PARAMETERS32
        {
                BYTE      Reserved1[16];
                POINTER32 Reserved2[10];
                UNICODE32 ImagePathName;
                UNICODE32 CommandLine;
        } process_parameters32;
#pragma pack(pop)

        if (!is_wow64)
        {
                address = peb.ProcessParameters;
                buffer  = &process_parameters;
                size    = sizeof(process_parameters);
        }
        else
        {
                address = (PVOID)(ULONG_PTR)peb32.ProcessParameters;
                buffer  = &process_parameters32;
                size    = sizeof(process_parameters32);
        }

        bool_result = ReadProcessMemory(process_handle, address, buffer, size, &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(Peb) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", size, bytes_read);
                return false;
        }

        if (!is_wow64)
        {
                address = process_parameters.CommandLine.Buffer;
                buffer  = command_line_buffer;
                size    = process_parameters.CommandLine.Length;
        }
        else
        {
                address = (PVOID)(ULONG_PTR)process_parameters32.CommandLine.Buffer;
                buffer  = command_line_buffer;
                size    = process_parameters32.CommandLine.Length;
        }

        buffer_size_in_wchar *= 2;                       // convert buffer size in wchar to byte
        if (size > buffer_size_in_wchar - sizeof(L'\0')) // leave room for '\0'
                size = buffer_size_in_wchar - sizeof(L'\0');

        bool_result = ReadProcessMemory(process_handle, address, buffer, size, &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(CmdLine) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", size, bytes_read);
                return false;
        }
        command_line_buffer[bytes_read / 2] = L'\0';

        return true;
}
