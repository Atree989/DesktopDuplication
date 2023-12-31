# DesktopDuplication
DirectX9<br>
Provide a desktop duplication solution based on DX9.<br>
The external methods are as follows：<br>
- extern "C" _declspec(dllexport) void __stdcall DisplayCloneInit();<br>
- extern "C" _declspec(dllexport) void __stdcall StartClone();<br>
- extern "C" _declspec(dllexport) void __stdcall QuitClone();<br>
- extern "C" _declspec(dllexport) int __stdcall GetAdapterDevice(const char* deviceName);<br>
- extern "C" _declspec(dllexport) void __stdcall AddCloneInfo(int adapter,<br>
                                                            int ScreenPosX, int ScreenPosY,<br>
                                                            int ScreenWidth, int ScreenHeight,<br>
                                                            int ScreenActualWidth, int ScreenActualHeight,<br>
                                                            int type);<br>
