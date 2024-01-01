#include "CloneDevice.h"

CloneDevice::CloneDevice()
{
    g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
}

CloneDevice::~CloneDevice()
{
    g_pDirect3D->Release();
}

int CloneDevice::GetAdapterDevice(const char *deviceName)
{
    if (!g_pDirect3D)
        return 0;
#pragma region GetAdapterIndex
    int adapterIndex = 0;
    int count = g_pDirect3D->GetAdapterCount();
    D3DADAPTER_IDENTIFIER9 pIdentifier = { 0 };
    for (int ix = 0; ix < count; ix++)
    {    
        g_pDirect3D->GetAdapterIdentifier(ix, 0, &pIdentifier);
        int res = strcmp(pIdentifier.DeviceName, deviceName);
        if (res == 0)
        {
            adapterIndex = ix;
            break;
        }
    }
#pragma endregion
    return adapterIndex;
}

IDirect3DDevice9 * CloneDevice::CreateCloneDevice(int adapterIndex, HWND hwnd)
{
    if (!g_pDirect3D)
        return NULL;
    IDirect3DDevice9 *g_pDevice;
    HRESULT hr = 0;
#pragma region fill D3DDevice params
    D3DPRESENT_PARAMETERS params = { 0 };
    // Setup Direct3D for windowed rendering
    params.BackBufferWidth = 0;
    params.BackBufferHeight = 0;
    // params.BackBufferFormat = desktop.Format;
    params.BackBufferFormat = D3DFMT_UNKNOWN;
    params.BackBufferCount = 1;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = hwnd;
    params.Windowed = TRUE;
    params.EnableAutoDepthStencil = TRUE;
    params.AutoDepthStencilFormat = D3DFMT_D24S8;
    params.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    params.FullScreen_RefreshRateInHz = 0;
    params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
#pragma endregion

#pragma region Create D3DDevice
    // Most modern video cards should have no problems creating pure devices.
    hr = g_pDirect3D->CreateDevice(adapterIndex, D3DDEVTYPE_HAL, hwnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
        &params, &g_pDevice);
    if (FAILED(hr))
    {
        // Fall back to software vertex processing for less capable hardware.
        hr = g_pDirect3D->CreateDevice(adapterIndex, D3DDEVTYPE_HAL,
            hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &g_pDevice);
    }
    if (FAILED(hr))
        return NULL;
#pragma endregion
    return g_pDevice;
}

IDirect3DVertexBuffer9 * CloneDevice::CreateVertex(IDirect3DDevice9 * g_pDevice)
{
    IDirect3DVertexBuffer9* g_pVB;
    if (FAILED(g_pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
    {
        return nullptr;
    }
    return g_pVB;
}

bool CloneDevice::FillVertex(IDirect3DDevice9* g_pDevice, IDirect3DVertexBuffer9* g_pVB, float circle, int cloneSourceScreenLeft, int cloneSourceScreenTop,
    int cloneSourceScreenWidth, int cloneSourceScreenHeight, int cloneDestScreenLeft, int cloneDestScreenTop)
{
    if (!g_pDevice)
        return false;
    //获取鼠标坐标
    POINT pt = { 0 };
    GetCursorPos(&pt);
    if (pt.x < cloneSourceScreenLeft
        || pt.y < cloneSourceScreenTop
        || pt.x >(cloneSourceScreenLeft + cloneSourceScreenWidth)
        || pt.y >(cloneSourceScreenTop + cloneSourceScreenHeight))
        return false;
    HRESULT hr = 0;
    if (!g_pVB)
    {
        //初始化顶点坐标
        if (FAILED(g_pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
        {
            return false;
        }
    }

    //鼠标绘制的基础尺寸
    int base = 10;
#pragma region 计算鼠标在源画面中的相对位置
    pt.x -= cloneSourceScreenLeft;
    pt.y -= cloneSourceScreenTop;
    pt.x = (int)((float)pt.x * circle);
    pt.y = (int)((float)pt.y * circle);
#pragma endregion

#pragma region 计算鼠标在克隆屏幕上的相对位置
    pt.x += cloneDestScreenLeft;
    pt.y += cloneDestScreenTop;
#pragma endregion

    //计算缩放后的鼠标尺寸
    base = (int)((float)base * circle);

#pragma region 填充顶点坐标
    CUSTOMVERTEX vertices[] =
    {
        {pt.x - base, pt.y, 0.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)},
        {pt.x + base, pt.y, 0.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)},
        {pt.x, pt.y - base, 0.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)},
        {pt.x, pt.y + base, 0.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)}
    };

    void *tmp;
    if (FAILED(g_pVB->Lock(0, sizeof(CUSTOMVERTEX), (void**)&tmp, 0)))
    {
        return false;
    }
    memcpy(tmp, vertices, 4 * sizeof(CUSTOMVERTEX));
    g_pVB->Unlock();
#pragma endregion
    return true;
}

