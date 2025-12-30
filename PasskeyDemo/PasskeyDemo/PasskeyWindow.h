#pragma once

#include <windows.h>
#include <string>

// Forward declarations for WebAuthn types (if webauthn.h is not available)
#ifndef WEBAUTHN_H
typedef enum WEBAUTHN_AUTHENTICATOR_ATTACHMENT {
    WEBAUTHN_AUTHENTICATOR_ATTACHMENT_ANY = 0,
    WEBAUTHN_AUTHENTICATOR_ATTACHMENT_PLATFORM = 1,
    WEBAUTHN_AUTHENTICATOR_ATTACHMENT_CROSS_PLATFORM = 2
} WEBAUTHN_AUTHENTICATOR_ATTACHMENT;

typedef enum WEBAUTHN_USER_VERIFICATION_REQUIREMENT {
    WEBAUTHN_USER_VERIFICATION_REQUIREMENT_ANY = 0,
    WEBAUTHN_USER_VERIFICATION_REQUIREMENT_REQUIRED = 1,
    WEBAUTHN_USER_VERIFICATION_REQUIREMENT_PREFERRED = 2,
    WEBAUTHN_USER_VERIFICATION_REQUIREMENT_DISCOURAGED = 3
} WEBAUTHN_USER_VERIFICATION_REQUIREMENT;

#define WEBAUTHN_CLIENT_DATA_CURRENT_VERSION 1
#define WEBAUTHN_CREDENTIAL_CURRENT_VERSION 1
#define WEBAUTHN_GET_ASSERTION_OPTIONS_CURRENT_VERSION 1
#define WEBAUTHN_HASH_ALGORITHM_SHA_256 L"SHA-256"

typedef struct WEBAUTHN_CLIENT_DATA {
    DWORD dwVersion;
    DWORD cbClientDataJSON;
    PBYTE pbClientDataJSON;
    LPCWSTR pwszHashAlgId;
} WEBAUTHN_CLIENT_DATA, *PWEBAUTHN_CLIENT_DATA;
typedef const WEBAUTHN_CLIENT_DATA *PCWEBAUTHN_CLIENT_DATA;

typedef struct WEBAUTHN_CREDENTIAL {
    DWORD dwVersion;
    DWORD cbId;
    PBYTE pbId;
    LPCWSTR pwszCredentialType;
} WEBAUTHN_CREDENTIAL, *PWEBAUTHN_CREDENTIAL;
typedef const WEBAUTHN_CREDENTIAL *PCWEBAUTHN_CREDENTIAL;

typedef struct WEBAUTHN_CREDENTIALS {
    DWORD cCredentials;
    PWEBAUTHN_CREDENTIAL pCredentials;
} WEBAUTHN_CREDENTIALS, *PWEBAUTHN_CREDENTIALS;
typedef const WEBAUTHN_CREDENTIALS *PCWEBAUTHN_CREDENTIALS;

typedef struct WEBAUTHN_EXTENSION {
    LPCWSTR pwszExtensionIdentifier;
    DWORD cbExtension;
    PBYTE pbExtension;
    BOOL bIsCritical;
} WEBAUTHN_EXTENSION, *PWEBAUTHN_EXTENSION;
typedef const WEBAUTHN_EXTENSION *PCWEBAUTHN_EXTENSION;

typedef struct WEBAUTHN_EXTENSIONS {
    DWORD cExtensions;
    PCWEBAUTHN_EXTENSION pExtensions;
} WEBAUTHN_EXTENSIONS, *PWEBAUTHN_EXTENSIONS;
typedef const WEBAUTHN_EXTENSIONS *PCWEBAUTHN_EXTENSIONS;

typedef struct WEBAUTHN_GET_ASSERTION_OPTIONS {
    DWORD dwVersion;
    DWORD dwTimeoutMs;
    WEBAUTHN_CREDENTIALS CredentialList;
    WEBAUTHN_EXTENSIONS Extensions;
    DWORD dwAuthenticatorAttachment;
    DWORD dwUserVerificationRequirement;
    DWORD dwFlags;
} WEBAUTHN_GET_ASSERTION_OPTIONS, *PWEBAUTHN_GET_ASSERTION_OPTIONS;
typedef const WEBAUTHN_GET_ASSERTION_OPTIONS *PCWEBAUTHN_GET_ASSERTION_OPTIONS;

typedef struct WEBAUTHN_ASSERTION {
    DWORD dwVersion;
    DWORD cbAuthenticatorData;
    PBYTE pbAuthenticatorData;
    DWORD cbSignature;
    PBYTE pbSignature;
    WEBAUTHN_CREDENTIAL Credential;
    DWORD cbUserId;
    PBYTE pbUserId;
} WEBAUTHN_ASSERTION, *PWEBAUTHN_ASSERTION;
#endif

class PasskeyWindow
{
public:
    PasskeyWindow();
    ~PasskeyWindow();

    void ShowModal(HINSTANCE hInstance, HWND hParent);

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
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
