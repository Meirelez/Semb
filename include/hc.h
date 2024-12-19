#pragma once

#include <stdint.h>

#define CLK 18
#define INIT 19

bool hc_setup();
void hc_clear();

void hc_writeb(uint8_t b);
void hc_write(int b);
