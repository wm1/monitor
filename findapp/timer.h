
class Timer
{
public:
        typedef void(type_TimerCallback)(PVOID);
        Timer(int seconds, type_TimerCallback*, PVOID);

private:
        int                 seconds;
        type_TimerCallback* callback;
        PVOID               context;
        HANDLE              timer_handle;

        static void CALLBACK ApcRoutine(PVOID lpArg, DWORD, DWORD);
        static void __cdecl TimerThread(PVOID);
        void TimerThread();
        void StartTimer();
};

#define SECONDS_TO_MILLISECOND(x) ((x)*1000)                   // 1 ms = 10e-3 s
#define SECONDS_TO_100_NANOSECOND(x) ((x)*10i64 * 1000 * 1000) // 1 ns = 10e-9 s
#define RELATIVE_DURATION(x) (-x)
