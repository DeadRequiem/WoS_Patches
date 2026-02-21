#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "comctl32.lib")

#define ID_SELECT_FILE  1001
#define ID_PATCH_BUTTON 1002
#define ID_STATUS_TEXT  1003
#define ID_PROGRESS_BAR 1004
#define ID_OK_BUTTON    1005

HWND hMainWindow, hSelectButton, hPatchButton, hStatusText, hProgressBar, hOkButton;
HFONT hUIFont;
std::string selectedFile;

class DLLPatcher {
private:
    std::string filename;
    std::vector<char> fileData;

    const std::string ORIGINAL_URL = "http://www.synthetic-reality.com/synreal.ini";
    const std::string PATCHED_URL  = "http://www.reality-synthetic.com/synreal.ini";

public:
    DLLPatcher(const std::string& file) : filename(file) {}

    bool loadFile() {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        fileData.resize(fileSize);
        file.read(fileData.data(), fileSize);
        return true;
    }

    bool patchURL() {
        if (ORIGINAL_URL.length() != PATCHED_URL.length()) return false;

        auto it = std::search(fileData.begin(), fileData.end(),
                              ORIGINAL_URL.begin(), ORIGINAL_URL.end());

        if (it == fileData.end()) return false;

        std::copy(PATCHED_URL.begin(), PATCHED_URL.end(), it);
        return true;
    }

    bool saveFile() {
        // Backup first
        std::string backupName = filename + ".backup";
        std::ifstream src(filename, std::ios::binary);
        std::ofstream backup(backupName, std::ios::binary);
        backup << src.rdbuf();
        src.close();
        backup.close();

        std::ofstream file(filename, std::ios::binary);
        if (!file) return false;

        file.write(fileData.data(), fileData.size());
        return true;
    }
};

void UpdateStatus(const char* message) {
    SetWindowTextA(hStatusText, message);
    UpdateWindow(hStatusText);
}

void SetProgress(int percent) {
    SendMessage(hProgressBar, PBM_SETPOS, percent, 0);
    UpdateWindow(hProgressBar);
}

void SelectFile() {
    OPENFILENAMEA ofn;
    char szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hMainWindow;
    ofn.lpstrFile   = szFile;
    ofn.nMaxFile    = sizeof(szFile);
    ofn.lpstrFilter = "DLL Files\0*.dll\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        selectedFile = std::string(szFile);
        std::string displayName = selectedFile.substr(selectedFile.find_last_of("\\") + 1);
        std::string statusMsg = "Selected: " + displayName;
        UpdateStatus(statusMsg.c_str());
        EnableWindow(hPatchButton, TRUE);
    }
}

DWORD WINAPI PatchThread(LPVOID) {
    if (selectedFile.empty()) return 0;

    DLLPatcher patcher(selectedFile);

    UpdateStatus("Loading DLL...");
    SetProgress(25);

    if (!patcher.loadFile()) {
        UpdateStatus("Error: Failed to load file!");
        SetProgress(0);
        EnableWindow(hPatchButton, TRUE);
        return 0;
    }

    UpdateStatus("Searching and patching URL...");
    SetProgress(50);

    if (!patcher.patchURL()) {
        UpdateStatus("Error: URL string not found in file!");
        SetProgress(0);
        EnableWindow(hPatchButton, TRUE);
        return 0;
    }

    UpdateStatus("Creating backup and saving...");
    SetProgress(75);

    if (!patcher.saveFile()) {
        UpdateStatus("Error: Failed to save patched file!");
        SetProgress(0);
        EnableWindow(hPatchButton, TRUE);
        return 0;
    }

    SetProgress(100);
    UpdateStatus("Patching completed successfully! Backup created.");

    ShowWindow(hOkButton, SW_SHOW);
    EnableWindow(hOkButton, TRUE);

    return 0;
}

void StartPatching() {
    EnableWindow(hPatchButton, FALSE);
    SetProgress(0);

    DWORD threadId;
    CreateThread(NULL, 0, PatchThread, NULL, 0, &threadId);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC  = ICC_PROGRESS_CLASS;
        InitCommonControlsEx(&icex);

        hUIFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

        hSelectButton = CreateWindowA("BUTTON", "Select SRNet.dll",
                                      WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                      20, 20, 220, 30, hwnd,
                                      (HMENU)ID_SELECT_FILE,
                                      GetModuleHandle(NULL), NULL);
        SendMessage(hSelectButton, WM_SETFONT, (WPARAM)hUIFont, TRUE);

        hPatchButton = CreateWindowA("BUTTON", "Patch DLL",
                                     WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_DISABLED,
                                     260, 20, 100, 30, hwnd,
                                     (HMENU)ID_PATCH_BUTTON,
                                     GetModuleHandle(NULL), NULL);
        SendMessage(hPatchButton, WM_SETFONT, (WPARAM)hUIFont, TRUE);

        hStatusText = CreateWindowA("STATIC", "Ready - Select SRNet.dll to patch",
                                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                                    20, 70, 360, 25, hwnd,
                                    (HMENU)ID_STATUS_TEXT,
                                    GetModuleHandle(NULL), NULL);
        SendMessage(hStatusText, WM_SETFONT, (WPARAM)hUIFont, TRUE);

        hProgressBar = CreateWindowA("msctls_progress32", NULL,
                                     WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
                                     20, 110, 360, 20, hwnd,
                                     (HMENU)ID_PROGRESS_BAR,
                                     GetModuleHandle(NULL), NULL);
        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

        hOkButton = CreateWindowA("BUTTON", "OK",
                                  WS_CHILD | BS_DEFPUSHBUTTON,
                                  150, 140, 100, 30, hwnd,
                                  (HMENU)ID_OK_BUTTON,
                                  GetModuleHandle(NULL), NULL);
        SendMessage(hOkButton, WM_SETFONT, (WPARAM)hUIFont, TRUE);
        ShowWindow(hOkButton, SW_HIDE);

        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SELECT_FILE:  SelectFile();      break;
        case ID_PATCH_BUTTON: StartPatching();   break;
        case ID_OK_BUTTON:    PostQuitMessage(0); break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    if (__argc > 1) {
        DLLPatcher patcher(__argv[1]);
        if (!patcher.loadFile())  { printf("Failed to load file\n");  return 1; }
        if (!patcher.patchURL())  { printf("URL not found\n");        return 1; }
        if (!patcher.saveFile())  { printf("Failed to save file\n");  return 1; }
        printf("Patched successfully! Backup created.\n");
        return 0;
    }

    WNDCLASSA wc    = {};
    wc.lpfnWndProc  = WindowProc;
    wc.hInstance    = hInstance;
    wc.lpszClassName = "SRNetPatcherWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon        = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassA(&wc);

    hMainWindow = CreateWindowExA(
        0, "SRNetPatcherWindow", "SRNet.dll Patcher",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 220,
        NULL, NULL, hInstance, NULL);

    if (!hMainWindow) return 0;

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
