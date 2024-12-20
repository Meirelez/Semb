#include <Arduino.h>

#include "animations.h"
#include "grid.h"
#include "mcp.h"
#include "hc.h"

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

    for (;;) {
        for (int z = 0; z < 6; z++) {
            uint16_t m[MCP_N] = {0};
            for (int i = 0; i < MCP_N; i++)
                m[i] = (uint16_t)grid_getZY(z, 2 * i) << 8 | grid_getZY(z, 2 * i + 1) << 2;

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