#ifndef _DLL_H_
#define _DLL_H_

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#include<windows.h>
#include<winternl.h>

typedef NTSTATUS(WINAPI*MHF)(SYSTEM_INFORMATION_CLASS,PVOID,ULONG,PULONG);
typedef DWORD(WINAPI*tZQSI)(SYSTEM_INFORMATION_CLASS SystemInformationClass,PVOID SystemInformation,ULONG SystemInformationLength,PULONG ReturnLength);
DLLIMPORT NTSTATUS myhook(SYSTEM_INFORMATION_CLASS SystemInformationClass,PVOID SystemInformation,ULONG SystemInformationLength,PULONG ReturnLength);
DLLIMPORT LRESULT CALLBACK WindowHookCallback(int nCode,WPARAM wParam,LPARAM lParam);

#endif
