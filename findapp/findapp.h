class FindApp
{
public:
        FindApp(HWND);

private:
        Win32Helper helper;
        Logger      logger;
        Timer       timer;

        void          TimerCallback();
        static void   TimerCallback(PVOID);
        bool          GetForegroundApp();
        static HANDLE OpenProcess(DWORD process_id);

        WCHAR window_title[MAX_PATH];
        WCHAR command_line[MAX_PATH];
};
