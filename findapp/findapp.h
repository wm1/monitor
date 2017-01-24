class FindApp
{
public:
        FindApp(HWND);

private:
        Win32Helper helper;
        DWORD       GetForegroundWindowProcessId();
        PCWSTR GetProcessCommandLine(DWORD process_id);

        Timer       timer;
        void        TimerCallback();
        static void TimerCallback(PVOID);
};
