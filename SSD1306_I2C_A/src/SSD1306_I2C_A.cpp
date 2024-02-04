/*
   SSD_1306.cpp - Library for controlling a 128x64 SSD1306 OLED
   Created June 28, 2020
   MIT License
*/

#include "Arduino.h"
#include "SSD1306_I2C_A.h"

void SSD1306::_drawCircleBase(int x0, int y0, int x, int y, bool color) {
  drawPixel(x0 + x, y0 + y, color);
  drawPixel(x0 - x, y0 + y, color);
  drawPixel(x0 + x, y0 - y, color);
  drawPixel(x0 - x, y0 - y, color);
  drawPixel(x0 + y, y0 + x, color);
  drawPixel(x0 - y, y0 + x, color);
  drawPixel(x0 + y, y0 - x, color);
  drawPixel(x0 - y, y0 - x, color);
}

void SSD1306::_fillCircleBase(int x0, int y0, int x, int y, bool color) {
  drawLine(x0 + x, y0 + y, x0 - x, y0 + y, color);
  drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);
  drawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
  drawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
}

void SSD1306::_printBase(String s) {
  // Print a string
  s.replace("\t", "    "); // Replace tabs with four spaces
  for (int i = 0; i < s.length(); i++) {
    if (s[i] != '\n' && _curX < 128 - (_textSize + 5)) {
      // Print the printing character
      uint8_t currentChar[5];
      int chrVal = int(s[i]) - 32;  // Convert char to ASCII value, subract 32 for offset
      for (int j = 0; j < 5; j++) { // Iterate through character data
        // Copy character data from progmem to RAM
        currentChar[j] = pgm_read_byte(&ASCII_5X7[chrVal][j]);
        for (int k = 0; k < 7 * _textSize; k++) { // Iterate through vertical pixels
          for (int l = 0; l < _textSize * _textSize; l++) {
            // This loop changes the character's size
            drawPixel(_curX + l % _textSize, _curY + k * _textSize + floor(l / _textSize), bitRead(currentChar[j], k)); // Draw the pixel
          }
        }
        _curX += _textSize; // Reposition cursor's X location
      }
      _curX += _textSize; // Padding between chars is proportional to font size
    }
    else {
      // New line, reposition cursor to next line
      setCursor(_curFront, _curY + _textSize * 8); // Change cursor position to new line
    }
  }
}

void SSD1306::begin(uint8_t addr) {
  _addr = addr;
  Wire.begin(); // Init I2C bus
  // Send initialization commands
  setState(false);                      // Display off while being configured
  sendCommand(0xD5); sendCommand(0x80); // Set display clock divide
  sendCommand(0xA8); sendCommand(0x3F); // Set multiplex ratio
  sendCommand(0xD3); sendCommand(0x00); // Set display offset
  sendCommand(0x40);                    // Set display start line
  sendCommand(0x8D); sendCommand(0x14); // Set charge pump setting
  sendCommand(0x20); sendCommand(0x00); // Set memory addressing mode
  sendCommand(0xA1);                    // Set segment re-map
  sendCommand(0xC8);                    // Set COM output scan direction
  sendCommand(0xDA); sendCommand(0x12); // Config COM pins
  sendCommand(0x81); sendCommand(0xCF); // Set contrast
  sendCommand(0xD9); sendCommand(0xF1); // Set pre-charge period
  sendCommand(0xDB); sendCommand(0x40); // Set VCOMH deselect level
  setInvert(false);                     // Display is not inverted
  clear();                              // Clear display
  updateDisplay();                      // Send buffer to display
  resumeDisplay();                      // RAM data is displayed
  setState(true);                       // Display on
}

void SSD1306::sendCommand(uint8_t x) {
  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  Wire.write(x);
  Wire.endTransmission();
}

void SSD1306::setInvert(bool invert) {
  if (invert) sendCommand(0xA7);
  else sendCommand(0xA6);
}

void SSD1306::setState(bool on) {
  if (on) sendCommand(0xAF);
  else sendCommand(0xAE);
}

void SSD1306::displayAllOn() {
  sendCommand(0xA5);
}

