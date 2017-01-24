
#pragma once

typedef NTSTATUS(NTAPI* type_NtQueryInformationProcess)(
        _In_ HANDLE ProcessHandle,
        _In_ PROCESSINFOCLASS ProcessInformationClass,
        _Out_ PVOID ProcessInformation,
        _In_ ULONG ProcessInformationLength,
        _Out_opt_ PULONG ReturnLength);

class Win32Helper
{
public:
        Win32Helper();
        bool GetProcessCommandLine(HANDLE process_handle, _Out_ PWSTR, size_t);

private:
        bool                           valid;
        bool                           InitializeDelayLoading();
        HMODULE                        ntdll;
        type_NtQueryInformationProcess _NtQueryInformationProcess;
};
