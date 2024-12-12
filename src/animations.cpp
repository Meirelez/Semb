#include <Arduino.h>
#include "animations.h"

extern int grid[6][6][6];

void cube()
{
  int size = 0;
  int d = 1;

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(200);
  BaseType_t xWasDelayed;

  clear_grid();

  size += d * 1;
  if (size == 3)
    d = -1;
  if (size == 0)
    d = 1;

  for(;;) {
    for (int x = 0; x < 6; x++)
    {
      for (int y = 0; y < 6; y++)
      {
        for (int z = 0; z < 6; z++)
        {
          int arestas = (x == (3 - size) || x == (size + 2)) ? 1 : 0;
          arestas += (y == (3 - size) || y == (size + 2)) ? 1 : 0;
          arestas += (z == (3 - size) || z == (size + 2)) ? 1 : 0;

          if (x < (3 - size) || x > (size + 2))
            arestas = 0;
          if (y < (3 - size) || y > (size + 2))
            arestas = 0;
          if (z < (3 - size) || z > (size + 2))
            arestas = 0;

          grid[x][y][z] = size > 0 ? (arestas > 1 ? HIGH : LOW) : LOW;
        }
      }
    }
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void ligarArestas()
{
  int N = 6;
  // Ativar as arestas no eixo X
  for (int y = 0; y < N; y++)
  {
    for (int z = 0; z < N; z = z + 5)
    {
      grid[0][y][z] = HIGH;     // Aresta 1: x=0, variando y e z
      grid[N - 1][y][z] = HIGH; // Aresta 2: x=5, variando y e z
    }
  }

  // Ativar as arestas no eixo Y
  for (int x = 0; x < N; x++)
  {
    for (int z = 0; z < N; z = z + 5)
    {
      grid[x][0][z] = HIGH;     // Aresta 3: y=0, variando x e z
      grid[x][N - 1][z] = HIGH; // Aresta 4: y=5, variando x e z
    }
  }

  // Ativar as arestas no eixo Z
  for (int x = 0; x < N; x++)
  {
    for (int y = 0; y < N; y = y + 5)
    {
      grid[x][y][0] = HIGH;     // Aresta 5: z=0, variando x e y
      grid[x][y][N - 1] = HIGH; // Aresta 6: z=5, variando x e y
    }
  }
}

void rain()
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(200);
  BaseType_t xWasDelayed;

  clear_grid();
  for (int x = 0; x < 6; x++)
  {
    for (int y = 0; y < 6; y++)
    {
      grid[x][y][5] = HIGH; // 1 camada sempre ligada
    }
  }

  for(;;) {
    for (int x = 0; x < 6; x++)
      for (int y = 0; y < 6; y++)
        for (int z = 0; z < 5; z++)
          grid[x][y][z] = z != 4 ? grid[x][y][z + 1] : rand() % 100 < 10; // 20% probability of turning on
    
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

}
void firework()
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(200);
  BaseType_t xWasDelayed;

  int state = LOW;
  int radius = 1;
  int dx, dy, dz;

  clear_grid();

  for(;;) {
    int centerX = rand() % 6;
    int centerY = rand() % 6;
    int centerZ = rand() % 6;

    grid[centerX][centerY][0] = HIGH; // liga um led aleatorio da camada z=0

    int z;
    for (int x = 0; x < 6; x++)
    {
      for (int y = 0; y < 6; y++)
      {
        for (z = 0; z < centerZ; z++)
        {
          grid[x][y][z + 1] = grid[x][y][z]; // camada de cima = a de baixo
          grid[x][y][z] = LOW;               // camada de baixo= low next
        }
      }
    }
    if (z == centerZ) // camada do centro da explosao
    {
      for (radius = 1; radius < 4; radius++)
      {
        clear_grid();
        for (int x = 0; x < 6; x++)
        {
          for (int y = 0; y < 6; y++)
          {
            for (z = 0; z < 6; z++)
            {
              dx = x - centerX;
              dy = y - centerY;
              dz = z - centerZ;
              if ((dx * dx + dy * dy + dz * dz) <= radius * radius) //(x-x0)^2+(y-y0)^2+(z-z0)^2=raio^2 eq da esfera deveria adicionar alguma aleatoreiedade em roda da explosao?
              {
                switch (radius)
                {
                case 1:
                  grid[x][y][z] = HIGH;
                  break;
                case 2:
                  grid[x][y][z] = rand() % 100 < 50;
                  break;
                case 3:
                  grid[x][y][z] = rand() % 100 < 10;
                  break;
                }
              }
            }
          }
        }
      }
    }

    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}
void firework2()
{
  static unsigned long last = 0;
  static int state = LOW;
  static int centerX = 0, centerY = 0, centerZ = 0;
  static int step = 0;           // Tracks the expansion step
  static int directions[6][3] = {// Predefined directions for palm trails
                                 {1, 0, 1},
                                 {-1, 0, 1},
                                 {1, 1, 1},
                                 {-1, -1, 1},
                                 {0, 1, 1},
                                 {0, -1, 1}};
  static int trail_lengths[6]; // Randomized trail lengths for each direction

  if (millis() - last < 200)
  { // Wait 0.2s since last run
    return;
  }
  last = millis();

  // Initial state: Set explosion center and configure trails
  if (state == LOW)
  {
    clear_grid();
    centerX = rand() % 6;
    centerY = rand() % 6;
    centerZ = rand() % 6;

    for (int i = 0; i < 6; i++)
    {                                    // Randomize trail lengths
      trail_lengths[i] = rand() % 3 + 2; // Length between 2 and 4
    }

    grid[centerX][centerY][centerZ] = HIGH; // Central point of explosion
    step = 1;                               // Reset step for expansion
    state = HIGH;                           // Transition to expansion state
    return;
  }

  // Expansion state: Create palm effect
  if (state == HIGH)
  {
    // Fade the grid slightly (older particles fade out)
    for (int x = 0; x < 6; x++)
    {
      for (int y = 0; y < 6; y++)
      {
        for (int z = 0; z < 6; z++)
        {
          if (grid[x][y][z] > 0)
          {
            grid[x][y][z]--; // Decrease brightness for fading
          }
        }
      }
    }

    // Add new particles for the current expansion step
    for (int d = 0; d < 6; d++)
    { // Loop through each trail direction
      int dx = directions[d][0];
      int dy = directions[d][1];
      int dz = directions[d][2];

      // Calculate particle positions along the trail
      for (int t = 0; t <= step; t++)
      {
        if (t > trail_lengths[d])
          break; // Stop if trail exceeds length

        int x = centerX + dx * t + (rand() % 3 - 1); // Add slight randomness
        int y = centerY + dy * t + (rand() % 3 - 1);
        int z = centerZ + dz * t - (step / 3); // Gravity effect (z drops over time)

        // Ensure the position is within bounds
        if (x >= 0 && x < 6 && y >= 0 && y < 6 && z >= 0 && z < 6)
        {
          grid[x][y][z] = 3; // Set brightness (max value for new particles)
        }
      }
    }

    step++; // Increment the expansion step
    if (step > 4)
    { // End the palm effect after a few steps
      clear_grid();
      state = LOW; // Reset to initial state
    }
  }
}
void initial()
{

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(200);
  BaseType_t xWasDelayed;

  for (int j = 0; j < 2; j++)
  {
    int x;
    for (j == 0 ? x = 5 : x = 0; j == 0 ? x >= 0 : x < 5; j == 0 ? x-- : x++)
    {
      for (int y = 0; y < 6; y++)
      {
        for (int z = 0; z < 6; z++)
        {
          if (j == 0)
          {
            if (x == 5)
            {
              grid[x][y][z] = HIGH;
            }
            grid[x][y][z] = grid[x + 1][y][z];
            grid[x + 1][y][z] = LOW;
          }
          else
          {
            grid[x + 1][y][z] = grid[x][y][z];
            grid[x][y][z] = LOW;
          }
        }
      }
    }
    clear_grid();
  }
}