#pragma once
#include <SDL3/SDL.h>
#include "Vectors.h"
#include <string>

class RenderEngine
{
private:
	SDL_Renderer* renderer;
	void LogError(std::string text);
	void LogInfo(std::string text);
public:
	RenderEngine(SDL_Renderer* renderer);
	// removes renderer pointer from this class to help with cleanup
	void ClearRenderer();
	// set color before using any of draw/fill functions
	bool SetColor(Vectors::RGB color);
	bool SetColor(int r, int g, int b);
	bool FillScreen();
	bool DrawLine(Vectors::Vector2 start, Vectors::Vector2 end);
	bool DrawRectangle(Vectors::Rect rect, bool filled);
	// clear the screen and show new frame
	bool Flip();
};

