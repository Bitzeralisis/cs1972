#pragma once

#include "../util/HeaderUtils.h"
#include <list>

namespace CS1972Engine {

class Entity;
class Game;
class TerrainManager;

class World {
public:
    explicit World(Game *parent);
    virtual ~World();

private:
    Game *const m_parent;

    TerrainManager *m_terrain = 0;

    bool m_deleteOnDeconstruct = false;
    std::list<Entity *> m_entities;
    std::list<Entity *> m_addEntities;

public:

public:
    VALUE_ACCESSOR(Game*,parent)

    VALUE_ACCESSOR(TerrainManager*,terrain)
    void useTerrain(TerrainManager *terrain);

    MUTATOR_DEEP(bool,deleteEntitiesOnDeconstruct,m_deleteOnDeconstruct)
    void addEntity(Entity *ent);
    void removeEntity(Entity *ent); // Just remove ent from the world
    void deleteEntity(Entity *ent); // Remove ent from the world and also deallocate it

    void tick(float seconds);
    void draw();
};

}
