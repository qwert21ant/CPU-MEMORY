/**
Link:
    gdi32
    user32
    kernel32
    comctl32
Add '-static' flag
**/

#include "data.h"

#define ID_LIST       0x0101
#define ID_BUT_OK     0x0102
#define ID_BUT_UPDATE 0x0103
#define ID_BUT_STOP   0x0104
#define ID_TIMER      0x0105

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

///главные ф-ции 'общени€' с arduino
BOOL startComm(int);
void doComm();
void endComm();

int nPage = 0;                        ///номер отображаемой 'страниыц'

HWND hCombo;                          ///переменна€ дл€ ComboBox'а
HWND hBut_OK, hBut_UPDATE, hBut_STOP; ///переменные дл€ кнопок
vector <string> ports;                ///названи€ портов
const char text[] = "¬ыберите порт:";

HANDLE hSerial;
DWORD nBytesWritten;
char _msg[2];                         ///отправл€емое на arduino сообщение

int wndWidth  = 400;                  ///ширина окна
int wndHeight = 200;                  ///высота окна

char wndTitle[] = "CPU-MEMORY";       ///заголовок окна
char wndClass[] = "main";             ///им€ класса окна

HINSTANCE hInst;

int WINAPI WinMain (HINSTANCE hInsta, ///ф-ци€ создани€ окна
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow){
    InitCommonControls();

    hInst = hInsta;
    WNDCLASSEX wincl;

    wincl.cbSize        = sizeof (WNDCLASSEX);
    wincl.hInstance     = hInst;
    wincl.lpszClassName = wndClass;
    wincl.lpfnWndProc   = WndProc;
    wincl.style         = CS_HREDRAW | CS_VREDRAW;

    wincl.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName  = NULL;
    wincl.cbClsExtra    = 0;
    wincl.cbWndExtra    = 0;
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    if (!RegisterClassEx (&wincl))
        return 0;

    int xPos = (GetSystemMetrics(SM_CXSCREEN) - wndWidth) / 2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - wndHeight) / 2;

    hMainWnd = CreateWindow(wndClass, wndTitle, WS_OVERLAPPEDWINDOW, xPos, yPos, wndWidth, wndHeight, NULL, NULL, hInst, NULL);

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    MSG msg;
    while (GetMessage (&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void updateCOM(){ ///получаем список портов и добавл€ем их в ComboBox
    for(int i = ports.size() - 1; i >= 0; i--)
        SendMessage(hCombo, CB_DELETESTRING, NULL, i);
    ports = findCOMPorts();
    for(int i = 0; i < ports.size(); i++)
        SendMessage(hCombo, CB_ADDSTRING, NULL, (LPARAM) ports[i].c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){ ///обработчик событий окна
    switch (msg){
        case WM_CREATE: {
            ///создаЄм кнопки и ComboBox
            hCombo      = CreateWindow("combobox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, 100, 75, 200, 0, hWnd, (HMENU) ID_LIST, hInst, NULL);
            hBut_OK     = CreateWindow("button", "OK", WS_CHILD | WS_VISIBLE | BS_SOLID, 200, 100, 70, 30, hWnd, (HMENU) ID_BUT_OK, hInst, NULL);
            hBut_UPDATE = CreateWindow("button", "Update", WS_CHILD | WS_VISIBLE | BS_SOLID, 130, 100, 70, 30, hWnd, (HMENU) ID_BUT_UPDATE, hInst, NULL);
            hBut_STOP   = CreateWindow("button", "Stop", WS_CHILD | BS_SOLID, 165, 85, 70, 30, hWnd, (HMENU) ID_BUT_STOP, hInst, NULL);

            updateCOM(); ///заполн€ем список
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);

            if(!nPage){ ///если отображаетс€ 1-€ страница (nPage = 0)
                SIZE szText;
                GetTextExtentPoint32(hDC, text, strlen(text), &szText);           ///получаем размер строки в px
                TextOut(hDC, (wndWidth - szText.cx) / 2, 50, text, strlen(text)); ///рисуем текст
            }

            if(lastMSG[0]){
                RECT clRect;
                GetClientRect(hWnd, &clRect);
                SetTextColor(hDC, msgColor ? RGB(0, 255, 0) : RGB(255, 0, 0));
                TextOut(hDC, 5, clRect.bottom - 20, lastMSG, strlen(lastMSG));
                lastMSG[0] = 0;
            }

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_COMMAND: { ///обрабатываем событи€ кнопок
            if(wParam == ID_BUT_OK){
                int uItem = (int) SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                if(uItem == -1){
                    MessageBox(hWnd, "¬ыберите COM порт из списка", "ќшибка", MB_OK | MB_ICONWARNING);
                    break;
                }
                if(startComm(uItem)){
                    SetTimer(hWnd, ID_TIMER, 1000, NULL);
                    nPage = 1;
                    ShowWindow(hBut_STOP, SW_SHOW);
                    ShowWindow(hCombo, SW_HIDE);
                    ShowWindow(hBut_OK, SW_HIDE);
                    ShowWindow(hBut_UPDATE, SW_HIDE);

                    sprintf(lastMSG, "Success open port!");
                    msgColor = 1;

                    redraw();
                }
            }
            if(wParam == ID_BUT_UPDATE){
                updateCOM();
            }
            if(wParam == ID_BUT_STOP){
                endComm();

                KillTimer(hWnd, ID_TIMER);
                nPage = 0;
                ShowWindow(hBut_STOP, SW_HIDE);
                ShowWindow(hCombo, SW_SHOW);
                ShowWindow(hBut_OK, SW_SHOW);
                ShowWindow(hBut_UPDATE, SW_SHOW);
                redraw();
                updateCOM();
            }
            break;
        }
        case WM_TIMER: {
            doComm();
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

BOOL startComm(int iPort){
    hSerial = CreateFile(ports[iPort].c_str(), GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(hSerial == INVALID_HANDLE_VALUE){
        sprintf(lastMSG, "startComm: Invalid handle! (%d)", GetLastError());
        msgColor = 0;
        redraw();
        return FALSE;
    }

    DCB dcb;
    ZeroMemory(&dcb, sizeof(DCB));
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcb)){
        sprintf(lastMSG, "startComm: Error setting port state! (%d)", GetLastError());
        msgColor = 0;
        redraw();
        CloseHandle(hSerial);
        return FALSE;
    }

    return TRUE;
}

void doComm(){
    _msg[0] = int(GetCPULoad());
    _msg[1] = int(GetMemoryLoad());
    WriteFile(hSerial, _msg, 2, &nBytesWritten, NULL);
}

void endComm(){
    CloseHandle(hSerial);
}
