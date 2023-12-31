#pragma once
//创建克隆显示设备
void CreateCloneDevice();
//计算克隆画面显示需要的参数
bool MakeCloneShowParams();
//清除克隆设备资源
void CloneDeviceRelease();
//克隆数据交换
void CloneDataExchange();

extern "C" _declspec(dllexport) void __stdcall DisplayCloneInit();
extern "C" _declspec(dllexport) void __stdcall StartClone();
extern "C" _declspec(dllexport) void __stdcall QuitClone();
extern "C" _declspec(dllexport) int __stdcall GetAdapterDevice(const char* deviceName);

//deviceName:屏幕名称
//ScreenPosX,ScreenPosY:屏幕起始坐标
//ScreenWidth,ScreenHeight:屏幕缩放后尺寸
//ScreenActualWidth,ScreenActualHeight:屏幕实际分辨率
//type:标识当前屏幕用于克隆还是显示，克隆为0， 显示为1
extern "C" _declspec(dllexport) void __stdcall AddCloneInfo(int adapter,
                                                            int ScreenPosX, int ScreenPosY,
                                                            int ScreenWidth, int ScreenHeight,
                                                            int ScreenActualWidth, int ScreenActualHeight,
                                                            int type);
