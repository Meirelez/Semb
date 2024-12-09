#include <Arduino.h>
#include "animations.h"

extern int grid[6][6][6];

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

  if (millis() - last < 200) { // wait 0.2s since last run
    return;
  }

  if (last == 0) { // first run
    clear_grid();
    for (int x = 0; x < 6; x++) {
      for (int y = 0; y < 6; y++) {
        grid[x][y][5] = HIGH; // 1 camada sempre ligada
      }
    }
    last = millis();
    return;
  }

  for (int x = 0; x < 6; x++)
    for (int y = 0; y < 6; y++)
      for (int z = 0; z < 5; z++)
        grid[x][y][z] = z != 4 ? grid[x][y][z + 1] : rand() % 100 < 10; // 20% probability of turning on

  last = millis();
}