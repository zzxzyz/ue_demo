#include "PasskeyWindow.h"
#include <commctrl.h>
#include <comdef.h>
#include <vector>
#include <sstream>
#include <string>

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
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
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
        
        // IMPORTANT: Set m_hWnd here, before any message handling
        pThis->m_hWnd = hWnd;
    }
    else
    {
        pThis = (PasskeyWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->HandleMessage(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT PasskeyWindow::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

    // Use the official webauthn.h structures
    // WEBAUTHN_CLIENT_DATA structure
    struct WEBAUTHN_CLIENT_DATA_LOCAL {
        DWORD dwVersion;              // Version of this structure
        DWORD cbClientDataJSON;       // Size of the pbClientDataJSON
        PBYTE pbClientDataJSON;       // UTF-8 encoded JSON
        LPCWSTR pwszHashAlgId;        // Hash algorithm ID
    };

    // Credential structure
    struct WEBAUTHN_CREDENTIAL_LOCAL {
        DWORD dwVersion;
        DWORD cbId;
        PBYTE pbId;
        LPCWSTR pwszCredentialType;
    };

    // Credentials list
    struct WEBAUTHN_CREDENTIALS_LOCAL {
        DWORD cCredentials;
        WEBAUTHN_CREDENTIAL_LOCAL* pCredentials;
    };

    // Extensions
    struct WEBAUTHN_EXTENSIONS_LOCAL {
        DWORD cExtensions;
        PVOID pExtensions;
    };

    // WEBAUTHN_AUTHENTICATOR_GET_ASSERTION_OPTIONS - Version 1 (simplest)
    struct WEBAUTHN_GET_ASSERTION_OPTIONS_V1 {
        DWORD dwVersion;                       // 1
        DWORD dwTimeoutMilliseconds;
        WEBAUTHN_CREDENTIALS_LOCAL CredentialList;
        WEBAUTHN_EXTENSIONS_LOCAL Extensions;
        DWORD dwAuthenticatorAttachment;
        DWORD dwUserVerificationRequirement;
        DWORD dwFlags;
    };

    // Get function pointer
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_GET_ASSERTION)(
        HWND hWnd,
        LPCWSTR pwszRpId,
        WEBAUTHN_CLIENT_DATA_LOCAL* pWebAuthNClientData,
        WEBAUTHN_GET_ASSERTION_OPTIONS_V1* pWebAuthNGetAssertionOptions,
        PVOID* ppWebAuthNAssertion
    );

    typedef VOID(WINAPI* PFN_WEBAUTHN_FREE_ASSERTION)(PVOID pWebAuthNAssertion);

    PFN_WEBAUTHN_GET_ASSERTION pfnGetAssertion = 
        (PFN_WEBAUTHN_GET_ASSERTION)GetProcAddress(hWebAuthn, "WebAuthNAuthenticatorGetAssertion");
    PFN_WEBAUTHN_FREE_ASSERTION pfnFreeAssertion = 
        (PFN_WEBAUTHN_FREE_ASSERTION)GetProcAddress(hWebAuthn, "WebAuthNFreeAssertion");

    if (!pfnGetAssertion)
    {
        FreeLibrary(hWebAuthn);
        UpdateStatus(L"WebAuthNAuthenticatorGetAssertion not available");
        return false;
    }

    // RP ID - this is the domain shown in the passkey dialog
    LPCWSTR rpId = L"levelinfinite.com";

    // Client data JSON (UTF-8 encoded)
    std::string clientDataJsonStr = "{\"type\":\"webauthn.get\",\"challenge\":\"AQIDBAUGBwgJCgsMDQ4PEBESExQVFhcYGRobHB0eHyA\",\"origin\":\"https://levelinfinite.com\",\"crossOrigin\":false}";
    
    // Set up client data
    WEBAUTHN_CLIENT_DATA_LOCAL clientData = {};
    clientData.dwVersion = 1;  // WEBAUTHN_CLIENT_DATA_CURRENT_VERSION
    clientData.cbClientDataJSON = (DWORD)clientDataJsonStr.size();
    clientData.pbClientDataJSON = (PBYTE)clientDataJsonStr.c_str();
    clientData.pwszHashAlgId = L"SHA-256";

    // Set up assertion options (Version 1 - simplest form)
    WEBAUTHN_GET_ASSERTION_OPTIONS_V1 options = {};
    options.dwVersion = 1;
    options.dwTimeoutMilliseconds = 60000;  // 60 seconds
    options.CredentialList.cCredentials = 0;
    options.CredentialList.pCredentials = nullptr;
    options.Extensions.cExtensions = 0;
    options.Extensions.pExtensions = nullptr;
    options.dwAuthenticatorAttachment = 0;  // WEBAUTHN_AUTHENTICATOR_ATTACHMENT_ANY
    options.dwUserVerificationRequirement = 0;  // WEBAUTHN_USER_VERIFICATION_REQUIREMENT_ANY
    options.dwFlags = 0;

    UpdateStatus(L"Opening Passkey authentication window...");

    // Call WebAuthNAuthenticatorGetAssertion
    PVOID pAssertion = nullptr;
    HRESULT hr = pfnGetAssertion(
        m_hWnd,
        rpId,
        &clientData,
        &options,
        &pAssertion
    );

    bool success = false;
    if (SUCCEEDED(hr) && pAssertion)
    {
        success = true;
        if (pfnFreeAssertion)
        {
            pfnFreeAssertion(pAssertion);
        }
        UpdateStatus(L"Authentication successful!");
    }
    else
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            UpdateStatus(L"Authentication cancelled by user");
        }
        else
        {
            std::wstringstream ss;
            ss << L"Passkey error: 0x" << std::hex << hr;
            UpdateStatus(ss.str());
        }
    }

    FreeLibrary(hWebAuthn);
    return success;
}
