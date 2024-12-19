#include <Arduino.h>
#include "hc.h"

void hc_pinmode() {
    pinMode(CLK, OUTPUT);
    pinMode(INIT, OUTPUT);
    digitalWrite(CLK, HIGH);
}

bool hc_setup() {
    hc_pinmode();
    return true;
}

void hc_clear() {
    hc_writeb(0);
    hc_writeb(0);
}

void hc_writeb(uint8_t b) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(INIT, b & (0x1 << i));
        digitalWrite(CLK, LOW);
        delayMicroseconds(5);
        digitalWrite(CLK, HIGH);
    }
}

void hc_write(int b) {
    digitalWrite(INIT, b);
    digitalWrite(CLK, HIGH);
    delayMicroseconds(5);
    digitalWrite(CLK, LOW);
}
