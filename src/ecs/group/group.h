#pragma once

#include <functional>
#include <list>
#include <vector>

#include "../filter/filter.h"
#include "../defs.h"

class Scene;
class Entity;

// Entity Container
class Group {
   public:
    using _process = std::function<void(Entity&)>;
    using _predicate = std::function<bool(const Entity&)>;
    using _compare = std::function<bool(EntityID, EntityID)>;

    // return a list of entities conforming to predicate
    std::vector<Entity*> get(_predicate);

    // return a list of entites having required components
    std::vector<Entity*> view(const IFilter& filter);

    // retrieve by tag
    // return null pointer if not found
    Entity* operator[](const std::string&);

    // retrieve by ID
    // return null pointer if not found
    Entity* operator[](EntityID);

    // apply a callback to entities in this group
    void for_each(_process);

    // apply a callback to entities of this group, respecting
    // condition defined by the predicate function
    void for_each(_process, _predicate);

    // create an entity
    Entity& create();

    // create an entity and associate with an ID
    Entity& create(EntityID);

    // create an entity and attach a tag component to it
    Entity& create(const std::string&);

    // Remove Entity from this group without destroy
    void erase(EntityID);

    // reorder entities according to entity index
    void reorder();

    // reorder entites according to the comparator passed as argument
    void reorder(_compare);

   private:
    ~Group();

   private:
    std::list<EntityID> _ids;

    friend class Scene;
};
