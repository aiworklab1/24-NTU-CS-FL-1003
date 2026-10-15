#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- OLED configuration ----------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1       // no reset pin
#define OLED_ADDRESS   0x3C     // I2C address of the SSD1306
#define SDA_PIN        21
#define SCL_PIN        22

// ---------- Button configuration ----------
const int BUTTON_PIN = 4;                       // button between GPIO4 and GND
const unsigned long DEBOUNCE_INTERVAL_MS = 50;  // input must be stable this long

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Debounce state variables ----------
int lastRawReading = HIGH;          // previous raw pin reading
int stableState = HIGH;             // last accepted (debounced) state
unsigned long lastChangeTime = 0;   // time the raw reading last changed

// ---------- Application state ----------
unsigned int pressCount = 0;        // number of valid presses

// Draw the status screen: title, button status, press counter
void updateDisplay(bool isPressed) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 Button Monitor");
  display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);

  display.setCursor(0, 20);
  display.print("Button: ");
  display.println(isPressed ? "PRESSED" : "RELEASED");

  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print("Count: ");
  display.print(pressCount);

  display.display();   // push buffer to the screen
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // idle = HIGH, pressed = LOW

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 initialization failed!");
    while (true) { delay(1000); }      // stop here if the display is not found
  }

  Serial.println("System started. Press count = 0");
  updateDisplay(false);                // initial screen: RELEASED, count 0
}

void loop() {
  // 1. Read the raw (possibly bouncing) pin value
  int rawReading = digitalRead(BUTTON_PIN);

  // 2. If the raw value changed, restart the stability timer
  if (rawReading != lastRawReading) {
    lastChangeTime = millis();
  }

  // 3. If the value has stayed unchanged longer than the debounce interval,
  //    treat it as a real, stable state (no blocking delay is used)
  if ((millis() - lastChangeTime) > DEBOUNCE_INTERVAL_MS) {
    if (rawReading != stableState) {
      stableState = rawReading;

      if (stableState == LOW) {
        // Valid press: count exactly once per physical press
        pressCount++;
        Serial.print("Button PRESSED  | Accepted press count = ");
        Serial.println(pressCount);
        updateDisplay(true);
      } else {
        Serial.println("Button RELEASED");
        updateDisplay(false);
      }
    }
  }

  // 4. Remember the raw reading for the next loop pass
  lastRawReading = rawReading;
}