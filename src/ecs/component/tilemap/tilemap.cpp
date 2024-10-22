#include <algorithm>

#include "../../../logger/logger.h"
#include "../../../renderer/renderer.h"
#include "../../components.h"

namespace entix {
namespace ecs {
namespace component {

tson::Tileson Tilemap::tileson;

namespace fs = std::filesystem;

Tilemap::Tilemap(const Path &path) {
    _map = tileson.parse(path);

    if (_map->getStatus() == tson::ParseStatus::OK) {
        _source = (fs::path)path;
        loadTilesets(fs::path(path).parent_path());
        Logger::info("Component", "Tilemap") << path << " loaded";
    } else
        Logger::error("Component", "Tilemap")
            << "Tilemap-error : " << _map->getStatusMessage();

    Logger::endline();
}

Tilemap::~Tilemap() {
    // if this tilemap uses the default drawer,
    // the 'drawer' instance was not registered into component manager
    // hence, must be destroyed manually
    if (_defaultDrawer) delete _drawer;
}

void Tilemap::Render() {
    // Creating default Drawer if no Tilemap::Drawer component added
    auto eID = entity->id();
    if (auto drawer = Drawer::instances[eID]; drawer) {
        _drawer = drawer;
    } else {
        _defaultDrawer = true;
        if (!_drawer) _drawer = new Drawer;
    }

    core::RenderManager::Get()->submit([&](SDL_Renderer *renderer) {
        eachLayer([&](tson::Layer &layer) { drawLayer(layer, renderer); });
    });
}

void Tilemap::eachLayer(const std::function<void(tson::Layer &)> &process,
                        const std::vector<tson::LayerType> &layType) {
    for (auto &layer : _map->getLayers())
        if (layType.empty() or std::find(layType.begin(), layType.end(),
                                         layer.getType()) != layType.end())
            process(layer);
}

void Tilemap::loadTilesets(const fs::path &mapFolder) {
    for (auto &tileset : _map->getTilesets()) {
        auto imagePath = tileset.getImagePath();
        _textures.try_emplace(imagePath.string(), mapFolder / imagePath);
    }
}

void Tilemap::drawLayer(tson::Layer &layer, SDL_Renderer *renderer) {
    using type = tson::LayerType;
    switch (layer.getType()) {
        case type::Group:
            for (auto &lay : layer.getLayers()) drawLayer(lay, renderer);
            break;

        case type::ImageLayer:
            _drawer->drawImage(layer.getImage(), layer.getOffset(), renderer);
            break;

        case type::TileLayer:
            for (auto &[pos, tile] : layer.getTileData()) {
                auto tileset = tile->getTileset();
                auto tileSize = _map->getTileSize();
                auto &texture = _textures[tileset->getImagePath().string()];

                auto tileId = tile->getId() - tileset->getFirstgid();
                SDL_Rect src = {int(tileId % (uint32_t)tileset->getColumns() *
                                    (uint32_t)tileSize.x),
                                int(tileId / (uint32_t)tileset->getColumns() *
                                    (uint32_t)tileSize.y),
                                tileSize.x, tileSize.y};

                VectorI tilePosition(tileSize.x * std::get<0>(pos),
                                     tileSize.y * std::get<1>(pos));

                texture.draw(src, tilePosition, Vector(false, false),
                             Vector(1.0, 1.0));
            }
            break;

        case type::ObjectGroup:
            for (auto &object : layer.getObjects()) {
                auto objPos = object.getPosition();
                auto objSize = object.getSize();
                SDL_Rect objBoundingRect = {objPos.x, objPos.y, objSize.x,
                                            objSize.y};

                switch (object.getObjectType()) {
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
            break;

        default:;
    }
}

std::string Tilemap::getSource() const { return _source.string(); }

}  // namespace component
}  // namespace ecs
}  // namespace entix
