#include "timer.h"


void create_timer(Timer *timer, int seconds) {
  timer->time_remaining = (unsigned long)seconds * 1000;
  timer->last_updated = millis();
  timer->sand_timer = SAND_TIME;
  timer->sand_let_through = 0;
}

void update_timer(Timer *timer) {
  unsigned long elapsed = millis();
  int delta_time = elapsed - timer->last_updated;
  timer->time_remaining -= delta_time;
  timer->sand_timer += delta_time;

  int change = timer->sand_timer / SAND_TIME;
  timer->sand_let_through += change;
  timer->sand_timer -= SAND_TIME * change;

  timer->last_updated = elapsed;
}
