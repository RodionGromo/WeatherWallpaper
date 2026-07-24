#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <csignal>

#include "WallpaperGetter.h"
#include "Vectors.h"
#include "RenderEngine.h"

// thank you windows.h for this macro
#undef RGB

typedef struct AppState {
	SDL_Renderer* renderer;
	SDL_Window* window;
	WallpaperGetter* wallpaper;
	RenderEngine* rengine;
} AppState;
int screenX;
int screenY;

void* global_appstate;
Vectors::Vector2 pos = Vectors::Vector2(0, 500);
bool flip_mov = false;


static AppState* GetAppState(void* appstate) {
	return (AppState*)appstate;
}

static void cleanup() {
	if (global_appstate) {
		AppState* state = GetAppState(global_appstate);

		if (state->rengine) {
			state->rengine->ClearRenderer();
			delete state->rengine;
		}

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
}

static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		cleanup();
		SDL_Quit();
		exit(0);
		return TRUE;
	default:
		return FALSE;
	}
}
static void SignalHandler(int signal) {
	switch (signal) {
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
		cleanup();
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
	screenX = GetSystemMetrics(SM_CXSCREEN);
	screenY = GetSystemMetrics(SM_CYSCREEN);

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
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, screenX);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, screenY);
	
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
	//SDL_SetRenderLogicalPresentation(renderer, screenX, screenY, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	RenderEngine* rengine = new RenderEngine(renderer);

	// save data to appstate
	AppState* state = (AppState*)SDL_malloc(sizeof(AppState));
	state->renderer = renderer;
	state->wallpaper = wpg;
	state->window = wallpaper_window;
	state->rengine = rengine;
	*appstate = state;
	global_appstate = state;

	// sanity protection (from force-closes)
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);
	signal(SIGABRT, SignalHandler);
	
	// pre-render data
	pos.set(pos.X(), screenY / 2);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	RenderEngine* engine = GetAppState(appstate)->rengine;

	// +- 60 fps
	int tick = SDL_GetTicks() % 16;
	if (tick == 0) {
		if (!flip_mov) {
			pos.set(pos.X() + 1, pos.Y());
		}
		else
		{
			pos.set(pos.X() - 1, pos.Y());
		}

		if (pos.X() > 2460)
		{
			flip_mov = true;
		}
		else if (pos.X() < 240)
		{
			flip_mov = false;
		}
	}

	engine->SetColor(0, 0, 0);
	engine->FillScreen();

	engine->SetColor(255,255,255);
	Vectors::Rect rect = Vectors::Rect(pos, Vectors::Vector2(45, 45));
	engine->DrawRectangle(rect, true);

	engine->Flip();

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	if (appstate) {
		AppState* state = GetAppState(appstate);

		if (state->rengine) {
			state->rengine->ClearRenderer();
			delete state->rengine;
		}

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