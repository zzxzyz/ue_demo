#include "MainWindow.h"
#include "PasskeyWindow.h"
#include "resource.h"
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

MainWindow::MainWindow() : m_hWnd(nullptr), m_hLoginButton(nullptr), m_hInstance(nullptr)
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::Create(HINSTANCE hInstance, int nCmdShow)
{
    m_hInstance = hInstance;

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
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"PasskeyDemoMainWindow";
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        return false;
    }

    // Create window
    m_hWnd = CreateWindowEx(
        0,
        L"PasskeyDemoMainWindow",
        L"Passkey Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (!m_hWnd)
    {
        return false;
    }

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    return true;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else
    {
        pThis = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate();
        return 0;

    case WM_COMMAND:
        OnCommand(wParam);
        return 0;

    case WM_DESTROY:
        OnDestroy();
        return 0;

    default:
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
}

void MainWindow::OnCreate()
{
    // Create login button
    m_hLoginButton = CreateWindow(
        L"BUTTON",
        L"Login with Passkey",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        120, 100,
        160, 40,
        m_hWnd,
        (HMENU)IDC_LOGIN_BUTTON,
        m_hInstance,
        nullptr
    );
}

void MainWindow::OnCommand(WPARAM wParam)
{
    if (LOWORD(wParam) == IDC_LOGIN_BUTTON)
    {
        // Show passkey authentication window
        PasskeyWindow passkeyWindow;
        passkeyWindow.ShowModal(m_hInstance, m_hWnd);
    }
}

void MainWindow::OnDestroy()
{
    PostQuitMessage(0);
}
