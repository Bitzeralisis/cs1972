#include "world.h"
#include "entity.h"

using namespace CS1972Engine;

World::World(Game *par)
    : parent(par)
{ }

World::~World() {
    if (m_deleteOnDeconstruct) {
        for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
            // Items that have been flagged for removal but not flagged for deletion are in
            // the caller's care, so only delete items that are not flagged for removal or
            // are flagged for removal and flagged to be deleted by us
            if (!(*it)->m_removeFlag || (*it)->m_deleteFlag)
                if ((*it)->m_deleteFlag) delete *it;
        }
    }
}

void World::addEntity(Entity *ent) {
    m_addEntities.push_back(ent);
}

void World::removeEntity(Entity *ent) {
    ent->m_removeFlag = true;
}

void World::deleteEntity(Entity *ent) {
    ent->m_removeFlag = true;
    ent->m_deleteFlag = true;
}

void World::tick(float seconds) {
    // Tick all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        (*it)->tick(seconds);
    }

    // Collide all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        for (std::list<Entity *>::iterator ot = it; ot != m_entities.end(); ++ot) {
            if (it == ot) continue;
            (*it)->checkCollide(*ot);
        }
    }

    // Remove and delete entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ) {
        if ((*it)->m_removeFlag) {
            Entity *item = *it;
            item->m_parent = 0;
            if (item->m_deleteFlag) delete item;
            it = m_entities.erase(it);
        } else
            ++it;
    }

    // Add entities
    for (std::list<Entity *>::iterator it = m_addEntities.begin(); it != m_addEntities.end(); ++it) {
        m_entities.push_back(*it);
        (*it)->m_parent = this;
    }
    m_addEntities.clear();
}

void World::draw() {
    // Draw all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        (*it)->draw();
    }
}
