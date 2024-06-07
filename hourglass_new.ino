#include "bitmaps.h"
#include <Adafruit_SSD1306.h>

#define SAND_TIME 2000

#define swap(a, b) (a ^= b ^= a ^= b)

Adafruit_SSD1306 display(128, 64, &Wire, -1);

typedef struct {
  unsigned long time_remaining;
  unsigned long last_updated;
  unsigned long sand_timer;
  int sand_let_through;
} Timer;

void create_timer(Timer *timer, int seconds) {
  timer->time_remaining = seconds * 1000;
  timer->last_updated = millis();
  timer->sand_timer = 0;
  timer->sand_let_through = 0;
}

void update_timer(Timer *timer) {
  unsigned long elapsed = millis();
  timer->last_updated = elapsed;
  timer->time_remaining -= elapsed;
  timer->sand_timer += elapsed;
  if (timer->sand_timer >= SAND_TIME) {
    timer->sand_let_through++;
  }
}

Timer timer;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  draw_hourglass(&display);
  start_timer(&display, &timer, 60);
}

void loop() {
  update_sim(&display);
  display.display();
  
  update_timer(&timer);
}

bool is_valid(Adafruit_SSD1306 *display, int x, int y) {
  if (x < 0 || x >= display->width() || y < 0 || y >= display->height()) {
    return false;
  }

  int width = display->width();
  int height = display->height();
  if (display->getRotation() / 2) {
    x = display->width() - x - 1;
  }
  if (!(display->getRotation() % 2)) {
    swap(x, y);
    swap(width, height);
  }

  int index = y * width + x;
  return !((pgm_read_byte(&(bitmap_valid_pixels[index / 8])) << (index % 8)) &
           0x80);
}

void draw_hourglass(Adafruit_SSD1306 *display) {
  display->clearDisplay();
  display->drawBitmap(0, 0, bitmap_hourglass_texture, 64, 128, true);
}

void fill_hourglass(Adafruit_SSD1306 *display, int sand_count) {
  for (int y = 63; y >= 0; y--) {
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

void update_sand(Adafruit_SSD1306 *display, int x, int y) {
  static int neighs[3][2] = {{0, 1}, {-1, 1}, {1, 1}};

  if (!is_valid(display, x, y) || !display->getPixel(x, y)) {
    return;
  }
  for (int i = 0; i < 3; i++) {
    int nx = x + neighs[i][0], ny = y + neighs[i][1];
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

void update_sim(Adafruit_SSD1306 *display) {
  for (int y = display->height() - 1; y >= 0; y--) {
    for (int x = display->width() - 1; x >= 0; x--) {
      update_sand(display, x, y);
    }
  }
}

void start_timer(Adafruit_SSD1306 *display, Timer *timer, int seconds) {
  display->setRotation(1);
  display->clearDisplay();
  draw_hourglass(display);

  create_timer(timer, seconds);
  fill_hourglass(display, seconds * (SAND_TIME / 1000));
}
