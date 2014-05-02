// Copyright (c) 2014, Activision
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided 
// that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and 
//    the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and 
// the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or 
// promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <vector>
#include <cstdlib>
#include <stdio.h>

#include "../codecs.h"

void fillSemiRandom(unsigned char* buffer, unsigned int length, int stride=1)
{
    for (unsigned int i=0;i<length;i++)
        buffer[i*stride] = (rand()&0xFF);
    for (int smooth=0;smooth<3;smooth++)
    {
        for (unsigned int i=1;i<length;i++)
            buffer[i*stride] = (buffer[(i-1)*stride]+buffer[i*stride])/2;
    }
}

int main(int argc, char **argv)
{

    printf("Zoe Codec Test\n\n");

    static const int test_width = 64;
    static const int test_height = 64;

    printf("Test grayscale 8 bit\n");
    {
        std::vector<unsigned char> input_data(test_width * test_height*3);
        srand(2501);
        fillSemiRandom(&input_data[0], test_width * test_height*3);

        std::vector<unsigned char> compressed(test_width * test_height*3 * 2);
        unsigned int compressed_size = Compress_Y8_To_HY8(test_width*3, test_height, &input_data[0], &compressed[0]);

        printf("  Compressed size %d/%d\n", compressed_size, test_width*3 * test_height);

        compressed.resize(compressed_size);

        std::vector<unsigned char> output_data(test_width*3 * test_height);
        Decompress_HY8_To_Y8(compressed_size, test_width*3, test_height, &compressed[0], &output_data[0]);

        for (int i=0;i<test_width*3 * test_height;i++)
        {
            if (input_data[i] != output_data[i])
            {
                printf("Error at offset %d, %02X != %02X\n", i, input_data[i], output_data[i]);
                return 1;
            }
        }
        printf("  Passed\n");
    }

    // RGB24
    {
        static const int nb_channels = 3;
        std::vector<unsigned char> input_data(test_width * test_height * nb_channels);
        srand(2501);
        for (int c=0;c<nb_channels;c++)
            fillSemiRandom(&input_data[c], test_width * test_height, nb_channels);

        printf("Test RGB24 8 bit compressed as if it was a single grayscale image of 3x the size\n");
        {
            std::vector<unsigned char> compressed(test_width * test_height * nb_channels * 1000, 0);
            unsigned int compressed_size = Compress_Y8_To_HY8(test_width * nb_channels, test_height, &input_data[0], &compressed[0]);

            printf("  Compressed size %d/%d\n", compressed_size, test_width * test_height * nb_channels);

            compressed.resize(compressed_size);

            std::vector<unsigned char> output_data(test_width * test_height * nb_channels, 0);
            Decompress_HY8_To_Y8(compressed_size, test_width * nb_channels, test_height, &compressed[0], &output_data[0]);

            for (int i=0;i<test_width * test_height * nb_channels;i++)
            {
                if (input_data[i] != output_data[i])
                {
                    printf("Error at offset %d, %02X != %02X\n", i, input_data[i], output_data[i]);
                    return 1;
                }
            }
            printf("  Passed\n");
        }

        printf("Test RGB24 8 bit compressed\n");
        {
            std::vector<unsigned char> compressed(test_width * test_height * nb_channels * 1000, 0);
            unsigned int compressed_size = Compress_RGB24_To_HRGB24(test_width, test_height, &input_data[0], &compressed[0]);

            printf("  Compressed size %d/%d\n", compressed_size, test_width * test_height * nb_channels);

            compressed.resize(compressed_size);

            std::vector<unsigned char> output_data(test_width * test_height * nb_channels, 0);
            Decompress_HRGB24_To_RGB24(compressed_size, test_width, test_height, &compressed[0], &output_data[0]);

            for (int i=0;i<test_width * test_height * nb_channels;i++)
            {
                if (input_data[i] != output_data[i])
                {
                    printf("Error at offset %d, %02X != %02X\n", i, input_data[i], output_data[i]);
                    return 1;
                }
            }
            printf("  Passed\n");
        }
    }

    return 0;
}

