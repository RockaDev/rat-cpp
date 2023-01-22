#include "client.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:28251)

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"ws2_32.lib")

#define WM_SOCKET 1048


wchar_t g_szClassName[] = L"Client";
const wchar_t* wn_title = L"Rocka's - RAT [Client]";

SOCKET s;
WSADATA w;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);

void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
}

bool ConnectToHost(int PortNo)
{
    WSADATA wsadata;

    int error = WSAStartup(0x0202, &wsadata);


    if (error)
        return false;

    if (wsadata.wVersion != 0x0202)
    {
        WSACleanup();
        return false;
    }

    SOCKADDR_IN target;

    target.sin_family = AF_INET;
    target.sin_port = htons(PortNo);
    target.sin_addr.s_addr = inet_addr("127.0.0.1");

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        return false;
    }

    //Try connecting...

    if (connect(s, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
    {
        return false; //Connection error
    }
    else
        return true; //Success
}

void CloseConnection()
{
    if (s)
        closesocket(s);

    WSACleanup();
}

HINSTANCE hInstance;
ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SOCKET:
    {
        switch (lParam)
        {
        case FD_ACCEPT:
            OutputDebugStringW(L"\ACCEPT!\n");
            break;

        case FD_CONNECT:

            OutputDebugStringW(L"\nCONNECTED CLIENT!\n");
            break;

        case FD_READ:
            char buffer[80];
            memset(buffer, 0, sizeof(buffer));

            recv(s, buffer, sizeof(buffer) - 1, 0);
            break;

        case FD_CLOSE:
            CloseConnection();
            break;
        }
    }
    case WM_CREATE:
        OutputDebugStringW(L"create");
        break;

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
};


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wc);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class

    MyRegisterClass(hInstance);

    // Step 2: Creating the Window
    int wWidth = 1000;
    int wHeight = 675;
    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        wn_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, wWidth, wHeight,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    if (ConnectToHost(9544))
    {
        send(s, "Hello", strlen("Hello"), 0);
        OutputDebugStringW(L"\nCONNECTION SUCCESSFUL!!!\n");
        SendMessageW(hwnd, WM_SOCKET, NULL, FD_CONNECT);
    }
    else
    {
        OutputDebugStringW(L"\nCONNECTION UNSUCCESSFUL! :(\n");
    }

    while (GetMessageW(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
    return Msg.wParam;
}