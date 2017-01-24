#include "precomp.h"

LRESULT CALLBACK WndProc(
        _In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
{
        switch (uMsg)
        {
        case WM_POWERBROADCAST:
                if (wParam == PBT_POWERSETTINGCHANGE)
                {
                        Timer* timer = (Timer*)GetWindowLongPtr(hWnd, 0);
                        if (timer != NULL && lParam != NULL)
                                timer->PowerEvent((POWERBROADCAST_SETTING*)lParam);
                }
                break;

        case WM_DESTROY: //WM_CLOSE:
                PostQuitMessage(0);
                break;

        default:
                break;
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(
        _In_ HINSTANCE hInstance,
        _In_           HINSTANCE /*hPrevInstance*/,
        _In_           LPSTR /*lpCmdLine*/,
        _In_ int /*nCmdShow*/)
{
        WNDCLASS wc      = {0};
        wc.lpfnWndProc   = WndProc;
        wc.hInstance     = hInstance;
        wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
        wc.lpszClassName = L"{8677407E-01E9-4D3E-8BF5-F9082CE08AEB}";
        wc.cbWndExtra    = sizeof(Timer*);

        if (!RegisterClass(&wc))
                return 1;

        HWND hwnd = CreateWindow(wc.lpszClassName,
                        L"Monitor",
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);
        if (!hwnd)
                return 1;

        // for debugging output
        //
        FILE* ignored;
        AllocConsole();
        freopen_s(&ignored, "CON", "w", stdout);

        FindApp find(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0)
                DispatchMessage(&msg);

        return 0;
}
