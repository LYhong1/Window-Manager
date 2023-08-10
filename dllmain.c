#include"dll.h"
RECT n;
POINT m;
char*fileName;
char path[1024];
APPBARDATA appbd;
WPARAM wParam1,wParam2;
LPARAM lParam1,lParam2;
void show(int a){
	if(a){
		appbd.lParam=ABS_ALWAYSONTOP;
	    SHAppBarMessage(ABM_SETSTATE,&appbd);
	}else{
		appbd.lParam=ABS_AUTOHIDE;
		SHAppBarMessage(ABM_SETSTATE,&appbd);
	}
}
DWORD WINAPI thread1(){
	switch(wParam1){
		case SC_MAXIMIZE:{
			show(0);
			break;
		}
		case SC_RESTORE:{
			GetCursorPos(&m);
			if(!(m.x>=n.left&&m.x<=n.right&&m.y>=n.top&&m.y<=n.bottom)){
				if((GetAsyncKeyState(VK_CONTROL)&0x8000)&&!(GetAsyncKeyState(VK_MENU)&0x8000))show(1);
        	}
			break;
		}
	}
}
DWORD WINAPI thread2(){
	WORD low=LOWORD(lParam2);
	switch(low){
		case SW_MAXIMIZE:{
			show(0);
			break;
		}
	}
}
DLLIMPORT LRESULT CALLBACK WindowHookCallback(int nCode,WPARAM wParam,LPARAM lParam){
	switch(nCode){
		case HCBT_SYSCOMMAND:{
			wParam1=wParam;
			lParam1=lParam;
			CreateThread(NULL,0,thread1,0,0,0);
			break;
		}
		case HCBT_MINMAX:{
			wParam2=wParam;
			lParam2=lParam;
			CreateThread(NULL,0,thread2,0,0,0);
			break;
		}
    }
	return CallNextHookEx(0,nCode,wParam,lParam);
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved){
	switch(fdwReason){
		case DLL_PROCESS_ATTACH:{
			appbd.cbSize=sizeof(appbd);
			appbd.hWnd=FindWindowA("Shell_TrayWnd",0);
			GetWindowRect(appbd.hWnd,&n);
			GetModuleFileName(0,path,1024);
			fileName=strrchr(path,'\\');
			break;
		}
		case DLL_PROCESS_DETACH:{
			if(fileName){
				fileName++;
				if(!strcmp(fileName,"Touch Fish2.exe")){
					show(1);
				}
			}
			break;
		}
		case DLL_THREAD_ATTACH:{
			break;
		}
		case DLL_THREAD_DETACH:{
			break;
		}
	}
	return 1;
}
