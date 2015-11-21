//////////////////////////////////////////////////////////////////////////////
// soundplayer --- an OpenAL sound player
// Copyright (C) 2015 Katayama Hirofumi MZ. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////////

#ifndef KATAHIROMZ_SOUNDPLAYER_H
#define KATAHIROMZ_SOUNDPLAYER_H

//////////////////////////////////////////////////////////////////////////////

#define ALUT_STATIC
#include <AL/alut.h>

#if defined(_WIN32) && defined(_MSC_VER)
    #pragma comment(lib, "OpenAL32.lib")
    #pragma comment(lib, "alut.lib")
#endif

#ifdef _WIN32
    #include "win32_pevent.h"
#else
    #include "pevent.h"
#endif

//////////////////////////////////////////////////////////////////////////////

struct VskSoundPlayer;

struct VskNote {
    int     m_tempo;
    int     m_octave;
    int     m_tone;
    char    m_note;
    bool    m_dot;
    int     m_length;
    char    m_sign;
    float   m_freq;
    float   m_sec;
    ALuint  m_buffer;
    ALuint  m_source;
    float   m_gate;

    VskNote(int tempo, int octave, int tone, char note,
            bool dot = false, int length = 4, char sign = 0) :
        m_tempo(tempo), m_octave(octave), m_tone(tone), m_note(note),
        m_dot(dot), m_length(length), m_sign(sign)
    {
        m_freq = get_freq(m_octave);
        m_sec = get_sec(m_tempo);
        m_buffer = -1;
        m_source = -1;
        m_gate = 0;
    }

    ~VskNote() {
        destroy();
    }

    float get_freq(int octave) const;
    float get_sec(int tempo) const;

    void realize(VskSoundPlayer *player);
    void destroy();

private:
    VskNote();
}; // struct VskNote

//////////////////////////////////////////////////////////////////////////////

struct VskPhrase {
    int                                 m_tempo;
    int                                 m_octave;
    int                                 m_length;
    int                                 m_tone;
    std::vector<shared_ptr<VskNote>>    m_notes;

    VskPhrase() : m_tempo(120), m_octave(4), m_length(4), m_tone(-1) { }

    void add_note(char note) {
        add_note(note, false);
    }
    void add_note(char note, bool dot) {
        add_note(note, dot, m_length);
    }
    void add_note(char note, bool dot, int length) {
        add_note(note, dot, length, 0);
    }
    void add_note(char note, bool dot, int length, char sign) {
        add_note(m_tone, note, dot, length, sign);
    }
    void add_note(int tone, char note, bool dot, int length, char sign) {
        m_notes.push_back(
            make_shared<VskNote>(
                m_tempo, m_octave, tone, note, dot, length, sign)
        );
    }
}; // struct VskPhrase

//////////////////////////////////////////////////////////////////////////////

struct VskSoundPlayer {
    bool                                m_playing_music;
    PE_event                            m_stopping_event;
    std::deque<VskPhrase>               m_phrases;
    std::mutex                          m_lock;
    std::vector<shared_ptr<VskNote>>    m_notes;

    VskSoundPlayer() : m_playing_music(false), m_stopping_event(false, false) {
        init_beep();
    }

    virtual ~VskSoundPlayer() {
        free_beep();
    }

    void add_phrase(const VskPhrase& phrase) {
        m_lock.lock();
        m_phrases.push_back(phrase);
        m_lock.unlock();
    }

    void play();
    bool wait_for_stop(uint32_t milliseconds = -1);
    void stop();

    bool play_and_wait(uint32_t milliseconds = -1) {
        play();
        return wait_for_stop(milliseconds);
    }

    void beep(int i) {
        switch (i) {
        case -1:
            alSourceStop(m_beep_source);
            alSourcei(m_beep_source, AL_LOOPING, AL_FALSE);
            alSourcePlay(m_beep_source);
            break;
        case 0:
            alSourceStop(m_beep_source);
            alSourcei(m_beep_source, AL_LOOPING, AL_FALSE);
            break;
        case 1:
            alSourceStop(m_beep_source);
            alSourcei(m_beep_source, AL_LOOPING, AL_TRUE);
            alSourcePlay(m_beep_source);
        }
    } // beep

    virtual ALuint create_buffer(int tone, float freq, float sec) {
        // TODO: tone
        ALuint buffer = alutCreateBufferWaveform(
            ALUT_WAVEFORM_SINE,
            freq,
            0,
            sec
        );
        return buffer;
    } // create_buffer

protected:
    void init_beep() {
        m_beep_buffer = alutCreateBufferWaveform(
            ALUT_WAVEFORM_SINE,
            2400,
            0,
            0.5
        );
        //m_beep_buffer = alutCreateBufferFromFile("beep.wav");
        alGenSources(1, &m_beep_source);
        alSourcei(m_beep_source, AL_BUFFER, m_beep_buffer);
    }

    void free_beep() {
        alDeleteBuffers(1, &m_beep_buffer);
        alDeleteSources(1, &m_beep_source);
    }

    void realize_phrase(VskPhrase& phrase);
    void realize();
    bool wait_for_note(float sec);

protected:
    ALuint  m_beep_buffer;
    ALuint  m_beep_source;
}; // struct VskSoundPlayer

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef KATAHIROMZ_SOUNDPLAYER_H
