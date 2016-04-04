#include "world.h"
#include "terrainmanager.h"
#include "entity.h"

using namespace CS1972Engine;

World::World(Game *par)
    : m_parent(par)
{ }

World::~World() {
    if (m_terrain)
        delete m_terrain;

    if (m_deleteOnDeconstruct) {
        for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
            // Items that have been flagged for removal but not flagged for deletion are in
            // the caller's care, so only delete items that are not flagged for removal or
            // are flagged for removal and flagged to be deleted by us
            if (!(*it)->m_removeFlag || (*it)->m_deleteFlag)
                if ((*it)->m_deleteFlag) delete *it;
        }

        for (std::list<Entity *>::iterator it = m_addEntities.begin(); it != m_addEntities.end(); ++it) {
            // Do the same thing in m_addEntities
            if (!(*it)->m_removeFlag || (*it)->m_deleteFlag)
                if ((*it)->m_deleteFlag) delete *it;
        }
    }
}

void World::useTerrain(TerrainManager *terrain) {
    m_terrain = terrain;
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
    // Tick terrain
    if (m_terrain)
        m_terrain->tick(seconds);

    // Tick all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        (*it)->m_startPosition = (*it)->m_position;
        (*it)->tick(seconds);
    }

    // Collide all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        for (std::list<Entity *>::iterator ot = it; ot != m_entities.end(); ++ot) {
            if (it == ot) continue;
            if (!(*it)->m_collides || !(*ot)->m_collides) continue;
            (*it)->checkCollide(*ot);
        }
    }

    // Collide entities with terrain
    if (m_terrain) {
        for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
            if (!(*it)->m_collidesTerrain) continue;
            glm::vec3 pos0 = (*it)->m_startPosition;
            glm::vec3 pos1 = (*it)->m_position;
            glm::vec3 pos2, n;
            if (m_terrain->collideEntity(*it, pos0, pos1, pos2, n)) {
                (*it)->m_position = pos2;
                (*it)->collideTerrain(pos2-pos1, n);
            }
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

void World::draw(int pass) {
    // Draw terrain
    if (m_terrain)
        m_terrain->draw(pass);

    // Draw all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        (*it)->draw(pass);
    }
}
