// WeatherWallpaper.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SDL_MAIN_USE_CALLBACKS 1

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char *argv[]) {
	SDL_SetAppMetadata("Test render", "1.0", "com.thegrom.weatherwallpaper");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Can't init SDL: %s", SDL_GetError());
	}

	if (!SDL_CreateWindowAndRenderer("test", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log
	}
}
