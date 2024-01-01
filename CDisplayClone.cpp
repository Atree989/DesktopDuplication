#include "pch.h"
#include "CDisplayClone.h"
#include "CloneDevice.h"
#include <list>
#include<iostream>
using namespace std;

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#pragma region ScreenInfoStruct
struct ScreenInfo
{
    int adapter;
    int posX;
    int posY;
    int width;
    int height;
    int actualWidth;
    int actualHeight;
    int type;
};
#pragma endregion

#pragma region CloneInfoStruct
struct sourceInfo
{
    IDirect3DDevice9* g_pDevice;
    IDirect3DVertexBuffer9* g_pVB;
    RECT cloneRect;
    float circle;
    ScreenInfo CloneSourceScreen;
};
struct CloneInfo
{
    HWND hwnd;
    ScreenInfo CloneShowScreen;
    list<sourceInfo> m_Source;
};
#pragma endregion

#pragma managed
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool isClone;
CloneDevice* m_clone;
list<ScreenInfo>* m_screen;
list<CloneInfo>* m_DirectCloneInfo;
HWND CreateCloneWindow(int posX, int posY, int width, int height);

void __stdcall DisplayCloneInit()
{
    WNDCLASSEX wcl = { 0 };
    wcl.cbSize = sizeof(wcl);
    wcl.style = CS_CLASSDC;
    wcl.lpfnWndProc = WindowProc;
    wcl.cbClsExtra = 0;
    wcl.cbWndExtra = 0;
    wcl.hInstance = GetModuleHandle(0);
    wcl.hIcon = NULL;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = 0;
    wcl.lpszMenuName = NULL;
    wcl.lpszClassName = TEXT("ScreenClone");
    wcl.hIconSm = NULL;
    if (!RegisterClassEx(&wcl))
        return;
    m_screen = new list<ScreenInfo>;
    m_DirectCloneInfo = new list<CloneInfo>;
    m_clone = new CloneDevice();
    isClone = false;
}

void __stdcall StartClone()
{
    MSG msg = { 0 };
    if (m_screen->size() <= 0)
        return;
#pragma region CreateCloneDevice
    CreateCloneDevice(); // 初始化克隆信息  list<CloneInfo>
    if (m_DirectCloneInfo->size() <= 0)
    {
        CloneDeviceRelease();
        return;
    }
#pragma endregion

#pragma region GetScreenCircle
    if (!MakeCloneShowParams()) // 计算源显示到目标窗口显示器的位置及大小
    {
        CloneDeviceRelease();
        return;
    }
#pragma endregion
    isClone = true;
    while (isClone)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        CloneDataExchange(); // 复制数据
        Sleep(10);
    }
    for (list<CloneInfo>::iterator it = m_DirectCloneInfo->begin(); it != m_DirectCloneInfo->end(); it++)
    {
        DestroyWindow((*it).hwnd);
    }

    CloneDeviceRelease();
}

void __stdcall QuitClone()
{
    isClone = false;
}

void __stdcall AddCloneInfo(int adapter, int ScreenPosX, int ScreenPosY, int ScreenWidth, int ScreenHeight,
    int ScreenActualWidth, int ScreenActualHeight, int type)
{
    ScreenInfo info = { 0 };
    info.adapter = adapter;
    info.posX = ScreenPosX;
    info.posY = ScreenPosY;
    info.width = ScreenWidth;
    info.height = ScreenHeight;
    info.actualWidth = ScreenActualWidth;
    info.actualHeight = ScreenActualHeight;
    info.type = type;

    m_screen->push_back(info);
}

int __stdcall GetAdapterDevice(const char* deviceName)
{
    std::cout << deviceName << std::endl;
    return m_clone->GetAdapterDevice(deviceName);
}

HWND CreateCloneWindow(int posX, int posY, int width, int height)
{
    HWND g_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, TEXT("ScreenClone"),
        TEXT("ScreenClone"), WS_POPUP, posX, posY,
        width, height, 0, 0,
        GetModuleHandle(0), 0);

    if (g_hwnd == NULL)
        return NULL;
    ShowWindow(g_hwnd, 10);
    UpdateWindow(g_hwnd);
    return g_hwnd;
}

