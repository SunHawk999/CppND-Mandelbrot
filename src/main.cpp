#include "BMP.h"
//#include "Drawing.h"
#include<iostream>

//Start by creating an image making program. Be sure to refrence where the code was based from.


int main(){

    //Create a BMP image in memory, modify it and save to disk
    BMP bmp1(800, 800);

    //bmp1.fillRegion(0, 0, 800, 800, 255, 255, 255, 0);
    
   // bmp1.mandelbrot(800, 800, 0, 150, 0, 0);
    
    bmp1.write("image_test.bmp");
 
    return 0;
}