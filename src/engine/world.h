#pragma once

#include <list>

namespace CS1972Engine {

class Entity;
class Game;

class World {
public:
    explicit World(Game *par) : parent(par) {}
    virtual ~World() {}

private:
    std::list<Entity *> m_entities;
    std::list<Entity *> m_addEntities;

public:
    Game *const parent;

public:
    void addEntity(Entity *ent);
    void removeEntity(Entity *ent); // Just remove ent from the world
    void deleteEntity(Entity *ent); // Remove ent from the world and also deallocate it

    void tick();
    void draw();
};

}
