#pragma once

#include <windows.h>
#include <webauthn.h>
#include <string>

class PasskeyWindow
{
public:
    PasskeyWindow();
    ~PasskeyWindow();

    void ShowModal(HINSTANCE hInstance, HWND hParent);

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnCreate();
    void OnCommand(WPARAM wParam);
    void OnClose();
    void OnAuthenticate();
    void UpdateStatus(const std::wstring& message);
    bool PerformPasskeyAuthentication();

    HWND m_hWnd;
    HWND m_hStatusText;
    HWND m_hAuthenticateBtn;
    HINSTANCE m_hInstance;
    HWND m_hParent;
    bool m_bModal;
};
