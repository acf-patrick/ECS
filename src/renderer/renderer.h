/**
 * @author acf-patrick (miharisoap@gmail.com)
 *
 * The rendering system
 */

#pragma once

#include <SDL.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <string>

#include "../manager/manager.h"
#include "../ecs/components.h"

class Application;

// Rendering System
class RenderManager: Manager<RenderManager> {
   public:
    using Camera = Component::camera;
    using Process = std::function<void(SDL_Renderer*)>;
    
    struct Drawer {
        std::shared_ptr<RenderManager> renderManager = RenderManager::Get();
        std::queue<Process> process;
        SDL_Texture* target = nullptr;

        SDL_Renderer* renderer = renderManager->renderer;
        int currentTarget = 0;

        Drawer() {
            auto s = renderManager->getSize();
            target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, s.x, s.y);
            assert(target && "Unable to create texture target for layer\n");
            SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
        }

        ~Drawer() {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_DestroyTexture(target);
        }

        void add(const Process& p) { process.push(p); }

        void clear() {
            std::queue<Process> empty;
            std::swap(empty, process);
        }

        void prepare() {
            SDL_SetRenderTarget(renderer, target);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, target);
        }

        void operator()() {
            while (!process.empty()) {
                process.front()(renderer);
                process.pop();
            }
        }
    };

    static std::shared_ptr<RenderManager> Get();

    // clear a portion of the screen with the given color, default is black.
    void clear(const SDL_Color& color = {0, 0, 0, 255});

    // clear a portion of the screen with the given color, default is black.
    void clear(const SDL_Rect&, const SDL_Color& color = {0, 0, 0, 255});

    // perform drawing
    void draw();

    // use n-th layer to perform drawing
    // default : first layer (index 0)
    void submit(const Process&, std::size_t index = 0);

    VectorI getSize() const;

    VectorI globalCoordinates(float, float) const;

    VectorI globalCoordinates(const VectorF&) const;

    VectorF viewportCoordinates(int, int) const;

    VectorF viewportCoordinates(const VectorI&) const;

    SDL_Renderer* renderer;

   private:
    // SDL_Texture* view;

    // There is always one layer remaining
    std::map<int, Drawer> layers;

    RenderManager();
    ~RenderManager();

    friend class Application;
    friend class Manager<RenderManager>;
};