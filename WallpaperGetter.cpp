#include "WallpaperGetter.h"

static LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

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

static HWND createWindow() {
	HWND m_worker = FindWorkerW();
	if (!m_worker) {
		std::cerr << "[WPG] Can't find WorkerW: " << std::endl;
		return NULL;
	}
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MyWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = L"WeatherWallpaperWindow";

	if (!RegisterClassEx(&wc)) {
		std::cerr << "[WPG] Can't register class!" << std::endl;
		return NULL;
	}
	int screenW = GetSystemMetrics(SM_CXSCREEN);
	int screenH = GetSystemMetrics(SM_CYSCREEN);
	HWND app_window = CreateWindowEx(
		WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		L"WeatherWallpaperWindow",
		L"Wallpaper Renderer",
		WS_CHILD | WS_VISIBLE,
		0, 0, screenW, screenH,
		m_worker, NULL, GetModuleHandle(NULL), NULL
	);

	if (!app_window) {
		std::cerr << "[WPG] Can't create window! " << GetLastError() << std::endl;
		return NULL;
	}
	SetWindowLong(app_window, GWL_EXSTYLE,
		GetWindowLong(app_window, GWL_EXSTYLE | WS_EX_LAYERED | WS_EX_TRANSPARENT));
	SetLayeredWindowAttributes(app_window, 0, 255, LWA_ALPHA);
	return app_window;
}

void WallpaperGetter::ReturnHandle()
{
	if (AppWindow) {
		DestroyWindow(AppWindow);
		AppWindow = NULL;
	}
}

HWND WallpaperGetter::GetHandle()
{
	AppWindow = createWindow();
	return AppWindow;
}