bool CloneDevice::DrawCursor(IDirect3DDevice9 * g_pDevice, IDirect3DVertexBuffer9 * g_pVB)
{
    if (!g_pDevice || !g_pVB)
        return false;
    if (SUCCEEDED(g_pDevice->BeginScene()))
    {
        g_pDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
        g_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        g_pDevice->DrawPrimitive(D3DPT_LINELIST, 0, 2);
        g_pDevice->EndScene();
    }
    else
    {
        return false;
    }
    return true;
}

bool CloneDevice::ScreenClone(IDirect3DDevice9 * g_pCloneDevice, IDirect3DSurface9 *g_OffScreenSurface, RECT cloneRect, int actualCloneSourceWidth, int actualCloneSourceHeight)
{
    HRESULT hr;
    if (!g_pCloneDevice || !g_OffScreenSurface)
        return false;
#pragma region CreateTmpCloneOffScreenSurface
    IDirect3DSurface9 * g_OffScreenSurfaceTmp;
    if (FAILED(hr = g_pCloneDevice->CreateOffscreenPlainSurface(actualCloneSourceWidth,
        actualCloneSourceHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &g_OffScreenSurfaceTmp, NULL)))
    {
        return false;
    }
    D3DLOCKED_RECT rcLock = { 0 };
    D3DLOCKED_RECT rcLocks = { 0 };


    hr = g_OffScreenSurface->LockRect(&rcLock, 0, 0);
    if (SUCCEEDED(hr))
    {
        hr = g_OffScreenSurfaceTmp->LockRect(&rcLocks, 0, 0);
        if (SUCCEEDED(hr))
        {
            memcpy(rcLocks.pBits, rcLock.pBits, rcLock.Pitch * actualCloneSourceHeight);
        }
        g_OffScreenSurface->UnlockRect();
        g_OffScreenSurfaceTmp->UnlockRect();
    }
    DeleteObject(&rcLock);
    DeleteObject(&rcLocks);
#pragma endregion
#pragma region GetCloneSurfaceData
    IDirect3DSurface9 * g_CloneSurface;
    if (FAILED(hr = g_pCloneDevice->CreateOffscreenPlainSurface(actualCloneSourceWidth,
        actualCloneSourceHeight, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_CloneSurface, NULL)))
    {
        return false;
    }
#pragma endregion
    //D3DXSaveSurfaceToFile("E:\\123.jpg", D3DXIMAGE_FILEFORMAT::D3DXIFF_JPG, g_OffScreenSurface, NULL, NULL);
#pragma region Swap SurfaceData to CloneDevice
    if (FAILED(hr = g_pCloneDevice->UpdateSurface(g_OffScreenSurfaceTmp, NULL, g_CloneSurface, NULL)))
    {
        return false;
    }
#pragma endregion

#pragma region D3D Clone to Device
    IDirect3DSurface9 * g_BackSurface;
    if (SUCCEEDED((g_pCloneDevice->BeginScene())))
    {
        if (FAILED((hr = g_pCloneDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_BackSurface))))
        {
            return false;
        }
        g_pCloneDevice->StretchRect(g_CloneSurface, NULL, g_BackSurface, &cloneRect, D3DTEXF_LINEAR);
        g_pCloneDevice->EndScene();
    }
    else
    {
        return false;
    }
    g_OffScreenSurfaceTmp->Release();
    g_CloneSurface->Release();
    g_BackSurface->Release();
#pragma endregion

    return true;
}

IDirect3DSurface9 * CloneDevice::GetOffScreenSurface(IDirect3DDevice9 * g_pSourceDevice, int actualCloneSourceWidth, int actualCloneSourceHeight)
{
    HRESULT hr;
    IDirect3DSurface9 *g_OffScreenSurface;
    if (FAILED(hr = g_pSourceDevice->CreateOffscreenPlainSurface(actualCloneSourceWidth,
        actualCloneSourceHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &g_OffScreenSurface, NULL))) //注意第四个参数不能是D3DPOOL_DEFAULT
    {
        return nullptr;
    }
    if (FAILED(hr = g_pSourceDevice->GetFrontBufferData(0, g_OffScreenSurface)))
    {
        return nullptr;
    }
    return g_OffScreenSurface;
}

CMode.h
#pragma once
#include <d3dx9.h>
#include <dxgi1_2.h>

struct CUSTOMVERTEX
{
    float x, y, z, rhw;
    DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct SCREEN_VERTEX_UNTEX
{
    float x, y, z, h;
    D3DCOLOR color;
    static DWORD FVF;
};
