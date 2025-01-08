#include <Adafruit_MCP23X17.h>
#include <Arduino.h>

#include "animations.h"
#include "grid.h"

#define scl 5
#define sda 4
#define clk 18
#define init 19

#define MCP_ADDRESS(x) ((int[]){0x24, 0x26, 0x27}[x])
#define MCP(x, y) ((int[]){0, 0, 1, 1, 2, 2}[x])
#define MCP_PIN(x, y) (y + ((x % 2 == 0) * 6) + 2)

Adafruit_MCP23X17 mcp[3];
int buttons[3][4] = {0};
int buttons_prev[3][4] = {0};
int buttons_press[3][4] = {0};
int buttons_release[3][4] = {0};

void mcp_pinmode();
void hc_writeb(uint8_t b);
void mcp_clear();

void inputs(void *);
void calc(void *);
void outputs(void *);

void setup() {
    Wire.begin();
    Wire.setClock(400000);
    Serial.begin(115200);

retry:
    bool err = false;
    for (int i = 0; i < 3; i++) {
        if (mcp[i].begin_I2C(MCP_ADDRESS(i)))
            Serial.printf("ok: mcp %d\n", i);
        else {
            err = true;
            Serial.printf("error: mcp %d\n", i);
        }
    }
    if (err) {
      delay(5000);
      goto retry;
    }

    mcp_pinmode();
    pinMode(clk, OUTPUT);
    pinMode(init, OUTPUT);
    digitalWrite(clk, HIGH);

    mcp_clear();

    hc_writeb(0);
    hc_writeb(0);

    xTaskCreate(inputs, "inputs", 10000, NULL, 1, NULL);
    xTaskCreate(outputs, "outputs", 10000, NULL, 2, NULL);
    xTaskCreate(calc, "calc", 10000, NULL, 3, NULL);
}

void loop() {

}

void mcp_pinmode() {
    for (int i = 0; i < 3; i++) {
        mcp[i].pinMode(0, INPUT_PULLUP);
        mcp[i].pinMode(1, INPUT_PULLUP);
        mcp[i].pinMode(14, INPUT_PULLUP);
        mcp[i].pinMode(15, INPUT_PULLUP);

        mcp[i].pinMode(2, OUTPUT);
        mcp[i].pinMode(3, OUTPUT);
        mcp[i].pinMode(4, OUTPUT);
        mcp[i].pinMode(5, OUTPUT);
        mcp[i].pinMode(6, OUTPUT);
        mcp[i].pinMode(7, OUTPUT);
        mcp[i].pinMode(8, OUTPUT);
        mcp[i].pinMode(9, OUTPUT);
        mcp[i].pinMode(10, OUTPUT);
        mcp[i].pinMode(11, OUTPUT);
        mcp[i].pinMode(12, OUTPUT);
        mcp[i].pinMode(13, OUTPUT);
    }
}

void hc_writeb(uint8_t b) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(init, b & (0x1 << i));
        digitalWrite(clk, LOW);
        delayMicroseconds(100);
        digitalWrite(clk, HIGH);
    }
}

void hc_write(int b) {
    digitalWrite(init, b);
    digitalWrite(clk, HIGH);
    delayMicroseconds(5);
    digitalWrite(clk, LOW);
}

void mcp_clear() {
    for (int i = 0; i < 3; i++)
        mcp[i].writeGPIOAB(~0);
}

void inputs(void *args) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(50);
    BaseType_t xWasDelayed;
    unsigned long prev = 0, average=0;
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    for (;;) {
        prev = micros();
        for (int i = 0; i < 3; i++) {

            uint16_t b = mcp[i].readGPIOAB();
            buttons[i][0] = (b & (0x01 << 0))>>0;
            buttons[i][1] = (b & (0x01 << 1))>>1;
            buttons[i][2] = (b & (0x01 << 14))>>14;
            buttons[i][3] = (b & (0x01 << 15))>>15;
            for (int j = 0; j < 4; j++) {
                //buttons_press[i][j] = 0;
                buttons_release[i][j] = 0;
                if (buttons[i][j] != 0 && buttons_prev[i][j] == 0)
                    buttons_press[i][j] = 1;
                if (buttons[i][j] == 0 && buttons_prev[i][j] != 0) {
                    buttons_release[i][j] = 1;
                }
                buttons_prev[i][j] = buttons[i][j];
            }
        }
        unsigned long elapsed = micros() - prev;
        if(cont < n){
        average = (average*cont + elapsed)/(cont+1);
        cont++;
        if(elapsed > max){
            max = elapsed;
        }
        if(elapsed < min){
            min = elapsed;
        }
        }
        else{
            if(stop==0){
            Serial.printf("Input exec time: %lu,max: %d, min: %d\n",average,max,min);
            stop = 1;
            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        
    }
}

void outputs(void *args) {
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    BaseType_t xWasDelayed;
    unsigned long prev = micros(), average=0;

    for (;;) {
        for (int z = 0; z < 6; z++) {
            unsigned long elapsed = micros() - prev;
        if(cont < n){
        average = (average*cont + elapsed)/(cont+1);
        cont++;
        if(elapsed > max){
            max = elapsed;

        }
        if(elapsed < min){
            min = elapsed;
        }
        }
        else{
            if(stop==0){
            Serial.printf("Output exec time: %lu, max: %d, min: %d\n",average,max,min);

            stop = 1;

            }
        }
            xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
            prev = micros();
            hc_write(LOW);
            for (int i = 0; i < 3; i++) {
                mcp[i].writeGPIOAB(~((uint16_t)grid_getZY(z, 2 * i) << 8 | (uint16_t)grid_getZY(z, 2 * i + 1)<<2));
            }
            hc_write(z != 0 ? LOW : HIGH);
        }
    }

    /*int m[3] = {0};
    unsigned long prev = micros(), average=0;
    for (;;) {
        for (int z = 0; z < 6; z++) {
            m[0] = m[1] = m[2] = 0;
            for (int y = 0; y < 6; y++) {
                for (int x = 0; x < 6; x++) {
                    m[MCP(x, y)] |= (grid_get(x, y, z)&1) << MCP_PIN(x, y);
                }
            }
        unsigned long elapsed = micros() - prev;
        if(cont < n){
        average = (average*cont + elapsed)/(cont+1);
        cont++;
        if(elapsed > max){
            max = elapsed;

        }
        if(elapsed < min){
            min = elapsed;
        }
        }
        else{
            if(stop==0){
            Serial.printf("Output exec time: %lu, max: %d, min: %d\n",average,max,min);

            stop = 1;

            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        prev = micros();
            hc_write(LOW);
            for (int i = 0; i < 3; i++)
                mcp[i].writeGPIOAB(~m[i]);

            hc_write(z != 0 ? LOW : HIGH);
        }
    }
    */
}


void calc(void *args) {
  //for(;;) {
    // initial();
    // delay(2000);
  //}
    //snake();
    //cube();
    initial();
    cube_fixed_vertice();
    rain();
    cube();
    snake();

}