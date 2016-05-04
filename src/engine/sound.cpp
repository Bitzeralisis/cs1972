#include "sound.h"
#include "audio.h"

using namespace CS1972Engine;

Sound::Sound(Audio *parent, FMOD_SOUND *sound)
    : m_parent(parent)
    , m_sound(sound)
{ }

Sound::~Sound() {
    FMOD_Sound_Release(m_sound);
}

void Sound::setMusicParams(float bpm, float offset) {
    m_bpm = bpm;
    m_offset = offset;
}

void Sound::setLoop(bool loop) {
    FMOD_MODE mode;
    FMOD_Sound_GetMode(m_sound, &mode);
    mode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL);
    mode |= (loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
    FMOD_Sound_SetMode(m_sound, mode);
}

void Sound::setLoopBeats(float beat1, float beat2) {
    float freq;
    FMOD_Sound_GetDefaults(m_sound, &freq, 0);
    unsigned int startPcm = (unsigned int) (freq*m_offset) + (unsigned int) (beat1*60*freq/m_bpm);
    unsigned int endPcm = (unsigned int) (freq*m_offset) + (unsigned int) (beat2*60*freq/m_bpm) - 1U;
    FMOD_Sound_SetLoopPoints(m_sound, startPcm, FMOD_TIMEUNIT_PCM, endPcm, FMOD_TIMEUNIT_PCM);
    unsigned int start, end;
    FMOD_Sound_GetLoopPoints(m_sound, &start, FMOD_TIMEUNIT_PCM, &end, FMOD_TIMEUNIT_PCM);
}

void Sound::stop() const {
    FMOD_Channel_Stop(m_channel);
}
