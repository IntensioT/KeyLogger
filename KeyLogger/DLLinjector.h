#pragma once

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

typedef void (*ChangeWordProc)(HANDLE, char*);

DWORD get_PID(CHAR* PrName);
char* getCharWCHAR(WCHAR* str);
HANDLE inject_DLL(const char* file_name, int PID);
int mainInjector(DWORD PID);
