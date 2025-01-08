#include <Arduino.h>

#include "animations.h"
#include "grid.h"

void cube() {
    int size = 0;
    int d = 1;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    BaseType_t xWasDelayed;

    grid_clear();
    unsigned long prev = micros(), average=0;
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    for (;;) {
        size += d * 1;
        if (size == 3)
            d = -1;
        if (size == 0)
            d = 1;

        for (int x = 0; x < 6; x++) {
            for (int y = 0; y < 6; y++) {
                for (int z = 0; z < 6; z++) {
                    int arestas = (x == (3 - size) || x == (size + 2)) ? 1 : 0;
                    arestas += (y == (3 - size) || y == (size + 2)) ? 1 : 0;
                    arestas += (z == (3 - size) || z == (size + 2)) ? 1 : 0;

                    if (x < (3 - size) || x > (size + 2))
                        arestas = 0;
                    if (y < (3 - size) || y > (size + 2))
                        arestas = 0;
                    if (z < (3 - size) || z > (size + 2))
                        arestas = 0;

                    grid_set(x, y, z, size > 0 ? (arestas > 1 ? HIGH : LOW) : LOW);
                }
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
            Serial.printf("Cube exec time: %lu,max: %d, min: %d\n",average,max,min);
            return;
            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        prev = micros();
    }
}

void rain() {
    unsigned long prev = micros(), average=0;
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    BaseType_t xWasDelayed;

    grid_clear();
   // grid_setZ(5, HIGH);

    for (;;) {
        for (int x = 0; x < 6; x++)
            for (int y = 0; y < 6; y++)
                for (int z = 0; z < 6; z++)
                    grid_set(x, y, z, z != 5 ? grid_get(x, y, z + 1) : rand() % 100 < 10);  // 10% probability of turning on
        

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
            Serial.printf("Rain exec time: %lu,max: %d, min: %d\n",average,max,min);
            stop = 1;
            return;
            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        prev = micros();
    }
}

void firework() {
    unsigned long prev = micros(), average=0;
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    BaseType_t xWasDelayed;

    int state = LOW;
    int radius = 1;
    int dx, dy, dz;

    grid_clear();

    for (;;) {
        int centerX = rand() % 6;
        int centerY = rand() % 6;
        int centerZ = rand() % 6;

        //grid_set(centerX, centerY, 0, HIGH);  // liga um led aleatorio da camada z=0~
        grid_set(centerX, centerY, 0, HIGH);

        int z;
        for (int x = 0; x < 6; x++) {
            for (int y = 0; y < 6; y++) {
                for (z = 0; z < centerZ; z++) {
                    grid_set(x, y, z + 1, grid_get(x, y, z));  // camada de cima = a de baixo
                    grid_set(x, y, z, LOW);                // camada de baixo= low next
                }
            }
        }
        if (z == centerZ)  // camada do centro da explosao
        {
            for (radius = 1; radius < 4; radius++) {
                grid_clear();
                for (int x = 0; x < 6; x++) {
                    for (int y = 0; y < 6; y++) {
                        for (z = 0; z < 6; z++) {
                            dx = x - centerX;
                            dy = y - centerY;
                            dz = z - centerZ;
                            if ((dx * dx + dy * dy + dz * dz) <= radius * radius)  //(x-x0)^2+(y-y0)^2+(z-z0)^2=raio^2 eq da esfera deveria adicionar alguma aleatoreiedade em roda da explosao?
                            {
                                switch (radius) {
                                    case 1:
                                        grid_set(x, y, z, HIGH);
                                        break;
                                    case 2:
                                        grid_set(x, y, z, rand() % 100 < 50);
                                        break;
                                    case 3:
                                        grid_set(x, y, z, rand() % 100 < 10);
                                        break;
                                }
                            }
                        }
                    }
                }
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
            Serial.printf("Firework exec time: %lu,max: %d, min: %d\n",average,max,min);
            stop = 1;
            return;
            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        prev = micros();
    }
}

void firework2() {
    unsigned long prev = micros(), average=0;
    int n = 500, cont = 0, stop =0;
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
    static int trail_lengths[6];  // Randomized trail lengths for each direction

    if (millis() - last < 200) {  // Wait 0.2s since last run
        return;
    }
    last = millis();
}

void initial() {
    unsigned long prev = micros(), average=0;
    int n = 500, cont = 0, stop =0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    BaseType_t xWasDelayed;

	grid_clear();

	for (int n = 0; n < 3; n++) {
        auto f = (void (*[])(int, int)){grid_setX, grid_setY, grid_setZ}[n];
		f(0, HIGH);
		delay(200);
		for (int m = 1; m < 6; m++) {
            f(m - 1, LOW);
			f(m, HIGH);
			delay(200);
		}
		for (int m = 5; m > 0; m--) {
            f(m, LOW);
            f(m - 1, HIGH);
            delay(200);
        }
        f(0, LOW);
		delay(1000);
	}

	grid_clear();
}

void cube_fixed_vertice()
{
    unsigned long prev = micros(), average=0;
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    int GRID_SIZE = 6;
    int size = 0;        // Current size of the cube
    int nvertice = 0;    // Current vertex index
    int d = 1;           // Direction of size change (1 = expanding, -1 = shrinking)
    // Define the 8 vertices of the cube
    int vertices[8][3] = {
        {0, 0, 0}, {5, 0, 0}, {0, 5, 0}, {0, 0, 5},
        {5, 5, 0}, {5, 0, 5}, {0, 5, 5}, {5, 5, 5}};
    // Initialize timing variables for FreeRTOS
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200); // Delay between frames
    BaseType_t xWasDelayed;

    // Clear the grid initially
    grid_clear();

    for (;;)
    {
        // Get the current vertex coordinates
        int vx = vertices[nvertice][0];
        int vy = vertices[nvertice][1];
        int vz = vertices[nvertice][2];

        // Clear the grid for the new frame
        grid_clear();

        // Render the cube relative to the fixed vertex
        for (int x = vx - size; x <= vx + size; x++)
        {
            for (int y = vy - size; y <= vy + size; y++)
            {
                for (int z = vz - size; z <= vz + size; z++)
                {
                    // Check if the coordinates are within grid bounds
                    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && z >= 0 && z < GRID_SIZE)
                    {
                        // Check if the current point lies on any edge of the cube
                        bool isEdge = false;
                        
                        // Point is on an edge if:
                        // 1. One coordinate is at its extreme (min or max)
                        // 2. The other two coordinates are at their extremes OR between extremes
                        if (x == vx - size || x == vx + size)
                        {
                            if (y >= vy - size && y <= vy + size && 
                                (z == vz - size || z == vz + size)) isEdge = true;
                            if (z >= vz - size && z <= vz + size && 
                                (y == vy - size || y == vy + size)) isEdge = true;
                        }
                        if (y == vy - size || y == vy + size)
                        {
                            if (x >= vx - size && x <= vx + size && 
                                (z == vz - size || z == vz + size)) isEdge = true;
                            if (z >= vz - size && z <= vz + size && 
                                (x == vx - size || x == vx + size)) isEdge = true;
                        }
                        if (z == vz - size || z == vz + size)
                        {
                            if (x >= vx - size && x <= vx + size && 
                                (y == vy - size || y == vy + size)) isEdge = true;
                            if (y >= vy - size && y <= vy + size && 
                                (x == vx - size || x == vx + size)) isEdge = true;
                        }

                        if (isEdge)
                        {
                            grid_set(x, y, z, HIGH);
                        }
                    }
                }
            }
        }

        // Update the cube size
        size += d;

        // Handle size limits
        if (size == 5)
        {
            d = -1; // Start shrinking
        }
        else if (size == 0)
        {
            d = 1; // Start expanding
            nvertice = (nvertice + 1) % 8; // Move to the next vertex
        }

        // Delay until the next frame
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
            Serial.printf("Firework exec time: %lu,max: %d, min: %d\n",average,max,min);
            stop = 1;
            return;
            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        prev = micros();
    }
}
