#pragma once

COLORREF clrRed = RGB(114, 4, 4);
COLORREF clrLightRed = RGB(255, 204, 203);
HBITMAP hCmdItemIcon = (HBITMAP)LoadImageW(NULL, L"cmd.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hScreenItemIcon = (HBITMAP)LoadImageW(NULL, L"desktop.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hFunItemIcon = (HBITMAP)LoadImageW(NULL, L"fun.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hRmClientItemIcon = (HBITMAP)LoadImageW(NULL, L"remove.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
HBITMAP hFtpClientItemIcon = (HBITMAP)LoadImageW(NULL, L"ftp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);