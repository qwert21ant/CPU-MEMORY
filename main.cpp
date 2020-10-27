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

int main(){
    while(true){
        cout<<"CPU usage: "<<int(GetCPULoad())<<"%"<<endl;
        cout<<"Memory usage: "<<int(GetMemoryLoad())<<"%"<<endl;
        Sleep(1000);
        system("cls");
    }
    return 0;
}
