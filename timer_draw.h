#ifndef TIMER_DRAW_H
#define TIMER_DRAW_H

#include <Adafruit_SSD1306.h>
#include "timer.h"

#define MINUTES_DIGIT_POS_X 4
#define MINUTES_DIGIT_POS_Y 60
#define MINUTES_DIGIT_WIDTH 16
#define MINUTES_DIGIT_HEIGHT 8
#define SECONDS_DIGIT_POS_X 44
#define SECONDS_DIGIT_POS_Y 60
#define SECONDS_DIGIT_WIDTH 16
#define SECONDS_DIGIT_HEIGHT 8

bool draw_remaining_time(Adafruit_SSD1306 *display, Timer *timer);

#endif
