#include <Arduino.h>
#include <vector>

#include "animations.h"
#include "grid.h"

extern int buttons[3][4];
extern int buttons_prev[3][4];
extern int buttons_press[3][4];
extern int buttons_release[3][4];

struct coords {
    int x, y, z;
};

static void snake_move(std::vector<struct coords> &snake, struct coords &dir);
static void snake_draw(std::vector<struct coords> &snake, struct coords food);

void snake() {
     unsigned long prev = 0, average=0;
    int n = 500, cont = 0, stop =0, max =0, min=1000000;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(400);
    BaseType_t xWasDelayed;

	grid_clear();

    int size = 1;
    struct coords dir = {1, 0, 0};

    std::vector<struct coords> snake = {{rand() % 6, rand() % 6, rand() % 6}};

    struct coords food = {rand() % 6, rand() % 6, rand() % 6};

    for(;;) {
        prev = micros();
        grid_clear();

        snake_move(snake, dir);

        if (snake[0].x == food.x && snake[0].y == food.y && snake[0].z == food.z) {
            size++;
            food = {rand() % 6, rand() % 6, rand() % 6};
            snake.insert(snake.end(), snake.back());
        }

        snake_draw(snake, food);

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
            Serial.printf("Snake exec time: %lu,max: %d, min: %d\n",average,max,min);
            stop = 1;
            }
        }
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        
    }
}

static void snake_move(std::vector<struct coords> &snake, struct coords &dir) {
    static int *left = &buttons_press[0][3], *right = &buttons_press[1][3], *up = &buttons_press[0][1], *down = &buttons_press[0][0], *forw = &buttons_press[2][1], *bckw = &buttons_press[2][0];

    struct coords head = snake[0];

    if (*left) {
        dir = {0};
        dir.x = -1;
        *left = 0;
    }
    if (*right) {
        dir = {0};
        dir.x = 1;
        *right = 0;
    }
    if (*up) {
        dir = {0};
        dir.z = 1;
        *up = 0;
    }
    if (*down) {
        dir = {0};
        dir.z = -1;
        *down = 0;
    }
    if (*forw) {
        dir = {0};
        dir.y = -1;
        *forw = 0;
    }
    if (*bckw) {
        dir = {0};
        dir.y = 1;
        *bckw = 0;
    }

    //if (!(*left || *right || *up || *down || *forw || *bckw))
    head.x += dir.x;  head.y += dir.y;  head.z += dir.z;
    if (head.x > 5) head.x = 0;
    if (head.x < 0) head.x = 5;
    if (head.y > 5) head.y = 0;
    if (head.y < 0) head.y = 5;
    if (head.z > 5) head.z = 0;
    if (head.z < 0) head.z = 5;


    snake.pop_back();

    snake.insert(snake.begin(), head);
}

static void snake_draw(std::vector<struct coords> &snake, struct coords food) {
    for (const auto &e : snake) {
        grid_set(e.x, e.y, e.z, HIGH);
    }
    grid_set(food.x, food.y, food.z, HIGH);
}
