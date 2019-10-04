
#pragma once
#include<fstream>
#include<stdexcept>
#include<vector>

//Header code based from https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/


#pragma pack(push, 1)
//File header, all BMP images starts with a five elements file header. This has info about 
//the file type, file size and location of the pixel data.
struct BMPFileHeader{
    uint16_t file_type{0x4D42};     //File type always BM which is 0x4D42
    uint32_t file_size{0};          //Size of the file in bytes
    uint16_t reserved1{0};          //Reserved, always 0
    uint32_t reserved2{0};          //Reserved, always 0
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
    uint32_t color_space_type{0x75324742}; //Default "sRGB" (0x75324742)
    uint32_t unused[16]{0};              //Unusued data for sRGB color space 
};
#pragma pack(pop)


struct BMP{

    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;
    std::vector<uint8_t> data;

/*
    //Writing an image to disk
    void write(const char *fname){

        std::ofstream of{fname, std::ios_base::binary};

        if(of){
            if(bmp_info_header.bit_count == 32)
                write_headers_and_data(of);
        }

    }
*/
private:
/*
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
*/
};
