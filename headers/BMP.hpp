#pragma once
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <glad/glad.h>

struct BMP {
    #pragma pack(push, 1)
    struct BMPFileHeader {
        uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
        uint32_t file_size{ 0 };               // Size of the file (in bytes)
        uint16_t reserved1{ 0 };               // Reserved, always 0
        uint16_t reserved2{ 0 };               // Reserved, always 0
        uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
    };

    struct BMPInfoHeader {
        uint32_t size{ 0 };                      // Size of this header (in bytes)
        int32_t width{ 0 };                      // width of bitmap in pixels
        int32_t height{ 0 };                     // width of bitmap in pixels
                                                 //       (if positive, bottom-up, with origin in lower left corner)
                                                 //       (if negative, top-down, with origin in upper left corner)
        uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
        uint16_t bit_count{ 0 };                 // No. of bits per pixel
        uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
        uint32_t size_image{ 0 };                // 0 - for uncompressed images
        int32_t x_pixels_per_meter{ 0 };
        int32_t y_pixels_per_meter{ 0 };
        uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
        uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
    };

    struct BMPColorHeader {
        uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
        uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
        uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
        uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
        uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
        uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
    };
    #pragma pack(pop)

    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;
    std::vector<uint8_t> data;
    unsigned int textureID;
    bool validSource = true;

    BMP(const char *fname) {
        read(fname);
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp_info_header.width, bmp_info_header.height, 0, GL_BGR, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void read(const char *fname) {
        std::ifstream inp{ fname, std::ios_base::binary };
        if (inp) {
            inp.read((char*)&file_header, sizeof(file_header));
            if(file_header.file_type != 0x4D42) {
                std::cout << "Texture Error! Unrecognized file format.\n";
                validSource = false;
                return;
            }
            inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

            // The BMPColorHeader is used only for transparent images
            if(bmp_info_header.bit_count == 32) {
                // Check if the file has bit mask color information
                if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
                    inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
                    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                    if(!check_color_header(bmp_color_header)){
                        validSource = false;
                        return;
                    }
                } else {
                    std::cout << "Texture Error! The file \"" << fname << "\" does not seem to contain bit mask information\n";
                    std::cout << "Texture Error! Unrecognized file format.\n";
                    validSource = false;
                    return;
                }
            }

            // Jump to the pixel data location
            inp.seekg(file_header.offset_data, inp.beg);

            // Adjust the header fields for output.
            // Some editors will put extra info in the image file, we only save the headers and the data.
            if(bmp_info_header.bit_count == 32) {
                bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
                file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            } else {
                bmp_info_header.size = sizeof(BMPInfoHeader);
                file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
            }
            file_header.file_size = file_header.offset_data;

            if (bmp_info_header.height < 0) {
                std::cout << "The program can treat only BMP images with the origin in the bottom left corner!\n";
                validSource = false;
                return;
            }

            data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

            // Here we check if we need to take into account row padding
            if (bmp_info_header.width % 4 == 0) {
                inp.read((char*)data.data(), data.size());
                file_header.file_size += static_cast<uint32_t>(data.size());
            }
            else {
                row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
                uint32_t new_stride = make_stride_aligned(4);
                std::vector<uint8_t> padding_row(new_stride - row_stride);

                for (int y = 0; y < bmp_info_header.height; ++y) {
                    inp.read((char*)(data.data() + row_stride * y), row_stride);
                    inp.read((char*)padding_row.data(), padding_row.size());
                }
                file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
            }
        }
        else {
            std::cout << "Unable to open the input image file \"" << fname << "\"\n";
            validSource = false;
            return;
        }
    }

    operator bool() const{ return validSource; }

private:
    uint32_t row_stride{ 0 };

    // Add 1 to the row_stride until it is divisible with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride) {
        uint32_t new_stride = row_stride;
        while (new_stride % align_stride != 0) {
            new_stride++;
        }
        return new_stride;
    }

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    bool check_color_header(BMPColorHeader &bmp_color_header) {
        BMPColorHeader expected_color_header;
        if(expected_color_header.red_mask != bmp_color_header.red_mask ||
            expected_color_header.blue_mask != bmp_color_header.blue_mask ||
            expected_color_header.green_mask != bmp_color_header.green_mask ||
            expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
            std::cout << "Unexpected color mask format! The program expects the pixel data to be in the BGRA format\n";
            return false;
        }
        if(expected_color_header.color_space_type != bmp_color_header.color_space_type) {
            std::cout << "Unexpected color space type! The program expects sRGB values\n";
            return false;
        }
        return true;
    }
};
