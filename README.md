LPD8806-Miniprojects
====================

A collection of Arduino mini-projects for LPD8806 RGB LED-strips. These are generally small (&lt;200 SLOC) and are intended to demonstrate and explore animation possibilities.

GreenFlyingDot
--------------

A basic Arduino sketch sending a green('ish) dot up and down the strip, bouncing at either end.


WalkingLights
-------------

Arduino sketch for color waves on LPD8806 powered RGB LED strips. This has three colored waves (red, green, and blue) moving across the strip. The blue and red ones in the forward direction, green in the opposite direction. Where they touch/overlap each other, the resulting color is blended and displayed. The waves can be turned on and off using serial commands (letters 'r', 'g', and 'b' toggle each wave).


RunningColorDot
---------------

In this sketch, a colored dot is painted on the strip. The dot moves along the strip at a considerable speed (this delay is configurable) while changing colors all the while (again, configurable). Behind the dot is a fading trail and the length of this combined trail can be set. As this is achieved by simple bit shifting the maximum length is 7, though 5 or 6 look better.

In addition to moving along the strip, there is a "one in a number" chance for the trail to change direction. The main dot's direction changes and the trail follows the dot in the other direction. Where the change of direction happened, a light on the strip lights up, as if the dot had collided with it. There is also a brief pause when the 'collision' occurs. This dot remains lit on the strip until the trail passes over it, and has no further purpose.
