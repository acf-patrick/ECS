/**
 * @author acf-patrick (miharisoap@gmail.com)
 * 
 * Definition of Texture
 */

#pragma once

#include <map>
#include <SDL2/SDL.h>

#include "../util/geometry/vector.h"

/**
 * Texture 
 *
 * Prevent memory leak and ensure texture reusability
 * by avoid multiple loading of the same texture
 */
class Texture
{
private:
    std::string _file = "";
    SDL_Texture* _texture = nullptr;

    static std::map<std::string, SDL_Texture*> _loadedTextures;

public:
    Texture() = default;
    Texture(const std::string&);

    ~Texture();

    // Make sure to unload left textures
    static void unload();

    void load(const std::string&);

    // Get filename used to load the texture
    std::string getName() const;

    // Get SDL_Texture* raw data
    SDL_Texture* get() const;

    // Return texture size
    VectorI getSize() const;

    // Set texture content
    // This method reset the texture's initial name
    void set(SDL_Texture*);

    // check if inner SDL Texture is null
    operator bool() const;

    Texture& operator=(const Texture& );

    void draw(const VectorI& dst);
    void draw(const VectorI& dst, const Vector<bool>& flip, const VectorF& scale);
    void draw(const SDL_Rect& src, const VectorI& dst, const Vector<bool>& flip, const VectorF& scale);
    void draw(const VectorI& dst, const VectorI& center, float rotation, const Vector<bool>& flip = { false, false }, const VectorF& scale = { 1.0f, 1.0f });
    void draw(const SDL_Rect& src, const VectorI& dst, const VectorI& center, float rotation, const Vector<bool>& flip = { false, false }, const VectorF& scale = { 1.0f, 1.0f });
};