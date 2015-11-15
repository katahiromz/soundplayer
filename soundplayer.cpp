//////////////////////////////////////////////////////////////////////////////
// soundplayer --- an OpenAL sound player
// Copyright (C) 2015 Katayama Hirofumi MZ. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////

float Note::get_freq(int octave) const {
    static const char *notes = "C+D+EF+G+A+B";
    const char *ptr = strchr(notes, m_note);
    int i = int(ptr - notes);

    switch (m_sign) {
    case '#': case '+':
        if (i == 11) {
            octave += 1;
            i = 0;
        } else {
            ++i;
        }
        break;
    case '-':
        if (i == 0) {
            octave -= 1;
            i = 11;
        } else {
            --i;
        }
        break;
    default:
        break;
    }

    double freq = 260 * std::pow(2, i / 12.0) * std::pow(2, octave - 4);
    return float(freq);
} // Note::get_freq

float Note::get_sec(int tempo) {
    float sec;
    assert(tempo != 0);
    if (m_dot) {
        sec = float((4 * 60.0 * 1.5) / m_length / tempo);
    } else {
        sec = float((4 * 60.0) / m_length / tempo);
    }
    return sec;
} // Note::get_sec

//////////////////////////////////////////////////////////////////////////////

void Phrase::add_note(char note, int length, bool dot, char sign) {
    m_notes.emplace_back(
        m_player->m_tempo, m_player->m_octave, note, length, dot, sign);
}

void Phrase::realize() {
    free();

    m_buffers.clear();
    m_sources.clear();

    for (auto& note : m_notes) {
        if (note.m_note != 'R') {
            ALuint buffer = m_player->create_buffer(note.m_freq, note.m_sec);
            m_buffers.push_back(buffer);

            ALuint source;
            alGenSources(1, &source);
            alSourcei(source, AL_BUFFER, buffer);
            m_sources.push_back(source);
        } else {
            m_buffers.push_back(0);
            m_sources.push_back(0);
        }
    }
} // Phrase::realize

//////////////////////////////////////////////////////////////////////////////

void SoundPlayer::play() {
    std::thread(
        [this](int dummy) {
            for (;;) {
                if (this->m_phrases.empty()) {
                    break;
                }
                auto phrase = this->m_phrases.front();
                this->m_phrases.pop_front();

                const size_t count = phrase.m_notes.size();
                assert(count == phrase.m_buffers.size());
                assert(count == phrase.m_sources.size());

                float gate = 0;
                for (size_t i = 0; i < count; ++i) {
                    auto& note = phrase.m_notes[i];
                    if (note.m_note != 'R') {
                        alSourcePlay(phrase.m_sources[i]);
                    }
                    gate += note.m_sec;
                    alutSleep(note.m_sec);
                }
            }
        },
        0
    ).detach();
} // SoundPlayer::play

//////////////////////////////////////////////////////////////////////////////
