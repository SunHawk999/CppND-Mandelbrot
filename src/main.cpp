#include "BMP.h"

//Start by creating an image making program. Be sure to refrence where the code was based from.


int main(){

    //Create a BMP image in memory, modify it and save to disk
    BMP bmp1(800, 600);
    bmp1.fillRegion(50, 20, 100, 200, 0, 0, 255, 255);
    bmp1.write("image_test.bmp");

    return 0;
}