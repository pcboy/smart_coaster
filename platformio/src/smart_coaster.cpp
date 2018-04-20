#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleEsp32_BLE.h>

#include "smart_coaster.h"
#include "vfs.h"
#include "secrets.h"  // Contains the following:
// #define BLYNK_AUTH_KEY 7est7df605ab4543bf7b358bf22161bb // sample key


BlynkTimer timer;

CircularBuffer<int, 20> fsrValues;

boolean waiting;
#define FSR_PIN 34
int beerCounter;
int emptyGlassValue;
int fullGlassValue;

#define FEATHERWING_PIN 23

#define DEBUG true

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, FEATHERWING_PIN, NEO_GRB + NEO_KHZ800);

int average(CircularBuffer<int, 20> *values) {
  int res = 0;
  int i;

  for (i = 0; i < values->size(); ++i) {
    res += (*values)[i];
  }

  return res / i;
}

void init_state() {
  waiting = false;
  beerCounter = 0;
  emptyGlassValue = 900;
  fullGlassValue = 3500;
}

BLYNK_WRITE(V1) {
  //restoring int value
  beerCounter = param.asInt();
  Serial.print("Restoring beerCounter to:");
  Serial.println(beerCounter);
}

BLYNK_WRITE(V2) {
  beerCounter = 0;
  Blynk.virtualWrite(V1, beerCounter);
}

BLYNK_WRITE(V3) {
  int pinValue = param.asInt();

  emptyGlassValue = pinValue;
}

BLYNK_WRITE(V4) {
  int pinValue = param.asInt();

  fullGlassValue = pinValue;
}

BLYNK_WRITE(V5) {
  ESP.deepSleep(INT_MAX);
}

// To manually set the beerCounter
WidgetTerminal terminal(V6);
BLYNK_WRITE(V6) {
  int value = param.asInt();

  terminal.println(value);
  terminal.flush();
  if (value == -1) {
    strip.clear();
    strip.show();
  } else {
    beerCounter = value;
  }
  Blynk.virtualWrite(V1, beerCounter);
}

void incrementGlassCounter() {
  beerCounter++;
  Blynk.virtualWrite(V1, beerCounter);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

BLYNK_WRITE(V7)
{
  int shift = param.asInt();
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, Wheel(shift & 255));
  }
  strip.show();
}

void setPixelColor(Adafruit_NeoPixel *strip, uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) {
  strip->setPixelColor(n, (brightness*r/255) , (brightness*g/255), (brightness*b/255));
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
  strip.clear();
  strip.show();
}

void elevatorAnimation(uint8_t wait, int red, int green, int blue) {

  for (int i=0; i < 8; i++) {
    for (int j = 0; j <= 24; j += 8) {
      setPixelColor(&strip, i + j, red, green, blue, 100);
    }
    strip.show();
    delay(wait);
  }

  strip.clear();
  strip.show();
}

void beerAnim(uint8_t wait) {
  elevatorAnimation(wait, 255, 243, 0);
}

void waterAnim(uint8_t wait) {
  elevatorAnimation(wait, 0, 0, 255);
}

void sync_blynk() {
  Blynk.virtualWrite(V1, beerCounter);
  Blynk.virtualWrite(V3, emptyGlassValue);
  Blynk.virtualWrite(V4, fullGlassValue);
}

BLYNK_WRITE(V8)
{
  //theaterChaseRainbow(50);
  sync_blynk();
}

nvs_handle vfs_handle;

void save_state() {
  vfs_handle = vfs_get_handle("storage");
  vfs_write_i32(vfs_handle, "beerCounter", beerCounter);
  vfs_write_i32(vfs_handle, "waiting", waiting);
  vfs_write_i32(vfs_handle, "emptyGlassValue", emptyGlassValue);
  vfs_write_i32(vfs_handle, "fullGlassValue", fullGlassValue);
}

void restore_state() {
  Serial.println("beerCounter");
  Serial.println(beerCounter);
  vfs_handle = vfs_get_handle("storage");
  beerCounter = vfs_read_i32(vfs_handle, "beerCounter");
  waiting = vfs_read_i32(vfs_handle, "waiting");
  emptyGlassValue = vfs_read_i32(vfs_handle, "emptyGlassValue");
  fullGlassValue = vfs_read_i32(vfs_handle, "fullGlassValue");
}

void myLoop() {
  int fsrReading = analogRead(FSR_PIN);

  Serial.print("FSRReading: ");
  Serial.println(fsrReading);

  fsrValues.push(fsrReading);
  if (!fsrValues.isFull()) // We need more values.
    return;

  int avg = average(&fsrValues);
#ifdef DEBUG
  Serial.print("AVERAGE: ");
  Serial.println(avg);
  Serial.print("fullGlassValue: ");
  Serial.println(fullGlassValue);
  Serial.print("emptyGlassValue: ");
  Serial.println(emptyGlassValue);
  Serial.print("waiting: ");
  Serial.println(waiting);
  Blynk.virtualWrite(V0, fsrReading);
  sync_blynk();
#endif

  if (waiting == false && avg < emptyGlassValue) {
    Serial.println("Beer finished. Waiting for new one.");
    if (!(beerCounter % 2))
      waterAnim(200);
    waiting = true;
    save_state();
    initialize_ULP(FSR_PIN, emptyGlassValue, 0);
  } else if (waiting == true && avg >= fullGlassValue) {
    Serial.println("New beer");
    beerAnim(200);
    incrementGlassCounter();
    waiting = false;
    save_state();
    initialize_ULP(FSR_PIN, 4096, emptyGlassValue + (0.2 * emptyGlassValue));
  }
}

#include <esp_wifi.h>
#include <esp_wifi_types.h>

void setup() {
  Serial.begin(9600);
  esp_wifi_set_mode(WIFI_MODE_NULL);

  vfs_setup();
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  init_state();

  if (cause == ESP_SLEEP_WAKEUP_ULP) {
    restore_state();
  }

  Blynk.begin(BLYNK_AUTH_KEY);

  timer.setInterval(1000L, myLoop);

  strip.begin();
  strip.clear();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);
}


void loop() {
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
