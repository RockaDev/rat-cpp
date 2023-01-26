#pragma once
#define ITEM_RUN 2050
#define ITEM_PATH 2051
#define ITEM_DEFAULT_PATH 2052

class ItemMenu
{
public:
	void CallItemMenu(HWND hwnd, HWND mainHwnd);
    void GlobalButtons(HWND hwnd);
};

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
        (HMENU)211,
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
        (HMENU)212,
        GetModuleHandle(NULL),
        NULL
    );
}