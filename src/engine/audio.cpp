#include "audio.h"
#include "sound.h"

using namespace CS1972Engine;

Audio::Audio() {
    FMOD_RESULT err;
    err = FMOD_System_Create(&m_fmod);
    if (err != FMOD_OK) fprintf(stderr, "Error initializing FMOD: %i\n", err);
    FMOD_System_GetSoftwareFormat(m_fmod, &m_sampleRate, 0, 0);
    err = FMOD_System_Init(m_fmod, 32, FMOD_INIT_NORMAL, 0);
    if (err != FMOD_OK) fprintf(stderr, "Error initializing FMOD: %i\n", err);
}

Audio::~Audio() {
    FMOD_System_Release(m_fmod);
}

float Audio::getBeat() {
    if (m_bgm == 0)
        return 0.f;

    unsigned long long dsp;
    FMOD_Channel_GetDSPClock(m_bgm->m_channel, 0, &dsp);
    return (dsp - m_bgmOffset) / (60 * m_sampleRate / m_bgm->m_bpm);
}

void Audio::tick(float) {
    FMOD_System_Update(m_fmod);
}

Sound *Audio::createSound(const char *path, int flags) {
    FMOD_SOUND *sound;
    FMOD_System_CreateSound(m_fmod, path, flags, 0, &sound);
    return new Sound(this, sound);
}

Sound *Audio::createSoundSample(const char *path) {
    return createSound(path, FMOD_2D | FMOD_CREATESAMPLE);
}

Sound *Audio::createSoundStream(const char *path) {
    return createSound(path, FMOD_2D | FMOD_CREATESTREAM);
}

void Audio::playSound(Sound *sound) {
    FMOD_System_PlaySound(m_fmod, sound->m_sound, 0, false, &sound->m_channel);
}

void Audio::queueSoundOnBeat(Sound *sound, float beat) {
    // Have `sound` start at a time such that its start offset matches up with `beat` in the current bgm
    if (m_bgm == 0) {
        playSound(sound);
    } else {
        unsigned long long soundStartOffsetDSP = m_sampleRate * sound->m_offset;
        unsigned long long beatMasterDSP =  m_bgmOffset + (unsigned long long) (beat * (60.f * m_sampleRate / m_bgm->m_bpm));
        FMOD_System_PlaySound(m_fmod, sound->m_sound, 0, true, &sound->m_channel);
        FMOD_Channel_SetDelay(sound->m_channel, beatMasterDSP - soundStartOffsetDSP, 0, true);
        FMOD_Channel_SetPaused(sound->m_channel, false);
    }
}

void Audio::queueBgmOnBeat(Sound *sound, float beat) {
    // Have `sound` start at a time such that its start offset matches up with `beat` in the current bgm, then set `sound` as the new bgm
    if (m_bgm == 0) {
        playSound(sound);
        m_bgm = sound;

        unsigned long long bgmNowDSP, parentNowDSP;
        FMOD_Channel_GetDSPClock(m_bgm->m_channel, &bgmNowDSP, &parentNowDSP);
        m_bgmOffset = (parentNowDSP - bgmNowDSP) + (m_sampleRate * m_bgm->m_offset);
    } else {
        queueSoundOnBeat(sound, beat);
        m_bgmOffset +=  beat * (60.f * m_sampleRate / m_bgm->m_bpm);
        m_bgm = sound;
    }
}
