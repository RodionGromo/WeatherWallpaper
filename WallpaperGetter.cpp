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
    WorkerW = NULL;
    // I hate this, but it is working
    // straight up kill explorer to restore wallpaper, and no, I do not care for your tabs
    system("taskkill /f /im explorer.exe >nul 2>&1");
    system("start explorer.exe");
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
