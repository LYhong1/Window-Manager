#include<thread>
#include<stdio.h>
#include<Windows.h>
#include<TlHelp32.h>
using namespace std;
HWND x,xm;
FARPROC wph,wph2;
HMODULE hdll,hdll2;
unsigned long y;
APPBARDATA appbd;
HANDLE z,hMapFile,hMapFile2;
HHOOK kHook,mHook,wHook,wHook2;
WINDOWPLACEMENT placement;
bool m,k,l=1,p,q,a1[256],b1[3];
unsigned long long hidepid,taskpid;
char WindowTitle[256],s[1001],cn,dpath[1024],*fileName,comd[100][256]={
"help","list","kill","mark","hide","show","topmost","nottopmost","all","taskbar","auto","disable","window","process","plus"
};
struct ax{
	HWND a;
	ax *b=0,*c=0;
	int type=1;
	unsigned long p;
	bool d=0;
	char t[256];
}c,*a,*b=&c;
int chge(char*a){
	int b=0,c=strlen(a);
	for(unsigned int i=0;i<c;i++){
		if(a[i]>47&&a[i]<58){
			b=b*10+a[i]-48;
		}else{
			return 0;
		}
	}
	return b;
}
unsigned long hchge(const char*str){
    unsigned long a=0;
    while(*str){
        char c=*str++;
		if('0'<=c&&c<='9')
            a=(a<<4)+c-48;
        else if('a'<=c&&c<='f')
            a=(a<<4)+c-87;
        else if('A'<=c&&c<='F')
            a=(a<<4)+c-55;
        else
            break;
    }
    return a;
}
void destorylist(ax*a){
	if(!a->b)b=b->c;
	else a->b->c=a->c;
	a->c->b=a->b;
	free(a);
}
bool GetProcessNameByPID(int pid,char*s){
    HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hSnapshot==INVALID_HANDLE_VALUE){
        return 0;
    }
    PROCESSENTRY32 pe32;
    pe32.dwSize=sizeof(PROCESSENTRY32);
    if(!Process32First(hSnapshot,&pe32)){
        CloseHandle(hSnapshot);
        return 0;
    }
    while(Process32Next(hSnapshot,&pe32)){
        if(pe32.th32ProcessID==pid){
            strcpy(s,pe32.szExeFile);
            CloseHandle(hSnapshot);
            return 1;
        }
    }
    CloseHandle(hSnapshot);
    return 0;
}
bool injectdll(unsigned long long dpid,const char*dpath){
    void*dproc=OpenProcess(PROCESS_ALL_ACCESS,0,dpid);
    if(!dproc){
        printf("%s\n","Failed to open process.");
        return 0;
    }
    void*dprocadd=VirtualAllocEx(dproc,NULL,strlen(dpath)+1,MEM_COMMIT,PAGE_READWRITE);
    if(!dprocadd){
        printf("%s\n","Failed to allocate memory in the remote process.");
        CloseHandle(dproc);
        return 0;
    }
    if(!WriteProcessMemory(dproc,dprocadd,dpath,strlen(dpath)+1,0)){
        printf("%s\n","Failed to write to remote process memory.");
        VirtualFreeEx(dproc,dprocadd,0,MEM_RELEASE);
        CloseHandle(dproc);
        return 0;
    }
    HMODULE dmod=LoadLibraryA("kernel32.dll");
    LPTHREAD_START_ROUTINE founadd=(LPTHREAD_START_ROUTINE)GetProcAddress(dmod,"LoadLibraryA");
    void*dthre=CreateRemoteThread(dproc,0,0,founadd,dprocadd,0,0);
    if(!dthre){
        printf("%s\n","Failed to create a remote thread in the target process.");
        VirtualFreeEx(dproc,dprocadd,0,MEM_RELEASE);
        CloseHandle(dproc);
        return 0;
    }
    WaitForSingleObject(dthre,INFINITE);
    CloseHandle(dthre);
    FreeLibrary(dmod);
    VirtualFreeEx(dproc,dprocadd,0,MEM_RELEASE);
    CloseHandle(dproc);
    return 1;
}
bool uninstalldll(unsigned long long dpid,const char*dpath){
    void*dproc=OpenProcess(PROCESS_ALL_ACCESS,0,dpid),*vdll;
    HMODULE dmod=LoadLibraryA("kernel32.dll");
    LPTHREAD_START_ROUTINE founadd=(LPTHREAD_START_ROUTINE)GetProcAddress(dmod,"FreeLibrary");
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dpid);
    if (snapshot==INVALID_HANDLE_VALUE){
    	printf("%s\n","Failed to create a process snapshot for the target process.");
	}
	MODULEENTRY32 me32;
    me32.dwSize=sizeof(MODULEENTRY32);
    if(Module32First(snapshot,&me32)){
        do{
            if(strstr(me32.szExePath,dpath)){
                vdll=me32.modBaseAddr;
            }

        }while(Module32Next(snapshot,&me32));
    }
    CloseHandle(snapshot);
    void*dthre=CreateRemoteThread(dproc,0,0,founadd,vdll,0,0);
    if(!dthre){
        printf("%s\n","Failed to create a remote thread in the target process.");
        CloseHandle(dproc);
        return 0;
    }
    WaitForSingleObject(dthre,INFINITE);
    CloseHandle(dproc);
    return 1;
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam){
	GetWindowTextA(hwnd,WindowTitle,256);
    if(!GetWindowThreadProcessId(hwnd,&y))return 1;
    if(strcmp(s,comd[8])&&!IsWindowVisible(hwnd))return 1;
    m=1;
   	for(a=c.b;a;a=a->b){
   		if(a->a==hwnd){
   			m=0;
   			strcpy(a->t,WindowTitle);
   			break;
		}
	}
	if(m){
		b->b=new ax;
		b->b->a=hwnd;
		b->b->c=b;
		b->b->p=y;
		strcpy(b->b->t,WindowTitle);
		b=b->b;
	}
	return 1;
}
LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam){
	p=0;
    if(nCode==HC_ACTION){
        KBDLLHOOKSTRUCT*pkbhs=(KBDLLHOOKSTRUCT*)lParam;
        switch(wParam){
            case WM_KEYDOWN:{
                if(!a1[pkbhs->vkCode]){
					p=a1[pkbhs->vkCode]=1;
				}
                break;
            }
            case WM_KEYUP:{
                if(a1[pkbhs->vkCode]){
                	a1[pkbhs->vkCode]=0;
				}
                break;
            }
            case WM_SYSKEYDOWN:{
            	if(!a1[pkbhs->vkCode]){
            		p=a1[pkbhs->vkCode]=1;
            	}
				break;
			}
            case WM_SYSKEYUP:{
            	if(a1[pkbhs->vkCode]){
            		a1[pkbhs->vkCode]=0;
            	}
				break;
			}
        }
    }
    if(p){
		if(a1[164]&&a1[90]&&c.b!=0){
			for(a=c.b;a;a=a->b){
				if(a->d){
					if(!k){
						GetWindowPlacement(a->a,&placement);
						a->type=placement.showCmd;
						ShowWindow(a->a,0);
					}else{
						ShowWindow(a->a,a->type);
					}
				}
			}
			k=!k;
			goto a;
		}
		if(a1[162]&&a1[81]){
			UnhookWindowsHookEx(kHook);
			UnhookWindowsHookEx(mHook);
			UnhookWindowsHookEx(wHook);
    		FreeLibrary(hdll);
			for(a=c.b;a;a=a->b){
				if(a->d){
					if(a->a==x){
						continue;
					}
					GetWindowThreadProcessId(a->a,&y);
					z=OpenProcess(PROCESS_ALL_ACCESS,0,y);
					TerminateProcess(z,0);
					CloseHandle(z);
				}
			}
			exit(0);
		}
	}
	return CallNextHookEx(NULL,nCode,wParam,lParam);
a:
	return 1;
}
LRESULT CALLBACK MouseHookProc(int nCode,WPARAM wParam,LPARAM lParam){
	q=0;
	if(nCode==HC_ACTION){
		switch(wParam){
	        case WM_LBUTTONDOWN:
				if(!b1[0])q=b1[0]=1;
	            break;
	        case WM_LBUTTONUP:
	            if(b1[0])b1[0]=0;
	            break;
	        case WM_RBUTTONDOWN:
				if(!b1[1])q=b1[1]=1;
	            break;
	        case WM_RBUTTONUP:
				if(b1[1])b1[1]=0;
	            break;
    	}
	}
    if(q){
	   	if(a1[162]&&b1[1]){
		 	xm=GetForegroundWindow();
			GetWindowThreadProcessId(xm,&y);
			m=1;
			for(a=c.b;a;a=a->b){
				if(a->a==xm){
					m=0;
					if(a->d){
						destorylist(a);
					}else{
						a->d=1;
					}
					break;
				}
			}
			if(m){
  				GetWindowTextA(xm,WindowTitle,256);
				b->b=new ax;
				b->b->a=xm;
				b->b->c=b;
				b->b->p=y;
				strcpy(b->b->t,WindowTitle);
				b->b->d=1;
				b=b->b;
			}
			goto a;
		}
		if(a1[164]&&b1[1]){
			for(a=c.b;a;a=a->b){
				if(a->d){
					if(l)SetWindowPos(a->a,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
					else SetWindowPos(a->a,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
				}
			}
			l=!l;
			goto a;
		}
	}
    return CallNextHookEx(NULL,nCode,wParam,lParam);
a:
	return 1;
}
void thread1(){
	while(1){
		scanf("%s",s);
		if(!strcmp(s,comd[0])){
			printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
			"help--显示所有本程序支持的指令",
			"list--列出所有可见的窗口(前缀***表示被标记，后缀--窗口[HWND]--对应进程[PID])",
			"list all--列出所有窗口",
			"mark [HWND]--标记/取消标记一个窗口[HWND]",
			"hide window [HWND]--隐藏指定窗口[HWND]",
			"show [HWND]--显示指定窗口[HWND]",
			"topmost [HWND]--将指定窗口[HWND]置顶",
			"nottopmost [HWND]--取消指定窗口[HWND]的置顶",
			"kill [PID]--终止进程[PID]",
			"hide process [PID1] [PID2]--面对进程[PID1]隐藏进程[PID2]",
			"hide process plus [PID1] [PID2]--面对进程[PID1]隐藏进程[PID2](注意:即使进程[PID1]重启也无法解除隐藏)",
			"disable hide process--取消所有的进程隐藏",
			"hide taskbar auto--自动隐藏任务栏：最大化窗口时隐藏，取消窗口最大化并且按下Ctrl时恢复任务栏显示",
			"disable hide taskbar auto--取消hide taskbar auto的设置",
			"Author:LYhong-bilibili摸鱼自动机-依",
			"此程序完全免费，使用即保证用途合理、保证承担使用此程序的一切后果,后续更新会优化使用体验和增加功能");
		}
		if(!strcmp(s,comd[1])){
			scanf("%c",&cn);
			if(cn==' '){
				scanf("%s",s);
			}
			EnumWindows(EnumWindowsProc,0);
			for(a=c.b;a;a=a->b){
a:
				if(!IsWindow(a->a)){
					a=a->b;
					destorylist(a->c);
					goto a;
				}
				if(strcmp(s,comd[8])&&!IsWindowVisible(a->a))continue;
				if(a->d)printf("%s","***");
				printf("%s--%X--%ld\n",a->t,a->a,a->p);
			}
		}
		if(!strcmp(s,comd[2])){
			scanf("%s",s);
			y=chge(s);
			if(y){
				for(a=c.b;a;a=a->b){
					if(a->p==y){
						destorylist(a);
						break;
					}
				}
			}
			z=OpenProcess(PROCESS_ALL_ACCESS,0,y);
			if(TerminateProcess(z,0)){
				printf("%s\n","Success");
			}else{
				printf("%s\n","Fail");
			}
			CloseHandle(z);
		}
		if(!strcmp(s,comd[3])){
			scanf("%p",&xm);
			if(xm){
				for(a=c.b;a;a=a->b){
					if(a->a==xm){
						a->d=!a->d;
						printf("%s\n","Done");
						break;
					}
				}
			}
		}
		if(!strcmp(s,comd[4])){
			scanf("%s",s);
			if(!strcmp(s,comd[13])){
				scanf("%s",s);
				taskpid=chge(s);
				hMapFile=CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,256,"taskp");
				if(hMapFile==0){
			        continue;
			    }
			    char*sharedData=(char*)MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS,0,0,256);
			    if(sharedData==0){
			        CloseHandle(hMapFile);
			        continue;
			    }
			    hMapFile2=CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,sizeof(unsigned long long),"hidepid");
				if(hMapFile2==0){
			        continue;
			    }
			    unsigned long long*sharedData2=(unsigned long long*)MapViewOfFile(hMapFile2,FILE_MAP_ALL_ACCESS,0,0,sizeof(unsigned long long));
			    if(sharedData2==0){
			        CloseHandle(hMapFile2);
			        continue;
			    }
				if(taskpid){
					scanf("%llu",&hidepid);
					strcpy(sharedData,"ILLTY");
			    	*sharedData2=hidepid;
			    	if(injectdll(taskpid,dpath)){
						printf("%s\n","Success");
					}else{
						printf("%s\n","Fail");
					}
					continue;
				}
				if(strcmp(s,comd[14]))continue;
				scanf("%llu",&taskpid);
				scanf("%llu",&hidepid);
			    GetProcessNameByPID(taskpid,sharedData);
			    *sharedData2=hidepid;
			    hdll2=LoadLibrary("Dll2.dll");
			    wph2=GetProcAddress(hdll2,"WindowHookCallback");
			    wHook2=SetWindowsHookEx(WH_CBT,(HOOKPROC)wph2,hdll2,0);
				if(wHook2){
					printf("%s\n","Success");
				}else{
					printf("%s\n","Fail");
				}
			}
			if(!strcmp(s,comd[9])){
				scanf("%s",s);
				if(!strcmp(s,comd[10])){
					hdll=LoadLibrary("Dll1.dll");
					wph=GetProcAddress(hdll,"WindowHookCallback");
					wHook=SetWindowsHookEx(WH_CBT,(HOOKPROC)wph,hdll,0);
					printf("%s\n","Done");
				}
			}
			if(!strcmp(s,comd[12])){
				scanf("%p",&xm);
				if(xm){
					for(a=c.b;a;a=a->b){
						if(a->a==xm){
							GetWindowPlacement(a->a,&placement);
							a->type=placement.showCmd;
							ShowWindow(a->a,0);
							printf("%s\n","Done");
							break;
						}
					}
				}
			}
		}
		if(!strcmp(s,comd[5])){
			scanf("%p",&xm);
			if(xm){
				for(a=c.b;a;a=a->b){
					if(a->a==xm){
						ShowWindow(a->a,a->type);
						printf("%s\n","Done");
						break;
					}
				}
			}
		}
		if(!strcmp(s,comd[6])){
			scanf("%p",&xm);
			if(xm){
				for(a=c.b;a;a=a->b){
					if(a->a==xm){
						SetWindowPos(a->a,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
						printf("%s\n","Done");
						break;
					}
				}
			}
		}
		if(!strcmp(s,comd[7])){
			scanf("%p",&xm);
			if(xm){
				for(a=c.b;a;a=a->b){
					if(a->a==xm){
						SetWindowPos(a->a,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
						printf("%s\n","Done");
						break;
					}
				}
			}
		}
		if(!strcmp(s,comd[11])){
			scanf("%s",s);
			if(!strcmp(s,comd[4])){
				scanf("%s",s);
				if(!strcmp(s,comd[9])){
					scanf("%s",s);
					if(!strcmp(s,comd[10])){
						if(wHook)UnhookWindowsHookEx(wHook);
    					if(hdll)FreeLibrary(hdll);
    					printf("%s\n","Done");
					}
				}
				if(!strcmp(s,comd[13])){
    				if(hdll2)FreeLibrary(hdll2);
					if(wHook2)UnhookWindowsHookEx(wHook2);
    				else uninstalldll(taskpid,dpath);
					printf("%s\n","Done");
					continue;
				}
			}
		}
	}
}
void init(){
	x=GetConsoleWindow();
	placement.length=sizeof(WINDOWPLACEMENT);
	GetWindowThreadProcessId(x,&y);
  	GetWindowTextA(x,WindowTitle,256);
	b->b=new ax;
	b->b->a=x;
	b->b->c=b;
	b->b->p=y;
	strcpy(b->b->t,WindowTitle);
	b->b->d=1;
	b=b->b;
	thread command(thread1);
	command.detach();
	EnumWindows(EnumWindowsProc,0);
	SetLayeredWindowAttributes(x,RGB(0,0,0),125,2);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    GetModuleFileName(0,dpath,1024);
    fileName=strrchr(dpath,'\\')+1;
	strcpy(fileName,"Dll2.dll");
}
int main(){
	init();
    kHook=SetWindowsHookEx(WH_KEYBOARD_LL,KeyboardProc,0,0);
    if(kHook==NULL){
        return 0;
    }
	mHook=SetWindowsHookEx(WH_MOUSE_LL,MouseHookProc,0,0);
    if(mHook==NULL){
        return 0;
    }
    MSG msg;
    while(GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(kHook);
    UnhookWindowsHookEx(mHook);
    if(hMapFile)CloseHandle(hMapFile);
    if(wHook)UnhookWindowsHookEx(wHook);
    if(wHook2)UnhookWindowsHookEx(wHook2);
    if(hdll)FreeLibrary(hdll);
    if(hdll2)FreeLibrary(hdll2);
}
