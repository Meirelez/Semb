#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

#include "animations.h"

#define scl 5
#define sda 4
#define clk 14
#define init 12

#define MCP_ADDRESS(x) ((int[]){0x24, 0x26, 0x27}[x])
#define MCP(x, y) ((int[]){0, 0, 1, 1, 2, 2}[x])
#define MCP_PIN(x, y) (y + ((x % 2 == 0) * 6) + 2)

Adafruit_MCP23X17 mcp[3];
int buttons[3][4];
volatile int grid[6][6][6] = {0};

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

  bool err = false;
  for (int i = 0; i < 3; i++) {
    if (mcp[i].begin_I2C(MCP_ADDRESS(i)))
      Serial.printf("ok: mcp %d\n", i);
    else {
      err = true;
      Serial.printf("error: mcp %d\n", i);
    }
  }
  if (err)
    for (;;);

  mcp_pinmode();
  pinMode(clk, OUTPUT);
  pinMode(init, OUTPUT);
  digitalWrite(clk, HIGH);

  mcp_clear();

  hc_writeb(0);
  hc_writeb(0);
}

void loop() {
  xTaskCreate(inputs, "inputs", 10000, NULL, 1, NULL);  
  xTaskCreate(outputs, "outputs", 10000, NULL, 2, NULL);  
  xTaskCreate(calc, "calc", 10000, NULL, 3, NULL);  
}

void mcp_pinmode() {
  for (int i = 0; i < 3; i++) {
    mcp[i].pinMode(0, INPUT);
    mcp[i].pinMode(1, INPUT);
    mcp[i].pinMode(14, INPUT);
    mcp[i].pinMode(15, INPUT);

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
  digitalWrite(clk, LOW);
  delayMicroseconds(5);
  digitalWrite(clk, HIGH);
  delayMicroseconds(5);
}

void hc_clock(int b) {
  digitalWrite(init, b);
  digitalWrite(clk, HIGH);
  delayMicroseconds(5);
  digitalWrite(clk, LOW);
}

void mcp_clear() {
  for (int i = 0; i < 3; i++) {
    mcp[i].digitalWrite(2, HIGH);
    mcp[i].digitalWrite(3, HIGH);
    mcp[i].digitalWrite(4, HIGH);
    mcp[i].digitalWrite(5, HIGH);
    mcp[i].digitalWrite(6, HIGH);
    mcp[i].digitalWrite(7, HIGH);
    mcp[i].digitalWrite(8, HIGH);
    mcp[i].digitalWrite(9, HIGH);
    mcp[i].digitalWrite(10, HIGH);
    mcp[i].digitalWrite(11, HIGH);
    mcp[i].digitalWrite(12, HIGH);
    mcp[i].digitalWrite(13, HIGH);
  }
}

void inputs(void *args) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(50);
  BaseType_t xWasDelayed;

  for(;;) {
    for (int i = 0; i < 3; i++) {
      uint16_t b = mcp[i].readGPIOAB();
      buttons[i][0] = b & (0x01 << 0);
      buttons[i][1] = b & (0x01 << 1);
      buttons[i][2] = b & (0x01 << 14);
      buttons[i][3] = b & (0x01 << 15);
    }
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void outputs(void *args) {
  uint16_t m[3] = {0};
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1);
  BaseType_t xWasDelayed;

  for (;;) {
    for (int z = 0; z < 6; z++) {
      m[0] = m[1] = m[2] = 0;
      for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 6; x++) {
          m[MCP(x, y)] |= ((grid[x][y][z] & 0x1) << MCP_PIN(x, y));
        }
      }
      xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
      hc_clock(LOW);
      for (int i = 0; i < 3; i++)
        mcp[i].writeGPIOAB(~m[i]);

      hc_clock(z != 0 ? LOW: HIGH);
    }    
  }
}

void clear_grid() {
  for (int x = 0; x < 6; x++)
    for (int y = 0; y < 6; y++)
      for (int z = 0; z < 6; z++)
        grid[x][y][z] = LOW;
}

void calc(void *args) {
  cube();
  //rain();
}