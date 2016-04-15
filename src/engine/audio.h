#pragma once

#include "../util/CommonIncludes.h"
#include "../util/HeaderUtils.h"

namespace CS1972Engine {

class Sound;

class Audio {
public:
    Audio();
    ~Audio();

private:
    FMOD_SYSTEM *m_fmod;
    int m_sampleRate;

    unsigned long long m_bgmOffset = 0ULL;
    Sound *m_bgm = 0;

public:
    VALUE_ACCESSOR(FMOD_SYSTEM *,fmod)
    float getBeat();

    void tick(float seconds);

    Sound *createSound(const char *path, int flags);
    Sound *createSoundSample(const char *path);
    Sound *createSoundStream(const char *path);

    void playSound(Sound *sound);
    void queueSoundOnBeat(Sound *sound, float beat);
    void queueBgmOnBeat(Sound *sound, float beat);
};

}
