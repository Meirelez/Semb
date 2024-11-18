#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#define scl 5
#define sda 4
#define clk 14
#define init 12

#define MCP1_ADDRESS 0x24
#define MCP2_ADDRESS 0x26
#define MCP3_ADDRESS 0x27

Adafruit_MCP23X17 MCP1,MCP2,MCP3;

void mcp_pinmode();
void hc_writeb(uint8_t b);
void mcp_clear();

void setup() {
  Wire.begin();
  Serial.begin(9600);
  // put your setup code here, to run once:
 
  if (MCP1.begin_I2C(MCP1_ADDRESS))
    Serial.printf("ok: mcp %d\n", 1);
  else
    Serial.printf("error: mcp %d\n", 1);
  
  if (MCP2.begin_I2C(MCP2_ADDRESS))
    Serial.printf("ok: mcp %d\n", 2);
  else
    Serial.printf("error: mcp %d\n", 2);
  
  if (MCP3.begin_I2C(MCP3_ADDRESS))
    Serial.printf("ok: mcp %d\n", 3);
  else
    Serial.printf("error: mcp %d\n", 3);


  mcp_pinmode();
  Serial.println("mcp pinMode");
  pinMode(clk, OUTPUT);
  pinMode(init, OUTPUT);
  Serial.println("hc pinMode");
  digitalWrite(clk, HIGH);

  mcp_clear();
  Serial.println("mcp clear");

  hc_writeb(~0);
  hc_writeb(~0);
  Serial.println("hc write");
  //hc_writeb(~(1<<7));

  MCP1.digitalWrite(8, LOW);
  Serial.println("mcp write");
}

void loop() {
  Serial.print(".");
  // put your main code here, to run repeatedly:
  digitalWrite(init, LOW);
  digitalWrite(clk, LOW);
  delayMicroseconds(1000);
  digitalWrite(clk, HIGH);
  delayMicroseconds(500000);
  for (int i = 0; 1 < 15; i++) {
    digitalWrite(init, HIGH);
    digitalWrite(clk, LOW);
    delayMicroseconds(1000);
    digitalWrite(clk, HIGH);
    delayMicroseconds(500000);
  }
}

void mcp_pinmode() {
  MCP1.pinMode(0, INPUT);
  MCP1.pinMode(1, INPUT);
  MCP1.pinMode(14, INPUT);
  MCP1.pinMode(15, INPUT);

  MCP2.pinMode(0, INPUT);
  MCP2.pinMode(1, INPUT);
  MCP2.pinMode(14, INPUT);
  MCP2.pinMode(15, INPUT);

  MCP3.pinMode(0, INPUT);
  MCP3.pinMode(1, INPUT);
  MCP3.pinMode(14, INPUT);
  MCP3.pinMode(15, INPUT);

  MCP1.pinMode(2, OUTPUT);
  MCP1.pinMode(3, OUTPUT);
  MCP1.pinMode(4, OUTPUT);
  MCP1.pinMode(5, OUTPUT);
  MCP1.pinMode(6, OUTPUT);
  MCP1.pinMode(7, OUTPUT);
  MCP1.pinMode(8, OUTPUT);
  MCP1.pinMode(9, OUTPUT);
  MCP1.pinMode(10, OUTPUT);
  MCP1.pinMode(11, OUTPUT);
  MCP1.pinMode(12, OUTPUT);
  MCP1.pinMode(13, OUTPUT);

  MCP2.pinMode(2, OUTPUT);
  MCP2.pinMode(3, OUTPUT);
  MCP2.pinMode(4, OUTPUT);
  MCP2.pinMode(5, OUTPUT);
  MCP2.pinMode(6, OUTPUT);
  MCP2.pinMode(7, OUTPUT);
  MCP2.pinMode(8, OUTPUT);
  MCP2.pinMode(9, OUTPUT);
  MCP2.pinMode(10, OUTPUT);
  MCP2.pinMode(11, OUTPUT);
  MCP2.pinMode(12, OUTPUT);
  MCP2.pinMode(13, OUTPUT);

  MCP3.pinMode(2, OUTPUT);
  MCP3.pinMode(3, OUTPUT);
  MCP3.pinMode(4, OUTPUT);
  MCP3.pinMode(5, OUTPUT);
  MCP3.pinMode(6, OUTPUT);
  MCP3.pinMode(7, OUTPUT);
  MCP3.pinMode(8, OUTPUT);
  MCP3.pinMode(9, OUTPUT);
  MCP3.pinMode(10, OUTPUT);
  MCP3.pinMode(11, OUTPUT);
  MCP3.pinMode(12, OUTPUT);
  MCP3.pinMode(13, OUTPUT);
}

void hc_writeb(uint8_t b) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(init, b&(0x1<<i));
    digitalWrite(clk, LOW);
    delayMicroseconds(1000);
    digitalWrite(clk, HIGH);
  }
}

void mcp_clear() {
  MCP1.digitalWrite(2, HIGH);
  MCP1.digitalWrite(3, HIGH);
  MCP1.digitalWrite(4, HIGH);
  MCP1.digitalWrite(5, HIGH);
  MCP1.digitalWrite(6, HIGH);
  MCP1.digitalWrite(7, HIGH);
  MCP1.digitalWrite(8, HIGH);
  MCP1.digitalWrite(9, HIGH);
  MCP1.digitalWrite(10, HIGH);
  MCP1.digitalWrite(11, HIGH);
  MCP1.digitalWrite(12, HIGH);
  MCP1.digitalWrite(13, HIGH);

  MCP2.digitalWrite(2, HIGH);
  MCP2.digitalWrite(3, HIGH);
  MCP2.digitalWrite(4, HIGH);
  MCP2.digitalWrite(5, HIGH);
  MCP2.digitalWrite(6, HIGH);
  MCP2.digitalWrite(7, HIGH);
  MCP2.digitalWrite(8, HIGH);
  MCP2.digitalWrite(9, HIGH);
  MCP2.digitalWrite(10, HIGH);
  MCP2.digitalWrite(11, HIGH);
  MCP2.digitalWrite(12, HIGH);
  MCP2.digitalWrite(13, HIGH);

  MCP3.digitalWrite(2, HIGH);
  MCP3.digitalWrite(3, HIGH);
  MCP3.digitalWrite(4, HIGH);
  MCP3.digitalWrite(5, HIGH);
  MCP3.digitalWrite(6, HIGH);
  MCP3.digitalWrite(7, HIGH);
  MCP3.digitalWrite(8, HIGH);
  MCP3.digitalWrite(9, HIGH);
  MCP3.digitalWrite(10, HIGH);
  MCP3.digitalWrite(11, HIGH);
  MCP3.digitalWrite(12, HIGH);
  MCP3.digitalWrite(13, HIGH);
}
