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

#define SEGMENT_SIZE 16384

using namespace std;

struct stoppedDownload
{
	string filename;
	size_t sentPart = 0;
};

int downloadFileFromServer(string filename, SOCKET& socket, size_t offset)
{
	char synchroBytes[2];
	FILE* fileToSend;
	filename.pop_back();
	fileToSend = fopen(filename.c_str(), "rb");
	if (fileToSend == nullptr)
	{
		send(socket, "-", 2, 0);
		return -1;
	}
	send(socket, "+", 2, 0);
	fseek(fileToSend, 0, SEEK_END);
	size_t fileSize = ftell(fileToSend);
	fseek(fileToSend, offset, SEEK_SET);
	string sFileSize = to_string(fileSize);
	size_t sentBytes = offset, readPackage;
	send(socket, sFileSize.c_str(), sFileSize.size(), 0);
	recv(socket, synchroBytes, 2, 0);
	send(socket, filename.c_str(), filename.size(), 0);
	recv(socket, synchroBytes, 2, 0);
	char* filePart = new char[SEGMENT_SIZE];
	ZeroMemory(filePart, SEGMENT_SIZE);
	while (sentBytes < fileSize)
	{
		readPackage = fread(filePart, 1, SEGMENT_SIZE, fileToSend);
		if (send(socket, filePart, readPackage, 0) != SOCKET_ERROR)
		{
			sentBytes += readPackage;
			recv(socket, synchroBytes, 2, 0);
			cout << sentBytes << " wrote" << endl;
		}
		else
		{
			cout << WSAGetLastError() << endl;
			cout << "Client has disconnected" << endl;
			fclose(fileToSend);
			return sentBytes - readPackage;
		}
	}
	fclose(fileToSend);
	return -2;
}

int downloadFileToClient(SOCKET& socket, char* recvBuffer, size_t offset)
{
	char size[32], filename[255];
	ZeroMemory(filename, 255);
	recv(socket, recvBuffer, 2, 0);
	if (strcmp(recvBuffer, "-") != 0)
	{
		recv(socket, size, 32, 0);
		send(socket, "!", 2, 0);
		recv(socket, filename, 255, 0);
		send(socket, "!", 2, 0);
		FILE* downloadedFile;
		downloadedFile = fopen(filename, "ab");
		size_t fileSize = atoi(size), currentSize = offset, recievedData = 0;
		char* filePart = new char[SEGMENT_SIZE];
		ZeroMemory(filePart, SEGMENT_SIZE);
		while (currentSize < fileSize)
		{
			if (send(socket, "", 0, 0) != SOCKET_ERROR)
			{
				recievedData = recv(socket, filePart, SEGMENT_SIZE, 0);
				send(socket, "!", 2, 0);
				currentSize += recievedData;
				fwrite(filePart, 1, recievedData, downloadedFile);
			}
			else
			{
				fclose(downloadedFile);
				return currentSize - recievedData;
			}
		}
		fclose(downloadedFile);
	}
	send(socket, "!", 2, 0);
	recv(socket, recvBuffer, 30, 0);
	return -2;
}

void checkForDownloadingState(SOCKET& socket, string clientAddress,
	map<string, stoppedDownload>& stoppedDownloads, string filename, int opResult, const char *message)
{
	char synchroBytes[2];
	if (opResult >= 0)
		stoppedDownloads[clientAddress] = stoppedDownload{ filename, (size_t)opResult };
	else
	{
		recv(socket, synchroBytes, 2, 0);
		if (opResult == -2)
			send(socket, message, 30, 0);
		else
			send(socket, "File doesn't exists", 20, 0);
	}
}

