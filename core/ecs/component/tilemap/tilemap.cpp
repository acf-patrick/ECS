#include "../../components.h"
#include "../../../renderer/renderer.h"

#include <algorithm>

namespace Component
{

    tson::Tileson Tilemap::tileson;

    Tilemap::Tilemap(const std::string &rsc) : file(rsc)
    {
        _map = tileson.parse(fs::path(rsc));
        if (_map->getStatus() == tson::ParseStatus::OK)
        {
        }
        else
            std::cerr << "Tilemap-error : " << _map->getStatusMessage() << std::endl;
    }

    Tilemap::~Tilemap()
    {
        // if this tilemap use the default drawer,
        // the 'drawer' instance was not registered into component manager
        // hence, must be destroyed manually
        if (_defaultDrawer)
            delete _drawer;
    }

    void Tilemap::Render()
    {
        // Creating default Drawer if no Tilemap::Drawer component added
        auto eID = entity->id();
        _drawer = Drawer::instances[eID];
        if (!_drawer)
        {
            _defaultDrawer = true;
            _drawer = new Drawer;
        }

        Renderer->submit([&](SDL_Renderer *renderer)
                         { eachLayer([&](tson::Layer &layer)
                                     { _drawLayer(layer, renderer); }); });
    }

    void Tilemap::eachLayer(const std::function<void(tson::Layer &)> &process, const std::vector<tson::LayerType> &layType)
    {
        for (auto &layer : _map->getLayers())
            if (layType.empty() or std::find(layType.begin(), layType.end(), layer.getType()) != layType.end())
                process(layer);
    }

    void Tilemap::_drawLayer(tson::Layer& layer, SDL_Renderer* renderer)
    {
        using type = tson::LayerType;
        switch (layer.getType())
        {
        case type::Group:
            for (auto& lay: layer.getLayers())
                _drawLayer(lay, renderer);
            break;

        case type::ImageLayer:
            _drawer->drawImage(layer.getImage(), layer.getOffset(), renderer);
            break;

        case type::ObjectGroup:
            for (auto& object: layer.getObjects())
            {
                auto objPos = object.getPosition();
                auto objSize = object.getSize();
                SDL_Rect objBoundingRect = { objPos.x, objPos.y, objSize.x, objSize.y };

                switch(object.getObjectType())
                {                
                case tson::ObjectType::Ellipse:
                    _drawer->drawEllipse(objBoundingRect, renderer);
                    break;

                case tson::ObjectType::Point:
                    _drawer->drawPoint(objPos, renderer);
                    break;

                case tson::ObjectType::Polygon:
                    _drawer->drawPolygon(object.getPolygons(), renderer);
                    break;

                case tson::ObjectType::Polyline:
                    _drawer->drawPolyline(object.getPolylines(), renderer);
                    break;
                    
                case tson::ObjectType::Rectangle:
                    _drawer->drawRectangle(objBoundingRect, renderer);
                    break;

                case tson::ObjectType::Text:
                    _drawer->drawText(object.getText(), objPos, renderer);
                    break;

                default:
                    _drawer->drawObject(object, renderer);
                    break;
                }
            }
        }
    }

};