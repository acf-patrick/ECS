/**
 * @author acf-patrick (miharisoap@gmail.com)
 * 
 * Define application main class
 */

#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "../renderer/renderer.h"
#include "../serializer/serializer.h"

class Application final
{
public:
    ~Application();

    void run();

    void quit();
    void log(const std::string&) const;

    static Application& Get();

    static Serializer* serializer;

private:
    Application(const std::string&, int, int, SDL_WindowFlags windowFlag = SDL_WINDOW_SHOWN);

    bool _running = true;
    SDL_Window* _window;

    static Application* instance;

friend int main(int, char**);
};
