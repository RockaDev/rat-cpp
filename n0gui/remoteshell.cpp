/*
 This code in unfinished, I did not have much time to finish Remote CMD
*/

#include "server.h"
#include "shell.h"

#include <windows.h>
#include <Lmcons.h> // For UNLEN constant
#include <format>

using namespace std;

string getCurrentUser()
{
    TCHAR username[UNLEN + 1];
    DWORD size = UNLEN + 1;

    if (GetUserName(username, &size))
    {
        // Convert TCHAR to std::string
        wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        wstring wideUsername(username);
        string currentUser = converter.to_bytes(wideUsername);

        return currentUser;
    }

    return "";
}

// now a LPCWSTR !!! forgot about c_str()
std::wstring filePath = format(L"C:\\Users\\{}\\Desktop\\ttt.ico", getCurrentUser());
LPCWSTR str = filePath.c_str();

wchar_t g_szClientCMDClassName[] = L"Remote Shell";
HICON hcCMDIcon = (HICON)LoadImageW(NULL, str, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
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
