#include "precomp.h"

Timer::Timer(
        int                 _seconds,
        type_TimerCallback* _callback,
        PVOID               _context)
{
        seconds  = _seconds;
        callback = _callback;
        context  = _context;

        timer_handle = CreateWaitableTimer(NULL, TRUE, NULL);
        if (timer_handle == NULL)
        {
                printf("CreateWaitableTimer failed with 0x%x", GetLastError());
                return;
        }

        // https://msdn.microsoft.com/en-us/library/ms686898.aspx
        //
        // The completion routine will be executed by the same thread that called SetWaitableTimer.
        // This thread must be in an alertable state to execute the completion routine. It accomplishes
        // this by calling the SleepEx function, which is an alertable function.

        printf("timer is setup to fire every %d seconds\n", seconds);

        _beginthread(TimerThread, 0, this);
}

void Timer::StartTimer()
{
        LARGE_INTEGER due_time;
        due_time.QuadPart = -1; // trigger immediately

        if (SetWaitableTimer(timer_handle,
                             &due_time,
                             SECONDS_TO_MILLISECOND(seconds),
                             ApcRoutine,
                             this,
                             FALSE) == 0)
        {
                printf("SetWaitableTimer failed with 0x%x", GetLastError());
                return;
        }
}

void CALLBACK Timer::ApcRoutine(PVOID _context, DWORD, DWORD)
{
        Timer* _this = (Timer*)_context;
        _this->callback(_this->context);
}

void __cdecl Timer::TimerThread(PVOID _context)
{
        Timer* _this = (Timer*)_context;
        _this->TimerThread();
}

void Timer::TimerThread()
{
        StartTimer();
        while (1)
        {
                SleepEx(INFINITE, TRUE); // put the thread into alertable state
        }
}
