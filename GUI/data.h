#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <thread>

using namespace std;

HWND hMainWnd;

void redraw();
char lastMSG[1000];
int  msgColor = 0;

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

vector<string> findCOMPorts(){
    vector<string> res(0);
    HKEY hKey = NULL;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM\\", 0, KEY_READ, &hKey)){
        sprintf(lastMSG, "ShowCOMPorts: Error open hKey! (%d)", GetLastError());
        msgColor = 0;
        redraw();
        return res;
    }
    DWORD nVal = 0, maxValNameLen = 0, maxValLen = 0;
    if(RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &nVal, &maxValNameLen, &maxValLen, NULL, NULL)){
        sprintf(lastMSG, "ShowCOMPorts: Error get info about hKey! (%d)", GetLastError());
        msgColor = 0;
        redraw();
        RegCloseKey(hKey);
        return res;
    }
    if(!nVal){
        RegCloseKey(hKey);
        return res;
    }

    CHAR *valName = new CHAR[++maxValNameLen];
    BYTE *value   = new BYTE[++maxValLen];

    DWORD szName, type, szValue;
    for(int i = 0; i < nVal; i++){
        szName  = maxValNameLen;
        szValue = maxValLen;
        if(RegEnumValue(hKey, i, valName, &szName, NULL, &type, value, &szValue)){
            sprintf(lastMSG, "ShowCOMProts: Error get value! (%d)", GetLastError());
            msgColor = 0;
            redraw();
            RegCloseKey(hKey);
            delete [] valName;
            delete [] value;
            res.clear();
            return res;
        }
        res.push_back(string((char*)value));
    }

    delete [] valName;
    delete [] value;

    return res;
}

void redraw(){
    InvalidateRect(hMainWnd, NULL, TRUE);
    UpdateWindow(hMainWnd);
}

#endif // DATA_H_INCLUDED
