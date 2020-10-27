/**
Link:
    gdi32
    user32
    kernel32
    comctl32
Add '-static' flag
**/

#include <windows.h>
#include <iostream>
#include <stdio.h>

using namespace std;

DWORD64 prevTotal = 0;
DWORD64 prevIdle = 0;

DWORD64 get64Time(FILETIME ft){
    return (((DWORD64)ft.dwHighDateTime) << 32) | (DWORD64)ft.dwLowDateTime;
}

float GetCPULoad(){
    FILETIME idleTime, kernelTime, userTime;

    if(!GetSystemTimes(&idleTime, &kernelTime, &userTime)) return 0;

    DWORD64 totalTicks = get64Time(kernelTime) + get64Time(userTime);
    DWORD64 idleTicks = get64Time(idleTime);

    DWORD64 curTotal = totalTicks - prevTotal;
    DWORD64 curIdle = idleTicks - prevIdle;

    prevTotal = totalTicks;
    prevIdle = idleTicks;

    return (1.0 - ((curTotal > 0) ? curIdle / (float) curTotal : 0)) * 100;
}

float GetMemoryLoad(){
    MEMORYSTATUSEX memStat;
    ZeroMemory(&memStat, sizeof(memStat));
    memStat.dwLength = sizeof(memStat);
    GlobalMemoryStatusEx(&memStat);
    return memStat.dwMemoryLoad;
}

HANDLE hSerial;
char sPortName[100];
char msg[2];
DWORD nBytesWritten;

void TranslateError(DWORD error){
    switch(error){
    case ERROR_FILE_NOT_FOUND:
        cout<<">>Error: port not exists"<<endl;
        break;
    case ERROR_ACCESS_DENIED:
        cout<<">>Error: access denied"<<endl;
        break;
    default:
        cout<<">>Error: "<<GetLastError()<<endl;
    }
}

int main(){
    cout<<"Enter port name: ";
    cin>>sPortName;

    hSerial = CreateFile(sPortName, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(hSerial == INVALID_HANDLE_VALUE){
        cout<<">>Invalid handle!"<<endl;
        TranslateError(GetLastError());
        return 0;
    }
    cout<<"Port oppened!"<<endl;

    DCB dcb;
    ZeroMemory(&dcb, sizeof(DCB));
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcb)){
        cout<<">>Error setting port state!"<<endl;
        TranslateError(GetLastError());
    }

    while(true){
        msg[0] = int(GetCPULoad());
        msg[1] = int(GetMemoryLoad());
        WriteFile(hSerial, msg, 2, &nBytesWritten, NULL);
        Sleep(1000);
    }

    CloseHandle(hSerial);

    return 0;
}
