#include "PasskeyWindow.h"
#include <commctrl.h>
#include <comdef.h>
#include <vector>
#include <sstream>

#pragma comment(lib, "comctl32.lib")

#define IDC_STATUS_TEXT        1002
#define IDC_AUTHENTICATE_BTN   1003
#define IDC_CANCEL_BTN         1004

PasskeyWindow::PasskeyWindow() : m_hWnd(nullptr), m_hStatusText(nullptr), m_hAuthenticateBtn(nullptr), 
    m_hInstance(nullptr), m_hParent(nullptr), m_bModal(false)
{
}

PasskeyWindow::~PasskeyWindow()
{
}

void PasskeyWindow::ShowModal(HINSTANCE hInstance, HWND hParent)
{
    m_hInstance = hInstance;
    m_hParent = hParent;
    m_bModal = true;

    // Register window class
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_DIALOG + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"PasskeyDemoPasskeyWindow";
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    // Create window
    m_hWnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        L"PasskeyDemoPasskeyWindow",
        L"Passkey Authentication",
        WS_DLGFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        450, 250,
        hParent,
        nullptr,
        hInstance,
        this
    );

    if (m_hWnd)
    {
        // Center the window
        RECT rcWnd, rcParent;
        GetWindowRect(m_hWnd, &rcWnd);
        GetWindowRect(hParent, &rcParent);

        int x = rcParent.left + (rcParent.right - rcParent.left - (rcWnd.right - rcWnd.left)) / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcWnd.bottom - rcWnd.top)) / 2;
        SetWindowPos(m_hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);

        // Enable parent window
        EnableWindow(hParent, FALSE);

        // Message loop for modal dialog
        MSG msg = {};
        while (IsWindow(m_hWnd) && GetMessage(&msg, nullptr, 0, 0))
        {
            if (!IsDialogMessage(m_hWnd, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        EnableWindow(hParent, TRUE);
        SetForegroundWindow(hParent);
    }
}

LRESULT CALLBACK PasskeyWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PasskeyWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (PasskeyWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else
    {
        pThis = (PasskeyWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT PasskeyWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate();
        return 0;

    case WM_COMMAND:
        OnCommand(wParam);
        return 0;

    case WM_CLOSE:
        OnClose();
        return 0;

    default:
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
}

void PasskeyWindow::OnCreate()
{
    // Create status text
    m_hStatusText = CreateWindow(
        L"STATIC",
        L"Click 'Authenticate' to start passkey login",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        20, 30,
        400, 30,
        m_hWnd,
        (HMENU)IDC_STATUS_TEXT,
        m_hInstance,
        nullptr
    );

    // Create authenticate button
    m_hAuthenticateBtn = CreateWindow(
        L"BUTTON",
        L"Authenticate",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        120, 100,
        100, 35,
        m_hWnd,
        (HMENU)IDC_AUTHENTICATE_BTN,
        m_hInstance,
        nullptr
    );

    // Create cancel button
    CreateWindow(
        L"BUTTON",
        L"Cancel",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        240, 100,
        100, 35,
        m_hWnd,
        (HMENU)IDC_CANCEL_BTN,
        m_hInstance,
        nullptr
    );
}

void PasskeyWindow::OnCommand(WPARAM wParam)
{
    if (LOWORD(wParam) == IDC_AUTHENTICATE_BTN)
    {
        OnAuthenticate();
    }
    else if (LOWORD(wParam) == IDC_CANCEL_BTN)
    {
        OnClose();
    }
}

void PasskeyWindow::OnClose()
{
    if (m_bModal)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
        PostQuitMessage(0);
    }
}

void PasskeyWindow::OnAuthenticate()
{
    UpdateStatus(L"Authenticating with passkey...");
    EnableWindow(m_hAuthenticateBtn, FALSE);

    // Perform passkey authentication
    bool success = PerformPasskeyAuthentication();

    if (success)
    {
        UpdateStatus(L"Authentication successful!");
        MessageBox(m_hWnd, L"Passkey authentication successful!", L"Success", MB_OK | MB_ICONINFORMATION);
        OnClose();
    }
    else
    {
        UpdateStatus(L"Authentication failed. Please try again.");
        EnableWindow(m_hAuthenticateBtn, TRUE);
    }
}

void PasskeyWindow::UpdateStatus(const std::wstring& message)
{
    if (m_hStatusText)
    {
        SetWindowText(m_hStatusText, message.c_str());
    }
}

bool PasskeyWindow::PerformPasskeyAuthentication()
{
    // Check if WebAuthn API is available
    HMODULE hWebAuthn = LoadLibrary(L"webauthn.dll");
    if (!hWebAuthn)
    {
        UpdateStatus(L"WebAuthn API not available on this system");
        return false;
    }

    // Get function pointers
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_GET_API_VERSION)(DWORD*);
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_IS_USER_VERIFYING_PLATFORM_AUTHENTICATOR_AVAILABLE)(WEBAUTHN_AUTHENTICATOR_ATTACHMENT, BOOL*);
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_GET_ASSERTION)(HWND, PCWSTR, PCWEBAUTHN_CLIENT_DATA, PCWEBAUTHN_GET_ASSERTION_OPTIONS, PWEBAUTHN_ASSERTION*, DWORD*);
    typedef VOID(WINAPI* PFN_WEBAUTHN_FREE_ASSERTION)(PWEBAUTHN_ASSERTION);

    PFN_WEBAUTHN_GET_API_VERSION pfnGetApiVersion = 
        (PFN_WEBAUTHN_GET_API_VERSION)GetProcAddress(hWebAuthn, "WebAuthNGetApiVersionNumber");
    PFN_WEBAUTHN_IS_USER_VERIFYING_PLATFORM_AUTHENTICATOR_AVAILABLE pfnIsUVPAAvailable = 
        (PFN_WEBAUTHN_IS_USER_VERIFYING_PLATFORM_AUTHENTICATOR_AVAILABLE)GetProcAddress(hWebAuthn, "WebAuthNIsUserVerifyingPlatformAuthenticatorAvailable");
    PFN_WEBAUTHN_GET_ASSERTION pfnGetAssertion = 
        (PFN_WEBAUTHN_GET_ASSERTION)GetProcAddress(hWebAuthn, "WebAuthNGetAssertion");
    PFN_WEBAUTHN_FREE_ASSERTION pfnFreeAssertion = 
        (PFN_WEBAUTHN_FREE_ASSERTION)GetProcAddress(hWebAuthn, "WebAuthNFreeAssertion");

    if (!pfnGetApiVersion || !pfnIsUVPAAvailable || !pfnGetAssertion || !pfnFreeAssertion)
    {
        FreeLibrary(hWebAuthn);
        UpdateStatus(L"WebAuthn API functions not available");
        return false;
    }

    // Check API version
    DWORD apiVersion = 0;
    HRESULT hr = pfnGetApiVersion(&apiVersion);
    if (FAILED(hr))
    {
        FreeLibrary(hWebAuthn);
        UpdateStatus(L"Failed to get WebAuthn API version");
        return false;
    }

    // Check if platform authenticator is available
    BOOL isAvailable = FALSE;
    hr = pfnIsUVPAAvailable(WEBAUTHN_AUTHENTICATOR_ATTACHMENT_ANY, &isAvailable);
    if (FAILED(hr) || !isAvailable)
    {
        FreeLibrary(hWebAuthn);
        UpdateStatus(L"Platform authenticator (Windows Hello) not available. Please set up Windows Hello.");
        return false;
    }

    // Prepare WebAuthn GetAssertion options
    // For demo, we use a simple RP ID (localhost for testing)
    PCWSTR rpId = L"localhost";
    
    // Generate a simple challenge (in production, this should be cryptographically random)
    BYTE challenge[32] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                          0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                          0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                          0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20 };

    // Set up client data JSON
    std::wstring clientDataJson = L"{\"type\":\"webauthn.get\",\"challenge\":\"";
    // Base64 encode challenge (simplified for demo - in production use proper base64)
    const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < 32; i += 3)
    {
        BYTE b1 = challenge[i];
        BYTE b2 = (i + 1 < 32) ? challenge[i + 1] : 0;
        BYTE b3 = (i + 2 < 32) ? challenge[i + 2] : 0;
        clientDataJson += (wchar_t)base64Chars[(b1 >> 2) & 0x3F];
        clientDataJson += (wchar_t)base64Chars[((b1 << 4) | (b2 >> 4)) & 0x3F];
        if (i + 1 < 32)
            clientDataJson += (wchar_t)base64Chars[((b2 << 2) | (b3 >> 6)) & 0x3F];
        if (i + 2 < 32)
            clientDataJson += (wchar_t)base64Chars[b3 & 0x3F];
    }
    clientDataJson += L"\",\"origin\":\"http://localhost\"}";

    std::vector<BYTE> clientDataBytes;
    for (wchar_t c : clientDataJson)
    {
        clientDataBytes.push_back((BYTE)(c & 0xFF));
        clientDataBytes.push_back((BYTE)((c >> 8) & 0xFF));
    }

    WEBAUTHN_CLIENT_DATA clientData = {};
    clientData.dwVersion = WEBAUTHN_CLIENT_DATA_CURRENT_VERSION;
    clientData.cbClientDataJSON = (DWORD)clientDataBytes.size();
    clientData.pbClientDataJSON = clientDataBytes.data();
    clientData.pwszHashAlgId = WEBAUTHN_HASH_ALGORITHM_SHA_256;

    WEBAUTHN_GET_ASSERTION_OPTIONS options = {};
    options.dwVersion = WEBAUTHN_GET_ASSERTION_OPTIONS_CURRENT_VERSION;
    options.dwTimeoutMs = 60000; // 60 seconds
    options.CredentialList = { 0, nullptr }; // Empty credential list - allow any credential
    options.Extensions = { 0, nullptr };
    options.dwAuthenticatorAttachment = WEBAUTHN_AUTHENTICATOR_ATTACHMENT_ANY;
    options.dwUserVerificationRequirement = WEBAUTHN_USER_VERIFICATION_REQUIREMENT_ANY;
    options.dwFlags = 0;

    // Call WebAuthNGetAssertion
    PWEBAUTHN_ASSERTION pAssertion = nullptr;
    DWORD dwUsedCredentialList = 0;

    UpdateStatus(L"Waiting for user authentication (Windows Hello)...");

    hr = pfnGetAssertion(
        m_hWnd,
        rpId,
        &clientData,
        &options,
        &pAssertion,
        &dwUsedCredentialList
    );

    bool success = false;
    if (SUCCEEDED(hr) && pAssertion)
    {
        // Authentication succeeded
        success = true;
        pfnFreeAssertion(pAssertion);
    }
    else
    {
        // Authentication failed or was cancelled
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            UpdateStatus(L"Authentication cancelled by user");
        }
        else
        {
            std::wstringstream ss;
            ss << L"Authentication failed (Error: 0x" << std::hex << hr << L")";
            UpdateStatus(ss.str());
        }
    }

    FreeLibrary(hWebAuthn);
    return success;
}
