/*
The MIT License (MIT)

Copyright (c) 2015 Christopher Pearson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// Debouncing library, why reinvent the wheel!
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>

// Grove 10 segment LED driver
// https://github.com/Seeed-Studio/Grove_LED_Bar
#include <Grove_LED_Bar.h>

// AdaFruit Neo Pixel
// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

#define BUTTON_TRIGGER 2
#define BUTTON_MODE 3
#define BUTTON_RELOAD 4

#define LED_POWER 11
#define LED_BLASTER 7

#define BAR_DATA 5
#define BAR_CLOCK 6
#define BAR_ORIENTATION 0

#define DELAY_BLAST 1000
#define DELAY_STUN 1100
#define MAX_AMMO 10 // LED has 10 segements so 10 shots!
#define FLASH_MILLIS 3000

#define SOUND_BLAST 1
#define SOUND_STUN 2
#define SOUND_EMPTY 3
#define SOUND_RELOAD 4

#define PIX_NUM 1

int ammoCount = 0;
boolean stunMode = false;

Bounce debouncerTrigger = Bounce();
Bounce debouncerMode = Bounce();
Bounce debouncerReload = Bounce();

Grove_LED_Bar bar(BAR_CLOCK, BAR_DATA, BAR_ORIENTATION);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIX_NUM, LED_BLASTER, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  // Setup all the input buttons.
  pinMode(BUTTON_TRIGGER, INPUT_PULLUP);
  pinMode(BUTTON_RELOAD, INPUT_PULLUP);
  pinMode(BUTTON_MODE, INPUT_PULLUP);

  // Setup the power LED and default it to off.
  pinMode(LED_POWER, OUTPUT);
  analogWrite(LED_POWER, 0);

  // Initialise the sound volume in the range 0-31
  SetVolume(28);

  // Setup the debouncers.
  debouncerTrigger.attach(BUTTON_TRIGGER);
  debouncerMode.attach(BUTTON_MODE);
  debouncerReload.attach(BUTTON_RELOAD);

  // Setup 10 segment display
  bar.begin();
  ClearDisplay();

  // Setup Neo pixels
  pixels.begin();
  /*pixels.setBrightness(255); // max out the brightness*/
  pixels.show(); // Initialise to off

  // Now core init is done do any run once on load stuff
  ReloadBlaster();
}

void loop() {
  // These are events that can happen in each loop.
  // Read the values from the debouncer and act accordingly
  boolean fire = debouncerTrigger.update() && debouncerTrigger.fell();
  boolean changeMode = debouncerMode.update() && debouncerMode.fell();
  boolean reload = debouncerReload.update() && debouncerReload.fell();

  // Act on the current state of the world

  SetPowerLED();

  // Only do 1 job at a time, we don't want to fire reload and change mode at
  // once! 1 it makes no sence and 2 it would sound horrific.
  // * Reloading will cancel a mode change.
  // * Mode change will cancel a shot.

  if (reload) {
    ReloadBlaster();
  } else if (changeMode) {
    ToggleMode();
  } else if (fire) {
    PullTrigger();
  }

}

void SetPowerLED() {
  // Start with the LEDs
  if (!stunMode) {
    // Normal mode so power on full
    analogWrite(LED_POWER, 255);
  } else {
    // Stun mode so pulse the LED over 3 seconds
    // Maths magic will give us a value 0-255 we can pass into to the
    // analogWrite.
    double v = 128 + 127 * sin(2 * PI / FLASH_MILLIS * millis());

    analogWrite(LED_POWER, (int)v);
  }
}

void PullTrigger(){

  if (ammoCount > 0) {

    int ledPin;
    int delayTime;
    byte sound;

    // Light up the blaster!

    if (stunMode){
      delayTime = DELAY_STUN;
      sound = SOUND_STUN;
      SetPixelBlue();
    } else {
      delayTime = DELAY_BLAST;
      sound = SOUND_BLAST;
      SetPixelRed();
    }

    PlayTrack(sound);
    // Wait for the sound to play)
    delay(delayTime);
    // Turn off the blaster.
    SetPixelOff();

    // Decrement ammo count and update the display.
    ammoCount--;
    UpdateAmmoDisplay();
  } else {
    // Empty!
    PlayTrack(SOUND_EMPTY);
    // Flash the ammo display to highlight we're empty whilst the sound plays.
    FlashDisplay();
  }
}

void ToggleMode(){
  // Simple two state modle so just toggle the mode
  stunMode = !stunMode;
}

void ReloadBlaster(){
  // Play reload noise and cycle the lights on the 10 segment.
  PlayTrack(SOUND_RELOAD);
  // Cycle LEDs

  // Reset the ammo counter.
  ammoCount = MAX_AMMO;
  UpdateAmmoDisplay();
}

void UpdateAmmoDisplay(){
  SetDisplay(ammoCount);
}

// LED bar helper functions.
void ClearDisplay(){bar.setLevel(0);}
void FillDisplay(){bar.setLevel(10);}
void SetDisplay(int count){bar.setLevel(count);}

void FlashDisplay(){
  for (int i = 0 ; i < 2 ; i++){
    FillDisplay();
    delay(300);
    ClearDisplay();
    delay(300);
  }
  // Reset the display back to where it was.
  UpdateAmmoDisplay();
}

// Sound helper functions
void SetVolume(byte vol){
  Serial.write(0x7E); // Start
  Serial.write(0x03); // Length
  Serial.write(0xA7); // Command
  Serial.write(vol); // Volume 00-31
  Serial.write(0x7E); // End
}

void PlayTrack(byte track){
  Serial.write(0x7E); // Start
  Serial.write(0x04); // Length
  Serial.write(0xA0); // SD Card
  Serial.write(0x00); // High Byte
  Serial.write(track); // Low Byte
  Serial.write(0x7E); // End
}

// Pixels helper FUNCTIONS
void SetPixelRGB(byte r, byte g, byte b){
  for(int i = 0 ; i < PIX_NUM ; i++){
    pixels.setPixelColor(i, pixels.Color(r,g,b));
  }
  pixels.show();
}

void SetPixelBlue(){SetPixelRGB(0, 0, 255);}
void SetPixelRed(){SetPixelRGB(255, 0, 0);}
void SetPixelOff(){SetPixelRGB(0, 0, 0);}
