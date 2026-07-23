// WeatherWallpaper.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "WallpaperGetter.h"


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

typedef struct AppState {
	SDL_Renderer* renderer;
	SDL_Window* window;
	WallpaperGetter* wallpaper;
} AppState;

static AppState* GetAppState(void* appstate) {
	return (AppState*)appstate;
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

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, window);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, GetSystemMetrics(SM_CXSCREEN));
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, GetSystemMetrics(SM_CYSCREEN));
	
	SDL_Window* wallpaper_window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);
	if(!wallpaper_window) {
		SDL_Log("Can't grab window: %s", SDL_GetError());
		delete wpg;
		return SDL_APP_FAILURE;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(wallpaper_window, NULL);
	if (!renderer) {
		SDL_Log("Can't create renderer: %s", SDL_GetError());
		delete wpg;
		return SDL_APP_FAILURE;
	}

	AppState* state = (AppState*)SDL_malloc(sizeof(AppState));
	state->renderer = renderer;
	state->wallpaper = wpg;
	state->window = wallpaper_window;
	*appstate = state;

	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	// pointer assist: appstate is cast into double pointer SDL_Renderer, and is being deref'ed to pointer to SDL_Renderer
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
		}
		if (state->window) {
			SDL_DestroyWindow(state->window);
		}

		WallpaperGetter wp = *(state->wallpaper);
		wp.ReturnHandle();
		delete& wp;
		delete state->wallpaper;
		SDL_free(appstate);
	}
	
	SDL_Quit();
}