#pragma once

#include "util/CommonIncludes.h"
#include "util/HeaderUtils.h"

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
    FMOD_CHANNEL *m_channel = 0;

    float m_bpm;
    float m_offset = 0.f;

public:
    VALUE_ACCESSOR(float,bpm)

    void setMusicParams(float bpm, float offset);
    void setLoop(bool loop);
    void setLoopBeats(float beat1, float beat2);
    void stop();
};

}
