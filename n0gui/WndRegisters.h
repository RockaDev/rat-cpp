#pragma once

wchar_t g_szClassName[] = L"Server";
const wchar_t* wn_title = L"R4T $HIT [Server]";
wchar_t g_szClientClassName[] = L"Client Screen";
wchar_t g_szClientFTPClassName[] = L"File Transfer";

HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, L"kkk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HICON hcIcon = (HICON)LoadImageW(NULL, L"C:\\Users\\mineo\\Desktop\\ttt.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

ATOM WndRegisterClass(HINSTANCE hInstance);
ATOM WndClientRegisterClass(HINSTANCE hClientInstance);
ATOM WndClientFTPRegisterClass(HINSTANCE hClientInstance);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ClientWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ClientFTPWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HINSTANCE hInstance;
HINSTANCE hClientInstance = GetModuleHandle(NULL);

ATOM WndRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = hcIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreatePatternBrush(hBitmap);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wc);
}

ATOM WndClientRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcClient;
    wcClient.cbSize = sizeof(WNDCLASSEX);
    wcClient.style = 0;
    wcClient.lpfnWndProc = ClientWndProc;
    wcClient.cbClsExtra = 0;
    wcClient.cbWndExtra = 0;
    wcClient.hInstance = hInstance;
    wcClient.hIcon = hcIcon;
    wcClient.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcClient.hbrBackground = NULL;
    wcClient.lpszMenuName = NULL;
    wcClient.lpszClassName = g_szClientClassName;
    wcClient.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wcClient);
}

ATOM WndClientFTPRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcFtp;
    wcFtp.cbSize = sizeof(WNDCLASSEX);
    wcFtp.style = 0;
    wcFtp.lpfnWndProc = ClientFTPWndProc;
    wcFtp.cbClsExtra = 0;
    wcFtp.cbWndExtra = 0;
    wcFtp.hInstance = hInstance;
    wcFtp.hIcon = hcIcon;
    wcFtp.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcFtp.hbrBackground = CreateSolidBrush(RGB(0,0,0));
    wcFtp.lpszMenuName = NULL;
    wcFtp.lpszClassName = g_szClientFTPClassName;
    wcFtp.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wcFtp);
}