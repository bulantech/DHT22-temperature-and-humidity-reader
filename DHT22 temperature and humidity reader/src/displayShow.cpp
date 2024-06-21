#include "displayShow.h"

bool tickToggle = 0;

void display2line( Adafruit_SSD1306 &display, int interval, unsigned long &time, String line1, String line2) {
  if(millis() - time <= interval) return;
  time = millis();

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print(line1); 

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 45);
  display.print(line2); 

  display.setTextSize(2);
  display.setCursor(0, 50);
  if(tickToggle) {
    tickToggle = 0;
    display.print(F("."));
  }
  else {
    tickToggle = 1;
    display.print(F(" "));
  }
  
  display.display(); 
}

void display1lineCenter( Adafruit_SSD1306 &display, int interval, unsigned long &time, String line1) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(20, 30);
  display.print(line1); 

  display.display(); 
}