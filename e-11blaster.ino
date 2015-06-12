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

#define BUTTON_TRIGGER 2
#define BUTTON_MODE 3
#define BUTTON_RELOAD 4

#define LED_POWER 11
#define LED_BLASTER 13
#define LED_STUN 13

#define BLAST_DELAY 300
#define STUN_DELAY 300
#define MAX_AMMO 10 // LED has 10 segements so 10 shots!
#define FLASH_MILLIS 3000

int ammoCount = 0;
boolean stunMode = false;

Bounce debouncerTrigger = Bounce();
Bounce debouncerMode = Bounce();
Bounce debouncerReload = Bounce();

void setup() {
  Serial.begin(9600);

  // Setup all the input buttons.
  pinMode(BUTTON_TRIGGER, INPUT_PULLUP);
  pinMode(BUTTON_RELOAD, INPUT_PULLUP);
  pinMode(BUTTON_MODE, INPUT_PULLUP);

  // Setup all the outputs and write a default state.
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_BLASTER, OUTPUT);
  pinMode(LED_STUN, OUTPUT);

  analogWrite(LED_POWER, 0);
  digitalWrite(LED_BLASTER, LOW);
  digitalWrite(LED_STUN, LOW);

  // Setup the debouncers.
  debouncerTrigger.attach(BUTTON_TRIGGER);
  debouncerMode.attach(BUTTON_MODE);
  debouncerReload.attach(BUTTON_RELOAD);

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
  Serial.println("Trigger pulled");

  if (ammoCount > 0) {
    Serial.println("Shoot!");

    int ledPin;
    int delayTime;

    if (stunMode) {
      ledPin = LED_STUN;
      delayTime = STUN_DELAY;
    } else {
      ledPin = LED_BLASTER;
      delayTime = BLAST_DELAY;
    }

    // Light up the blaster!
    digitalWrite(ledPin, HIGH);

    // todo Play sound here!

    // Wait for the sound to play)
    delay(delayTime);
    // Turn off the blaster.
    digitalWrite(ledPin, LOW);

    // Decrement ammo count and update the display.
    ammoCount--;
    UpdateAmmoDisplay();
  } else {
    // Empty!
    // todo Play empty sound
    // todo Flash the ammo display to highlight we're empty whilst the sound plays.
    Serial.println("Empty!");
  }
}

void ToggleMode(){
  // Simple two state modle so just toggle the mode
  stunMode = !stunMode;

  Serial.print("Mode Switch : ");
  if (stunMode){
    Serial.println("stun");
  } else {
    Serial.println("blast");
  }
}

void ReloadBlaster(){
  Serial.println("Reloading!");
  // Play reload noise and cycle the lights on the 10 segment.

  // Reset the ammo counter.
  ammoCount = MAX_AMMO;
  UpdateAmmoDisplay();
}

void UpdateAmmoDisplay(){
  // todo display ammo to 10 segment.
  Serial.print("Ammo : ");
  Serial.println(ammoCount);
}
