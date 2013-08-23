LPD8806-Miniprojects
====================

A collection of Arduino mini-projects for LPD8806 RGB LED-strips. These are generally small (&lt;200 SLOC) and are intended to demonstrate and explore animation possibilities. Some of these projects require the [LPD8806 library](https://github.com/adafruit/LPD8806) by [Adafruit](http://adafruit.com).

BouncyBallPhysics
-------------------

This sketch is a physics (gravity) simulation on Arduino. Three balls, visualized by three colored pixels, are dropped from distances between 1 and 1.5 meters above the ground (the 'ground' is the beginning of the strip). From where they start aboveground, they fall down according to the laws of gravity until they hit the floor. From there they bounce up, losing a fraction of their kinetic energy determined by their elasticity coefficient. This causes the ball to bounce up slighlty to significantly less high, and this continues until all kinetic energy has been sapped from the ball.

Using the serial interface, you can 'push' (add kinetic energy to) the balls. Each ball has its own weight, and thus reacts differently to the push they receive. If the balls are currently moving, energy is added in that direction, otherwise the balls are pushed away from gravity. Old and new speed resulting from the push are reported back over serial for each ball. To keep things balanced, the lighter the ball, the less elastic its bounce, so that everything comes to a halt relatively close together.

This sketch assumes a strip with 52 LEDs per meter. If your strip has a different LED density, change the _ledPerMeter_ constant to match your strip. If you don't, the displayed acceleration will not match our terrestrial acceleration. Negative and zero gravity are supported, as well as balls that _gain_ energy when bouncing (an elasticity coefficient >1). Zero-weight objects are not supported, physics doesn't allow for them to travel at any speed other than lightspeed.


GreenFlyingDot
--------------

This sketch sends a green dot flying over the strip. It builds on the examples provided in the LPD8806 library by Adafruit, reducing the whole of it down to the bare minimum. As the green dot moves over the strip, it leaves behind a permanent blue background. Once it reaches either end of the strip, it reverses direction and proceeds down the entire length again.


MiniFade
--------

A basic example of communicating with the LPD8806 LED-strip, using only default Arduino libraries. This sketch uses direct SPI communication. This reduces the memory (RAM) footprint of the application significantly (the LPD8806 library requires 3 bytes for each LED), which might be an interesting technique for sketches that require most of the 2kB of available SRAM on a typical Arduino. The animation shown is a very plain and simple full-strip animated white blinking (with a few steps of smoothing in place).


RunningColorDot
---------------

In this sketch, a colored dot is painted on the strip. The dot moves along the strip at a considerable speed (the delay between steps is configurable) while changing colors all the while (again, configurable speed). Behind the dot is a fading trail and the length of this combined trail can be set. As this is achieved by simple bit shifting the maximum length is 7, though it looks considerably better with a shorter length of 5 or 6 LEDs.

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
