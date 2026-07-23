#include "WallpaperGetter.h"

// I suppose you can find this function anywhere if you want to get handle of this magic window
static HWND FindWorkerW() {
    HWND progman = FindWindow(L"Progman", NULL);
    if (!progman) return NULL;

    SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);

    HWND workerw = NULL;
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
    HWND defview = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
        if (defview) {
            HWND* ret = (HWND*)lParam;
            *ret = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&workerw);

    return workerw;
}

void WallpaperGetter::ReturnHandle()
{
    // do NOT ask me what the hell is this, I don't know either
    // well I do know, it restores default wallpaper by reading it from registry
	if (WorkerW) {
        InvalidateRect(WorkerW, NULL, TRUE);
        UpdateWindow(WorkerW);

        wchar_t wallpaperPath[MAX_PATH] = { 0 };
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD size = sizeof(wallpaperPath);
            RegQueryValueExW(hKey, L"Wallpaper", NULL, NULL, (LPBYTE)wallpaperPath, &size);
            RegCloseKey(hKey);
        }

        if (wcslen(wallpaperPath) > 0) {
            SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        }

        SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);

        WorkerW = NULL;
	}
}

HWND WallpaperGetter::GetHandle() 
{
    WorkerW = FindWorkerW();
    if (!WorkerW) {
        std::cerr << "Can't find WorkerW!" << std::endl;
        return NULL;
    }
	return WorkerW;
}
