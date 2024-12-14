// BrowserLanucher.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include <Psapi.h>
#include "BrowserLauncher.h"
#include <vector>
#include <string>
#include <iostream>
#include <tlhelp32.h>
#include <format>
#include <regex>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

std::vector<std::wstring> GetProcessNames() {
    std::vector<std::wstring> processNames;

    // Take a snapshot of all processes in the system
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to create snapshot. Error: " << GetLastError() << std::endl;
        return processNames;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process
    if (Process32First(hSnapshot, &pe32)) {
        do {
            // Add the process name to the vector
            processNames.push_back(pe32.szExeFile);
        } while (Process32Next(hSnapshot, &pe32)); // Get the next process
    }
    else {
        std::wcerr << L"Failed to retrieve process information. Error: " << GetLastError() << std::endl;
    }

    // Close the snapshot handle
    CloseHandle(hSnapshot);

    return processNames;
}

void LaunchDetachedProcess(const std::wstring& programPath, std::wstring& arg)
{
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = { 0 };

    // Use DETACHED_PROCESS flag to detach from the console
    DWORD creationFlags = DETACHED_PROCESS;

    std::wstring arg_ = std::wstring(arg.begin(), arg.end());

    if (!CreateProcess(
        programPath.c_str(),  // Path to the program
        arg_.data(),             // Command-line arguments
        NULL,                 // Process handle not inheritable
        NULL,                 // Thread handle not inheritable
        FALSE,                // Handle inheritance flag
        creationFlags,        // Creation flags
        NULL,                 // Use parent's environment block
        NULL,                 // Use parent's starting directory
        &si,                  // Pointer to STARTUPINFO structure
        &pi))                 // Pointer to PROCESS_INFORMATION structure
    {
        std::wcerr << L"CreateProcess failed (" << GetLastError() << L")\n";
    }
    else
    {
        // Close process and thread handles as we don't need them
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (__argc <= 1) {
        return 1;
    }
    std::wstring url = __wargv[1];

    bool foundFirefox = false;
    bool foundChrome = false;
    auto processNames = GetProcessNames();
    for (const auto& name : processNames)
    {
        if (name.find(L"firefox") != std::wstring::npos)
        {
            foundFirefox = true;
        }
        if (name.find(L"chrome") != std::wstring::npos)
        {
            foundChrome = true;
        }
    }

    std::wstring chromeExe(L"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
    std::wstring firefoxExe(L"C:\\Program Files\\Mozilla Firefox\\firefox.exe");
    std::wstring arg = L"--new-tab " + url;
    if (foundFirefox)
        LaunchDetachedProcess(firefoxExe, arg);
    else
        LaunchDetachedProcess(chromeExe, arg);

    //int msgboxID = MessageBox(
    //    NULL,
    //    std::format(L"Firefox: {}, Chrome: {}, URL: {}", foundFirefox, foundChrome, url).c_str(),
    //    L"",
    //    MB_ICONEXCLAMATION | MB_YESNO
    //);

    //if (msgboxID == IDYES)
    //{
    //    // TODO: add code
    //}

    return 0;
}

