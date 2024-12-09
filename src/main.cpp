#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#define scl 5
#define sda 4
#define clk 14
#define init 12

#define MCP_ADDRESS(x) ((int[]){0x24, 0x26, 0x27}[x])
#define MCP(x, y) ((int[]){0, 0, 1, 1, 2, 2}[x])
#define MCP_PIN(x, y) (y + ((x % 2 == 0) * 6) + 2)

Adafruit_MCP23X17 mcp[3];
int buttons[3][4];
volatile int grid[6][6][6] = {
    {
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
    },
    {
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
    },
    {
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
    },
    {
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
    },
    {
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
    },
    {
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
        {LOW, HIGH, LOW, HIGH, LOW, HIGH},
        {HIGH, LOW, HIGH, LOW, HIGH, LOW},
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
void rain();

void setup() {
  Wire.begin();
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

void inputs() {
  for (int i = 0; i < 3; i++) {
    buttons[i][0] = mcp[i].digitalRead(0);
    buttons[i][1] = mcp[i].digitalRead(1);
    buttons[i][2] = mcp[i].digitalRead(14);
    buttons[i][3] = mcp[i].digitalRead(15);
  }
}

void outputs() {
  uint16_t m[3] = {0};
  for (int z = 0; z < 6; z++) {
    m[0] = m[1] = m[2] = 0;
    for (int y = 0; y < 6; y++) {
      for (int x = 0; x < 6; x++) {
        m[MCP(x, y)] |= ((grid[x][y][z] & 0x1) << MCP_PIN(x, y));
      }
    }
    delay(1);
    hc_clock(LOW);
    for (int i = 0; i < 3; i++)
      mcp[i].writeGPIOAB(~m[i]);

    hc_clock(z != 0 ? LOW: HIGH);
  }
}

void clear_grid() {
  for (int x = 0; x < 6; x++)
    for (int y = 0; y < 6; y++)
      for (int z = 0; z < 6; z++)
        grid[x][y][z] = LOW;
}

void cube() {
  static unsigned long last = 0;
  static int size = 0;
  static int d = 1;

  if (millis() - last < 100) { // wait 0.1s since last run
    return;
  }

  if (last == 0) { // first run
    clear_grid();
    last = millis();
    return;
  }

  size += d*1;
  if (size == 3) d = -1;
  if (size == 0) d = 1;

  for (int x = 0; x < 6; x++) {
    for (int y = 0; y < 6; y++) {
      for (int z = 0; z < 6; z++) {
        int arestas = (x == (3-size) || x == (size+2)) ? 1 : 0;
        arestas += (y == (3-size) || y == (size+2)) ? 1 : 0;
        arestas += (z == (3-size) || z == (size+2)) ? 1 : 0;
        
        if (x < (3-size) || x > (size+2)) arestas = 0;
        if (y < (3-size) || y > (size+2)) arestas = 0;
        if (z < (3-size) || z > (size+2)) arestas = 0;

        grid[x][y][z] = size > 0 ? (arestas > 1 ? HIGH : LOW) : LOW;
      }
    }
  }

  last = millis();
}

void ligarArestas() {
  int N = 6;
  // Ativar as arestas no eixo X
  for (int y = 0; y < N; y++) {
    for (int z = 0; z < N; z = z + 5) {
      grid[0][y][z] = HIGH;     // Aresta 1: x=0, variando y e z
      grid[N - 1][y][z] = HIGH; // Aresta 2: x=5, variando y e z
    }
  }

  // Ativar as arestas no eixo Y
  for (int x = 0; x < N; x++) {
    for (int z = 0; z < N; z = z + 5) {
      grid[x][0][z] = HIGH;     // Aresta 3: y=0, variando x e z
      grid[x][N - 1][z] = HIGH; // Aresta 4: y=5, variando x e z
    }
  }

  // Ativar as arestas no eixo Z
  for (int x = 0; x < N; x++) {
    for (int y = 0; y < N; y = y + 5) {
      grid[x][y][0] = HIGH;     // Aresta 5: z=0, variando x e y
      grid[x][y][N - 1] = HIGH; // Aresta 6: z=5, variando x e y
    }
  }
}

void rain() {
  static unsigned long last = 0;
  static int state = LOW;

  if (millis() - last < 200) { // wait 0.5s since last run
    return;
  }
  if (last == 0) { // first run
    clear_grid();
    for (int x = 0; x < 6; x++) {
      for (int y = 0; y < 6; y++) {
        grid[x][y][5] = HIGH; //1 camada toda ligada
      }
    }
    last = millis();
    return;
  }
  static int z = 4;
  for (int x = 0; x < 6; x++) {
    for (int y = 0; y < 6; y++) {
      if (z == 4) {
        grid[x][y][4] = rand() % 2;
        grid[x][y][0] = LOW;
      }
      else {
        grid[x][y][z] = grid[x][y][z + 1];
        grid[x][y][z + 1] = LOW;
      }
    }
  }
  z--;
  if (z < 0)
    z = 4;
  last = millis();
}

void calc() {
  //cube();
  //ligarArestas();
  rain();
}