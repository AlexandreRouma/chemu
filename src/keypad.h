#pragma once
#include <SDL2/SDL.h>

namespace CODAN {
    enum Key {
        KEY_STAR        = 0x00,
        KEY_7           = 0x01,
        KEY_4           = 0x02,
        KEY_1           = 0x03,
        KEY_DOWN        = 0x04,
        KEY_UP          = 0x05,
        KEY_OK          = 0x06,
        
        KEY_0           = 0x08,
        KEY_8           = 0x09,
        KEY_5           = 0x0A,
        KEY_2           = 0x0B,
        KEY_CANCEL      = 0x0C,
        KEY_SCAN        = 0x0D,
        KEY_MUTE        = 0x0E,
        KEY_CALL        = 0x0F,
        KEY_HASH        = 0x10,
        KEY_9           = 0x11,
        KEY_6           = 0x12,
        KEY_3           = 0x13,
        KEY_VOL_DOWN    = 0x14,
        KEY_VOL_UP      = 0x15,
        KEY_SEARCH      = 0x16,

        KEY_TRIANGLE    = 0x1D,
        KEY_POWER       = 0x1E,
        KEY_PTT         = 0x1F,

        _KEY_COUNT
    };

    class Keypad {
    public:
        void press(Key key);
        void release(Key key);

    private:
        void sendState();

        bool keyStates[_KEY_COUNT];
    };
}
