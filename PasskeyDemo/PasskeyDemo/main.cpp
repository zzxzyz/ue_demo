#include <windows.h>
#include <stdio.h>
#include "MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize COM for WebAuthn API
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        MessageBox(nullptr, L"Failed to initialize COM", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Create and show main window
    MainWindow mainWindow;
    SetLastError(0); // Clear any previous error
    if (!mainWindow.Create(hInstance, nCmdShow))
    {
        DWORD error = GetLastError();
        if (error == 0)
        {
            error = 1400; // Default to ERROR_INVALID_WINDOW_HANDLE if no error set
        }
        wchar_t errorMsg[512];
        swprintf_s(errorMsg, 512, L"Failed to create main window.\n\nError code: %lu\n\nPossible causes:\n- Window class registration failed\n- Invalid window parameters\n- System resources exhausted", error);
        MessageBox(nullptr, errorMsg, L"Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return -1;
    }

    // Message loop
    MSG msg = {};
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // Error occurred
            break;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    CoUninitialize();
    return (int)msg.wParam;
}
