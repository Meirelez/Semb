#include <Arduino.h>

#include "animations.h"
#include "grid.h"
#include "mcp.h"
#include "hc.h"

#define MCP_ADDRESS(x) ((int[]){0x24, 0x26, 0x27}[x])
#define MCP(x, y) ((int[]){0, 0, 1, 1, 2, 2}[x])
#define MCP_PIN(x, y) (y + ((x % 2 == 0) * 6) + 2)

void inputs(void *);
void calc(void *);
void outputs(void *);

void setup() {
    Serial.begin(115200);

    if (!hc_setup()) {
        Serial.println("hc setup error");
        for(;;);
    }
    hc_clear();

    while (!mcp_setup()) {
        Serial.println("mcp setup error");
        delay(5000);
    }
    mcp_clear();

    xTaskCreate(inputs, "inputs", 10000, NULL, 1, NULL);
    xTaskCreate(outputs, "outputs", 10000, NULL, 2, NULL);
    xTaskCreate(calc, "calc", 10000, NULL, 3, NULL);
}

void loop() {

}

void inputs(void *args) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(40);
    BaseType_t xWasDelayed;

    for (;;) {
        mcp_readbuttons();
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void outputs(void *args) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    BaseType_t xWasDelayed;

    /* for (;;) {
        for (int z = 0; z < 6; z++) {
            xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
            hc_write(LOW);
            for (int i = 0; i < 3; i++) {
                mcp[i].writeGPIOAB(~((uint16_t)grid_getZY(z, 2 * i) << 8 | (uint16_t)grid_getZY(z, 2 * i + 1)));
                //Serial.printf("mcp %d: %x\n", i, (uint16_t)grid_getZY(z, 2 * i) << 8 | (uint16_t)grid_getZY(z, 2 * i + 1));
            }
            hc_write(z != 0 ? LOW : HIGH);
        }
    } */

    uint16_t m[3] = {0};
    for (;;) {
        for (int z = 0; z < 6; z++) {
            m[0] = m[1] = m[2] = 0;
            for (int y = 0; y < 6; y++) {
                for (int x = 0; x < 6; x++) {
                    m[MCP(x, y)] |= (grid_get(x, y, z)&1) << MCP_PIN(x, y);
                }
            }
            xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
            hc_write(LOW);
            mcp_write(m);
            hc_write(z != 0 ? LOW : HIGH);
        }
    }
}


void calc(void *args) {
    for(;;) {
        initial();
        delay(2000);
    }
    //cube();
    //rain();
}