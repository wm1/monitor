#include "precomp.h"

Timer::Timer(
        int                 seconds,
        type_TimerCallback* _callback,
        PVOID               _context)
{
        callback = _callback;
        context  = _context;

        timer_handle = CreateWaitableTimer(NULL, TRUE, NULL);
        if (timer_handle == NULL)
        {
                printf("CreateWaitableTimer failed with 0x%x", GetLastError());
                return;
        }

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

        printf("timer is setup to fire every %d seconds\n", seconds);
}

void CALLBACK Timer::ApcRoutine(LPVOID _context, DWORD, DWORD)
{
        Timer* _this = (Timer*)_context;
        _this->callback(_this->context);
}
