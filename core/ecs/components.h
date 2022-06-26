/**
 * @author acf-patrick (miharisoap@gmail.com)
 * 
 * Declaration of some useful components
 */

#pragma once

#include <set>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <tileson/tileson.h>

#include "../util/geometry/vector.h"
#include "../event/event.h"

#include "baseScript.h"
#include "baseCamera.h"
#include "entity/entity.h"
#include "../texture/texture.h"
#include "../util/observable/observable.h"

class Group;

// Data structure in this namespace are fully maintained by the library
namespace Component
{

    // Identify entities with tag and IDs
    struct tag
    {
        std::string content = "";
        bool operator==(const std::string &str) const
        {
            return content == str;
        }

        tag(const std::string &str) : content(str)
        {
        }

        tag() {}
    };

    // Space specs
    struct transform
    {
        // top-left, SDL coordinates system
        Vector<double> position = {0.0f, 0.0f};

        // Scale factor
        // default : no scale, i.e (1, 1)
        VectorF scale = {1.0f, 1.0f};

        // Rotation angle in degrees
        double rotation = 0.0f;

        transform(const VectorD &_position, const VectorF &_scale, double _rotation) : position(_position),
                                                                                       scale(_scale),
                                                                                       rotation(_rotation)
        {
        }

        transform() {}
    };

    // Entity Container
    // Component containing the group that created the entity
    struct group
    {
        Group *content;

        group(Group *g) : content(g)
        {
        }
    };

    // Sprite renderer component
    struct sprite
    {
        // the texture
        Texture texture;

        // if true, the texture is centered
        // default : false (draw from top-left)
        bool centered = false;

        // texture's drawing offset
        // according to 'centered' flag
        // default : (0, 0)
        VectorI offset = {0, 0};

        // flip the sprite texture along the checked axis
        // default : (false, false)
        Vector<bool> flip = {false, false};

        // sprite number of frame. Both x and y should obviously be non zero
        // along x : columns, along y : row according to SDL coordinate system
        // default : (1, 1) consider the whole texture
        VectorI framesNumber = {1, 1};

        // current frame to display
        // default : 0, i.e first frame
        int frame = 0;

        // use x-th in row and y-th in column frame
        // matrix coordinates system
        void setFrame(int, int);

        // shortcut to :
        // sprite.texture.load(fileName);
        void setTexture(const std::string &);

        // use only a part of the texture
        // default : false (use the whole texture)
        bool regionEnabled = false;

        // texture region that will be displayed
        // if regionEnabled flag is on
        // default : (0, 0, 0, 0)
        SDL_Rect region = {0, 0, 0, 0};
    };

    class camera : public ICamera
    {
        void _attachTransform() override;

    public:
        // size of view
        // (1, 1) the whole screen
        Vector<float> size = {1, 1};

        // indicates where on the screen this camera view will be drawn.
        // (0, 0) top-left and (1, 1) bottom-right
        // default : (0, 0)
        Vector<float> destination = {0, 0};

        // color applied before any rendering
        // default : dark grey
        SDL_Color background = {14, 16, 18, 255};
        SDL_Texture *_colorTexture = nullptr;

        // stores used for background
        // update the 'clear' property after setting this variable
        Texture backgroundImage;

        enum ClearMode
        {
            NONE,                   // Don't clear the background
            TEXTURE,                // Clear the background with texture set in backgroundImage property
            SOLID_COLOR,            // Clear the background with color set in background property
            TEXTURE_AND_SOLID_COLOR // Useless if the texture doesn't support transparency
        };

        // clear mode
        // default : SOLID_COLOR (clear the background with Camera::background)
        ClearMode clear = SOLID_COLOR;

        // flip rendered view
        // default : false, false (no flip)
        Vector<bool> flip = {false, false};

        // camera position in draw orders
        // cameras with larger value will be drawn on top of cameras with a smaller value.
        // default : 0
        int depth = 0;

        // layer indexs the camera will draw
        // default : contains first index, i.e 0
        std::vector<int> layers = {0};

        struct _compare
        {
            bool operator()(camera *c1, camera *c2) const
            {
                return c1->depth < c2->depth;
            }
        };

        // Instances sorted by depth value
        static std::set<camera *, _compare> instances;

        camera();
        ~camera();
    };

    // Script Component
    class script : public BaseScript
    {
    protected:
        EventListner event;

    public:
        template <typename T>
        bool has() const
        {
            return entity->has<T>();
        }

        template <typename... T>
        bool all_of() const
        {
            return entity->all_of<T...>();
        }

