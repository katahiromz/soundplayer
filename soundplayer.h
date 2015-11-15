//////////////////////////////////////////////////////////////////////////////
// soundplayer --- an OpenAL sound player
// Copyright (C) 2015 Katayama Hirofumi MZ. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////////

#ifndef KATAHIROMZ_SOUNDPLAYER_H
#define KATAHIROMZ_SOUNDPLAYER_H

//////////////////////////////////////////////////////////////////////////////

#include <AL/alut.h>

#if defined(_WIN32) && defined(_MSC_VER)
    #pragma comment(lib, "OpenAL32.lib")
    #pragma comment(lib, "alut.lib")
#endif

//////////////////////////////////////////////////////////////////////////////

struct Note {
    int     m_tempo;
    int     m_octave;
    char    m_note;
    int     m_length;
    bool    m_dot;
    char    m_sign;
    float   m_freq;
    float   m_sec;

    Note(int tempo, int octave, char note,
         int length = 4, bool dot = false, char sign = 0) :
        m_tempo(tempo), m_octave(octave), m_note(note),
        m_length(length), m_dot(dot), m_sign(sign)
    {
        m_freq = get_freq(m_octave);
        m_sec = get_sec(m_tempo);
    }

    float get_freq(int octave) const;
    float get_sec(int tempo);

private:
    Note();
}; // struct Note

//////////////////////////////////////////////////////////////////////////////

struct SoundPlayer;

struct Phrase {
    SoundPlayer *           m_player;
    std::vector<Note>       m_notes;
    std::vector<ALuint>     m_buffers;
    std::vector<ALuint>     m_sources;

    Phrase(SoundPlayer *player) : m_player(player) { }
    ~Phrase() {
    }

    void add_note(char note, int length = 4, bool dot = false, char sign = 0);
    void free() {
        alDeleteBuffers(int(m_buffers.size()), m_buffers.data());
        m_buffers.clear();

        alDeleteSources(int(m_sources.size()), m_sources.data());
        m_sources.clear();
    }

    void realize();

private:
    Phrase();
}; // struct Phrase

//////////////////////////////////////////////////////////////////////////////

struct SoundPlayer {
    int                 m_tempo;
    int                 m_octave;
    int                 m_length;
    std::deque<Phrase>  m_phrases;

    SoundPlayer() :
        m_tempo(120),
        m_octave(4),
        m_length(4)
    {
        init_beep();
    }

    virtual ~SoundPlayer() {
        free_beep();
    }

    void add_phrase(Phrase& phrase) {
        phrase.realize();
        m_phrases.push_back(phrase);
    }

    void play();

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

    virtual ALuint create_buffer(float freq, float sec) {
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

protected:
    ALuint  m_beep_buffer;
    ALuint  m_beep_source;
}; // struct SoundPlayer

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef KATAHIROMZ_SOUNDPLAYER_H
