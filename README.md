# ArtColors
Intuitive color mixing in subtractive Red-Yellow-Blue color space.  A traditional art school color wheel and palette selector provides triadic, split-complementary and tetradic color harmonies.  RGB-to-RYB conversion with simple formulas.  Built with RayLib.

# Uses
1. **Intuitive color mixing for graphic design, data visualization, GUI creation or video game graphics.**

   Everyone grows up learning that red and yellow make orange.  The RGB space is unintuitive.
   
2. **Easy generation of harmonious palettes for pleasing graphic design.**

   Color schemes in fine art are based on simple geometric relations on an RYB color wheel which has red, yellow and blue primaries 120 degrees apart.  The typical RGB color wheel has red, green and blue 120 degrees apart, so these relations fail.  The blue-green spectrum tends to dominate, obscuring rich yellows, purples and browns.
   
3. **Portable simple formulas for RYB-to-RGB conversion and subtractive color mixing.**

   There is a lot of literature on subtractive color mixing.  My approach aims for a compromise between simplicity and accuracy, with much less code than a reflectance-based approach.
   
# Challenges and Design Principles of ArtColors
A brief summary of the challenges of designing a subtractive Red-Yellow-Blue color space explains the principles I used to design ArtColors.  It also summarizes many of the things I learned while researching this project, and so I offer it in case it is instructive to others.

Hereafter common color space names will be abbreviated as follows:

**RYB-**=*Subtractive Red-Yellow-Blue color space*  Our color system.  
**RGB+**=*Additive Red-Green-Blue*  The system most widely used for monitor display. [Wiki](https://en.wikipedia.org/wiki/RGB_color_model)  
**HSV**=*Hue-Saturation-Value*  An alternative representation of the RGB+ system.  [Wiki](https://en.wikipedia.org/wiki/HSL_and_HSV)  
**CYMK**=*Cyan-Yellow-Magenta-Black*  A subtractive system, most frequently used in printing.  [Wiki](https://en.wikipedia.org/wiki/CMYK_color_model)  

## Challenge #1: Everyone learns color mixing using RYB- primaries
From the time you first played with crayons or fingerpaints, you've been using an RYB- color space.  Red plus green make a murky brown, but in RGB+ they make yellow.  Red and blue make a dark purple, but in RGB+ they make a bright magenta.  This can make it hard to quickly find the colors you want when blending.

RGB+ makes sense for what it was designed to do.  Computer monitors are emissive: they emit light which therefore blends additively.  No painting can do this.  Art media are reflective: the pigments we use absorb incident white light and reflect what they don't absorb.  Thus, they are subtractive: the more pigments one adds, the less light is reflected to the eye.  RGB+ was designed to align its primaries with the colors of light to which the three types of human [cone cells](https://en.wikipedia.org/wiki/Cone_cell) in the eye are most responsive.  Any sufficiently distinct set of primaries could have been chosen, but RGB was chosen for basically two reasons: the physiology of the human eye, and RGB are "spectral" colors -- colors seen when passing white light through a prism.  The disadvantages are: the vast majority our experience mixing colors is in a subtractive space, and many colors in our experience are non-spectral (see below).

Moreover, traditional instruction in art still uses RYB- to teach color theory.  Traditional art color theory has a long and complex history, in part driven by the range of pigments available to artists and their vast experience.  While the theoretical formalization of this body of knowledge into RYB- was premised in part on erroneous 19th century ideas of the physics of color, the approach is still fundamentally valid -- as evidenced by the fact that artists, who devote enormous study to color -- still use it as a reference.  The success of RYB- does not depend on its ability to theoretically unify the physics of light and human physiology, but on its *empirical validity* to accurately predict what color will result from subtractive color mixing.

Thus almost everyone who has ever taken an art class or read a book on art color theory has encountered the standard RYB- color wheel:  
![Artist Color Wheel](images/ArtColorWheel.jpg)

### Principle #1: ArtColors should mimic a standard RYB- color wheel

## Challenge #2: RGB+ and HSV color wheels are blue-green dominated
Because two of the three primaries are *green and blue*, the yellow-orange range is severely compressed, purple is likewise narrow and dominated by magenta.  One has to be sniper to select these hues.  Small changes in RGB values or HSV hue quickly sweep through these ranges of color.

Compare the standard HSV color wheel with the ArtColor color wheel to see the problem and its solution:
![HSV Color Wheel](images/HSV-wheel.png) ![ArtColor Wheel](images/ArtColor-ColorWheel.png)

### Principle #2: ArtColors should make it easy to select a wide range of hues by balancing the spectrum
The ArtColor Wheel broadens the yellow-orange spectrum and adds deeper purples, while keeping distinct lines for cyan and magenta.  Note that red, yellow and blue primaries (small squares) are separated by 120 degrees.  This will faciliate traditional palette selection and blending (see below).  *How* to transform RGB+ to RYB- is where the math gets interesting.

If you are the inquisitive sort who wonders why RGB+ fails at providing a balanced spectrum of colors, there are two reasons.  The first we can summarize by analogy: green and blue aren't very "orthonomal" in our color experience.  But this merely begs the question: why not?  The answer is not trivial.  Indeed the question is even more perpleixing when one considers that RGB is based in part on the physiology of human cone cells.  Shouldn't green and blue *seem* orthonormal to us insofar as they are aligned to the "vector bases" of our cone cell "inputs"?  The answer is that our *psychological* perception of color is not a direct result of human cone cell neuron response curves.  The brain *does its own mixing* to create colors we commonly experience but are not aligned to the frequency-response curve of any one cone cell.  These colors are [non-spectral colors](https://en.wikipedia.org/wiki/Spectral_color#Non-spectral_colors): you don't see them in prismatically separated light.

Non-spectral colors include:  
*All grays!* (thank you, rod cells)  
*All tints and shades* = art terms for adding white or black respectively to a "pure" color.  All mixtures of a spectral color plus any shade of gray (from white to black) result in a non-spectral color.  In physical media, this happens all the time!  
*Important hues like purple* (and any tint or shade thereof, like pink or rose).  "Purple" isn't a spectral color: the perception of purple is the psychological result when the brain receives stimuli from *both* the red-sensitive and blue-sensitive cone cells at the same time.

So RGB is based on the physics of emitted light, and its primaries are selected based on the physiology of human cone cells.  But RGB *doesn't* factor in higher-level yet *intrinsically human* neurological factors of how the brain generates certain color perceptions that are non-spectral.  Art color theory was attentive to this fact from mere experience, even without understanding of the underlying causes.  In this regard, they were better empiricists, and RYB- theory reflects that advantage in my opinion.


