#include "precomp.h"

Timer::Timer(
        HWND                _hwnd,
        int                 _seconds,
        type_TimerCallback* _callback,
        PVOID               _context)
{
        hwnd     = _hwnd;
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

        // create an auto-reset event with initial state = signaled
        start_event = CreateEvent(NULL, FALSE, TRUE, NULL);
        if (start_event == NULL)
        {
                printf("CreateEvent failed with 0x%x\n", GetLastError());
                return;
        }

        _beginthread(TimerThread, 0, this);

        SetLastError(0);
        LONG_PTR result = SetWindowLongPtr(hwnd, 0, (LONG_PTR)this);
        if (result == 0)
        {
                DWORD error = GetLastError();
                if (error != 0)
                {
                        printf("SetWindowLongPtr failed with 0x%x\n", error);
                        return;
                }
        }

        GUID power_settings[] = {
                GUID_SESSION_USER_PRESENCE,
                GUID_SESSION_DISPLAY_STATUS,
        };
        for (int i = 0; i < sizeof(power_settings) / sizeof(power_settings[0]); i++)
                if (!RegisterPowerSettingNotification(hwnd,
                                                      &power_settings[i],
                                                      DEVICE_NOTIFY_WINDOW_HANDLE))
                {
                        printf("RegisterPowerSettingNotification failed with 0x%x\n", GetLastError());
                        return;
                }

        BOOL bool_result = WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION);
        if (!bool_result)
        {
                printf("RegisterPowerSettingNotification failed with 0x%x\n", GetLastError());
                return;
        }
}

void Timer::PowerEvent(POWERBROADCAST_SETTING* setting)
{
        if (setting->PowerSetting == GUID_SESSION_USER_PRESENCE)
        {
                DWORD data = *(DWORD*)setting->Data;

                if (running && data == PowerUserInactive)
                        Stop();
                else if (!running && data == PowerUserPresent)
                        Start();
        }

        if (setting->PowerSetting == GUID_SESSION_DISPLAY_STATUS)
        {
                // 0x0 - The display is off.
                // 0x1 - The display is on.
                DWORD data = *(DWORD*)setting->Data;

                if (running && data == 0)
                        Stop();
                else if (!running && data == 1)
                        Start();
        }
}

void Timer::LogonEvent(WPARAM param)
{
        // WM_WTSSESSION_CHANGE message
        if (running && param == WTS_SESSION_LOCK)
                Stop();
        else if (!running && param == WTS_SESSION_UNLOCK)
                Start();
}

void Timer::Start()
{
        SetEvent(start_event);
}

void Timer::StartForReal()
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
        running = true;

        OutputTimeStamp();
        printf("Timer started\n");
}

void Timer::Stop()
{
        if (CancelWaitableTimer(timer_handle) == 0)
        {
                printf("CancelWaitableTimer failed with 0x%x", GetLastError());
                return;
        }
        running = false;

        OutputTimeStamp();
        printf("Timer stopped\n");
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
        while (1)
        {
                // put the thread into alertable state
                //
                DWORD result = WaitForSingleObjectEx(start_event, INFINITE, TRUE);

                if (result == WAIT_OBJECT_0)
                        StartForReal();
        }
}

void Timer::OutputTimeStamp()
{
        SYSTEMTIME now;
        GetLocalTime(&now);
        printf("\n%02d:%02d:%02d\n", now.wHour, now.wMinute, now.wSecond);
}
