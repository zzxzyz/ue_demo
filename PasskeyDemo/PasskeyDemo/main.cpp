#include <windows.h>
#include "MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize COM for WebAuthn API
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to initialize COM", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Create and show main window
    MainWindow mainWindow;
    if (!mainWindow.Create(hInstance, nCmdShow))
    {
        CoUninitialize();
        return -1;
    }

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}
