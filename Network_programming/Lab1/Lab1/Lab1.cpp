// Lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <ctime>
#include <chrono>
#include <Mswsock.h>
#include <fstream>
#include <map>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

using namespace std;

struct stoppedDownload
{
	string clientIp;
	size_t sentPart;
};

size_t downloadFile(string filename, SOCKET& socket, size_t offset)
{
	FILE* fileToSend;
	fileToSend = fopen(filename.c_str(), "rb");
	fseek(fileToSend, 0, SEEK_END);
	size_t fileSize = ftell(fileToSend);
	fseek(fileToSend, offset, SEEK_SET);
	string sFileSize = to_string(fileSize);
	size_t sentBytes = offset;
	int currentPacket = 0;
	char synchroBytes[2];
	send(socket, sFileSize.c_str(), sFileSize.size(), 0);
	recv(socket, synchroBytes, 2, 0);
	send(socket, filename.c_str(), filename.size(), 0);
	recv(socket, synchroBytes, 2, 0);
	char* filePart = new char[16384];
	ZeroMemory(filePart, 16384);
	while (sentBytes < fileSize)
	{
		if (ftell(fileToSend) != SEEK_END)
			currentPacket = fread(filePart, 1, 16384, fileToSend);
		if (currentPacket = send(socket, filePart, 16384, 0) != SOCKET_ERROR)
		{
			sentBytes += currentPacket;
			recv(socket, synchroBytes, 2, 0);
			cout << sentBytes << " wrote" << endl;
		}
		else
		{
			cout << WSAGetLastError() << endl;
			cout << "Client has disconnected" << endl;
			fclose(fileToSend);
			return sentBytes + 1 - currentPacket;
		}
	}
	fclose(fileToSend);
	return 0;
}

int startServer(string serverIp)
{
	string sendBuffer, sRecvBuffer;
	SOCKET serverSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
	struct addrinfo properties, * sresult, * cresult;
	map<string, stoppedDownload> stoppedDownloads;
	ZeroMemory(&properties, sizeof(properties));
	properties.ai_family = AF_INET;
	properties.ai_socktype = SOCK_STREAM;
	properties.ai_protocol = IPPROTO_TCP;
	auto date = chrono::steady_clock::now();
	int iResult = getaddrinfo(serverIp.c_str(), "8081", &properties, &sresult);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed" << WSAGetLastError() << "\n";
		WSACleanup();
		system("pause");
		return 1;
	}
	serverSocket = socket(sresult->ai_family, sresult->ai_socktype, sresult->ai_protocol);
	if (serverSocket == INVALID_SOCKET)
	{
		freeaddrinfo(sresult);
		cout << "Error at socket()" << WSAGetLastError() << "\n";
		WSACleanup();
		system("pause");
		return 1;
	}
	iResult = bind(serverSocket, sresult->ai_addr, (int)sresult->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		freeaddrinfo(sresult);
		WSACleanup();
		cout << "Bind error\n";
		system("pause");
		return 1;
	}
	freeaddrinfo(sresult);
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		WSACleanup();
		cout << "Listen error\n";
		system("pause");
		return 1;
	}
	fd_set sockets;
	sockets.fd_count = 1;
	sockets.fd_array[0] = serverSocket;
	int tryCounter = 0;
	sockaddr_in client = { 0 };
	socklen_t len = sizeof(client);
	do
	{
		clientSocket = INVALID_SOCKET;
		for (tryCounter = 1; tryCounter < 5; tryCounter++)
			if (select(0, &sockets, &sockets, &sockets, new TIMEVAL{ 2, 0 }) == 1)
			{
				clientSocket = accept(serverSocket, (sockaddr*)(&client), &len);
				break;
			}
			else
			{
				cout << "Connection failed(" << tryCounter << ")" << endl;
				sockets.fd_count = 1;
			}
		if (clientSocket == INVALID_SOCKET)
		{
			WSACleanup();
			cout << "Accept error\n";
			system("pause");
			return 1;
		}
		string clientAddress;
		clientAddress += to_string(client.sin_addr.S_un.S_un_b.s_b1) + "." + to_string(client.sin_addr.S_un.S_un_b.s_b2) +
			"." + to_string(client.sin_addr.S_un.S_un_b.s_b3) + "." + to_string(client.sin_addr.S_un.S_un_b.s_b4);
		cout << "Connection accepted from " << clientAddress << endl;
		send(clientSocket, "Connected succesfully\n\r", 24, 0);
		char ch;
		do
		{
			sendBuffer.clear();
			sRecvBuffer.clear();
			do {
				iResult = recv(clientSocket, &ch, 1, 0);
				if (iResult > 0)
					sRecvBuffer += ch;
			} while (ch != '\0' && iResult > 0);
			int i = 0;
			size_t pos;
			bool isInvalidCommand = true;
			if (iResult > 0)
			{
				if (sRecvBuffer == "EXIT")
				{
					cout << "Closing connection..." << "\n";
					send(clientSocket, "exit", 5, 0);
					return 0;
				}
				if (pos = sRecvBuffer.rfind("ECHO ", 0) == 0)
				{
					cout << "ECHO command" << endl;
					sendBuffer = sRecvBuffer;
					sendBuffer.erase(pos - 1, 5);
					isInvalidCommand = false;
				}
				if (sRecvBuffer == "TIME")
				{
					cout << "TIME command" << endl;
					auto date2 = chrono::steady_clock::now();
					auto timeInSeconds = chrono::duration_cast<chrono::seconds>(date2 - date).count();
					sendBuffer += "Server uptime: ";
					time_t hours = timeInSeconds / 3600, mins = timeInSeconds / 60, secs = timeInSeconds % 60;
					string sHours, sMins, sSecs;
					if (hours < 10)
						sHours += "0";
					sHours += to_string(hours);
					if (mins < 10)
						sMins += "0";
					sMins += to_string(mins);
					if (secs < 10)
						sSecs += "0";
					sSecs += to_string(secs);
					sendBuffer += sHours + ":" + sMins + ":" + sSecs;
					sendBuffer += "\0";
					isInvalidCommand = false;
				}
				if (pos = sRecvBuffer.rfind("DOWNLOAD ", 0) == 0)
				{
					char synchroBytes[2];
					string filename = sRecvBuffer;
					size_t offset = 0;
					filename.erase(pos - 1, 9);
					if (stoppedDownloads.count(clientAddress))
						offset = stoppedDownloads[clientAddress].sentPart;
					size_t opResult = downloadFile(filename, clientSocket, offset);
					if (opResult != 0)
						stoppedDownloads[clientAddress] = stoppedDownload{ clientAddress, opResult };
					else
					{
						recv(clientSocket, synchroBytes, 2, 0);
						send(clientSocket, "Download successful", 20, 0);
					}
					continue;
				}
				if (isInvalidCommand)
					sendBuffer += "Invalid command";
				send(clientSocket, sendBuffer.c_str(), 64, 0);
			}
			else if (iResult == 0)
				cout << "Closing connection..." << "\n";
			else
				cout << "Connection lost" << endl;
		} while (iResult > 0);
	} while (true);
}

