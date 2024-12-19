#pragma once

#include <Adafruit_MCP23X17.h>

#define SCL 5
#define SDA 4

#define MCP_N 3

extern Adafruit_MCP23X17 mcp[MCP_N];
extern int buttons[MCP_N][4];

bool mcp_setup();
void mcp_pinmode();
void mcp_clear();

void mcp_readbuttons();

void mcp_write(uint16_t v[MCP_N]);
