#include "precomp.h"

LRESULT CALLBACK WndProc(
        _In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
{
        Timer* timer = (Timer*)GetWindowLongPtr(hWnd, 0);
        if (timer != NULL)
                timer->WndProc(uMsg, wParam, lParam);

        switch (uMsg)
        {
        case WM_DESTROY:
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
        HANDLE mutex = CreateMutex(NULL, FALSE, L"Local\\{AB2F0A5E-FAA2-4664-B3C2-25D3984F0A20}");
        if (mutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
                return 1;

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
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 NULL, NULL, hInstance, NULL);
        if (!hwnd)
                return 1;

        // for debugging output
        //
        // FILE* ignored;
        // AllocConsole();
        // freopen_s(&ignored, "CON", "w", stdout);

        FindApp find(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0)
                DispatchMessage(&msg);

        return 0;
}
