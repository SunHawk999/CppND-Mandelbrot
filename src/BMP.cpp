#include "BMP.h"
#include<fstream>
#include<stdexcept>
#include<vector>
#include<iostream>
#include<complex>

//Create a BMP image in memory
BMP::BMP(int32_t width, int32_t height, bool has_alpha){

    if(width <= 0 || height <= 0)
       throw std::runtime_error("The image width and height must be positive numbers.");

    bmp_info_header.width = width;
    bmp_info_header.height = height;

    if(has_alpha){
        bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        
        bmp_info_header.bit_count = 32;
        bmp_info_header.compression = 3;
        row_stride = width * 4;
        data.resize(row_stride * height);
        file_header.file_size = file_header.offset_data + data.size();
    }
 
    else{
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        bmp_info_header.bit_count = 24;
        bmp_info_header.compression = 0;
        row_stride = width * 3;
        data.resize(row_stride * height);

        uint32_t new_stride = make_stride_aligned(4);
        file_header.file_size = file_header.offset_data + data.size() + bmp_info_header.height * (new_stride - row_stride);
    }
}

//Writing an image to disk
void BMP::write(const char *fname){

    std::ofstream of{fname, std::ios_base::binary};

    if(of){
        if(bmp_info_header.bit_count == 32)
            write_headers_and_data(of);

        else if(bmp_info_header.bit_count == 24){
            if(bmp_info_header.width % 4 == 0)
                write_headers_and_data(of);

            else{
                uint32_t new_stride = make_stride_aligned(4);
                std::vector<uint8_t> padding_row(new_stride - row_stride);

                write_headers(of);

                for(int y = 0; y < bmp_info_header.height; ++y){
                    of.write((const char*)(data.data() + row_stride * y), row_stride);
                    of.write((const char*)padding_row.data(), padding_row.size());
                }
            }
        }
        else
            throw std::runtime_error("This program can only treat 24 or 32 bits per pixel.");
    }
    else
        throw std::runtime_error("Unable to open the output image file.");
}

//Modify a specified region of a created or read bmp image
void BMP::fillRegion(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint8_t R, uint8_t G, uint8_t B, uint8_t A){

    if(x0 + w > (uint32_t)bmp_info_header.width || y0 + h > (uint32_t)bmp_info_header.height)
        throw std::runtime_error("This region does not fit in the image!");

    uint32_t channels = bmp_info_header.bit_count / 8;
    for(uint32_t x = x0; x < x0 + w; ++x){
        for(uint32_t y = y0; y < y0 + h; ++y){
            data[channels * (y * bmp_info_header.width + x) + 2] = R;
            data[channels * (y * bmp_info_header.width + x) + 1] = G;
            data[channels * (y * bmp_info_header.width + x) + 0] = B;

            if(channels == 4)
                data[channels * (y * bmp_info_header.width + x) + 3] = A;
        }
    }
}

//Given a the coordinates of a point, check if the convergences for c = x + i*y, and returns
//a value, 255 or 0
void BMP::mandelbrot(uint32_t x, uint32_t y, uint8_t R, uint8_t B, uint8_t G, uint8_t A){

    //Divide by the image dimensions to get values smaller than 1, then apply a translation
    std::complex<float> point( (float)x/bmp_info_header.width - 1.5, (float)y/bmp_info_header.height - 0.5);

    //translation
    std::complex<float> z(0, 0);

        //Set a point on the image in memory to 255 or 0 based on mandelbrot
        //Need to use channels somehow fro BMP.h
        /*
        if(nb_iter < 34) return 255;
        else return 0;
        */
        uint32_t channels = bmp_info_header.bit_count / 8;

        for(uint32_t i = 0; i < bmp_info_header.width; ++i){
            for(uint32_t j = 0; j < bmp_info_header.height; ++j){

                unsigned int nb_iter = 0;
                while(abs(z) < 2 && nb_iter <= 34){
                    z = z * z + point;
                    nb_iter++;
                }

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


void BMP::write_headers(std::ofstream &of){

    of.write((const char*)&file_header, sizeof(file_header));
    of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));

    if(bmp_info_header.bit_count = 32)
        of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));
}

void BMP::write_headers_and_data(std::ofstream &of){
    write_headers(of);
    of.write((const char*)data.data(), data.size());
}

//Add 1 to the row_stride until it is divisble with align_stride
uint32_t BMP::make_stride_aligned(uint32_t align_stride){
    uint32_t new_stride = row_stride;
    while(new_stride % align_stride != 0){
        new_stride++;
    }
    return new_stride;
}

// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void BMP::check_color_header(BMPColorHeader &bmp_color_header) {
    BMPColorHeader expected_color_header;
    if(expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
        throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
    }
    if(expected_color_header.color_space_type != bmp_color_header.color_space_type) {
        throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
    }
}