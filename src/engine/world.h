#pragma once

#include "../util/HeaderUtils.h"
#include <list>

namespace CS1972Engine {

class Entity;
class Game;

class World {
public:
    explicit World(Game *parent);
    virtual ~World();

private:
    bool m_deleteOnDeconstruct = false;
    std::list<Entity *> m_entities;
    std::list<Entity *> m_addEntities;

public:
    Game *const parent;

public:
    MUTATOR_DEEP(bool,deleteEntitiesOnDeconstruct,m_deleteOnDeconstruct)

    void addEntity(Entity *ent);
    void removeEntity(Entity *ent); // Just remove ent from the world
    void deleteEntity(Entity *ent); // Remove ent from the world and also deallocate it

    void tick(float seconds);
    void draw();
};

}
