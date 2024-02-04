/*
   SSD_1306_Graphics.h - Library for graphics on a 128x64 SSD1306 OLED
   Created February 3, 2024
   MIT License
*/

#include "Arduino.h"
#include "SSD1306_I2C_A.h"
#include "SSD1306_Graphics.h"

#define MIDPOINTX 64
#define MIDPOINTY 32

#define X32 16
#define X16 8

void genSplash(SSD1306 display) {
  display.clear();
  display.displayBitmapImage(heart0Bitmaps, MIDPOINTX - X32, MIDPOINTY - X32, 32, 32);
  display.updateDisplay();
  delay(400);
  display.clear();
  display.displayBitmapImage(heart1Bitmaps, MIDPOINTX - X32, MIDPOINTY - X32, 32, 32);
  display.updateDisplay();
  delay(400);
  display.clear();
  display.displayBitmapImage(heart2Bitmaps, MIDPOINTX - X32, MIDPOINTY - X32, 32, 32);
  display.updateDisplay();
  delay(400);
  display.clear();
  display.displayBitmapImage(heart3Bitmaps, MIDPOINTX - X32, MIDPOINTY - X32, 32, 32);
  display.updateDisplay();
  delay(400);
  for (uint8_t i = 0; i < 3; i++) {
      display.clear();
      display.displayBitmapImage(heart4Bitmaps, MIDPOINTX - X32, MIDPOINTY - X32, 32, 32);
      display.updateDisplay();
      delay(400);
      display.clear();
      display.displayBitmapImage(heart0Bitmaps, MIDPOINTX - X32, MIDPOINTY - X32, 32, 32);
      display.updateDisplay();
      delay(400);
  }
  delay(500);
};

void genCatpet(SSD1306 display) {
  for (uint8_t i = 0; i < 2; i++) {
      display.clear();
      display.displayBitmapImage(catpet0, MIDPOINTX - X16, MIDPOINTY - X16, 16, 16);
      display.updateDisplay();
      delay(200);
      display.clear();
      display.displayBitmapImage(catpet1, MIDPOINTX - X16, MIDPOINTY - X16, 16, 16);
      display.updateDisplay();
      delay(200);
      display.clear();
      display.displayBitmapImage(catpet2, MIDPOINTX - X16, MIDPOINTY - X16, 16, 16);
      display.updateDisplay();
      delay(200);
      display.clear();
      display.displayBitmapImage(catpet3, MIDPOINTX - X16, MIDPOINTY - X16, 16, 16);
      display.updateDisplay();
      delay(200);
      display.displayBitmapImage(catpet0, MIDPOINTX - X16, MIDPOINTY - X16, 16, 16);
      display.updateDisplay();
      delay(200);
  }
  delay(500);
};

void genSleepycat(SSD1306 display) {
  for (uint8_t i = 0; i < 2; i++) {
      display.clear();
      display.displayBitmapImage(sleepycat2, MIDPOINTX - 32, MIDPOINTY - 24, 64, 48);
      display.updateDisplay();
      delay(400);
      display.clear();
      display.displayBitmapImage(sleepycat1, MIDPOINTX - 32, MIDPOINTY - 24, 64, 48);
      display.updateDisplay();
      delay(400);
      display.clear();
      display.displayBitmapImage(sleepycat0, MIDPOINTX - 32, MIDPOINTY - 24, 64, 48);
      display.updateDisplay();
      delay(400);
  }
  delay(500);
};

void genBasicText(SSD1306 display) {
    long rand = random(0, 20);
    String choices[20] = {
        "Loff uuuu!!",
        "I don't need\n any dessert;\n I have you.",
        "You're just like a fine wine...\n my eyes could drink\n you in all night.",
        "I think you were\n made to fit\n perfectly in my arms.",
        "I miss your kisses!",
        "Candy and flowers\n aren't enough.\n Let me give\n you my heart.",
        "squish, squish, squish",
        "Bao bao!",
        "Sugar plum",
        "Love thee, sweetpea",
        "I love you",
        "Can't stop thinking\n of apple cakes ;)",
        "Are u CAB on\n registration\n, cuz u can crash\n at my place anytime",
        "Qt 3.14, hru",
        "Hugs and kisses",
        "XOXOXOXO",
        "Kk, hope to c u",
        "10/10",
        "Rrrrrrrrrrrrrrrrr",
        "Ur my home."
    };
    display.clear();
    display.setCursor(0, 0);
    display.print(choices[rand]);
    display.updateDisplay();
    // Horizontal scroll left, scroll pages 0-7, 2 frames interval
    display.setupScrollH(LEFT, 0, 7, FRAMES_4);
    display.startScroll(); // Begin scroll
    delay(100);           // Scroll for 2 seconds
    display.stopScroll();  // Stop scrolling
    delay(100);            // Wait half a second between scrolling

    // Horizontal scroll right, scroll pages 0-7, 2 frames interval
    display.setupScrollH(RIGHT, 0, 7, FRAMES_4);
    display.startScroll();
    delay(100);
    display.stopScroll();
    delay(100);

    // Vertical+horizontal scroll left and up, scroll pages 0-7, 2 frames interval
    display.setupScrollHV(LEFT, UP, 0, 7, FRAMES_4);
    display.startScroll();
    delay(100);
    display.stopScroll();
    delay(100);

    // Vertical+horizontal scroll right and down, scroll pages 0-7, 2 frames interval
    display.setupScrollHV(RIGHT, DOWN, 0, 7, FRAMES_4);
    display.startScroll();
    delay(100);
    display.stopScroll();
    delay(100);
    delay(1000);
}

void gen64x64(SSD1306 display, const unsigned char* img) {
    display.clear();
    display.displayBitmapImage(img, MIDPOINTX - 32, MIDPOINTY - 32, 64, 64);
    display.updateDisplay();
    delay(600);
}

void gen128x64(SSD1306 display, const unsigned char* img) {
    display.clear();
    display.displayBitmapImage(img, MIDPOINTX - 64, MIDPOINTY - 32, 128, 64);
    display.updateDisplay();
    delay(600);
};

void initGame(SSD1306 display) {

};