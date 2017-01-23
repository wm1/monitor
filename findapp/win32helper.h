
#pragma once

typedef NTSTATUS
(NTAPI *type_NtQueryInformationProcess)(
        _In_  HANDLE ProcessHandle,
        _In_  PROCESSINFOCLASS ProcessInformationClass,
        _Out_ PVOID ProcessInformation,
        _In_  ULONG ProcessInformationLength,
        _Out_opt_ PULONG ReturnLength
        );

class Win32Helper
{
public:
        Win32Helper();
        PCWSTR GetProcessCommandLine(HANDLE process_handle);

private:
        bool valid;
        bool InitializeDelayLoading();
        HMODULE ntdll;
        type_NtQueryInformationProcess _NtQueryInformationProcess;

        PCWSTR GetProcessCommandLineWow64(HANDLE process_handle);
};
