/*
   SSD_1306.h - Library for controlling a 128x64 SSD1306 OLED
   Created June 28, 2020
   MIT License
*/

#ifndef SSD1306_I2C_A_H
#define SSD1306_I2C_A_H

#include "Arduino.h"
#include "Wire.h"
#include "font.h"

// Constants
#define FRAMES_2   0b111
#define FRAMES_3   0b100
#define FRAMES_4   0b101
#define FRAMES_5   0b000
#define FRAMES_25  0b110
#define FRAMES_64  0b001
#define FRAMES_128 0b010
#define FRAMES_256 0b011

#define RIGHT false
#define LEFT  true

#define UP   false
#define DOWN true

class SSD1306 {
private:
  uint8_t _displayBuf[1024]; // Display buffer
  uint8_t _addr;         // I2C address
  uint8_t _curX = 0, _curY = 0;
  uint8_t _curFront        = 0;
  uint8_t _textSize        = 1;
  void _drawCircleBase(int x0, int y0, int x, int y, bool color);
  void _fillCircleBase(int x0, int y0, int x, int y, bool color);
  void _printBase(String s);

public:
  // Initialization and control methods
  void begin(uint8_t addr);
  void sendCommand(uint8_t x);
  void setInvert(bool invert);
  void setState(bool on);
  void displayAllOn();
  void resumeDisplay();
  void updateDisplay();
  // Scrolling
  void startScroll();
  void stopScroll();
  void setupScrollH(bool dir, uint8_t start, uint8_t end, uint8_t interval);
  void setupScrollHV(bool dirX, bool dirY, uint8_t start, uint8_t end, uint8_t interval);
  // Graphics
  void clear();
  void drawPixel(int x, int y, bool color);
  void drawLine(int x0, int y0, int x1, int y1, bool color);
  void drawAngledLine(int x, int y, int length, int angle, bool color);
  void drawRect(int x0, int y0, int w, int h, bool color);
  void drawRectByCoords(int x0, int y0, int x1, int y1, bool color);
  void fillRect(int x0, int y0, int w, int h, bool color);
  void fillRectByCoords(int x0, int y0, int x1, int y1, bool color);
  void drawCircle(int x, int y, int r, bool color);
  void fillCircle(int x, int y, int r, bool color);
  void displayBitmapImage(const unsigned char *img, int x, int y, int w, int h);
  // Text
  void setCursor(int x, int y);
  void setTextSize(int size);
  template <typename T>
  void print(T s);
  void print(float s, int nplaces);
  void print(double s, int nplaces);
  template <typename T>
  void println(T s);
  void println(float s, int nplaces);
  void println(double s, int nplaces);
};

// print() and println() definitions (doesn't work if placed in cpp file)
// Templates are used for more efficient overloading
template <typename T>
void SSD1306::print(T s) {
  _printBase(String(s));
}

template <typename T>
void SSD1306::println(T s) {
  print(String(s));
  print('\n');
}

#endif