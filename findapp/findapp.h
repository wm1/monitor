class FindApp
{
public:
        FindApp(HWND);
        const int CHECK_INTERNVAL_IN_SECONDS = 5;

private:
        Win32Helper helper;
        Logger      logger;
        Timer       timer;

        DWORD GetForegroundWindowProcessId();
        bool GetProcessCommandLine(DWORD process_id);
        void        TimerCallback();
        static void TimerCallback(PVOID);

        WCHAR window_title[MAX_PATH];
        WCHAR command_line[MAX_PATH];
};
