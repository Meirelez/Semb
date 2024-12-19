#include "mcp.h"

#define MCP_ADDRESS(x) ((int[]){0x24, 0x26, 0x27}[x])
#define MCP(x, y) ((int[]){0, 0, 1, 1, 2, 2}[x])
#define MCP_PIN(x, y) (y + ((x % 2 == 0) * 6) + 2)

Adafruit_MCP23X17 mcp[MCP_N];
int buttons[MCP_N][4];

static TwoWire mcp_wire = TwoWire(0);

bool mcp_setup() {
    mcp_wire.begin(SDA, SCL, 400000);

    int e = 0;
    for (int i = 0; i < MCP_N; i++) {
        if (!mcp[i].begin_I2C(MCP_ADDRESS(i), &mcp_wire)) {
            Serial.printf("error: mcp %d\n", i);
            e++;
        }
    }

    if (e > 0)
        return false;

    mcp_pinmode();
    return true;
}

void mcp_pinmode() {
    for (int i = 0; i < MCP_N; i++)
        for (int j = 0; j < 16; j++)
            mcp[i].pinMode(j, ((int[]){INPUT_PULLUP, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, INPUT_PULLUP}[j/2]));
}

void mcp_clear() {
    for (int i = 0; i < MCP_N; i++)
        mcp[i].writeGPIOAB(~0);
}

void mcp_readbuttons() {
    for (int i = 0; i < MCP_N; i++) {
        uint16_t b = mcp[i].readGPIOAB();
        buttons[i][0] = (b & (1 << 0))>>0;
        buttons[i][1] = (b & (1 << 1))>>1;
        buttons[i][2] = (b & (1 << 14))>>14;
        buttons[i][3] = (b & (1 << 15))>>15;
    }
}

void mcp_write(uint16_t v[MCP_N]) {
    for (int i = 0; i < MCP_N; i++)
        mcp[i].writeGPIOAB(~v[i]);
}
