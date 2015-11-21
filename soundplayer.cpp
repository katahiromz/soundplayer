//////////////////////////////////////////////////////////////////////////////
// soundplayer --- an OpenAL sound player
// Copyright (C) 2015 Katayama Hirofumi MZ. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////

float VskNote::get_freq(int octave) const {
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
} // VskNote::get_freq

float VskNote::get_sec(int tempo) const {
    float sec;
    assert(tempo != 0);
    if (m_dot) {
        sec = float((4 * 60.0 * 1.5) / m_length / tempo);
    } else {
        sec = float((4 * 60.0) / m_length / tempo);
    }
    return sec;
} // VskNote::get_sec

void VskNote::realize(VskSoundPlayer *player) {
    destroy();
    if (m_note != 'R') {
        m_buffer = player->create_buffer(m_tone, m_freq, m_sec);
        alGenSources(1, &m_source);
        alSourcei(m_source, AL_BUFFER, m_buffer);
    }
} // VskNote::realize

void VskNote::destroy() {
    if (m_buffer != ALuint(-1)) {
        alDeleteBuffers(1, &m_buffer);
        m_buffer = ALuint(-1);
    }
    if (m_source != ALuint(-1)) {
        alDeleteSources(1, &m_source);
        m_source = ALuint(-1);
    }
} // VskNote::destroy

//////////////////////////////////////////////////////////////////////////////

void VskSoundPlayer::realize_phrase(VskPhrase& phrase) {
    float gate = 0;
    for (auto& note : phrase.m_notes) {
        note->realize(this);
        note->m_gate = gate;
        gate += note->m_sec;
    }
} // VskSoundPlayer::realize_phrase

void VskSoundPlayer::realize() {
    // realize phrases
    for (auto& phrase : m_phrases) {
        realize_phrase(phrase);
    }

    // arrange notes flatly
    m_notes.clear();
    for (auto& phrase : m_phrases) {
        m_notes.insert(m_notes.end(),
            phrase.m_notes.begin(), phrase.m_notes.end());
    }

    // sort notes by gates
    std::sort(m_notes.begin(), m_notes.end(),
        [this](shared_ptr<VskNote> x, shared_ptr<VskNote> y) {
            return x->m_gate < y->m_gate;
        }
    );
} // VskSoundPlayer::realize

bool VskSoundPlayer::wait_for_note(float sec) {
    return m_stopping_event.wait_for_event(uint32_t(sec * 1000));
}

void VskSoundPlayer::play() {
    realize();

    m_playing_music = false;
    m_stopping_event.pulse();
    m_playing_music = true;

    std::thread(
        [this](int dummy) {
            for (;;) {
                // go next phrase
                m_lock.lock();
                if (m_phrases.empty()) {
                    break;
                }
                auto phrase = m_phrases.front();
                m_phrases.pop_front();
                m_lock.unlock();

                float gate = 0;
                for (auto& note : m_notes) {
                    std::cout << note->m_gate << std::endl;
                    if (gate < note->m_gate) {
                        // wait for next note
                        if (!wait_for_note(note->m_gate - gate)) {
                            // stop music if signaled
                            m_playing_music = false;
                            break;
                        }
                        gate = note->m_gate;
                    }
                    if (note->m_note != 'R') {
                        alSourcePlay(note->m_source);
                    }
                }
                m_playing_music = false;
                m_stopping_event.pulse();
            }
        },
        0
    ).detach();
} // VskSoundPlayer::play

void VskSoundPlayer::stop() {
    m_playing_music = false;
    m_stopping_event.pulse();
}

bool VskSoundPlayer::wait_for_stop(uint32_t milliseconds) {
    return m_stopping_event.wait_for_event(milliseconds);
}

//////////////////////////////////////////////////////////////////////////////

#ifdef SOUND_TEST
    int main(void) {
        alutInit(NULL, NULL);

        VskPhrase phrase;

        phrase.add_note('C', false, 4);
        phrase.add_note('D', false, 4);
        phrase.add_note('E', false, 4);
        phrase.add_note('F', false, 4);
        phrase.add_note('E', false, 4);
        phrase.add_note('D', false, 4);
        phrase.add_note('C', false, 4);
        phrase.add_note('R', false, 4);

        phrase.add_note('E', false, 4);
        phrase.add_note('F', false, 4);
        phrase.add_note('G', false, 4);
        phrase.add_note('A', false, 4);
        phrase.add_note('G', false, 4);
        phrase.add_note('F', false, 4);
        phrase.add_note('E', false, 4);
        phrase.add_note('R', false, 4);

        phrase.add_note('C', false, 4);
        phrase.add_note('R', false, 4);
        phrase.add_note('C', false, 4);
        phrase.add_note('R', false, 4);
        phrase.add_note('C', false, 4);
        phrase.add_note('R', false, 4);
        phrase.add_note('C', false, 4);
        phrase.add_note('R', false, 4);

        phrase.add_note('C', false, 8);
        phrase.add_note('C', false, 8);
        phrase.add_note('D', false, 8);
        phrase.add_note('D', false, 8);
        phrase.add_note('E', false, 8);
        phrase.add_note('E', false, 8);
        phrase.add_note('F', false, 8);
        phrase.add_note('F', false, 8);

        phrase.add_note('E', false, 8);
        phrase.add_note('R', false, 8);
        phrase.add_note('D', false, 8);
        phrase.add_note('R', false, 8);
        phrase.add_note('C', false, 8);
        phrase.add_note('R', false, 8);

        VskSoundPlayer player;
        player.add_phrase(phrase);
        player.play_and_wait();

        alutExit();
    } // main
#endif  // def SOUND_TEST

//////////////////////////////////////////////////////////////////////////////
