#include "M5Module_LAN.h"

void M5Module_LAN::reset() {
    pinMode(reset_pin, OUTPUT);
    digitalWrite(reset_pin, LOW);
    delay(50);
    digitalWrite(reset_pin, HIGH);
    delay(50);
}

void M5Module_LAN::setResetPin(uint8_t pin) {
    reset_pin = pin;
}