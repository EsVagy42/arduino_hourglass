#include "timer_draw.h"

bool draw_remaining_time(Adafruit_SSD1306 *display, Timer *timer) {
  display->fillRect(MINUTES_DIGIT_POS_X, MINUTES_DIGIT_POS_Y,
                    MINUTES_DIGIT_WIDTH, MINUTES_DIGIT_HEIGHT, false);
  display->fillRect(SECONDS_DIGIT_POS_X, SECONDS_DIGIT_POS_Y,
                    SECONDS_DIGIT_WIDTH, SECONDS_DIGIT_HEIGHT, false);

  long time_remaining = abs((long)timer->time_remaining / 1000L);
  display->setCursor(MINUTES_DIGIT_POS_X, MINUTES_DIGIT_POS_Y);
  display->print((int)(time_remaining / 60L));
  display->setCursor(SECONDS_DIGIT_POS_X, SECONDS_DIGIT_POS_Y);
  display->print((int)(time_remaining % 60L));
}
