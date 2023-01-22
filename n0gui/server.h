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
#include <windows.h>
#include <gdiplus.h>

#pragma comment(lib,"gdiplus.lib")

#define IMAGE_BUFFER_SIZE 6220800

#define CLIENT_SENDCOMMAND 1035
#define CLIENT_REMOVE 1036
#define CLIENT_FUN 1037
#define CLIENT_WATCHSCREEN 1038

#define SCREEN_TIMER 2051
#define FPS 1000/25