#pragma once

COLORREF clrRed = RGB(0,0,51);
COLORREF clrLightRed = RGB(255, 204, 203);
HBITMAP hCmdItemIcon = (HBITMAP)LoadImageW(NULL, L"cmd.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hScreenItemIcon = (HBITMAP)LoadImageW(NULL, L"desktop.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hFunItemIcon = (HBITMAP)LoadImageW(NULL, L"fun.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hRmClientItemIcon = (HBITMAP)LoadImageW(NULL, L"remove.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hFtpClientItemIcon = (HBITMAP)LoadImageW(NULL, L"ftp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
const int MapWidth = 800;
const int MapHeight = 600;

HDC hdcMap;
int dotX; // the X coordinate of the dot
int dotY; // the Y coordinate of the dot

HBITMAP LoadLocationMap(LPCTSTR szFileName) {
    HBITMAP hBitmap = NULL;
    HDC hDC = CreateCompatibleDC(NULL);
    if (hDC) {
        hBitmap = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        DeleteDC(hDC);
    }
    return hBitmap;
}

void DisplayDot(HDC hdc, int x, int y, int size) {
    Ellipse(hdc, x - size, y - size, x + size, y + size);
}

HBITMAP hMap = LoadLocationMap(L"hmap.bmp");