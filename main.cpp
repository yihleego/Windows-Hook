#include <windows.h>

HWND hgWnd;
HHOOK MOUSE_HOOK;
HHOOK KEYBOARD_HOOK;

/****************************************************************
  WH_KEYBOARD hook procedure
 ****************************************************************/
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT) lParam;
    POINT pt = p->pt;
    DWORD mouseData = p->mouseData;
    const char *info = NULL;
    char text[60], pData[50], mData[50];
    HDC hdc;

    if (nCode >= 0) {
        if (wParam == WM_MOUSEMOVE) info = "mouse moving　　　　";
        else if (wParam == WM_LBUTTONDOWN) info = "mouse left button down";
        else if (wParam == WM_LBUTTONUP) info = "mouse left button up";
        else if (wParam == WM_LBUTTONDBLCLK) info = "mouse left button double click";
        else if (wParam == WM_RBUTTONDOWN) info = "mouse right button down";
        else if (wParam == WM_RBUTTONUP) info = "mouse right button up";
        else if (wParam == WM_RBUTTONDBLCLK) info = "mouse right button double click";
        else if (wParam == WM_MBUTTONDOWN) info = "mouse wheel down";
        else if (wParam == WM_MBUTTONUP) info = "mouse wheel up";
        else if (wParam == WM_MBUTTONDBLCLK) info = "mouse wheel double click";
        else if (wParam == WM_MOUSEWHEEL) info = "mouse wheel scrolling";

        ZeroMemory(text, sizeof(text));
        ZeroMemory(pData, sizeof(pData));
        ZeroMemory(mData, sizeof(mData));

        wsprintf(text, "Status: %10s   ", info);
        wsprintf(pData, "0x%x - X: [%04d], Y: [%04d]  ", wParam, pt.x, pt.y);
        wsprintf(mData, "extra: %16u   ", mouseData);

        hdc = GetDC(hgWnd);
        TextOut(hdc, 10, 10, text, strlen(text));
        TextOut(hdc, 10, 30, pData, strlen(pData));
        TextOut(hdc, 10, 50, mData, strlen(mData));
        ReleaseDC(hgWnd, hdc);
    }

    return CallNextHookEx(MOUSE_HOOK, nCode, wParam, lParam);
}

/****************************************************************
  WH_KEYBOARD hook procedure
 ****************************************************************/
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
    const char *info = NULL;
    char text[50], data[20];
    HDC hdc;

    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) info = "key down";
        else if (wParam == WM_KEYUP) info = "key up";
        else if (wParam == WM_SYSKEYDOWN) info = "sys key down";
        else if (wParam == WM_SYSKEYUP) info = "sys key up";

        ZeroMemory(text, sizeof(text));
        ZeroMemory(data, sizeof(data));
        wsprintf(text, "%s - kCode [%04d], scanCode [%04d]  ", info, p->vkCode, p->scanCode);
        wsprintf(data, "key: %c  ", p->vkCode);

        hdc = GetDC(hgWnd);
        TextOut(hdc, 10, 70, text, strlen(text));
        TextOut(hdc, 10, 100, data, strlen(data));
        ReleaseDC(hgWnd, hdc);
    }

    return CallNextHookEx(KEYBOARD_HOOK, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    hgWnd = hwnd;

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    char text[30];

    const char szClassName[] = "myWindowClass";

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Failed"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            szClassName,
            TEXT("Title"),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 350, 200,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, TEXT("Failed"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MOUSE_HOOK = SetWindowsHookEx(
            WH_MOUSE_LL,
            MouseProc,
            hInstance,
            0
    );

    KEYBOARD_HOOK = SetWindowsHookEx(
            WH_KEYBOARD_LL,
            KeyboardProc,
            hInstance,
            0
    );

    if (MOUSE_HOOK == NULL) {
        wsprintf(text, "Error : %d n", GetLastError());
        MessageBox(hwnd, text, TEXT("error"), MB_OK);
    }

    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;

}