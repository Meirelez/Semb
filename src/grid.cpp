#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define GRID_SIZE 6
static_assert(GRID_SIZE <= 8, "Grid size must not be greather than 8");

volatile static uint8_t grid[GRID_SIZE * GRID_SIZE];  // z 0->size:  y0 x:0b76543210   y1 x:0b76543210   y2 x:0b76543210   y3 x:0b76543210   y4 x:0b76543210   y5 x:0b76543210

void grid_clear() {
    memset((void*)grid, 0, sizeof(grid));
}

void grid_clearX(int n) {
    for (int z = 0; z < GRID_SIZE; z++)
        for (int y = 0; y < GRID_SIZE; y++)
            grid[z * GRID_SIZE + y] = grid[z * GRID_SIZE + y] & ~(1 << n);
}

void grid_clearY(int n) {
    for (int z = 0; z < GRID_SIZE; z++)
        grid[z * GRID_SIZE + n] = 0;
}

void grid_clearZ(int n) {
    memset((void*)&grid[n * GRID_SIZE], 0, GRID_SIZE);
}

int grid_get(int x, int y, int z) {
    return (grid[z * GRID_SIZE + y] & (1 << x)) >> x;
}

uint8_t grid_getZY(int z, int y) {
    return grid[z * GRID_SIZE + y] & ((1<<GRID_SIZE)-1);
}

void grid_set(int x, int y, int z, int v) {
    grid[z * GRID_SIZE + y] = grid[z * GRID_SIZE + y] & ~(1 << x) | ((v & 1) << x);
}

void grid_setX(int n, int v) {
    for (int z = 0; z < GRID_SIZE; z++)
        for (int y = 0; y < GRID_SIZE; y++)
            grid[z * GRID_SIZE + y] = grid[z * GRID_SIZE + y] & ~(1 << n) | ((v & 1) << n);
}

void grid_setY(int n, int v) {
    for (int z = 0; z < GRID_SIZE; z++)
        grid[z * GRID_SIZE + n] = v != 0 ? (1<<GRID_SIZE)-1 : 0;
}

void grid_setZ(int n, int v) {
    memset((void*)&grid[n * GRID_SIZE], v != 0 ? (1<<GRID_SIZE)-1 : 0, GRID_SIZE);
}
