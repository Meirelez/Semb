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

    size += d * 1;
    if (size == 3)
        d = -1;
    if (size == 0)
        d = 1;

    for (;;) {
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
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void rain() {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    BaseType_t xWasDelayed;

    grid_clear();
	grid_setZ(5, HIGH);

    for (;;) {
        for (int x = 0; x < 6; x++)
            for (int y = 0; y < 6; y++)
                for (int z = 0; z < 5; z++)
                    grid_set(x, y, z, z != 4 ? grid_get(x, y, z+1) : rand() % 100 < 10);  // 10% probability of turning on

        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void initial() {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    BaseType_t xWasDelayed;

	grid_clear();

	for (int n = 0; n < 3; n++) {
		auto f = (void(*[])(int, int)){grid_setX, grid_setY, grid_setZ}[n];
		f(0, HIGH);
		delay(500);
		for (int m = 1; m < 6; m++) {
			f(m-1, LOW);
			f(m, HIGH);
			delay(500);
		}
		f(5, LOW);
		delay(2000);
	}

	grid_clear();
}