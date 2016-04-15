#pragma once

#include "util/CommonIncludes.h"

namespace CS1972Engine {

class Audio;

class Sound {
    friend class Audio;

private:
    Sound(Audio *parent, FMOD_SOUND *sound);

public:
    ~Sound();

private:
    Audio *m_parent;
    FMOD_SOUND *m_sound;
    FMOD_CHANNEL *m_channel;

    float m_bpm;
    float m_offset = 0.f;

public:
    void setMusicParams(float bpm, float offset);
    void setLoop(bool loop);
    void setLoopBeats(float beat1, float beat2);
};

}
