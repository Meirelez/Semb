#pragma once

#include <stdint.h>

void grid_clear();
void grid_clearX(int n);
void grid_clearY(int n);
void grid_clearZ(int n);

int grid_get(int x, int y, int z);
void grid_set(int x, int y, int z, int v);

int grid_getX(int n);
int grid_getY(int n);
int grid_getZ(int n);

uint8_t grid_getZY(int z, int y);

void grid_setX(int n, int v);
void grid_setY(int n, int v);
void grid_setZ(int n, int v);
