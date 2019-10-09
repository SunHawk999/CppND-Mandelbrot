
#pragma once
#include<fstream>
#include<stdexcept>
#include<vector>
#include<iostream>

//Header code based from https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/


#pragma pack(push, 1)
//File header, all BMP images starts with a five elements file header. This has info about 
//the file type, file size and location of the pixel data.
struct BMPFileHeader{
    uint16_t file_type{0x4D42};     //File type always BM which is 0x4D42
    uint32_t file_size{0};          //Size of the file in bytes
    uint16_t reserved1{0};          //Reserved, always 0
    uint16_t reserved2{0};          //Reserved, always 0
    uint32_t offset_data{0};        //Start position of pixel data, bytes from beginging of file
};

//Info header, Has info on the width and height of the image. Also bits depth, etc.
struct BMPInfoHeader{
    uint32_t size{0};           //Size of this header in bytes
    int32_t width{0};           //Width of bitmap in pixels
    int32_t height{0};          //Height of bitmap in pixels
                                    //(If positive, bottom-up with origin in lower left corner)
                                    //(If negative, top-down with origin in upper left corner)
    uint16_t planes{1};          //Number of planes for the target device, always 1
    uint16_t bit_count{0};       //Number of bits per pixel
    uint32_t compression{0};     //0 or 3 - uncompressed. Will only consider uncompressed BMP images!
    uint32_t size_image{0};      //0 - for uncompressed images 
    int32_t x_pixels_per_meter{0};
    int32_t y_pixels_per_meter{0};
    uint32_t colors_used{0};        //Number of color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important{0};   //Number of colors used for displaying the bitmap, If 0 all colors are required
};

//Color header, contains info on the color space and bit masks
struct BMPColorHeader{
    uint32_t red_mask{0x00ff0000};      //Bit mask for red channel
    uint32_t green_mask{0x0000ff00};    //Bit mask for green channel
    uint32_t blue_mask{0x000000ff};     //blue channel
    uint32_t alpha_mask{0xff000000};    //alpha channel
    uint32_t color_space_type{0x73524742}; //Default "sRGB" (0x73524742)
    uint32_t unused[16]{0};              //Unusued data for sRGB color space 
};
#pragma pack(pop)


struct BMP{

    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;
    std::vector<uint8_t> data;

    //Create a BMP image in memory
    BMP(int32_t width, int32_t height, bool has_alpha = true){

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

            //std::cout<<"Test"<<std::endl;
        }
    }

    //Modify a specified region of a created or read bmp image
    void fillRegion(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint8_t R, uint8_t G, uint8_t B, uint8_t A){

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
        //std::cout<<"TestFill"<<std::endl;
    }

    //Writing an image to disk
    void write(const char *fname){

        std::ofstream of{fname, std::ios_base::binary};

        if(of){
            if(bmp_info_header.bit_count == 32){
                write_headers_and_data(of);
                //std::cout<<"Test1"<<std::endl;
            }

            else if(bmp_info_header.bit_count == 24){
                if(bmp_info_header.width % 4 == 0){
                    write_headers_and_data(of);
                    //std::cout<<"Test2"<<std::endl;
                }

                else{
                    uint32_t new_stride = make_stride_aligned(4);
                    std::vector<uint8_t> padding_row(new_stride - row_stride);

                    write_headers(of);

                    for(int y = 0; y < bmp_info_header.height; ++y){
                        of.write((const char*)(data.data() + row_stride * y), row_stride);
                        of.write((const char*)padding_row.data(), padding_row.size());
                    }
                    std::cout<<"Test3"<<std::endl;
                }
            }
            else
                throw std::runtime_error("This program can only treat 24 or 32 bits per pixel.");
        }
        else
            throw std::runtime_error("Unable to open the output image file.");
    }

private:

    uint32_t row_stride{0};

    void write_headers(std::ofstream &of){

        of.write((const char*)&file_header, sizeof(file_header));
        of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));

        if(bmp_info_header.bit_count = 32){
            of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));
        }
    }

    void write_headers_and_data(std::ofstream &of){
        write_headers(of);
        of.write((const char*)data.data(), data.size());
    }

    //Add 1 to the row_stride until it is divisble with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride){
        uint32_t new_stride = row_stride;
        while(new_stride % align_stride != 0){
            new_stride++;
        }
        return new_stride;
    }

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void check_color_header(BMPColorHeader &bmp_color_header) {
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
};
