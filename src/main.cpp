#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#define scl 5
#define sda 4
#define clk 14
#define init 12

#define MCP_ADDRESS(x) ((int[]){0x24, 0x26, 0x27}[x])
#define MCP(x, y) ((int[]){0, 0, 1, 1, 2, 2}[x])
#define MCP_PIN(x, y) (y+((x%2 == 0)*6)+2)

Adafruit_MCP23X17 mcp[3];
int buttons[3][4];
int grid[6][6][6] = {
  {
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
  },
  {
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
  },
  {
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
  },
  {
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
  },
  {
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
  },
  {
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
    { LOW, HIGH, LOW, HIGH, LOW, HIGH},
    { HIGH, LOW, HIGH, LOW, HIGH, LOW},
  },
};

void mcp_pinmode();
void hc_writeb(uint8_t b);
void mcp_clear();

void inputs();
void calc();
void outputs();

void clear_grid();
void cube();

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  for (int i = 0; i < 3; i++) {
    if (mcp[i].begin_I2C(MCP_ADDRESS(i)))
      Serial.printf("ok: mcp %d\n", i);
    else
      Serial.printf("error: mcp %d\n", i);
  }

  mcp_pinmode();
  pinMode(clk, OUTPUT);
  pinMode(init, OUTPUT);
  digitalWrite(clk, HIGH);

  mcp_clear();

  hc_writeb(0);
  hc_writeb(0);
}

void loop() {
  inputs();
  calc();
  outputs();
  //for (int i = 0; i < 3; i++) {
    //Serial.printf("mcp %d\n", i);
    //for (int j = 2; j < 14; j++) {
    //  mcp[i].digitalWrite(j, LOW);
    //  Serial.printf("mcp pin %d\n", j);
    //  hc_write(HIGH);
    //  delay(100);
    //  for (int k = 0; k < 15; k++) {
    //    hc_write(HIGH);
    //    delay(100);
    //  }
    //  mcp[i].digitalWrite(j, HIGH);
    //  delay(100);
    //}
    
  //}
  //delay(20);
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
    digitalWrite(init, b&(0x1<<i));
    digitalWrite(clk, LOW);
    delayMicroseconds(100);
    digitalWrite(clk, HIGH);
  }
}

void hc_write(int b) {
  digitalWrite(init, b);
  digitalWrite(clk, LOW);
  delayMicroseconds(10);
  digitalWrite(clk, HIGH);
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

void inputs() {
  for (int i = 0; i < 3; i++) {
    buttons[i][0] = mcp[i].digitalRead(0);
    buttons[i][1] = mcp[i].digitalRead(1);
    buttons[i][2] = mcp[i].digitalRead(14);
    buttons[i][3] = mcp[i].digitalRead(15);
  }
}

void calc() {
  cube();
}

void outputs() {
  for (int z = 0; z < 6; z++) {
    for (int y = 0; y < 6; y++) {
      for (int x = 0; x < 6; x++) {
        mcp[MCP(x, y)].digitalWrite(MCP_PIN(x, y), grid[x][y][z]);
        hc_tick();
        Serial.printf("(%d, %d, %d) mcp %d pin %d output %d\n", x, y, z, MCP(x, y), MCP_PIN(x, y), grid[x][y][z]);
        delay(10);
        // for (int i = 0; i < 100; i++) {
        //   hc_tick();
        //   delay(2);
        // }
      }
    }
    
  }
  //for (int i = 0; i < 3; i++) {
  //  for (int j = 2; j < 14; j++) {
  //
  //    mcp[i].digitalWrite(j, grid[i][(j-2)%6][]);
  //    hc_write(HIGH);
  //    delay(100);
  //    for (int k = 0; k < 15; k++) {
  //      hc_write(HIGH);
  //      delay(100);
  //    }
  //    mcp[i].digitalWrite(j, HIGH);
  //    delay(100);
  //  }
  //}
}

void hc_tick() {
  static int counter = 0;
  switch (counter) {
    case 0:
    case 1:
    case 2:
    case 4:
      hc_write(LOW);
    case 3:
      hc_write(LOW);
      counter++;
      break;
    case 5:
      hc_write(HIGH);
    default:
      counter = 0;
  }
}

void clear_grid() {
  for (int x = 0; x < 6; x++)
    for (int y = 0; y < 6; y++)
      for (int z = 0; z < 6; z++)
        grid[x][y][x] = LOW;
}

void cube() {
  // blink cube edges with frequency 1Hz
  static unsigned long last = 0;
  static int state = LOW;

  if (millis()-last < 500) { // wait 0.5s since last run
    return;
  }

  state = state == HIGH? LOW: HIGH; // toggle edges state

  if (last == 0) { // first run
    state = LOW;
    clear_grid();
    last = millis();
    return;
  }

  for (int x = 0; x < 6; x++) {
    for (int y = 0; y < 6; y++) {
      for (int z = 0; z < 6; z++) {
        if (x == 0 || x == 5 || y == 0 || y == 5 || z == 0 || z == 5)
          grid[x][y][z] = state;
      }
    }
  }
  last = millis();
}
