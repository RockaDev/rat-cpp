#pragma once

class ItemMenu
{
public:
	void CallItemMenu(HWND hwnd, HWND mainHwnd);
    void GlobalButtons(HWND hwnd);
};

class FtpMenu
{
public:
    void UpdateItems(HWND hwnd,HWND hFtpList, char buffer[], int lenRecv);
};

void FtpMenu::UpdateItems(HWND hwnd, HWND hFtpList, char buffer[], int lenRecv)
{
    std::string fileList(buffer, lenRecv);
    int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer, lenRecv, NULL, 0);
    std::vector<wchar_t> wfileList(wlen + 1);
    MultiByteToWideChar(CP_UTF8, 0, buffer, lenRecv, wfileList.data(), wlen);

    std::vector<std::wstring> files;
    std::wstringstream ss(wfileList.data());
    std::wstring item;
    while (std::getline(ss, item, L'\n'))
    {
        files.push_back(item);
    }

    ListView_DeleteAllItems(hFtpList);
    int yPosFiles = 0; // initialize y-position to 0 for files
    int yPosFolders = 0; // initialize y-position to 0 for folders

    ListView_SetTextBkColor(hFtpList, RGB(0, 0, 51));

    for (int i = 0; i < files.size(); i++)
    {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i;

        // Add the file name as the first subitem
        lvItem.iSubItem = 0;
        lvItem.pszText = (LPWSTR)files[i].c_str();
        ListView_InsertItem(hFtpList, &lvItem);

        // Define the emoji characters to use
        WCHAR fileemoji[] = L"\U0000274C"; // Here, I'm using the folder emoji
        WCHAR folderemoji[] = L"\U0001F4C2";

        // Determine which emoji to use based on the file type
        WCHAR* pszEmoji = (files[i].length() >= 2 && files[i][files[i].length() - 1] == L'\\') ? folderemoji : fileemoji;

        // Add the emoji as the second subitem
        lvItem.iSubItem = 1;
        lvItem.pszText = pszEmoji;
        ListView_SetItem(hFtpList, &lvItem);

        // Set the text color based on the file type
        COLORREF textColor = RGB(0, 255,255);
        ListView_SetTextColor(hFtpList, textColor);
    }
}

void ItemMenu::CallItemMenu(HWND hwnd, HWND mainHwnd)
{
    int selectedMenuItem = 0;
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(mainHwnd, &cursorPos);

    LVHITTESTINFO hitTestInfo;
    cursorPos.x -= 10;
    cursorPos.y -= 15;
    hitTestInfo.pt = cursorPos;

    int itemIndex = ListView_HitTest(hwnd, &hitTestInfo);

    if (itemIndex != -1)
    {
        HMENU hMenu = CreatePopupMenu();

        InsertMenu(hMenu, 0, MF_BYPOSITION, ITEM_RUN, L"Run File");
        InsertMenu(hMenu, 1, MF_BYPOSITION, ITEM_DOWNLOAD, L"Download File");
        InsertMenu(hMenu, 2, MF_BYPOSITION, ITEM_DELETE_FILE, L"Delete File");
        SetMenuItemBitmaps(hMenu, ITEM_RUN, MF_BYCOMMAND, hCmdItemIcon, hCmdItemIcon);
        SetMenuItemBitmaps(hMenu, ITEM_DOWNLOAD, MF_BYCOMMAND, hFunItemIcon, hFunItemIcon);
        SetMenuItemBitmaps(hMenu, ITEM_DELETE_FILE, MF_BYCOMMAND, hRmClientItemIcon, hRmClientItemIcon);

        MENUINFO mi = { 0 };
        mi.cbSize = sizeof(MENUINFO);
        mi.fMask = MIM_BACKGROUND;
        mi.hbrBack = CreateSolidBrush(RGB(185,184,184));

        SetMenuInfo(hMenu, &mi);
        GetCursorPos(&cursorPos);
        TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, mainHwnd, NULL);

        selectedMenuItem = 0;
        int menuItemCount = GetMenuItemCount(hMenu);
        for (int i = 0; i < menuItemCount; i++) {
            if (GetMenuState(hMenu, i, MF_BYPOSITION) & MF_CHECKED) {
                selectedMenuItem = GetMenuItemID(hMenu, i);
                itemIndex = i;
                break;
            }
        }
    }

    else
    {
        return;
    }

    return;
}


