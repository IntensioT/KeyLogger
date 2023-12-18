#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sstream>
#include <string>

// ��� ���������� ������ freeaddrinfo � MinGW
#define _WIN32_WINNT 0x501

#include <WinSock2.h>
#include <WS2tcpip.h>

// ����������, ����� �������� ����������� � DLL-�����������
// ��� ������ � �������
#pragma comment(lib, "Ws2_32.lib")

using std::cerr;

SYSTEMTIME lTime;
std::wstring str;
char tempStr[64];

int StringToWString(std::wstring& ws, const std::string& s)
{
    std::wstring wsTmp(s.begin(), s.end());

    ws += wsTmp;

    return 0;
}

bool WriteToFile(const std::wstring& filePath, const std::wstring& content)
{
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(hFile, content.c_str(), static_cast<DWORD>(content.length() * sizeof(wchar_t)), &bytesWritten, NULL))
    {
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

WCHAR* ConvertUTF8ToWideString(const std::string& utf8Str)
{
    int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    WCHAR* wideStr = new WCHAR[wideStrLen];
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wideStr, wideStrLen);
    return wideStr;
}

char* ConvertWcharToChar(LPWSTR str)
{
    // Convert the wchar_t string to a char* string. Record
    // the length of the original string and add 1 to it to
    // account for the terminating null character.
    size_t origsize = wcslen(str) + 1;
    size_t convertedChars = 0;


    // Allocate two bytes in the multibyte output string for every wide
    // character in the input string (including a wide character
    // null). Because a multibyte character can be one or two bytes,
    // you should allot two bytes for each character. Having extra
    // space for the new string isn't an error, but having
    // insufficient space is a potential security problem.
    const size_t newsize = origsize * 2;
    // The new string will contain a converted copy of the original
    // string plus the type of string appended to it.
    char* nstring = new char[newsize];

    // Put a copy of the converted string into nstring
    wcstombs_s(&convertedChars, nstring, newsize, str, _TRUNCATE);
    return nstring;
}

int main()
{
    WSADATA wsaData;
    // ����� ������������� ���������� ������� ���������
    // (������������ Ws2_32.dll)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        cerr << "WSAStartup failed: " << result << "\n";
        return result;
    }

    struct addrinfo* addr = NULL; // IP-����� ���������� ������

    // ������ ��� ������������� ��������� ������
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET; // AF_INET ����������, ��� ����� �������������� ���� ��� ������ � �������
    hints.ai_socktype = SOCK_STREAM; // ������ ��������� ��� ������
    hints.ai_protocol = IPPROTO_TCP; // ���������� �������� TCP
    hints.ai_flags = AI_PASSIVE; // ����� ����� ��������� �� �����,
    // ����� ��������� �������� ����������

    // �������������� ���������, �������� ����� ������ - addr
    result = getaddrinfo("127.0.0.1", "8000", &hints, &addr);

    if (result != 0) {
        cerr << "getaddrinfo failed: " << result << "\n";
        WSACleanup(); // �������� ���������� Ws2_32.dll
        return 1;
    }

    // �������� ������
    int listen_socket = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);
    // ����������� ������, ���������� ��� ��������� addr,
    // ��������� dll-���������� � ��������� ���������
    if (listen_socket == INVALID_SOCKET) {
        cerr << "Error at socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    // ����������� ����� � IP-������
    result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

    // ������� ������, ����������� ������, ���������� ��� ��������� addr.
    // � ��������� �������� �����.
    // ��������� DLL-���������� �� ������ � ��������� ���������.
    if (result == SOCKET_ERROR) {
        cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    // �������������� ��������� �����
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }


    const int max_client_buffer_size = 4096;
    char buf[max_client_buffer_size];
    int client_socket = INVALID_SOCKET;

    for (;;) {
        // ��������� �������� ����������
        client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            cerr << "accept failed: " << WSAGetLastError() << "\n";
            closesocket(listen_socket);
            WSACleanup();
            return 1;
        }

        result = recv(client_socket, buf, max_client_buffer_size, 0);

        std::stringstream response; // ���� ����� ������������ ����� �������
        std::stringstream response_body; // ���� ������

        if (result == SOCKET_ERROR) {
            // ������ ��������� ������
            cerr << "recv failed: " << result << "\n";
            closesocket(client_socket);
        }
        else if (result == 0) {
            // ���������� ������� ��������
            cerr << "connection closed...\n";
        }
        else if (result > 0) {
            // �� ����� ����������� ������ ���������� ������, ������� ������ ����� ����� ������
            // � ������ �������.
            //buf[result] = '\0';
            WCHAR* textWchar = ConvertUTF8ToWideString(buf);
            printf(buf);
            GetLocalTime(&lTime);
            sprintf(tempStr, "%02d_%02d_%02d.txt", lTime.wHour, lTime.wMinute,lTime.wSecond, lTime.wDay, lTime.wMonth, lTime.wYear);
            StringToWString(str, tempStr);
            WriteToFile(str, textWchar);

            //

            // ������ ������� ��������
            // ��������� ���� ������ (HTML)
            response_body << "<title>HTTP Logger Server</title>\n"
                << "<h1>Test page</h1>\n"
                << "<p>This is body of the test page...</p>\n"
                << "<h2>Request headers</h2>\n"
                << "<pre>" << buf << "</pre>\n"
                << "<em><small>Test C++ Http Server</small></em>\n";

            // ��������� ���� ����� ������ � �����������
            response << "HTTP/1.1 200 OK\r\n"
                << "Version: HTTP/1.1\r\n"
                << "Content-Type: text/html; charset=utf-8\r\n"
                << "Content-Length: " << response_body.str().length()
                << "\r\n\r\n"
                << response_body.str();

            // ���������� ����� ������� � ������� ������� send
            result = send(client_socket, response.str().c_str(),
                response.str().length(), 0);

            if (result == SOCKET_ERROR) {
                // ��������� ������ ��� �������� ������
                cerr << "send failed: " << WSAGetLastError() << "\n";
            }
            // ��������� ���������� � ��������
            closesocket(client_socket);
        }
    }

    closesocket(listen_socket);
    freeaddrinfo(addr);
    WSACleanup();
    return 0;
}
