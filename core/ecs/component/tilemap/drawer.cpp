#include "../../components.h"

using Drawer = Component::Tilemap::Drawer;

Observable< std::map<EntityID, Drawer*> > Drawer::instances$;

void Drawer::onAttach()
{
    instances$.next([&](auto& instances)
    {
        instances[entity->id()] = this;
    });
}

void Drawer::onDistach()
{
    instances$.next([&](auto& instances)
    {
        instances.erase(entity->id());
    });
}

Drawer::~Drawer()
{
    onDistach();
}

void Drawer::drawImage(const std::string& imgPath, const tson::Vector2f& position, SDL_Renderer* renderer)
{

}

void Drawer::drawObject(tson::Object object, SDL_Renderer* renderer)
{

}

void Drawer::drawEllipse(const SDL_Rect& rect, SDL_Renderer* renderer)
{

}

void Drawer::drawPoint(const tson::Vector2i& position, SDL_Renderer* renderer)
{

}

void Drawer::drawRectangle(const SDL_Rect& rect, SDL_Renderer* renderer)
{

}

void Drawer::drawPolygon(const std::vector< tson::Vector2i >& vertexes, SDL_Renderer* renderer)
{

}

void Drawer::drawPolyline(const std::vector< tson::Vector2i >& vertexes, SDL_Renderer* renderer)
{

}

void Drawer::drawText(const tson::Text& text, const tson::Vector2i& position, SDL_Renderer* renderer)
{

}