void CreateCloneDevice()
{
    if (m_screen->size() <= 0)
        return;
#pragma region CreateCloneWindow
    for (list<ScreenInfo>::iterator it = m_screen->begin(); it != m_screen->end(); it++)
    {
        if ((*it).type == 1)
        {
            HWND hwnd = CreateCloneWindow((*it).posX, (*it).posY, (*it).width, (*it).height);    // 创建目标显示器的全屏窗口
            if (hwnd == NULL)
            {
                m_DirectCloneInfo->clear();
                return;
            }
            CloneInfo info;  // 目标显示器信息：窗口信息、关联的源显示器列表
            info.hwnd = hwnd;
            info.CloneShowScreen = (*it);
            m_DirectCloneInfo->push_back(info);
        }
    }
    if (m_DirectCloneInfo->size() <= 0)
        return;
#pragma endregion
#pragma region CreateDirectDeviceForCloneWidnow
    for (list<ScreenInfo>::iterator it = m_screen->begin(); it != m_screen->end(); it++)
    {
        if ((*it).type == 0)
        {
            for (list<CloneInfo>::iterator In = m_DirectCloneInfo->begin(); In != m_DirectCloneInfo->end(); In++)
            {
                sourceInfo source;// 源显示器信息
                source.CloneSourceScreen = (*it);
                source.g_pDevice = m_clone->CreateCloneDevice((*it).adapter, (*In).hwnd);    // 创建源显示器关联信息（初始化）
                if (!source.g_pDevice)
                {
                    m_DirectCloneInfo->clear();
                    return;
                }
                (*In).m_Source.push_back(source);
            }
        }
    }
    for (list<CloneInfo>::iterator In = m_DirectCloneInfo->begin(); In != m_DirectCloneInfo->end(); In++)
    {
        for (list<sourceInfo>::iterator it = (*In).m_Source.begin(); it != (*In).m_Source.end(); it++)
        {
            (*it).g_pVB = m_clone->CreateVertex((*(*In).m_Source.begin()).g_pDevice);
        }
    }
#pragma endregion
#pragma region Charge if there has no SourceDevice
    for (list<CloneInfo>::iterator it = m_DirectCloneInfo->begin(); it != m_DirectCloneInfo->end(); it++)
    {
        if ((*it).m_Source.size() <= 0)
        {
            m_DirectCloneInfo->clear();
            return;
        }
    }
#pragma endregion
    return;
}

bool MakeCloneShowParams()
{
    float circle = 1;
    for (list<CloneInfo>::iterator it = m_DirectCloneInfo->begin(); it != m_DirectCloneInfo->end(); it++)
    {
        int cutWidth = (*it).CloneShowScreen.width / (*it).m_Source.size();//多画面下计算每个画面的宽度
        int operat = 0;
        for (list<sourceInfo>::iterator sr = (*it).m_Source.begin(); sr != (*it).m_Source.end(); sr++)
        {
            int actualWidth = ((*sr).CloneSourceScreen.actualWidth > cutWidth) ? cutWidth : (*sr).CloneSourceScreen.actualWidth;
            float sizeable = (float)actualWidth / (float)(*sr).CloneSourceScreen.actualWidth;
            while ((int)(sizeable * (float)(*sr).CloneSourceScreen.actualHeight) > (*it).CloneShowScreen.height)
            {
                sizeable -= 0.05;
            }
            (*sr).circle = sizeable * (float)(*sr).CloneSourceScreen.actualWidth / (float)(*sr).CloneSourceScreen.width;
            (*sr).cloneRect.left = (cutWidth - (int)((float)(*sr).CloneSourceScreen.actualWidth * sizeable)) / 2 + operat * cutWidth;
            (*sr).cloneRect.right = (*sr).cloneRect.left + (int)((float)(*sr).CloneSourceScreen.actualWidth * sizeable);
            (*sr).cloneRect.top = ((*it).CloneShowScreen.height - (int)((float)(*sr).CloneSourceScreen.actualHeight * sizeable)) / 2;
            (*sr).cloneRect.bottom = (*sr).cloneRect.top + (int)((float)(*sr).CloneSourceScreen.actualHeight * sizeable);
            operat++;
        }
    }
    return true;
}

