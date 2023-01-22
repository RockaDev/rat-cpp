#include "server.h"



void ServerInitialize::ServerSetup(void)
{
	WSADATA wsaData;
	MessageBoxA(NULL, "L", "OK", 0);
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ExitProcess(EXIT_FAILURE);
	}
	SOCKADDR_IN sockAddr;
	sockAddr.sin_port = htons(PORT);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind(ServerSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		ExitProcess(EXIT_FAILURE);
	}
	listen(ServerSocket, 10);

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int clientsize = sizeof(clientAddr);
		SOCKET ClientSocket = accept(ServerSocket, (sockaddr*)&clientAddr, &clientsize);
		int bufferLength = 4096;
		char buffer[4096];

		int iResult = recv(ClientSocket, buffer, bufferLength, 0);

		std::string request = std::string(buffer, buffer + iResult);

		if (request == "request")
		{
			send(ClientSocket, currCommand.c_str(), currCommand.length(), 0);
		}


		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "Error in receiving data. Quitting" << std::endl;
			break;
		}


		closesocket(ClientSocket);
		Sleep(1);
	}

	closesocket(ServerSocket);
	WSACleanup();

	return;
}