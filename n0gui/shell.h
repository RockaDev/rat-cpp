#pragma once
#include <Windows.h>

extern wchar_t g_szClientCMDClassName[];

extern HINSTANCE hInstance;
extern HICON hcCMDIcon;
extern HBITMAP hCMDBG;

class RemoteShell
{
public:
    ATOM WndClientCMDRegisterClass(HINSTANCE hCMDInstance);
    static LRESULT CALLBACK ClientCMDWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};
