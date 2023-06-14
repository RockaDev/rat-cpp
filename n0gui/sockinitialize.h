#pragma once
#include <set>

const char* ServerIP = "0.0.0.0";
int PORT = 9865;

std::vector<SOCKET> clientSockets;
std::set<SOCKET> addedClients;

SOCKET s;
WSADATA w;
SOCKADDR_IN addr;
int newSd;
sockaddr_in newSockAddr;