int startServer(string serverIp)
{
	string sendBuffer, sRecvBuffer;
	SOCKET serverSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
	struct addrinfo properties, * sresult;
	map<string, stoppedDownload> stoppedDownloads, stoppedUploads;
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
		for (tryCounter = 1; tryCounter < 20; tryCounter++)
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
		char synchroBytes[7];
		clientAddress += to_string(client.sin_addr.S_un.S_un_b.s_b1) + "." + to_string(client.sin_addr.S_un.S_un_b.s_b2) +
			"." + to_string(client.sin_addr.S_un.S_un_b.s_b3) + "." + to_string(client.sin_addr.S_un.S_un_b.s_b4);
		cout << "Connection accepted from " << clientAddress << endl;
		send(clientSocket, "Connected succesfully\n\r", 24, 0);
		recv(clientSocket, synchroBytes, 2, 0);
		char recvBuffer[30];
		char cOffset[64];
		if (stoppedDownloads.count(clientAddress))
		{
			send(clientSocket, "resumeD", 8, 0);
			_itoa(stoppedDownloads[clientAddress].sentPart, cOffset, 10);
			send(clientSocket, cOffset, 64, 0);
			int opResult = downloadFileFromServer(stoppedDownloads[clientAddress].filename, clientSocket,
				stoppedDownloads[clientAddress].sentPart);
			checkForDownloadingState(clientSocket, clientAddress, stoppedDownloads,
				stoppedDownloads[clientAddress].filename, opResult, "Downloaded successfully");
		}
		else
			send(clientSocket, "ok", 2, 0);
		if (stoppedUploads.count(clientAddress))
		{
			send(clientSocket, "resumeU", 8, 0);
			_itoa(stoppedUploads[clientAddress].sentPart, cOffset, 10);
			send(clientSocket, cOffset, 64, 0);
			send(clientSocket, stoppedUploads[clientAddress].filename.c_str(), 255, 0);
			_itoa(stoppedUploads[clientAddress].sentPart, cOffset, 10);
			int opResult = downloadFileToClient(clientSocket, recvBuffer, stoppedUploads[clientAddress].sentPart);
			checkForDownloadingState(clientSocket, clientAddress, stoppedDownloads,
				stoppedDownloads[clientAddress].filename, opResult, "Uploaded successfully");
		}
		else
			send(clientSocket, "ok", 2, 0);
		char ch;
		do
		{
			sendBuffer.clear();
			sRecvBuffer.clear();
			do {
				iResult = recv(clientSocket, &ch, 1, 0);
				if (iResult > 0)
					sRecvBuffer += ch;
			} while (ch != '\n' && iResult > 0);
			int i = 0;
			size_t pos;
			bool isInvalidCommand = true;
			if (iResult > 0)
			{
				if (sRecvBuffer == string("EXIT\n"))
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
				if (sRecvBuffer == string("TIME\n"))
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
					cout << "Download command" << endl;
					string filename = sRecvBuffer;
					size_t offset = 0;
					filename.erase(pos - 1, 9);
					int opResult = downloadFileFromServer(filename, clientSocket, 0);
					checkForDownloadingState(clientSocket, clientAddress, stoppedDownloads,
						filename, opResult, "Downloaded successfully");
					continue;
				}
				if (pos = sRecvBuffer.rfind("UPLOAD ", 0) == 0)
				{
					cout << "Upload command" << endl;
					string filename = sRecvBuffer;
					size_t offset = 0;
					filename.erase(pos - 1, 7);
					int opResult = downloadFileToClient(clientSocket, recvBuffer, 0);
					cout << recvBuffer << endl;
					checkForDownloadingState(clientSocket, clientAddress, stoppedUploads, filename, opResult, recvBuffer);
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
	struct addrinfo properties, * cresult;
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
	send(clientSocket, "!", 2, 0);
	recv(clientSocket, recvBuffer, 8, 0);
	char offset[64];
	if (strcmp(recvBuffer, "resumeD") == 0)
	{
		ZeroMemory(offset, 64);
		recv(clientSocket, offset, 64, 0);
		cout << "Attempting to re-download recent file..." << endl;
		downloadFileToClient(clientSocket, recvBuffer, atoi(offset));
		cout << recvBuffer << endl;
	}
	recv(clientSocket, recvBuffer, 8, 0);
	if (strcmp(recvBuffer, "resumeU") == 0)
	{
		ZeroMemory(offset, 64);
		recv(clientSocket, offset, 64, 0);
		char filename[255];
		ZeroMemory(filename, 255);
		recv(clientSocket, filename, 255, 0);
		cout << "Attempting to re-upload recent file..." << endl;
		if (downloadFileFromServer(filename, clientSocket, atoi(offset)) == -2)
			cout << "Uploaded succesfully" << endl;
	}
	cout << "ECHO - print the string\nTIME - print server time\nDOWNLOAD - download a file from the server\nUPLOAD - upload a file to the server\nEXIT\n";
	ZeroMemory(recvBuffer, 64);
	cin.ignore(INT_MAX, '\n');
	while (true)
	{
		cout << "> ";
		getline(cin, sendBuffer);
		sendBuffer += '\n';
		bool skipResponse = false;
		iResult = send(clientSocket, sendBuffer.c_str(), sendBuffer.size(), 0);
		if (size_t pos = sendBuffer.rfind("DOWNLOAD ", 0) == 0)
		{
			downloadFileToClient(clientSocket, recvBuffer, 0);
			skipResponse = true;
		}
		if (size_t pos = sendBuffer.rfind("UPLOAD ", 0) == 0)
		{
			string filename = sendBuffer;
			filename.erase(pos - 1, 7);
			downloadFileFromServer(filename, clientSocket, 0);
			recv(clientSocket, recvBuffer, 2, 0);
			send(clientSocket, "Uploaded successfully", 22, 0);
			send(clientSocket, "!", 2, 0);
		}
		if (!skipResponse)
			recv(clientSocket, recvBuffer, 64, 0);
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
