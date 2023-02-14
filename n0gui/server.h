#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <thread>
#include <locale>
#include <codecvt>
#include <vector>
#include <map>
#include <cstdlib>
#include <Shlwapi.h>
#include <windows.h>
#include <gdiplus.h>
#include <CommCtrl.h>

#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"shlwapi.lib")

typedef int socklen_t;

#define LVM_SETITEMHEIGHT LVM_FIRST + 27

#define IMAGE_BUFFER_SIZE 6220800

#define WM_SOCKET WM_USER + 1
#define CLIENT_SENDCOMMAND 1035
#define CLIENT_REMOVE 1036
#define CLIENT_FUN 1037
#define CLIENT_WATCHSCREEN 1038
#define CLIENT_FTP 1039

#define SCREEN_TIMER 2051
#define FPS 1000/6
