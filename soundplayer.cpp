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
    if (m_note != 'R') {
        if (m_source != ALuint(-1)) {
            alDeleteSources(1, &m_source);
            m_source = ALuint(-1);
        }
        if (m_buffer != ALuint(-1)) {
            alDeleteBuffers(1, &m_buffer);
            m_buffer = ALuint(-1);
        }
    }
} // VskNote::destroy

//////////////////////////////////////////////////////////////////////////////

void VskPhrase::realize(VskSoundPlayer *player) {
    float gate = 0;
    for (auto& note : m_notes) {
        note->realize(player);
        note->m_gate = gate;
        gate += note->m_sec;
    }
    m_goal = gate;
} // VskPhrase::realize

//////////////////////////////////////////////////////////////////////////////

bool VskSoundPlayer::wait_for_note(float sec) {
    return m_stopping_event.wait_for_event(uint32_t(sec * 1000));
}

void VskSoundPlayer::play(shared_ptr<VskPhrase> phrase) {
    phrase->realize(this);

    m_lock.lock();
    m_phrases.push_back(phrase);
    m_lock.unlock();

    if (m_playing_music) {
        return;
    }

    m_playing_music = false;
    m_stopping_event.pulse();
    m_playing_music = true;

    std::thread(
        [this](int dummy) {
            for (;;) {
                // go next phrase
                m_lock.lock();
                if (m_phrases.empty()) {
                    m_lock.unlock();
                    break;
                }
                auto phrase = m_phrases.front();
                m_phrases.pop_front();
                m_lock.unlock();

                float gate = 0;
                for (auto& note : phrase->m_notes) {
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
                if (m_playing_music) {
                    if (gate < phrase->m_goal) {
                        wait_for_note(phrase->m_goal - gate);
                    }
                }
            }
            if (m_playing_music) {
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

        auto phrase = make_shared<VskPhrase>();

        phrase->add_note('C', false, 4);
        phrase->add_note('D', false, 4);
        phrase->add_note('E', false, 4);
        phrase->add_note('F', false, 4);
        phrase->add_note('E', false, 4);
        phrase->add_note('D', false, 4);
        phrase->add_note('C', false, 4);
        phrase->add_note('R', false, 4);

        phrase->add_note('E', false, 4);
        phrase->add_note('F', false, 4);
        phrase->add_note('G', false, 4);
        phrase->add_note('A', false, 4);
        phrase->add_note('G', false, 4);
        phrase->add_note('F', false, 4);
        phrase->add_note('E', false, 4);
        phrase->add_note('R', false, 4);

        phrase->add_note('C', false, 4);
        phrase->add_note('R', false, 4);
        phrase->add_note('C', false, 4);
        phrase->add_note('R', false, 4);
        phrase->add_note('C', false, 4);
        phrase->add_note('R', false, 4);
        phrase->add_note('C', false, 4);
        phrase->add_note('R', false, 4);

        phrase->add_note('C', false, 8);
        phrase->add_note('C', false, 8);
        phrase->add_note('D', false, 8);
        phrase->add_note('D', false, 8);
        phrase->add_note('E', false, 8);
        phrase->add_note('E', false, 8);
        phrase->add_note('F', false, 8);
        phrase->add_note('F', false, 8);

        phrase->add_note('E', false, 8);
        phrase->add_note('R', false, 8);
        phrase->add_note('D', false, 8);
        phrase->add_note('R', false, 8);
        phrase->add_note('C', false, 8);
        phrase->add_note('R', false, 8);

        VskSoundPlayer player;
        player.play_and_wait(phrase);

        alutExit();
    } // main
#endif  // def SOUND_TEST

//////////////////////////////////////////////////////////////////////////////
