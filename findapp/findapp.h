class FindApp
{
public:
        FindApp(HWND);

private:
        Win32Helper helper;
        DWORD       GetForegroundWindowProcessId();
        bool        GetProcessCommandLine(DWORD process_id);

        Timer       timer;
        void        TimerCallback();
        static void TimerCallback(PVOID);

        WCHAR window_title[MAX_PATH];
        WCHAR command_line[MAX_PATH];
};
