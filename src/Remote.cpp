#include "Remote.hpp"
#include "PINOUT.h"
#include <Wire.h>

#define PCF8574A_ADDRESS 0x38

void Remote::init(void) {
    Wire.begin();
}

RemoteButton Remote::getRemoteButton(void) {
    if (Wire.requestFrom(PCF8574A_ADDRESS, (uint8_t)1) != 1) {
        return REMOTE_NO_BUTTON;
    }

    uint8_t data = Wire.read();
    return (RemoteButton)(data & (~0xC0));
}