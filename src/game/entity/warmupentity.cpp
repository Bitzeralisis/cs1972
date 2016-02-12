#include "warmupentity.h"

using namespace WarmupGame;

WarmupEntity::WarmupEntity(Type type)
    : m_type(type)
{ }

void WarmupEntity::collide(glm::vec3 mtv, const CS1972Engine::Entity *other) {
    collide(mtv, (WarmupEntity *) other);
}
