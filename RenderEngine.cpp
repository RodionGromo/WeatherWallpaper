#include "RenderEngine.h"
#include <SDL3/SDL_render.h>
#include <iostream>

void RenderEngine::LogError(std::string text)
{
    std::cerr << "[RenderEngine Error] " << text << std::endl;
}

void RenderEngine::LogInfo(std::string text)
{
    std::cout << "[RenderEngine Info] " << text << std::endl;
}

RenderEngine::RenderEngine(SDL_Renderer* renderer)
{
    this->renderer = renderer;
}

void RenderEngine::ClearRenderer()
{
    this->renderer = NULL;
}

bool RenderEngine::SetColor(Vectors::RGB color)
{
    return SetColor(color.R(), color.G(), color.B());
}

bool RenderEngine::SetColor(int r, int g, int b)
{
    if (!this->renderer) return false;
    bool result = SDL_SetRenderDrawColor(this->renderer, r, g, b, SDL_ALPHA_OPAQUE);
    if (!result)
        LogError(("Failed to set color: " + (std::string)SDL_GetError()));
    return result;
}

bool RenderEngine::FillScreen()
{
    if (!this->renderer) return false;
    bool res = SDL_RenderClear(this->renderer);
    if (!res)
        LogError("Failed to clear screen: " + (std::string)SDL_GetError());
    return res;
}

bool RenderEngine::DrawLine(Vectors::Vector2 start, Vectors::Vector2 end)
{
    if (!this->renderer) return false;
    bool result = SDL_RenderLine(this->renderer, start.X(), start.Y(), end.X(), end.Y());
    if (!result)
        LogError("Failed to draw line: " + (std::string)SDL_GetError());
    return result;
}

bool RenderEngine::DrawRectangle(Vectors::Rect rect, bool filled)
{
    if (!this->renderer) return false;
    SDL_FRect sdl_rect = rect.getSDLRect();
    bool status;
    if (filled) 
    {
        status = SDL_RenderFillRect(this->renderer, &sdl_rect);
    }
    else
    {
        status = SDL_RenderRect(this->renderer, &sdl_rect);
    }
    if (!status)
        LogError("Failed to draw rect: " + (std::string)SDL_GetError());
    return status;
}

bool RenderEngine::Flip() {
    if (!this->renderer) return false;
    bool redrawScreen_res = SDL_RenderPresent(this->renderer);
    if (!redrawScreen_res)
        LogError("Failed to redraw screen: " + (std::string)SDL_GetError());
    return redrawScreen_res;
}
