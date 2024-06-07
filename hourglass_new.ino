#include "bitmaps.h"
#include <Adafruit_SSD1306.h>

#define SAND_TIME 2000

#define MINUTES_DIGIT_POS_X 4
#define MINUTES_DIGIT_POS_Y 60
#define MINUTES_DIGIT_WIDTH 16
#define MINUTES_DIGIT_HEIGHT 8
#define SECONDS_DIGIT_POS_X 44
#define SECONDS_DIGIT_POS_Y 60
#define SECONDS_DIGIT_WIDTH 16
#define SECONDS_DIGIT_HEIGHT 8

#define swap(a, b) (a ^= b ^= a ^= b)

int opening_pos[2] = {32, 63};

Adafruit_SSD1306 display(128, 64, &Wire, -1);

typedef struct {
  unsigned long time_remaining;
  unsigned long last_updated;
  unsigned long sand_timer;
  int sand_let_through;
} Timer;

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

Timer timer;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.setRotation(1);
  draw_hourglass(&display);
  display.setTextColor(true);
  start_timer(&display, &timer, 60);
}

void loop() {
  update_sim(&display, &timer, 0);
  draw_remaining_time(&display, &timer);
  display.display();

  update_timer(&timer);
}

bool is_valid(Adafruit_SSD1306 *display, int x, int y) {
  if (x < 0 || x >= display->width() || y < 0 || y >= display->height()) {
    return false;
  }

  int index = y * display->width() + x;
  return !((pgm_read_byte(&(bitmap_valid_pixels[index / 8])) << (index % 8)) &
           0x80);
}

void draw_hourglass(Adafruit_SSD1306 *display) {
  display->clearDisplay();
  display->drawBitmap(0, 0, bitmap_hourglass_texture, 64, 128, true);
}

void fill_hourglass(Adafruit_SSD1306 *display, int sand_count) {
  for (int y = display->height() / 2 - 1; y >= 0; y--) {
    for (int x = display->width(); x >= 0; x--) {
      if (sand_count == 0) {
        return;
      }

      if (is_valid(display, x, y)) {
        display->drawPixel(x, y, true);
        sand_count--;
      }
    }
  }
}

void update_sand(Adafruit_SSD1306 *display, int x, int y, int rotation) {
  static int neighs[4][3][2] = {{{0, 1}, {-1, 1}, {1, 1}},
                                {{1, 0}, {1, 1}, {1, -1}},
                                {{0, -1}, {1, -1}, {-1, -1}},
                                {{-1, 0}, {-1, -1}, {-1, 1}}};

  if (!is_valid(display, x, y) || !display->getPixel(x, y)) {
    return;
  }
  for (int i = 0; i < 3; i++) {
    int nx = x + neighs[rotation][i][0], ny = y + neighs[rotation][i][1];
    if (!is_valid(display, nx, ny)) {
      continue;
    }
    if (!display->getPixel(nx, ny)) {
      display->drawPixel(nx, ny, true);
      display->drawPixel(x, y, false);
      return;
    }
  }
}

void update_sim(Adafruit_SSD1306 *display, Timer *timer, int rotation) {
  for (int y = display->height() - 1; y >= 0; y--) {
    for (int x = display->width() - 1; x >= 0; x--) {
      if (!(x == opening_pos[0] && y == opening_pos[1]) ||
          is_opening_open(display, timer)) {
        update_sand(display, x, y, rotation);
      }
    }
  }
}

void start_timer(Adafruit_SSD1306 *display, Timer *timer, int seconds) {
  draw_hourglass(display);

  create_timer(timer, seconds);
  fill_hourglass(display, seconds / (SAND_TIME / 1000) + 1);
}

bool is_opening_open(Adafruit_SSD1306 *display, Timer *timer) {
  bool opening_pixel = display->getPixel(opening_pos[0], opening_pos[1]);
  if (!opening_pixel) {
    return true;
  }

  if (timer->sand_let_through) {
    timer->sand_let_through--;
    return true;
  }

  return false;
}

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
