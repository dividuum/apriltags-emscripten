= AprilTags in JavaScript with emscripten

This is a proof of concept project showing how to detect
[AprilTags](https://april.eecs.umich.edu/software/apriltag.html)
using JavaScript. The original C code was cross compile to
JavaScript with [emscripten](http://emscripten.org). The
result is `html/apriltag.js`. An example of how to use this
wrapped c library can be found in `html/index.html`.

= Limits

* Only the tag36h11 family is supported. I didn't bother including
the other families as that seems to be the recommended anyway.

* The example codes scales the input image down to a
maximum resolution of 800x800. Otherwise safari on my IPad crashes :-)

* All detection parameters are fixed at the moment. So you have to
recompile with emscripten if you want to change any of those.

* No error handling whatsoever :-P
