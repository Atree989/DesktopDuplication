#pragma once
#include "CMode.h"
using namespace std;
class CloneDevice
{
public:
    __declspec(dllexport) CloneDevice();
    __declspec(dllexport) ~CloneDevice();
    //__declspec(dllexport) int GetAdapterDevice(unsigned char deviceName[]);
    __declspec(dllexport) int GetAdapterDevice(const char* deviceName);
    __declspec(dllexport) IDirect3DDevice9* CreateCloneDevice(int adapterIndex, HWND hwnd);
    __declspec(dllexport) IDirect3DVertexBuffer9* CreateVertex(IDirect3DDevice9* g_pDevice);
    //填充绘制鼠标所需的顶点坐标
    //g_pDevice:克隆显示窗体的3D设备
    //g_pVB:鼠标的顶点坐标
    //克隆后画面中鼠标相对屏幕相对位置的缩放比例，计算方法为：(克隆画面尺寸/源画面尺寸)*(源画面实际尺寸/源画面文本缩放后尺寸)
    //cloneSourceScreenLeft,cloneSourceScreenTop:克隆源画面在系统中的起始坐标
    //cloneSourceScreenWidth,cloneSourceScreenHeight:克隆原画面缩放后尺寸
    //cloneDestScreenLeft,cloneDestScreenTop:克隆画面相对克隆窗体的起始坐标
    __declspec(dllexport) bool FillVertex(IDirect3DDevice9* g_pDevice, IDirect3DVertexBuffer9* g_pVB, float circle, int cloneSourceScreenLeft, int cloneSourceScreenTop,
        int cloneSourceScreenWidth, int cloneSourceScreenHeight, int cloneDestScreenLeft, int cloneDestScreenTop);
    //绘制鼠标
    //g_pDevice:克隆显示窗体的3D设备
    //g_pVB:鼠标的顶点坐标
    //调用此方法前需先填充绘制鼠标所需的顶点坐标
    __declspec(dllexport) bool DrawCursor(IDirect3DDevice9* g_pDevice, IDirect3DVertexBuffer9* g_pVB);
    //屏幕克隆
    //g_pCloneDevice:克隆显示窗体的3D设备
    //g_pSourceDevice:克隆源画面的3D设备
    //__declspec(dllexport) bool ScreenClone(IDirect3DDevice9* g_pCloneDevice, IDirect3DDevice9* g_pSourceDevice, RECT cloneRect, int actualCloneSourceWidth, int actualCloneSourceHeight);
    __declspec(dllexport) bool ScreenClone(IDirect3DDevice9* g_pCloneDevice, IDirect3DSurface9* g_OffScreenSurface, RECT cloneRect, int actualCloneSourceWidth, int actualCloneSourceHeight);
    __declspec(dllexport) IDirect3DSurface9* GetOffScreenSurface(IDirect3DDevice9* g_pCloneDevice, int actualCloneSourceWidth, int actualCloneSourceHeight);
private:
    IDirect3D9* g_pDirect3D;
};
