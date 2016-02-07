#include "world.h"
#include "entity.h"

using namespace CS1972Engine;

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

void World::tick() {
    // Tick all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        (*it)->tick();
    }

    // Collide all entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        for (std::list<Entity *>::iterator ot = it; ot != m_entities.end(); ++ot) {
            if (it == ot) continue;
            (*it)->checkCollide(*ot);
        }
    }

    // Remove and delete entities
    for (std::list<Entity *>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        if ((*it)->m_removeFlag) {
            m_entities.erase(it);
            (*it)->m_parent = 0;
            if ((*it)->m_deleteFlag) delete *it;
        }
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
