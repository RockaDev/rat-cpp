#pragma once

#define CURL_STATICLIB
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <thread>
#include <sstream>
#include <locale>
#include <codecvt>
#include <vector>
#include <algorithm>
#include <map>
#include <cstdlib>
#include <Shlwapi.h>
#include <windows.h>
#include <gdiplus.h>
#include <CommCtrl.h>
#include "curl/curl.h"

#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"Normaliz.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"Wldap32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"shlwapi.lib")

#pragma warning(disable:28251)
#pragma warning(disable:26812)
#pragma warning(disable:6054)
#pragma warning(disable:26454)
#pragma warning(disable:28183)
#pragma warning(disable:26451)
#pragma warning(disable:6387)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4101)
#pragma warning(disable:4267)
#pragma warning(disable:4098)
#pragma warning(disable:6031)

typedef int socklen_t;


#define WM_SOCKET WM_USER + 1
#define GWL_HICON (-14)
#define WM_FOLDERCONTENT (WM_USER + 1)

#define LVM_SETITEMHEIGHT LVM_FIRST + 27

#define IMAGE_BUFFER_SIZE 6220800

#define WM_SOCKET WM_USER + 1
#define CLIENT_SENDCOMMAND 1035
#define CLIENT_REMOVE 1036
#define CLIENT_FUN 1037
#define CLIENT_WATCHSCREEN 1038
#define CLIENT_FTP 1039
#define CLIENT_REMOTE_SHELL 1840

#define ITEM_RUN 2050
#define ITEM_DOWNLOAD 2051
#define ITEM_DELETE_FILE 2052
#define FTP_ADD_FILES 211
#define FTP_REMALL_FILES 212
#define FTP_REFRESH 214

#define DEF_PATH_STARTUP 310

#define SCREEN_TIMER 2051
#define FPS 1000/6

#define MAX_DOTS 100
