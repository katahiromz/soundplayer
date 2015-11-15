//////////////////////////////////////////////////////////////////////////////
// soundplayer sample
// Copyright (C) 2015 Katayama Hirofumi MZ. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    alutInit(NULL, NULL);

    SoundPlayer player;
    Phrase phrase(&player);

    phrase.add_note('C', 4, false, 0);
    phrase.add_note('D', 4, false, 0);
    phrase.add_note('E', 4, false, 0);
    phrase.add_note('F', 4, false, 0);
    phrase.add_note('E', 4, false, 0);
    phrase.add_note('D', 4, false, 0);
    phrase.add_note('C', 4, false, 0);
    phrase.add_note('R', 4, false, 0);

    phrase.add_note('E', 4, false, 0);
    phrase.add_note('F', 4, false, 0);
    phrase.add_note('G', 4, false, 0);
    phrase.add_note('A', 4, false, 0);
    phrase.add_note('G', 4, false, 0);
    phrase.add_note('F', 4, false, 0);
    phrase.add_note('E', 4, false, 0);
    phrase.add_note('R', 4, false, 0);

    phrase.add_note('C', 4, false, 0);
    phrase.add_note('R', 4, false, 0);
    phrase.add_note('C', 4, false, 0);
    phrase.add_note('R', 4, false, 0);
    phrase.add_note('C', 4, false, 0);
    phrase.add_note('R', 4, false, 0);
    phrase.add_note('C', 4, false, 0);
    phrase.add_note('R', 4, false, 0);

    phrase.add_note('C', 8, false, 0);
    phrase.add_note('C', 8, false, 0);
    phrase.add_note('D', 8, false, 0);
    phrase.add_note('D', 8, false, 0);
    phrase.add_note('E', 8, false, 0);
    phrase.add_note('E', 8, false, 0);
    phrase.add_note('F', 8, false, 0);
    phrase.add_note('F', 8, false, 0);

    phrase.add_note('E', 8, false, 0);
    phrase.add_note('R', 8, false, 0);
    phrase.add_note('D', 8, false, 0);
    phrase.add_note('R', 8, false, 0);
    phrase.add_note('C', 8, false, 0);
    phrase.add_note('R', 8, false, 0);
    player.add_phrase(phrase);
    player.play();

    getchar();
    phrase.free();
    alutExit();
    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////
