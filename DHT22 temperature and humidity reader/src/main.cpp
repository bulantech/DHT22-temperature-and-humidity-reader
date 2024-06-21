#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_NeoPixel.h>

#include "OneButton.h"

#include "displayShow.h"

#define BUZZER_PIN D8

#define DHTPIN D7     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIXEL_PIN        10 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 2 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels


#define INTERVAL_MESSAGE1 2000 // dht22
#define INTERVAL_DISPLAY 500 //display
#define INTERVAL_MESSAGE3 50 //sled
#define INTERVAL_MESSAGE4 250 // vr adc
#define INTERVAL_BUTTON_TICK 10 // vr adc
 
unsigned long time_1 = 0;
unsigned long time_display = 0;
unsigned long time_3 = 0;
unsigned long time_4 = 0;
unsigned long time_button_tick = 0;

// dht22
float temperature = 0;
float humidity = 0;

// display
int readCount=0;
bool forword=true;

// vr adc
const int vrPin = A0;  // input pin for the potentiometer
int vrValue = 0;// variable to store the value coming from the sensor
const int vrValueMin = 22;
const int vrValueMid = 388;
const int vrValueMax = 821;

// button
#define PIN_INPUT D3
#define PIN_LED D0
OneButton button(PIN_INPUT, true);

// current LED state, staring with LOW (0)
int ledState = HIGH;

//main
enum state_main_t { // <-- the use of typedef is optional.
  HOME,
  SETTING,
};
state_main_t state_main; // <-- the actual instance

enum state_home_t { 
  TEMP,
  HUMI,
  DATE,
  TIME,
} state_home;

enum state_button_t { 
  IDLE,
  CLICKED,
  DOUBLECLICK,
  LONGPRESSED,
} state_button;


// put function declarations here:
void dht22Read();
void loopSLED();
void readVr();

void displayTask();

void doubleClick();
void clicked();
void longPressed();


void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER_PIN, OUTPUT);

  // enable the standard led on pin 13.
  pinMode(PIN_LED, OUTPUT); // sets the digital pin as output
  // enable the standard led on pin 13.
  digitalWrite(PIN_LED, ledState);

  // link the doubleclick function to be called on a doubleclick event.
  button.attachClick(clicked);
  button.attachDoubleClick(doubleClick);
  button.attachLongPressStart(longPressed);

  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(50);

  // add this to use (IE) D2 (SDA) and D1 (SCL):
  // Wire.begin(D2,D1);

  //  SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  // Display static text
  display.println("   DHT22");
  display.println("  Reader..");
  display.display(); 
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  switch(state_main) {
    case HOME:
      switch(state_home) {
        case TEMP:
          display2line( display, INTERVAL_DISPLAY, time_display, String(temperature) + " C", String(humidity) + " %");

          switch(state_button) {
            case IDLE: break;
            case CLICKED: 
              display1lineCenter( display, INTERVAL_DISPLAY, time_display, String(humidity)+ " %");
              state_home = HUMI;
              state_button = IDLE;
            break;
            case DOUBLECLICK: break;
            case LONGPRESSED: break;
            default : state_button = IDLE;
          }
        break;
        case HUMI:
          display2line( display, INTERVAL_DISPLAY, time_display, String(humidity) + " %", String(temperature) + " C");

          switch(state_button) {
            case IDLE: break;
            case CLICKED: 
              display1lineCenter( display, INTERVAL_DISPLAY, time_display, String(temperature) + " C");
              state_home = TEMP;
              state_button = IDLE;
            break;
            case DOUBLECLICK: break;
            case LONGPRESSED: break;
            default : state_button = IDLE;
          }
        break;
        case DATE:

        break;
        case TIME:

        break;
      }
    break;

    case SETTING:

    break;
  }

  if(millis() - time_button_tick > INTERVAL_BUTTON_TICK) {
    time_button_tick = millis();
    button.tick();
  }

  dht22Read();
  // displayTask();
  loopSLED();
  readVr();

}

// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick() {
  Serial.println("x2");
  state_button = DOUBLECLICK;
  ledState = !ledState; // reverse the LED
  digitalWrite(PIN_LED, ledState);
} // doubleClick

void clicked() {
  Serial.println("clicked");
  state_button = CLICKED;
  tone(BUZZER_PIN, 500);
  delay(100);
} 

void longPressed() {
  Serial.println("longPressed");
  state_button = LONGPRESSED;
} 

void displayTask() {
  if(millis() - time_display <= INTERVAL_DISPLAY) return;
  time_display = millis();

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  // display.println("Hello, world!");

  display.print(temperature); display.print(F(" C"));

  display.setCursor(readCount*10, 40);
  display.print(F("-"));
  if(forword) {
    if(++readCount >= 11) { 
      forword = false;
    }
  }
  else {
    if(--readCount <= 0) { 
      forword = true;
    }
  }
  
  display.display(); 
}

void dht22Read() {
  if(millis() - time_1 <= INTERVAL_MESSAGE1) return;
  time_1 = millis();

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temperature, humidity, false);

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

}

long firstPixelHue = 0;
void loopSLED() {
  if(millis() - time_3 <= INTERVAL_MESSAGE3) return;
  time_3 = millis();

  if(vrValue < vrValueMin+5) pixels.setBrightness(0);
  else if(vrValue >= vrValueMax-2) pixels.setBrightness(100);
  else pixels.setBrightness(vrValue/10);

  for(int i=0; i<pixels.numPixels(); i++) { 
    int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
    pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
  }
  pixels.show();
  // delay(100);
  firstPixelHue += 256;
  if(firstPixelHue > 5*65536) firstPixelHue = 0; 

}

void readVr() {
  if(millis() - time_4 <= INTERVAL_MESSAGE4) return;
  time_4 = millis();

  vrValue = analogRead(vrPin);// read the value from the analog channel
  // Serial.print("vr value = ");
  // Serial.println(vrValue);        //print digital value on serial monitor
}