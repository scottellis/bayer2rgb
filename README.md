  bayer2rgb
=======

Convert a raw bayer image from mt9p031 at full size into a rgb (bgr)
image for display and also save a jpg version. 

A 10 -> 8 bit down sampling is necessary to use the OpenCV built-in 
cvCvtColor(). The implementation of that algo is standard though and 10 bit 
could be conserved. This is just a quick hack for testing.


