/**
 * @author acf-patrick (miharisoap@gmail.com)
 *
 * Contains classes for Scene management
 */

#pragma once

#include <deque>
#include <memory>
#include <string>

#include "../ecs/group/group.h"
#include "../event/event.h"
#include "../manager/manager.h"

class Application;
class Serializer;

// Scene Interface
class Scene {
   public:
    // Save to file
    void save(const std::string& fileName = "");

    // Merge given scene into this scene
    void merge(const Scene&);

    // Set this scene to be active
    void setActive();

    Group& getEntities();

   protected:
    Scene(const std::string&);
    virtual ~Scene() = default;

    // pop the scene from manager if this method return false
    virtual bool update();

    virtual void render();

    std::string tag = "Scene";

    // used to manage entities
    Group _entities;

    bool active = true;

    friend class Serializer;
    friend class SceneManager;
};

class EmptyScene : public Scene {};

class SceneManager : Manager<SceneManager> {
   public:
    // Load Scene from file
    void load(const std::string&);

    // Return active scene
    Scene& getActive();

    // Set scene at index to be active
    void setActive(std::size_t);

    // Set scene with the given name to be active
    void setActive(const std::string&);

    // Remove scene at index
    void remove(std::size_t);

    // Remove scene with the given name from SceneManager
    void remove(const std::string&);

    // Set next scene to be active
    void next();

    // No more scene left if return false
    bool update();

    // Render active scene
    void render();

    static std::shared_ptr<SceneManager> Get();

   private:
    std::deque<Scene*> scenes;

    SceneManager() = default;
    ~SceneManager();

    void push(Scene*);

    friend class Application;
    friend class Manager<SceneManager>;
    friend class Scene;
};