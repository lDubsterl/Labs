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

int FROMLEN = sizeof(sockaddr);

struct Packet
{
	char data[SEGMENT_SIZE];
	int sn = 0;
	int len;
	Packet(char* data, int sn, int length)
	{
		for (int i = 0; i < SEGMENT_SIZE; i++)
			this->data[i] = data[i];
		this->sn = sn;
		len = length;
	}
	Packet() {

	}
};

int downloadFileFromServer(string filename, SOCKET& socket, sockaddr* address, size_t offset)
{
	char synchroBytes[10];
	FILE* fileToSend;
	filename.pop_back();
	fileToSend = fopen(filename.c_str(), "rb");
	if (fileToSend == nullptr)
	{
		sendto(socket, "notexists", 10, 0, address, FROMLEN);
		return -1;
	}
	sendto(socket, "exists", 7, 0, address, FROMLEN);
	fseek(fileToSend, 0, SEEK_END);
	size_t fileSize = ftell(fileToSend);
	fseek(fileToSend, offset, SEEK_SET);
	string sFileSize = to_string(fileSize);
	size_t sentBytes = offset, readPackage;
	sendto(socket, sFileSize.c_str(), sFileSize.size(), 0, address, FROMLEN);
	recvfrom(socket, synchroBytes, 10, 0, address, &FROMLEN);
	sendto(socket, filename.c_str(), filename.size(), 0, address, FROMLEN);
	recvfrom(socket, synchroBytes, 10, 0, address, &FROMLEN);
	char* filePart = new char[SEGMENT_SIZE];
	ZeroMemory(filePart, SEGMENT_SIZE);
	int sn = 0, an = 0;
	while (sentBytes < fileSize)
	{
		if (sn == an)
		{
			readPackage = fread(filePart, 1, SEGMENT_SIZE, fileToSend);
			sn++;
		}
		else
			sentBytes -= readPackage;
		Packet packet(filePart, sn, readPackage);
		if (sendto(socket, (char*)&packet, sizeof(Packet), 0, address, FROMLEN) != SOCKET_ERROR)
		{
			sentBytes += readPackage;
			char* charAn = new char[15];
			int test = recvfrom(socket, charAn, 15, 0, address, &FROMLEN);
			fd_set sock;
			sock.fd_count = 1;
			sock.fd_array[0] = socket;
			if (select(0, &sock, 0, 0, new TIMEVAL{ 5, 0 }) == 1)
			{
				recvfrom(socket, charAn, 15, 0, address, &FROMLEN);
				an = atoi(charAn);
				cout << sentBytes << " wrote" << endl;
			}
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

int downloadFileToClient(SOCKET& socket, char* recvBuffer, sockaddr* recvFrom, size_t offset)
{
	char size[32], filename[255];
	ZeroMemory(filename, 255);
	recvfrom(socket, recvBuffer, 2, 0, recvFrom, &FROMLEN);
	if (strcmp(recvBuffer, "notexists") != 0)
	{
		recvfrom(socket, size, 32, 0, recvFrom, &FROMLEN);
		sendto(socket, "size", 5, 0, recvFrom, FROMLEN);
		recvfrom(socket, filename, 255, 0, recvFrom, &FROMLEN);
		sendto(socket, "name", 5, 0, recvFrom, FROMLEN);
		FILE* downloadedFile;
		downloadedFile = fopen(filename, "ab");
		size_t fileSize = atoi(size), currentSize = offset, recievedData = 0;
		char* filePart = new char[SEGMENT_SIZE];
		ZeroMemory(filePart, SEGMENT_SIZE);
		int an = 0;
		while (currentSize < fileSize)
		{
			if (sendto(socket, "", 0, 0, recvFrom, FROMLEN) != SOCKET_ERROR)
			{
				Packet dataPortion;
				recvfrom(socket, (char*)&dataPortion, sizeof(Packet), 0, recvFrom, &FROMLEN);
				recievedData = dataPortion.len;
				an = dataPortion.sn;
				string strAn = to_string(an);
				filePart = dataPortion.data;
				int test = sendto(socket, strAn.c_str(), strAn.size(), 0, recvFrom, FROMLEN);
				//int test = WSAGetLastError();
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
	sendto(socket, "!", 2, 0, recvFrom, FROMLEN);
	recvfrom(socket, recvBuffer, 30, 0, 0, &FROMLEN);
	return -2;
}

void checkForDownloadingState(SOCKET& socket, sockaddr* sendTo, string filename, int opResult, const char* message)
{
	char synchroBytes[2];
	if (opResult < 0)
	{
		recvfrom(socket, synchroBytes, 2, 0, 0, &FROMLEN);
		if (opResult == -2)
			sendto(socket, message, 30, 0, sendTo, FROMLEN);
		else
			sendto(socket, "File doesn't exists", 20, 0, sendTo, FROMLEN);
	}
}

int startServer(string serverIp)
{
	string sendBuffer, sRecvBuffer;
	SOCKET serverSocket = INVALID_SOCKET;
	struct addrinfo properties, * sresult;
	ZeroMemory(&properties, sizeof(properties));
	properties.ai_family = AF_INET;
	properties.ai_socktype = SOCK_DGRAM;
	properties.ai_protocol = IPPROTO_UDP;
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
	do
	{
		string clientAddress;
		char synchroBytes[7];
		char recvBuffer[30];
		sockaddr* address = sresult->ai_addr;
		char cOffset[64];
		do
		{
			sendBuffer.clear();
			sRecvBuffer.clear();
			ZeroMemory(recvBuffer, 30);
			iResult = recvfrom(serverSocket, recvBuffer, 30, 0, address, &FROMLEN);
			auto test = WSAGetLastError();
			if (iResult > 0)
				sRecvBuffer += recvBuffer;
			int i = 0;
			size_t pos;
			bool isInvalidCommand = true;
			if (iResult > 0)
			{
				if (sRecvBuffer == string("EXIT\n"))
				{
					cout << "Closing connection..." << "\n";
					sendto(serverSocket, "exit", 5, 0, address, FROMLEN);
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
					int opResult = downloadFileFromServer(filename, serverSocket, address, 0);
					checkForDownloadingState(serverSocket, address,
						filename, opResult, "Downloaded successfully");
					continue;
				}
				if (pos = sRecvBuffer.rfind("UPLOAD ", 0) == 0)
				{
					cout << "Upload command" << endl;
					string filename = sRecvBuffer;
					size_t offset = 0;
					filename.erase(pos - 1, 7);
					int opResult = downloadFileToClient(serverSocket, recvBuffer, address, 0);
					cout << recvBuffer << endl;
					checkForDownloadingState(serverSocket, address, filename, opResult, recvBuffer);
					continue;
				}
				if (isInvalidCommand)
					sendBuffer += "Invalid command";
				sendto(serverSocket, sendBuffer.c_str(), 64, 0, address, FROMLEN);
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
	properties.ai_socktype = SOCK_DGRAM;
	properties.ai_protocol = IPPROTO_UDP;
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
	char offset[64];
	cout << "ECHO - print the string\nTIME - print server time\nDOWNLOAD - download a file from the server\nUPLOAD - upload a file to the server\nEXIT\n";
	ZeroMemory(recvBuffer, 64);
	cin.ignore(INT_MAX, '\n');
	sockaddr* address = cresult->ai_addr;
	while (true)
	{
		cout << "> ";
		getline(cin, sendBuffer);
		sendBuffer += '\n';
		bool skipResponse = false;
		iResult = sendto(clientSocket, sendBuffer.c_str(), sendBuffer.size(), 0, address, FROMLEN);

		if (size_t pos = sendBuffer.rfind("DOWNLOAD ", 0) == 0)
		{
			downloadFileToClient(clientSocket, recvBuffer, address, 0);
			skipResponse = true;
		}
		if (size_t pos = sendBuffer.rfind("UPLOAD ", 0) == 0)
		{
			string filename = sendBuffer;
			filename.erase(pos - 1, 7);
			downloadFileFromServer(filename, clientSocket, address, 0);
			recvfrom(clientSocket, recvBuffer, 2, 0, address, &FROMLEN);
			sendto(clientSocket, "Uploaded successfully", 22, 0, address, FROMLEN);
			sendto(clientSocket, "!", 2, 0, address, FROMLEN);
		}
		if (!skipResponse)
			recvfrom(clientSocket, recvBuffer, 64, 0, address, &FROMLEN);
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