void SSD1306::resumeDisplay() {
  sendCommand(0xA4);
}
/*
void SSD1306::updateDisplay_A() {
  // Set column address to 0
  sendCommand(0x21);
  sendCommand(0);   // Column start address
  sendCommand(127); // Column end address
  // Set page address to 0
  sendCommand(0x22);
  sendCommand(0); // Start page address
  sendCommand(7); // End page address
  // Send display buffer
  for (int i = 0; i < 1024; i++) {
    Wire.beginTransmission(_addr);
    Wire.write(0x40);
    Wire.write(_displayBuf[i]);
    Wire.endTransmission();
  }
}
*/

void SSD1306::updateDisplay(void) {
  sendCommand(0x21);
  sendCommand(0);   // Column start address
  sendCommand(127); // Column end address
  // Set page address to 0
  sendCommand(0x22);
  sendCommand(0); // Start page address
  sendCommand(7); // End page address
  // ESP8266 needs a periodic yield() call to avoid watchdog reset.
  // With the limited size of SSD1306 displays, and the fast bitrate
  // being used (1 MHz or more), I think one yield() immediately before
  // a screen write and one immediately after should cover it.  But if
  // not, if this becomes a problem, yields() might be added in the
  // 32-byte transfer condition below.
  yield();
  uint16_t count = 1024;
  uint8_t *ptr = _displayBuf;
    Wire.beginTransmission(_addr);
    Wire.write((uint8_t)0x40);
    uint16_t bytesOut = 1;
    while (count--) {
        if (bytesOut >= 128) {
        Wire.endTransmission();
        Wire.beginTransmission(_addr);
        Wire.write((uint8_t)0x40);
        bytesOut = 1;
        }
        Wire.write(*ptr++);
        bytesOut++;
    }
    Wire.endTransmission();
  yield();
}

void SSD1306::startScroll() {
  sendCommand(0x2F);
}

void SSD1306::stopScroll() {
  sendCommand(0x2E);
}

void SSD1306::setupScrollH(bool dir, uint8_t start, uint8_t end, uint8_t interval) {
  stopScroll();
  sendCommand(0x26 + int(dir));
  sendCommand(0x00);
  sendCommand(start);
  sendCommand(interval);
  sendCommand(end);
  sendCommand(0x00);
  sendCommand(0xFF);
}

void SSD1306::setupScrollHV(bool dirX, bool dirY, uint8_t start, uint8_t end, uint8_t interval) {
  stopScroll();
  uint8_t addrs[] = {0x29, 0x2A};
  uint8_t dirs[]  = {1, 63};
  sendCommand(addrs[dirX]);
  sendCommand(0x00);
  sendCommand(start);
  sendCommand(interval);
  sendCommand(end);
  sendCommand(dirs[dirY]);
}

void SSD1306::clear() {
  for (int i = 0; i < 1024; i++) _displayBuf[i] = 0;
}

void SSD1306::drawPixel(int x, int y, bool color) {
  // Draw a single point on the screen
  bitWrite(_displayBuf[x + (y / 8) * 128], y % 8, color);
}

void SSD1306::drawLine(int x0, int y0, int x1, int y1, bool color) {
  // Draw a line with the Digital Differential Analyzer (DDA) algorithm
  // Get difference between endpoints
  int dx = x1 - x0;
  int dy = y1 - y0;
  // Get # of steps needed to put pixels
  int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
  // Get increment in X and Y
  float xInc = dx / float(steps);
  float yInc = dy / float(steps);
  // Put the line
  float x = float(x0), y = float(y0);
  for (int i = 0; i < steps; i++) {
    x += xInc;
    y += yInc;
    drawPixel(round(x), round(y), color);
  }
}

void SSD1306::drawAngledLine(int x, int y, int length, int angle, bool color) {
  // Use trigonometry to draw an angled line
  float x1 = cos(angle * (PI / 180)) * length;
  float y1 = sin(angle * (PI / 180)) * length;
  // Draw the line
  drawLine(x, y, x + x1, y + y1, color);
}

void SSD1306::drawRect(int x0, int y0, int w, int h, bool color) {
  // Draw a rectangle (not filled in)
  // Corner 1: (x0, y0)
  // Corner 2: (x0, y0 + h)
  // Corner 3: (x0 + w, y0)
  // Corner 4: (x0 + w, y0 + h)
  drawLine(x0, y0, x0, y0 + h, color); // Corner 1 -> 2
  drawLine(x0, y0, x0 + w, y0, color); // Corner 1 -> 3
  drawLine(x0, y0 + h, x0 + w, y0 + h, color); // Corner 2 -> 4
  drawLine(x0 + w, y0, x0 + w, y0 + h, color); // Corner 3 -> 4
}