        template <typename... T>
        bool any_of() const
        {
            return entity->any_of<T...>();
        }

        template <typename... T>
        bool none_of() const
        {
            return entity->none_of<T...>();
        }

        template <typename T>
        T &get()
        {
            return entity->get<T>();
        }

        template <typename... T>
        std::tuple<T &...> retrieve()
        {
            return entity->retrieve<T...>();
        }

        template <typename T, typename... TArgs>
        T &attach(TArgs &&...args)
        {
            return entity->attach<T>(std::forward<TArgs>(args)...);
        }

        template <typename T>
        void distach()
        {
            entity->distach<T>();
        }
    };

    class spriteRenderer : public script
    {
    public:
        virtual void Render() override;
    };

    /**
     * Load and render map created with Tiled.
     * - For physics and collision features create a object group
     *  layer within Tiled and add a boolean property named collider.
     * - Make Tilemap use your custom drawer by attaching your Tilemap::Drawer
     *  component to the entity.
    */
    class Tilemap : public script
    {
    public:
        // Component rendering object in a tilemap
        class Drawer : public script
        {
            static Observable< std::map<EntityID, Drawer *> > instances$;

        public:
            virtual ~Drawer();

            void onAttach() override;
            void onDistach() override;

            /**
         * Draw a layer image from tilemap
         * 
         * @param imgPath image file
         * @param position layer offset
         * @param renderer Renderer to be used
         */
            virtual void drawImage(const std::string &, const tson::Vector2f &, SDL_Renderer *);

            /**
         *  Draw an object from a tilemap
         *  
         * @param object the object to draw
         * @param renderer Renderer to be used
         */
            virtual void drawObject(tson::Object, SDL_Renderer *);

            /**
         * Draw an ellipse from a tilemap
         * 
         * @param rect bouding rect of the ellipse
         * @param renderer Renderer to be used
         */
            virtual void drawEllipse(const SDL_Rect &, SDL_Renderer *);

            /**
         * Draw a rectangle from a tilemap
         * 
         * @param rect the rectangle
         * @param renderer Renderer to be used
         */
            virtual void drawRectangle(const SDL_Rect &, SDL_Renderer *);

            /**
         * Draw a point indicator from a tilemap
         * 
         * @param point position of the point
         * @param renderer Renderer to be used
         */
            virtual void drawPoint(const tson::Vector2i &, SDL_Renderer *);

            /**
         * Draw a polygon from a tilemap
         * 
         * @param vertexes list of the polygon's vertexes
         * @param renderer Renderer to be used
         */
            virtual void drawPolygon(const std::vector<tson::Vector2i> &, SDL_Renderer *);

            /**
         * Draw a polyline from a tilemap
         * 
         * @param vertexes list of the polyline vertexes
         * @param renderer Renderer to be used
         */
            virtual void drawPolyline(const std::vector<tson::Vector2i> &, SDL_Renderer *);

            /**
         * Draw text from a tilemap
         * 
         * @param text data containing the string content, wrap and color of the text
         * @param position position of the
         * @param renderer Renderer to be used
         */
            virtual void drawText(const tson::Text &, const tson::Vector2i &, SDL_Renderer *);

            friend class Tilemap;
        };

    private:
        static tson::Tileson tileson;

        // Drawer used by this map
        Drawer *_drawer = nullptr;

        bool _defaultDrawer = false;

        // Map loaded by Tileson
        std::unique_ptr<tson::Map> _map;

        // Tile scale
        // Default : [1, 1]
        VectorF _scale = {1, 1};

        // Portion to draw
        // Rect in pixels coordinate system
        SDL_Rect* _viewport = nullptr;

        // Tileset textures used by tiles
        std::map< std::string, Texture > _tileTextures;

        // Draw a tile
        void _drawTile(tson::Tile&, int, int, SDL_Renderer*);

        // Draw a layer
        void _drawLayer(tson::Layer &, SDL_Renderer *);

    public:
        // File loaded
        const std::string file;

        // Parameter : file to load
        Tilemap(const std::string &);

        ~Tilemap();

        // Retrieve map data
        tson::Map &get();

        /**
         * Apply operation on each layers
         * 
         * @param process function called on each layer
         * @param layType layer type filter. Select all layers by default
         */
        void eachLayer(const std::function<void(tson::Layer &)> &, const std::vector<tson::LayerType> &layType = std::vector<tson::LayerType>());

        /**
         * Set viewport to be used on render
         * viewport in SDL coordinate system
         * 
         * @param viewport if nullptr, draw the entire map
         */
        void setViewport(SDL_Rect*);

        void Render() override;
    };

};
