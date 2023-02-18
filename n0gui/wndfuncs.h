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

    // Add items to the list control
    for (int i = 0; i < files.size(); i++)
    {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = (LPWSTR)files[i].c_str();
        ListView_InsertItem(hFtpList, &lvItem);

        // Set size to a dummy value for now
        lvItem.iSubItem = 1;
        lvItem.pszText = (LPWSTR)L"10 KB";
        ListView_SetItemText(hFtpList, i, 1, lvItem.pszText);

        if (!files[i].empty() && files[i].back() == L'\\')
        {
            // Set folder text and color
            ListView_SetItemText(hFtpList, i, 0, (LPWSTR)files[i].c_str());
            ListView_SetTextColor(hFtpList, i, RGB(255, 0, 0));

            // Set position for folder item
            ListView_SetItemPosition(hFtpList, i, 0, yPosFolders);
            yPosFolders += 20; // increase y-position to separate folder items
        }
        else
        {
            // Set file size to a dummy value for now
            lvItem.iSubItem = 1;
            lvItem.pszText = (LPWSTR)L"10 KB";
            ListView_SetItemText(hFtpList, i, 1, lvItem.pszText);

            // Set position for file item
            ListView_SetItemPosition(hFtpList, i, 0, yPosFiles);
            yPosFiles += 20; // increase y-position to separate file items
        }
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
        InsertMenu(hMenu, 1, MF_BYPOSITION, ITEM_PATH, L"Insert Own Path");
        InsertMenu(hMenu, 2, MF_BYPOSITION, ITEM_DEFAULT_PATH, L"Add To");
        SetMenuItemBitmaps(hMenu, ITEM_RUN, MF_BYCOMMAND, hCmdItemIcon, hCmdItemIcon);
        SetMenuItemBitmaps(hMenu, ITEM_PATH, MF_BYCOMMAND, hFunItemIcon, hFunItemIcon);
        SetMenuItemBitmaps(hMenu, ITEM_DEFAULT_PATH, MF_BYCOMMAND, hRmClientItemIcon, hRmClientItemIcon);

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
    HWND hwndBtnFile = CreateWindowW(
        L"BUTTON",
        L"Add File",
        WS_CHILD | WS_VISIBLE,
        150,
        400,
        150,
        50,
        hwnd,
        (HMENU)FTP_ADD_FILES,
        GetModuleHandle(NULL),
        NULL
    );

    HWND RemoveAll = CreateWindowW(
        L"BUTTON",
        L"Remove All",
        WS_CHILD | WS_VISIBLE,
        310,
        400,
        150,
        50,
        hwnd,
        (HMENU)FTP_REMALL_FILES,
        GetModuleHandle(NULL),
        NULL
    );
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

    HWND hwndButton = CreateWindowW(
        L"BUTTON",
        L"Send Command To All",
        WS_CHILD | WS_VISIBLE,
        750,
        350,
        150,
        50,
        hwnd,
        (HMENU)15,
        GetModuleHandle(NULL),
        NULL
    );

}
