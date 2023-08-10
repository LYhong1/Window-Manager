#ifndef _DLL_H_
#define _DLL_H_
#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif
#include<windows.h>
#include<pthread.h>
DLLIMPORT LRESULT CALLBACK WindowHookCallback();
#endif
