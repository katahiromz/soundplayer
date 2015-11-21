// C++ headers
#include <iostream>             // for std::cout, std::endl
#include <vector>               // for std::vector
#include <deque>                // for std::deque
#include <thread>               // for std::thread
#include <algorithm>            // for std::sort
#include <mutex>                // for std::mutex
#include <memory>               // for std::shared_ptr, std::weak_ptr, ...

// smart pointers
using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::make_shared;

// C headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

// portable event objects
#ifdef _WIN32
    #include "win32_pevent.h"
#else
    #include "pevent.h"
#endif

// VskSoundPlayer, VskPhrase, ...
#include "soundplayer.h"
