#pragma once

#include "util/CommonIncludes.h"
#include "util/HeaderUtils.h"
#include <unordered_map>

namespace CS1972Engine {

class Sound;

class Audio {
public:
    Audio();
    ~Audio();

private:
    FMOD_SYSTEM *m_fmod;
    int m_sampleRate;

    std::unordered_map<std::string, Sound *> m_sounds;

    unsigned long long m_bgmOffset = 0ULL;
    Sound *m_bgm = 0;

public:
    VALUE_ACCESSOR(FMOD_SYSTEM *,fmod)
    VALUE_ACCESSOR(Sound *,bgm)
    float getBeat();

    void tick(float seconds);

    Sound *createSound(const char *path, int flags);
    Sound *createSoundSample(const char *path);
    Sound *createSoundStream(const char *path);
    MAP_OPS(Sound *,Sound,sound)

    inline void clearBgm() { m_bgm = 0; }
    void playSound(Sound *sound);
    void playSound(const char *sound);
    void queueSoundOnBeat(Sound *sound, float beat);
    void queueSoundOnBeat(const char *sound, float beat);
    void queueBgmOnBeat(Sound *sound, float beat);
};

}
