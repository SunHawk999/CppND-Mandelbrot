#include "BMP.h"
#include<stdexcept>
#include<vector>
#include<complex>
#include<fstream>

//Seperate class from BMP.h file in order to have other drawings, and eventually mandelbrot generator
class Drawing : BMP{

    //Given a the coordinates of a point, check if the convergences for c = x + i*y, and returns
    //a value, 255 or 0
    void mandelbrot(uint32_t x, uint32_t y, uint8_t R, uint8_t B, uint8_t G, uint8_t A){

        //Divide by the image dimensions to get values smaller than 1, then apply a translation
        std::complex<float> point( (float)x/bmp_info_header.width - 1.5, (float)y/bmp_info_header.height - 0.5);

        //translation
        std::complex<float> z(0, 0);

            unsigned int nb_iter = 0;
            while(abs(z) < 2 && nb_iter <= 34){
                z = z * z + point;
                nb_iter++;
            }

            //Set a point on the image in memory to 255 or 0 based on mandelbrot
            //Need to use channels somehow fro BMP.h
            /*
            if(nb_iter < 34) return 255;
            else return 0;
            */
            uint32_t channels = bmp_info_header.bit_count / 8;

            for(uint32_t i = 0; i < bmp_info_header.width; ++i){
                for(uint32_t j = 0; j < bmp_info_header.height; ++j){

                    if(nb_iter < 34){
                        data[channels * (y * bmp_info_header.width + x) + 2] = R;
                        data[channels * (y * bmp_info_header.width + x) + 1] = G;
                        data[channels * (y * bmp_info_header.width + x) + 0] = B;

                        if(channels == 4)
                            data[channels * (y * bmp_info_header.width + x) + 3] = A;
                    }

                    else{
                        data[channels * (y * bmp_info_header.width + x) + 2] = 0;
                        data[channels * (y * bmp_info_header.width + x) + 1] = 0;
                        data[channels * (y * bmp_info_header.width + x) + 0] = 0;

                        if(channels == 4)
                            data[channels * (y * bmp_info_header.width + x) + 3] = 0;
                    }
                }
            }
    } 

};