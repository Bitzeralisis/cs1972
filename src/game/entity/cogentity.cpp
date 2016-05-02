#include "cogentity.h"
#include "game/coggame.h"

using namespace COG;

void COGEntity::tick(float seconds) {
    m_beat = seconds;
    if (m_beat > m_prevBeat) {
        tickBeats(m_beat - m_prevBeat);
        m_prevBeat = m_beat;
    }
}

void COGEntity::draw(int pass) {
    if (GAME->beat() >= m_firstBeat)
        draw(pass, GAME->beat());
}