int startClient(string serverIp)
{
	SOCKET clientSocket = INVALID_SOCKET;
	struct addrinfo properties, * sresult, * cresult;
	string sendBuffer, sRecvBuffer;
	ZeroMemory(&properties, sizeof(properties));
	properties.ai_family = AF_INET;
	properties.ai_socktype = SOCK_STREAM;
	properties.ai_protocol = IPPROTO_TCP;
	char recvBuffer[64];
	ZeroMemory(recvBuffer, 64);
	int iResult = getaddrinfo(serverIp.c_str(), "8081", &properties, &cresult);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed" << WSAGetLastError() << "\n";
		system("pause");
		WSACleanup();
		return 1;
	}
	clientSocket = socket(cresult->ai_family, cresult->ai_socktype, cresult->ai_protocol);
	if (clientSocket == INVALID_SOCKET)
	{
		freeaddrinfo(cresult);
		cout << "Error at socket()" << WSAGetLastError() << "\n";
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
			cout << "Connect error\n";
			system("pause");
			return 1;
		}
	}
	recv(clientSocket, recvBuffer, 24, 0);
	cout << recvBuffer;
	cout << "ECHO - print the string\nTIME - print server time\nEXIT\n";
	cin.ignore(INT_MAX, '\n');
	while (true)
	{
		getline(cin, sendBuffer);
		sendBuffer += '\0';
		iResult = send(clientSocket, sendBuffer.c_str(), sendBuffer.size(), 0);
		if (size_t pos = sendBuffer.rfind("DOWNLOAD ", 0) != 0)
			recv(clientSocket, recvBuffer, 64, 0);
		else
		{
			char size[32], filename[255];
			recv(clientSocket, size, 32, 0);
			send(clientSocket, "!", 2, 0);
			recv(clientSocket, filename, 255, 0);
			send(clientSocket, "!", 2, 0);
			ofstream downloadedFileStream;
			downloadedFileStream.open(filename, ios::binary | ios::out | ios::app);
			size_t fileSize = atoi(size), currentSize = 0, recievedData;
			char* filePart = new char[16384];
			ZeroMemory(filePart, 16384);
			while (currentSize < fileSize)
			{
				recievedData = recv(clientSocket, filePart, 16384, 0);
				send(clientSocket, "!", 2, 0);
				currentSize += recievedData;
				downloadedFileStream.write(filePart, string(filePart).size());
			}
			downloadedFileStream.close();
			send(clientSocket, "!", 2, 0);
			recv(clientSocket, recvBuffer, 20, 0);
			continue;
		}
		sRecvBuffer.clear();
		sRecvBuffer.append(recvBuffer);
		if (sRecvBuffer.rfind("exit", 0) == 0)
			return 0;
		cout << recvBuffer << endl;

	}
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	int choice;
	string serverIp;
	cout << "Enter server ip: ";
	cin >> serverIp;
	//serverIp += "localhost";
	cout << "1. Client\n2. Server\n";
	cin >> choice;
	if (choice == 1)
	{
		return(startClient(serverIp));
	}
	if (choice == 2)
	{
		return (startServer(serverIp));
	}
}
