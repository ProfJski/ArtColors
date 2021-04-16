# Roadmap for ArtColors

## TO DO:
Refine color wheel hue spectrum a little more to get better color harmonies.
  The main issue is this: For many values of the hue slider, the triadic palette corresponds properly with the values on an artist's color wheel.  Except for a set of secondary colors where the ArtColor's wheel lags a bit.  If you set the hue to about 72 degrees, it shows what you should rather see at 90 degrees.  Boo.

Calculate more exact inverse function for RGB-to-RYB.
  I.e., take the trilinear interpolation formulae, work them out into algebraic equations, invert them, and get direct formulae for both the RYB-to-RGB and its inverse.

Ditch the default RayGUI single-pixel font for text display and get a real font.  Raylib supports nice fonts, after all.  The single-pixel font works well for small text, but scales badly at medium sizes and is not very legible.

## Done in version 0.2:
Make a help file
Add facility for choosing Colorblind-friendly palettes.

## Vestigal code
There's some functions I have commented out but work fine, like an alternative rectangular color picker.  Go ahead and play with them (if you can find a place for them on the screen!)

## Fixed in 0.1:
Screen shot button works correctly now.
