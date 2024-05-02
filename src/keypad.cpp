#include "keypad.h"
#include <stdint.h>
#include <vector>

namespace CODAN {
    void Keypad::press(Key key) {
        keyStates[key] = true;
        sendState();
    }

    void Keypad::release(Key key) {
        keyStates[key] = false;
        sendState();
    }

    void Keypad::sendState() {
        std::vector<uint8_t> packet;

        // Command byte
        packet.push_back(0x4E);

        // Append all currently pressed keycodes
        for (int k = 0; k < _KEY_COUNT; k++) {
            if (keyStates[k]) { packet.push_back(k); }
        }
    }
}