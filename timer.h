#ifndef TIMER_H
#define TIMER_H

#define SAND_TIME 2000

typedef struct {
  unsigned long time_remaining;
  unsigned long last_updated;
  unsigned long sand_timer;
  int sand_let_through;
} Timer;

void create_timer(Timer* timer, int seconds);

void update_timer(Timer *timer);

#endif