ATOM CreateColumn(HWND hwndLV, int iCol, int width, wchar_t table_txt[])
{
    LVCOLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = width;
    lvc.pszText = table_txt;
    lvc.iSubItem = iCol;

    return ListView_InsertColumn(hwndLV, iCol, &lvc);
};

ATOM CreateItem(HWND hwndList, wchar_t column_txt[])
{
    LVITEM lvi = { 0 };

    lvi.mask = LVIF_TEXT;
    lvi.pszText = column_txt;
    lvi.state = LVIS_SELECTED;
    lvi.stateMask = LVIS_SELECTED;

    SendMessageW(hwndList, LVM_SETITEMSTATE, 0, (LPARAM)&lvi);
    SendMessageW(hwndList, LVM_SETTEXTCOLOR, (WPARAM)0, (LPARAM)RGB(122, 5, 5));

    return ListView_InsertItem(hwndList, &lvi);
}


void ItemMenu::GlobalButtons(HWND hwnd)
{

    HBITMAP hBitmapRefresh = (HBITMAP)LoadImageW(NULL, L"refresh.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // vytvoøení tlaèítka
    HWND hwndRefresh = CreateWindowExW(0, L"BUTTON", L"Add File", WS_CHILD | WS_VISIBLE | BS_BITMAP,
        50, 400, 70, 50, hwnd, (HMENU)FTP_REFRESH, GetModuleHandle(NULL), NULL);

    // nastavení obrázku tlaèítka
    SendMessageW(hwndRefresh, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmapRefresh);


    HBITMAP hBitmapAddFile = (HBITMAP)LoadImageW(NULL, L"addfile.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // vytvoøení tlaèítka
    HWND hwndBtnFile = CreateWindowExW(0, L"BUTTON", L"Add File", WS_CHILD | WS_VISIBLE | BS_BITMAP,
        150, 400, 150, 50, hwnd, (HMENU)FTP_ADD_FILES, GetModuleHandle(NULL), NULL);

    // nastavení obrázku tlaèítka
    SendMessageW(hwndBtnFile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmapAddFile);


    HBITMAP hBitmapremall = (HBITMAP)LoadImageW(NULL, L"removeall.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // vytvoøení tlaèítka
    HWND RemoveAll = CreateWindowExW(0, L"BUTTON", L"Remove All", WS_CHILD | WS_VISIBLE | BS_BITMAP,
        310, 400, 150, 50, hwnd, (HMENU)FTP_REMALL_FILES, GetModuleHandle(NULL), NULL);

    // nastavení obrázku tlaèítka
    SendMessageW(RemoveAll, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmapremall);
}

void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HWND input = CreateWindowW(
        L"EDIT",
        L"cmd: dir",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        535,
        350,
        200,
        50,
        hwnd,
        (HMENU)14,
        GetModuleHandle(NULL),
        NULL
    );

    HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, L"cmdbtn.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // vytvoøení tlaèítka
    HWND hwndButton = CreateWindowExW(0, L"BUTTON", L"Send Command To All", WS_CHILD | WS_VISIBLE | BS_BITMAP,
        750, 350, 150, 50, hwnd, (HMENU)15, hInstance, NULL);

    // nastavení obrázku tlaèítka
    SendMessageW(hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

    // Výpoèet støedu okna
    RECT rc;
    GetClientRect(hwnd, &rc);
    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;
    int x = (windowWidth - 150) / 2;
    int y = (windowHeight - 350) / 2;

}
