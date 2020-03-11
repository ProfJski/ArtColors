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
A brief summary of the challenges of designing a subtractive Red-Yellow-Blue color space explains the principles I used to design ArtColors.  

Hereafter common color space names will be abbreviated as follows:

**RYB-**=*Subtractive Red-Yellow-Blue color space*  Our color system.  
**RGB+**=*Additive Red-Green-Blue*  
**HSV**=*Hue-Saturation-Value*  
**CYMK**=*Cyan-Yellow-Magenta-Black*  A subtractive system, most frequently used in printing.









