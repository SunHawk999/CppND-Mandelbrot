#include "BMP.h"
#include<iostream>

int main(){

    //Create a BMP image in memory, modify it and save to disk
    BMP bmp1(1000, 1000);

    //Apply mandelbrot image to memory
    bmp1.mandelbrot(100, 150, 100, 0);
    
    //Write image in memory to a .bmp file
    bmp1.write("mandel_image.bmp");
 
    return 0;
}