void SSD1306::drawRectByCoords(int x0, int y0, int x1, int y1, bool color) {
  // Draw a rectangle (not filled in) by giving top-left and bottom-right coordinates
  // Corner 1: (x0, y0)
  // Corner 2: (x0, y1)
  // Corner 3: (x1, y0)
  // Corner 4: (x1, y1)
  drawLine(x0, y0, x0, y1, color); // Corner 1 -> 2
  drawLine(x0, y0, x1, y0, color); // Corner 1 -> 3
  drawLine(x0, y1, x1, y1, color); // Corner 2 -> 4
  drawLine(x1, y0, x1, y1, color); // Corner 3 -> 4
}

void SSD1306::fillRect(int x0, int y0, int w, int h, bool color) {
  // Draw a rectangle (filled in)
  for (int i = x0; i <= x0 + w; i++) drawLine(i, y0, i, y0 + h, color);
}

void SSD1306::fillRectByCoords(int x0, int y0, int x1, int y1, bool color) {
  // Draw a rectangle (filled in) by giving top-left and bottom-right coordinates
  for (int i = x0; i <= x1; i++) drawLine(i, y0, i, y1, color);
}

void SSD1306::drawCircle(int x, int y, int r, bool color) {
  // Draw a circle (not filled in) where (x, y) is the center and r is radius
  // Use Bresenham’s Algorithm
  int x1 = 0, y1 = r;
  int d = 3 - 2 * r;
  _drawCircleBase(x, y, x1, y1, color);
  while (x1 < y1) {
    x1++;
    if (d > 0) {
      y1--;
      d += 4 * (x1 - y1) + 10;
    }
    else {
      d += 4 * x1 + 6;
    }
    _drawCircleBase(x, y, x1, y1, color);
  }
}

void SSD1306::fillCircle(int x, int y, int r, bool color) {
  // Draw a circle (filled in) where (x, y) is the center and r is radius
  // Use Bresenham’s Algorithm
  int x1 = 0, y1 = r;
  int d = 3 - 2 * r;
  _fillCircleBase(x, y, x1, y1, color);
  while (x1 < y1) {
    x1++;
    if (d > 0) {
      y1--;
      d += 4 * (x1 - y1) + 10;
    }
    else {
      d += 4 * x1 + 6;
    }
    _fillCircleBase(x, y, x1, y1, color);
  }
}

void SSD1306::displayBitmapImage(const unsigned char* img, int x, int y, int w, int h) {
  uint8_t currentByte = 0;
  // Iterate through the area (w * h) of the given bitmap
  // Begin at (x, y), the top-left corner, and go until (x+w, y+h), the bottom-right corner.
  for (int i = 0; i < h; i++) {    // height
    for (int j = 0; j < w; j++) {  // width
      // Bitshift left all bits until there is room to fit another byte, then read in that byte.
      // A byte represents a 1x8 vertical column of pixels.
      currentByte = ((j % 8) > 0) ? currentByte << 1 : pgm_read_byte(&img[i * ((w + 7) / 8) + (j / 8)]);
      // Only draw a pixel if there is a 1 in the MSB and the byte is 8 bits long (8px tall column)
      // This is the top-most pixel in the column, the only one we care about in this iteration.
      drawPixel(x + j, y + i, (currentByte & 128) > 0);
    }
  }
}

void SSD1306::setCursor(int x, int y) {
  _curX = x;
  _curY = y;
  _curFront = x;
}

void SSD1306::setTextSize(int size) {
  _textSize = size;
}

void SSD1306::print(float s, int nplaces) {
  // Print a number with decimal (specify # of decimals)
  _printBase(String(s, nplaces));
}

void SSD1306::print(double s, int nplaces) {
  // Print a number with decimal (specify # of decimals)
  _printBase(String(s, nplaces));
}

void SSD1306::println(float s, int nplaces) {
  // Print a number with decimal (specify # of decimals)
  print(String(s, nplaces));
  print('\n');
}

void SSD1306::println(double s, int nplaces) {
  // Print a number with decimal (specify # of decimals)
  print(String(s, nplaces));
  print('\n');
}