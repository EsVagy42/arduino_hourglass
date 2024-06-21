#include "bitmaps.h"
#include "timer_draw.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050_modified.h>

#define swap(a, b) (a ^= b ^= a ^= b)
#define OPENING_POS_X 32
#define OPENING_POS_Y 63

Adafruit_MPU6050 gyro;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

Timer timer;

int random_update_pos[64];

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.setRotation(1);
  draw_hourglass(&display);
  display.setTextColor(true);
  start_timer(&display, &timer, 60 * 60);

  for (int i = 0; i < 64; i++) {
    random_update_pos[i] = i;
  }
}

void loop() {
  randomize_updates(random_update_pos);
  update_sim(&display, &timer, 0, random_update_pos);
  draw_remaining_time(&display, &timer);

  sensors_event_t acc;
  gyro.getEvent(&acc);
  display.setCursor(0, 0);

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
  static char neighs[4][3][2] = {{{0, 1}, {-1, 1}, {1, 1}},
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

void update_sim(Adafruit_SSD1306 *display, Timer *timer, int rotation,
                int *random_pos) {
  for (int y = 0; y < display->height(); y++) {
    for (int *x = random_pos; x < random_pos + 64; x++) {
      int update_x, update_y;
      switch (rotation) {
        case 0:
          update_y = display->height() - y - 1;
          update_x = *x;
          break;

        case 1:
          update_y = y / 64 ? *x + 64 : *x;
          update_x = y % 64;
          update_x = 64 - update_x - 1;
          break;

        case 2:
          update_x = *x;
          update_y = y;
          break;

        case 3:
          update_y = y / 64 ? *x + 64 : *x;
          update_x = y % 64;
          break;
      }
      if (!(update_x == OPENING_POS_X && update_y == OPENING_POS_Y) ||
          is_opening_open(display, timer)) {
        update_sand(display, update_x, update_y, rotation);
      }
    }
  }
}

void randomize_updates(int *positions) {
  for (int i = 0; i < 64; i++) {
    int a = random(0, 64);
    int b = random(0, 64);
    int temp = positions[a];
    positions[a] = positions[b];
    positions[b] = temp;
  }
}

void start_timer(Adafruit_SSD1306 *display, Timer *timer, int seconds) {
  draw_hourglass(display);

  create_timer(timer, seconds);
  fill_hourglass(display, seconds / (SAND_TIME / 1000) + 1);
}

bool is_opening_open(Adafruit_SSD1306 *display, Timer *timer) {
  bool opening_pixel = display->getPixel(OPENING_POS_X, OPENING_POS_Y);
  if (!opening_pixel) {
    return true;
  }

  if (timer->sand_let_through) {
    timer->sand_let_through--;
    return true;
  }

  return false;
}
