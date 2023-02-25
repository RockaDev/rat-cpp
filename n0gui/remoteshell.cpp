#include "server.h"
#include "shell.h"

wchar_t g_szClientCMDClassName[] = L"Remote Shell";
HICON hcCMDIcon = (HICON)LoadImageW(NULL, L"C:\\Users\\mineo\\Desktop\\ttt.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
HBITMAP hCMDBG = (HBITMAP)LoadImageW(NULL, L"ftpbg.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

ATOM RemoteShell::WndClientCMDRegisterClass(HINSTANCE hCMDInstance)
{
    WNDCLASSEX wcFtp;
    wcFtp.cbSize = sizeof(WNDCLASSEX);
    wcFtp.style = 0;
    wcFtp.lpfnWndProc = ClientCMDWndProc;
    wcFtp.cbClsExtra = 0;
    wcFtp.cbWndExtra = 0;
    wcFtp.hInstance = hInstance;
    wcFtp.hIcon = hcCMDIcon;
    wcFtp.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcFtp.hbrBackground = CreateSolidBrush(RGB(0,0,0));
    wcFtp.lpszMenuName = NULL;
    wcFtp.lpszClassName = g_szClientCMDClassName;
    wcFtp.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wcFtp);
}

LRESULT CALLBACK RemoteShell::ClientCMDWndProc(HWND CmdHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {

    case WM_CREATE:
    {

        break;
    }
    case WM_TIMER:
    {

        break;
    }


    case WM_CLOSE:
    {
        DestroyWindow(CmdHwnd);
        break;
    }

    case WM_DESTROY:
        DestroyWindow(CmdHwnd);
        return 0;

    default:
        return DefWindowProc(CmdHwnd, uMsg, wParam, lParam);
    }
    return 0;
}