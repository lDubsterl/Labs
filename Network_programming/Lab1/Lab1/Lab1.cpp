// Lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <ctime>
#include <chrono>
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET serverSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
	struct addrinfo properties, * sresult, *cresult;
	ZeroMemory(&properties, sizeof(properties));
	properties.ai_family = AF_INET;
	properties.ai_socktype = SOCK_STREAM;
	properties.ai_protocol = IPPROTO_TCP;
	int choice;
	std::cout << "1. Client\n2. Server\n";
	std::cin >> choice;
	char recvBuffer[64];
	std::string sendBuffer, sRecvBuffer;
	ZeroMemory(recvBuffer, 64);
	if (choice == 1)
	{
		int iResult = getaddrinfo(NULL, "8081", &properties, &cresult);
		if (iResult != 0)
		{
			std::cout << "getaddrinfo failed" << WSAGetLastError() << "\n";
			system("pause");
			WSACleanup();
			return 1;
		}
		clientSocket = socket(cresult->ai_family, cresult->ai_socktype, cresult->ai_protocol);
		if (clientSocket == INVALID_SOCKET)
		{
			freeaddrinfo(cresult);
			std::cout << "Error at socket()" << WSAGetLastError() << "\n";
			WSACleanup();
			system("pause");
			return 1;
		}
		iResult = connect(clientSocket, cresult->ai_addr, (int)cresult->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			freeaddrinfo(cresult);
			if (clientSocket == INVALID_SOCKET)
			{
				WSACleanup();
				std::cout << "Connect error\n";
				system("pause");
				return 1;
			}
		}
		recv(clientSocket, recvBuffer, 24, 0);
		std::cout << recvBuffer;
		std::cout << "ECHO - print the string\nTIME - print server time\nEXIT\n";
		bool isTimeRequested;
		while (true)
		{
			std::getline(std::cin, sendBuffer);
			size_t pos;
			if (pos = sendBuffer.rfind("ECHO ", 0) == 0)
			{
				sendBuffer.erase(pos - 1, 5);
			}
			if (sendBuffer.rfind("TIME", 0) == 0)
			{
				sendBuffer.clear();
				sendBuffer += "TIME";
			}
			if (sendBuffer.size() > 0)
				sendBuffer += '\r\0';
			iResult = send(clientSocket, sendBuffer.c_str(), sendBuffer.size(), 0);
			if (sendBuffer.size() > 0)
			{
				recv(clientSocket, recvBuffer, 64, 0);
				sRecvBuffer.clear();
				sRecvBuffer.append(recvBuffer);
				if (sRecvBuffer.rfind("exit", 0) == 0)
					return 0;
				std::cout << recvBuffer << std::endl;
			}
		}
	}
	if (choice == 2)
	{
		auto date = std::chrono::steady_clock::now();
		int iResult = getaddrinfo(NULL, "8081", &properties, &sresult);
		if (iResult != 0)
		{
			std::cout << "getaddrinfo failed" << WSAGetLastError() << "\n";
			WSACleanup();
			system("pause");
			return 1;
		}
		serverSocket = socket(sresult->ai_family, sresult->ai_socktype, sresult->ai_protocol);
		if (serverSocket == INVALID_SOCKET)
		{
			freeaddrinfo(sresult);
			std::cout << "Error at socket()" << WSAGetLastError() << "\n";
			WSACleanup();
			system("pause");
			return 1;
		}
		iResult = bind(serverSocket, sresult->ai_addr, (int)sresult->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			freeaddrinfo(sresult);
			WSACleanup();
			std::cout << "Bind error\n";
			system("pause");
			return 1;
		}
		freeaddrinfo(sresult);
		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			WSACleanup();
			std::cout << "Listen error\n";
			system("pause");
			return 1;
		}
		clientSocket = accept(serverSocket, 0, 0);
		if (clientSocket == INVALID_SOCKET)
		{
			WSACleanup();
			std::cout << "Accept error\n";
			system("pause");
			return 1;
		}
		send(clientSocket, "Connected succesfully\n\r", 24, 0);
		char ch;
		do
		{
			ZeroMemory(recvBuffer, 64);
			sRecvBuffer.clear();
			do {
				iResult = recv(clientSocket, &ch, 1, 0);
				sRecvBuffer += ch;
			} while (ch != '\n' && ch != '\r' && iResult > 0);
			int i = 0;
			sendBuffer = sRecvBuffer;
			if (iResult > 0)
			{
				if (sRecvBuffer.rfind("EXIT", 0) == 0)
				{
					std::cout << "Closing connection..." << "\n";
					send(clientSocket, "exit", 5, 0);
					return 0;
				}
				if (sRecvBuffer.rfind("TIME", 0) == 0)
				{
					auto date2 = std::chrono::steady_clock::now();
					sendBuffer.clear();
					auto time = std::chrono::duration_cast<std::chrono::seconds>(date2 - date).count();
					char* number = new char[10];
					sendBuffer.append(_itoa(time, number, 10));
					sendBuffer += "\r\0";
				}
				auto test = sendBuffer.c_str();
				send(clientSocket, sendBuffer.c_str(), 64, 0);
			}
			else if (iResult == 0)
				std::cout << "Closing connection..." << "\n";
			else
				return 1;
			std::cout << sRecvBuffer << std::endl;
		} while (iResult > 0);
	}
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
