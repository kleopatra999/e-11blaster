#E-11 blaster

**E-11 Blaster** is as basic arduino based sound and light system to add basic
effects to an E-11 blaster prop.

##Installation
The instructions here are reasonably basic, but if you're attempting this you
probably should have a reasonable idea of what you're doing!
 * Download and install [Arduino editor](http://www.arduino.cc/en/Main/Software)
 * Download this project
 * Download and install [BOUNCE 2](https://github.com/thomasfredericks/Bounce2)
 * Download and install [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
 * Load up `e-11blaster.ino` in Arduino.
 * Configure Arduino as appropriate for your hardware and then upload.

##Components
  * **AD1** - Arduino Nano
  * **W1** - WT5001M02-28P
  * **V1** - Battery
  * **S1** - Micro switch
  * **S2** - Micro switch
  * **S2** - push to make switch
  * **R1** - 220 Ohm
  * **R2** - 470 Ohm
  * **LED1** - Grove LED bar
  * **LED2** - AdaFruit Neo Pixel
  * **LED3** - Power indicator
  * **SP1, SP2** - 8Ohm 5W Speaker

### Schemeatic
![Schemeatic](/Docs/e-11blaster_schem.png)

##Acknowledgements
 * Inspired by work done by *skyone* and others on [FISD](http://www.whitearmor.net)
 * [BOUNCE 2](https://github.com/thomasfredericks/Bounce2) - debouncing library.
 * [Grove LED Bar](https://github.com/Seeed-Studio/Grove_LED_Bar) - 10 Segement LED driver.
 * [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) - Super bright RGB LED.
