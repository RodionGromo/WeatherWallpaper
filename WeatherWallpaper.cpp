// WeatherWallpaper.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "WallpaperGetter.h"
#include <csignal>


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

typedef struct AppState {
	SDL_Renderer* renderer;
	SDL_Window* window;
	WallpaperGetter* wallpaper;
} AppState
;
void* global_appstate;

static AppState* GetAppState(void* appstate) {
	return (AppState*)appstate;
}

BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		if (global_appstate) {
			AppState* state = GetAppState(global_appstate);
			if (state->renderer) {
				SDL_DestroyRenderer(state->renderer);
				state->renderer = NULL;
			}
			if (state->window) {
				SDL_DestroyWindow(state->window);
				state->window = NULL;
			}
			if (state->wallpaper) {
				state->wallpaper->ReturnHandle();
				delete state->wallpaper;
			}
			SDL_free(global_appstate);
		}
		SDL_Quit();
		exit(0);
		return TRUE;
	default:
		return FALSE;
	}
}
void SignalHandler(int signal) {
	switch (signal) {
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
		if (global_appstate) {
			AppState* state = GetAppState(global_appstate);
			if (state->renderer) {
				SDL_DestroyRenderer(state->renderer);
				state->renderer = NULL;
			}
			if (state->window) {
				SDL_DestroyWindow(state->window);
				state->window = NULL;
			}
			if (state->wallpaper) {
				state->wallpaper->ReturnHandle();
				delete state->wallpaper;
			}
			SDL_free(global_appstate);
		}
		SDL_Quit();
		exit(0);
		break;
	}
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char *argv[]) {
	SDL_SetAppMetadata("Test render", "1.0", "com.thegrom.weatherwallpaper");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Can't init SDL: %s", SDL_GetError());
	}

	WallpaperGetter* wpg = new WallpaperGetter();
	HWND window = wpg->GetHandle();
	if (!wpg->IsValid()) {
		SDL_Log("Can't create window!");
		delete wpg;
		return SDL_APP_FAILURE;
	}

	// since we are not creating a new process, only a drawing surface, we need to use CreateWindowWithProperties
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, window);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, GetSystemMetrics(SM_CXSCREEN));
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, GetSystemMetrics(SM_CYSCREEN));
	
	SDL_Window* wallpaper_window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);
	if(!wallpaper_window) {
		SDL_Log("Can't create window: %s", SDL_GetError());
		delete wpg;
		return SDL_APP_FAILURE;
	}

	// CreateWindowWithProperties does not create renderer, so here it is
	SDL_Renderer* renderer = SDL_CreateRenderer(wallpaper_window, NULL);
	if (!renderer) {
		SDL_Log("Can't create renderer: %s", SDL_GetError());
		delete wpg;
		return SDL_APP_FAILURE;
	}
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	// save data to appstate
	AppState* state = (AppState*)SDL_malloc(sizeof(AppState));
	state->renderer = renderer;
	state->wallpaper = wpg;
	state->window = wallpaper_window;
	*appstate = state;
	global_appstate = state;

	// sanity protection (from force-closes)
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);
	signal(SIGABRT, SignalHandler);
	
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	SDL_Renderer* renderer = GetAppState(appstate)->renderer;

	// converting from ms to s
	const double now = ((double)SDL_GetTicks()) / 1000;
	const float r = (float)(0.5 + 0.5 * SDL_sin(now));
	const float g = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2/3));
	const float b = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4/3));
	SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE_FLOAT);

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	if (appstate) {
		AppState* state = GetAppState(appstate);
		if (state->renderer) {
			SDL_DestroyRenderer(state->renderer);
			state->renderer = NULL;
		}
		if (state->window) {
			SDL_DestroyWindow(state->window);
			state->window = NULL;
		}
		if (state->wallpaper) {
			state->wallpaper->ReturnHandle();
			delete state->wallpaper;
		}
		SDL_free(appstate);
	}
	
	SDL_Quit();
}