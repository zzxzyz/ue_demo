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

    // Define the MakeCredential function type to trigger Windows Hello UI
    // This will show the Windows Hello prompt
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_MAKE_CREDENTIAL)(
        HWND hWnd,
        PCWSTR pwszRpId,
        PCWSTR pwszRpName,
        PCWSTR pwszUserName,
        PCWSTR pwszUserDisplayName,
        const BYTE* pbUserId,
        DWORD cbUserId,
        const BYTE* pbChallenge,
        DWORD cbChallenge,
        DWORD dwFlags,
        PVOID* ppCredentialAttestation
    );

    // Use CredUI to show Windows Hello dialog directly
    // This is a simpler approach - use Windows Security API
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_IS_UVPA_AVAILABLE)(BOOL*);
    
    PFN_WEBAUTHN_IS_UVPA_AVAILABLE pfnIsUVPAAvailable = 
        (PFN_WEBAUTHN_IS_UVPA_AVAILABLE)GetProcAddress(hWebAuthn, "WebAuthNIsUserVerifyingPlatformAuthenticatorAvailable");

    if (pfnIsUVPAAvailable)
    {
        BOOL isAvailable = FALSE;
        HRESULT hr = pfnIsUVPAAvailable(&isAvailable);
        if (FAILED(hr) || !isAvailable)
        {
            FreeLibrary(hWebAuthn);
            UpdateStatus(L"Windows Hello not available");
            return false;
        }
    }

    FreeLibrary(hWebAuthn);

    // Use Windows Credential UI to show Windows Hello prompt
    // This is a simpler way to trigger the Windows Hello UI
    HMODULE hCredUI = LoadLibrary(L"credui.dll");
    if (!hCredUI)
    {
        UpdateStatus(L"CredUI not available");
        return false;
    }

    // Use CredUIPromptForWindowsCredentialsW to show the Windows Hello dialog
    typedef DWORD(WINAPI* PFN_CRED_UI_PROMPT_FOR_WINDOWS_CREDENTIALS)(
        PVOID pUiInfo,
        DWORD dwAuthError,
        PULONG pulAuthPackage,
        LPCVOID pvInAuthBuffer,
        ULONG ulInAuthBufferSize,
        LPVOID* ppvOutAuthBuffer,
        PULONG pulOutAuthBufferSize,
        PBOOL pfSave,
        DWORD dwFlags
    );

    PFN_CRED_UI_PROMPT_FOR_WINDOWS_CREDENTIALS pfnCredUIPrompt = 
        (PFN_CRED_UI_PROMPT_FOR_WINDOWS_CREDENTIALS)GetProcAddress(hCredUI, "CredUIPromptForWindowsCredentialsW");

    if (!pfnCredUIPrompt)
    {
        FreeLibrary(hCredUI);
        UpdateStatus(L"CredUI function not available");
        return false;
    }

    // Set up CREDUI_INFO structure
    struct CREDUI_INFOW {
        DWORD cbSize;
        HWND hwndParent;
        LPCWSTR pszMessageText;
        LPCWSTR pszCaptionText;
        HBITMAP hbmBanner;
    };

    CREDUI_INFOW credInfo = {};
    credInfo.cbSize = sizeof(CREDUI_INFOW);
    credInfo.hwndParent = m_hWnd;
    credInfo.pszMessageText = L"Please verify your identity using Windows Hello";
    credInfo.pszCaptionText = L"Passkey Authentication";
    credInfo.hbmBanner = nullptr;

    ULONG authPackage = 0;
    LPVOID outAuthBuffer = nullptr;
    ULONG outAuthBufferSize = 0;
    BOOL save = FALSE;

    // CREDUIWIN_GENERIC = 0x1, CREDUIWIN_IN_CRED_ONLY = 0x20
    // CREDUIWIN_ENUMERATE_ADMINS = 0x100
    // Try with different flags to get Windows Hello
    #define CREDUIWIN_GENERIC 0x1
    #define CREDUIWIN_CHECKBOX 0x2
    #define CREDUIWIN_AUTHPACKAGE_ONLY 0x10
    #define CREDUIWIN_IN_CRED_ONLY 0x20
    #define CREDUIWIN_ENUMERATE_CURRENT_USER 0x200
    #define CREDUIWIN_SECURE_PROMPT 0x1000
    #define CREDUIWIN_PREPROMPTING 0x2000
    #define CREDUIWIN_PACK_32_WOW 0x10000000

    UpdateStatus(L"Opening Windows Hello authentication...");

    DWORD result = pfnCredUIPrompt(
        &credInfo,
        0,
        &authPackage,
        nullptr,
        0,
        &outAuthBuffer,
        &outAuthBufferSize,
        &save,
        CREDUIWIN_GENERIC | CREDUIWIN_ENUMERATE_CURRENT_USER
    );

    // Free the output buffer if allocated
    if (outAuthBuffer)
    {
        CoTaskMemFree(outAuthBuffer);
    }

    FreeLibrary(hCredUI);

    if (result == 0) // ERROR_SUCCESS
    {
        UpdateStatus(L"Authentication successful!");
        return true;
    }
    else if (result == 1223) // ERROR_CANCELLED
    {
        UpdateStatus(L"Authentication cancelled by user");
        return false;
    }
    else
    {
        std::wstringstream ss;
        ss << L"Authentication failed (Error: " << result << L")";
        UpdateStatus(ss.str());
        return false;
    }
}
