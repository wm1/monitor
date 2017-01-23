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

PCWSTR Win32Helper::GetProcessCommandLine(HANDLE process_handle)
{
        BOOL is_wow64;
        if (!IsWow64Process(process_handle, &is_wow64))
        {
                printf("IsWow64Process failed with 0x%x\n", GetLastError());
                return NULL;
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
                return NULL;
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

        if (!is_wow64)
                bool_result = ReadProcessMemory(
                        process_handle,
                        basic_info.PebBaseAddress,
                        &peb,
                        sizeof(peb),
                        &bytes_read);
        else
                bool_result = ReadProcessMemory(
                        process_handle,
                        peb32_address,
                        &peb32,
                        sizeof(peb32),
                        &bytes_read);

        if (bool_result == 0)
        {
                printf("ReadProcessMemory(Peb) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", sizeof(peb), bytes_read);
                return NULL;
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
                bool_result = ReadProcessMemory(
                        process_handle,
                        peb.ProcessParameters,
                        &process_parameters,
                        sizeof(process_parameters),
                        &bytes_read);
        else
                bool_result = ReadProcessMemory(
                        process_handle,
                        (PVOID)(ULONG_PTR)peb32.ProcessParameters,
                        &process_parameters32,
                        sizeof(process_parameters32),
                        &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(ProcessParameters) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%zx, actual size: 0x%zx\n", sizeof(process_parameters), bytes_read);
                return NULL;
        }

        static WCHAR buffer[MAX_PATH];
        if (!is_wow64)
                bool_result = ReadProcessMemory(
                        process_handle,
                        process_parameters.CommandLine.Buffer,
                        buffer,
                        process_parameters.CommandLine.Length,
                        &bytes_read);
        else
                bool_result = ReadProcessMemory(
                        process_handle,
                        (PVOID)(ULONG_PTR)process_parameters32.CommandLine.Buffer,
                        buffer,
                        process_parameters32.CommandLine.Length,
                        &bytes_read);
        if (bool_result == 0)
        {
                printf("ReadProcessMemory(CmdLine) failed with 0x%x\n", GetLastError());
                printf("Expect size: 0x%x, actual size: 0x%zx\n", process_parameters.CommandLine.Length, bytes_read);
                return NULL;
        }

        return buffer;
}