void CloneDeviceRelease()
{
    if (m_screen->size() > 0)
        m_screen->clear();
    for (list<CloneInfo>::iterator it = m_DirectCloneInfo->begin(); it != m_DirectCloneInfo->end(); it++)
    {
        for (list<sourceInfo>::iterator sr = (*it).m_Source.begin(); sr != (*it).m_Source.end(); sr++)
        {
            if ((*sr).g_pDevice)
                (*sr).g_pDevice->Release();
            if ((*sr).g_pVB)
                (*sr).g_pVB->Release();
        }
    }
    if (m_DirectCloneInfo->size() > 0)
        m_DirectCloneInfo->clear();
    UnregisterClass(L"ScreenClone", nullptr);
    delete m_clone;
}

void CloneDataExchange()
{
#pragma region GetSourceSurface
    list<IDirect3DSurface9*> m_OffScreenSurface;
    for (list<sourceInfo>::iterator sr = (*(m_DirectCloneInfo->begin())).m_Source.begin(); sr != (*(m_DirectCloneInfo->begin())).m_Source.end(); sr++)
    {
        //创建显示器纹理空间，一定使用源显示器的尺寸创建
        m_OffScreenSurface.push_back(m_clone->GetOffScreenSurface((*sr).g_pDevice, (*sr).CloneSourceScreen.actualWidth, (*sr).CloneSourceScreen.actualHeight));
    }
#pragma endregion
#pragma region Clone
    list< IDirect3DSurface9*>::iterator g_OffScreenSurface = m_OffScreenSurface.begin();
    for (list<CloneInfo>::iterator it = m_DirectCloneInfo->begin(); it != m_DirectCloneInfo->end(); it++)
    {
        (*(*it).m_Source.begin()).g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        g_OffScreenSurface = m_OffScreenSurface.begin();
        for (list<sourceInfo>::iterator sr = (*it).m_Source.begin(); sr != (*it).m_Source.end(); sr++)
        {
            // 开始复制屏幕信息
            m_clone->ScreenClone((*(*it).m_Source.begin()).g_pDevice, *(g_OffScreenSurface++), (*sr).cloneRect, (*sr).CloneSourceScreen.actualWidth, (*sr).CloneSourceScreen.actualHeight);
            // 填充绘制鼠标所需的顶点坐标
            if (m_clone->FillVertex((*(*it).m_Source.begin()).g_pDevice, (*sr).g_pVB, (*sr).circle, (*sr).CloneSourceScreen.posX, (*sr).CloneSourceScreen.posY, (*sr).CloneSourceScreen.width,
                (*sr).CloneSourceScreen.height, (*sr).cloneRect.left, (*sr).cloneRect.top))
            {
                // 绘制鼠标
                m_clone->DrawCursor((*(*it).m_Source.begin()).g_pDevice, (*sr).g_pVB);
            }
        }
        (*(*it).m_Source.begin()).g_pDevice->Present(NULL, NULL, NULL, NULL);


    }
#pragma endregion

#pragma region ReleaseSurface
    for (list<IDirect3DSurface9*>::iterator it = m_OffScreenSurface.begin(); it != m_OffScreenSurface.end(); it++)
    {
        (*it)->Release();
    }
#pragma endregion

    // 窗口呈现，遍历列表
#pragma region Windows Exsit?
    for (list<CloneInfo>::iterator it = m_DirectCloneInfo->begin(); it != m_DirectCloneInfo->end(); it++)
    {
        if (!UpdateWindow((*it).hwnd))
        {
            isClone = false;
            break;
        }


    }
#pragma endregion

    m_OffScreenSurface.clear();
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static TCHAR szBuffer[32] = { 0 };
    LRESULT res = DefWindowProc(hWnd, msg, wParam, lParam);
    switch (msg)
    {
    case WM_NCHITTEST:

        if (res == HTCAPTION || (wParam >= HTLEFT && res < HTBORDER))
            return HTCLIENT;
    case WM_CREATE:
        //timeBeginPeriod(1);
        SetTimer(hWnd, 1, 1000, 0);
        return 0;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        //timeEndPeriod(1);
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        }
        break;
        //case WM_NCHITTEST:
        //    return HTCAPTION;   // allows dragging of the window
    case WM_TIMER:
        SetWindowText(hWnd, szBuffer);
        return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
