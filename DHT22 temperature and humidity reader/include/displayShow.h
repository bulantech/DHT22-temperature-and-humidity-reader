#ifndef DISPLAY_SHOW_H
#define DISPLAY_SHOW_H

#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

void display2line( Adafruit_SSD1306 &display, int interval, unsigned long &time, String line1, String line2);

void display1lineCenter( Adafruit_SSD1306 &display, int interval, unsigned long &time, String line1);

#endif // DISPLAY_SHOW_H