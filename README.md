LPD8806-Miniprojects
====================

A collection of Arduino mini-projects for LPD8806 RGB LED-strips. These are generally small (&lt;200 SLOC) and are intended to demonstrate and explore animation possibilities. Some of these projects require the LPD8806 library by Adafruit: https://github.com/adafruit/LPD8806.

GreenFlyingDot
--------------

This sketch sends a green dot flying over the strip. It builds on the examples provided in the LPD8806 library by Adafruit, reducing the whole of it down to the bare minimum. As the green dot moves over the strip, it leaves behind a permanent blue background. Once it reaches either end of the strip, it reverses direction and proceeds down the entire length again.


MiniFade
--------

A basic example of communicating with the LPD8806 LED-strip, without using any existing libraries. This sketch uses direct SPI communication. This reduces the memory footprint of the application significantly (the LPD8806 library requires 3 bytes for each LED), which might be an interesting technique for sketches that require most of the 2kB of available SRAM on a typical Arduino. The animation shown is a very plain and simple full-strip animated white blinking (with a few steps of smoothing in place).


RunningColorDot
---------------

In this sketch, a colored dot is painted on the strip. The dot moves along the strip at a considerable speed (this delay is configurable) while changing colors all the while (again, configurable). Behind the dot is a fading trail and the length of this combined trail can be set. As this is achieved by simple bit shifting the maximum length is 7, though 5 or 6 look better.

In addition to moving along the strip, there is a "one in a number" chance for the trail to change direction. The main dot's direction changes and the trail follows the dot in the other direction. Where the change of direction happened, a light on the strip lights up, as if the dot had collided with it. There is also a brief pause when the 'collision' occurs. This dot remains lit on the strip until the trail passes over it, and has no further purpose.


StarsInSnow
-----------

A small winter themed sketch that was made in the days leading up to Christmas, to get some semblance of a white Christmas. There is a background that has a low-intensity white, over time slowly shifting up and down for each individual LED, attempting to mimic falling snow (and the corresponding 'noise' in local brightness).

On top of the snowy white background are bright blinking 'stars'. Every 100ms, a random dot is chosen and through 24 frames, its taken to its maximum brightness and back to the base level. This means that at any given time, there are a number of these points either rapidly lighting up, or fading away, making for a nice background display.

This project is again pure SPI, with no libraries needed beyond the ones that the Arduino environment includes by default.


WalkingLights
-------------

WalkingLights is another pure-SPI mini project. In this sketch, three colored waves (red, green, and blue) move across the strip. The blue and red waves move in the forward direction, the green in the opposite direction. Waves can be turned on or off using serial commands; The letters 'r', 'g', and 'b' toggle each wave.

Where the waves overlap, their color values are blended together to make the correct full RGB color. For example: green and red waves will make yellow interleaving patterns. All waves run at different speeds and have different whitespace between them on the strip. This ensures that all the various color blends will happen, and in a way that is not immediately repetitive.
