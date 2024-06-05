#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);


void setup() {
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(1);

  draw_hourglass(&display);
}

void loop() {
  // put your main code here, to run repeatedly:

  display.drawPixel(10, 11, true);
  update_sim(&display);
  
  display.display();

  delay(10);
}

bool is_valid(Adafruit_SSD1306 *display, int x, int y) {
  if (x < 0 || x >= display->width() || y < 0 || y >= display->height()) {
    return false;
  }

  int sub = x + 32 - y;
  bool diag1 = (sub <= 0);
  int add = x - 32 + y;
  bool diag2 = (add >= 64);

  bool hole = (x == 32);
  return (hole || !(diag1 ^ diag2));
}

void draw_hourglass(Adafruit_SSD1306 *display) {
  for (int y = 0; y < display->height(); y++) {
    for (int x = 0; x < display->width(); x++) {
      int x_diff = (y < 64 ? 33 : 31);
      
      int sub = x + x_diff - y;
      bool diag1 = (sub == 0);
      int add = x - x_diff + y;
      bool diag2 = (add == 64);

      bool hole = (x == 32);

      display->drawPixel(x, y, (!hole && (diag1 || diag2)));
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
