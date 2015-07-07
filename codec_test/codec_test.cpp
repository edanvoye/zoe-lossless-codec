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
#include "../huffman.h"


class TestBitPacker
{
public:
    TestBitPacker(char* bufferStart) : cur_ptr(bufferStart), cur(0), used_bits(0)
    {
    }
    void pack(int bitcount, unsigned int value)
    {
        while (bitcount>0)
        {
            int to_use = std::min(bitcount, 8-used_bits);
            cur <<= to_use;
            unsigned int mask = ((1<<to_use)-1) << (bitcount-to_use);
            cur |= (value&mask) >> (bitcount-to_use);
            bitcount -= to_use;
            used_bits += to_use;
            if (used_bits==8)
            {
                *cur_ptr = cur;
                cur = 0;
                used_bits = 0;
                cur_ptr++;
            }
        }
    }
    void flush()
    {
        if (used_bits>0)
            *cur_ptr = cur;
    }
private:
    unsigned char cur;
    int used_bits;
    char * cur_ptr;
};

void pack10Bits(const std::vector<unsigned short>& in, unsigned char * packed)
{
    TestBitPacker bitPacker((char*)packed);

    for (int i=0;i<in.size();i++)
    {
        unsigned short value_to_pack = in[i] & 0x03FF;
        bitPacker.pack(10, value_to_pack);
    }
    bitPacker.flush();
}

void pack12Bits(const std::vector<unsigned short>& in, unsigned char * packed)
{
    TestBitPacker bitPacker((char*)packed);

    for (int i=0;i<in.size();i++)
    {
        unsigned short value_to_pack = in[i] & 0x0FFF;
        bitPacker.pack(12, value_to_pack);
    }
    bitPacker.flush();
}

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

void swap(unsigned short* b)
{
    unsigned char * tmp = (unsigned char *)b;
    std::swap(tmp[0],tmp[1]);
}

void fillSemiRandom10(unsigned short* buffer, unsigned int length, int stride=1)
{
    for (unsigned int i=0;i<length;i++)
        buffer[i*stride] = (rand()&0x03FF); // 10 bits
    for (int smooth=0;smooth<3;smooth++)
    {
        for (unsigned int i=1;i<length;i++)
            buffer[i*stride] = (buffer[(i-1)*stride]+buffer[i*stride])/2;
    }
}

void fillSemiRandom12(unsigned short* buffer, unsigned int length, int stride=1)
{
    for (unsigned int i=0;i<length;i++)
        buffer[i*stride] = (rand()&0x0FFF); // 12 bits
    for (int smooth=0;smooth<3;smooth++)
    {
        for (unsigned int i=1;i<length;i++)
            buffer[i*stride] = (buffer[(i-1)*stride]+buffer[i*stride])/2;
    }
}

template <typename T>
void print_binary(T b)
{
    for (int i=(sizeof(T)*8)-1;i>=0;i--)
        printf("%c", (b&(1<<i))?'1':'0');
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

    // UYVY
    {
        static const int nb_channels = 2;
        std::vector<unsigned char> input_data(test_width * test_height * nb_channels);
        srand(2501);
        for (int c=0;c<nb_channels;c++)
            fillSemiRandom(&input_data[c], test_width * test_height, nb_channels);

        printf("Test UYVY 8 bit compressed as if it was a single grayscale image of 2x the size\n");
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

        printf("Test UYVY 16 bit compressed\n");
        {
            std::vector<unsigned char> compressed(test_width * test_height * nb_channels * 1000, 0);
            unsigned int compressed_size = Compress_UYVY_To_HUYVY(test_width, test_height, &input_data[0], &compressed[0]);

            printf("  Compressed size %d/%d\n", compressed_size, test_width * test_height * nb_channels);

            compressed.resize(compressed_size);

            std::vector<unsigned char> output_data(test_width * test_height * nb_channels, 0);
            Decompress_HUYVY_To_UYVY(compressed_size, test_width, test_height, &compressed[0], &output_data[0]);

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

    printf("Test grayscale 10 bit (10 bits of data in LSB of 16 bit words)\n");
    {
        std::vector<unsigned short> input_data(test_width * test_height); // 10 bits in LSB of 16
        srand(2501);
        fillSemiRandom10(&input_data[0], test_width * test_height);

        std::vector<unsigned char> compressed(test_width * test_height * 2 * 2);
        unsigned int compressed_size = Compress_Y10_To_HY10(test_width, test_height, (const unsigned char *)&input_data[0], &compressed[0]);

        printf("  Compressed size %d/%d\n", compressed_size, test_width * test_height * 2);

        compressed.resize(compressed_size);

        std::vector<unsigned short> output_data(test_width * test_height);
        Decompress_HY10_To_Y10(compressed_size, test_width, test_height, &compressed[0], (unsigned char *)&output_data[0]);

        for (int i=0;i<test_width * test_height;i++)
        {
            if ((output_data[i]&0xFC00) != 0)
            {
                printf("Error at offset %d, %04X\n", i, output_data[i]);
                return 1;
            }
            if ((input_data[i]&0x03FF) != (output_data[i]&0x03FF))
            {
                printf("Error at offset %d, %04X != %04X\n", i, input_data[i], output_data[i]);
                return 1;
            }
        }
        printf("  Passed\n");
    }

    {
        std::vector<unsigned short> input_data(16);
        input_data[0] = 0x03FF;
        input_data[1] = 0;
        input_data[2] = 0x03FF;
        input_data[3] = 0;
        input_data[4] = 0x03FF;
        input_data[5] = 0;
        input_data[6] = 0x03FF;
        input_data[7] = 0;
        input_data[8] = 0x03FF;
        input_data[9] = 0;
        input_data[10] = 0x03FF;
        input_data[11] = 0;
        input_data[12] = 0x03FF;
        input_data[13] = 0;
        input_data[14] = 0x03FF;
        input_data[15] = 0;

        std::vector<unsigned char> packed_buffer(20);

        pack10Bits(input_data, &packed_buffer[0]);

        for (int i=0;i<20;i++)
        {
            printf("byte %d : ", i);
            print_binary(packed_buffer[i]);
            printf(" (%08X)\n", packed_buffer[i]);
        }

        UnpackBitReader<10, unsigned short> reader((const unsigned short *)&packed_buffer[0]);
        for (int i=0;i<16;i++)
        {
            unsigned short data = reader.next();
            printf("result: %08X\n", data);
        }
    }

    printf("Test grayscale 10 bit (packed 10 bit data)\n");
    {
        std::vector<unsigned short> input_data(test_width * test_height); // 10 bits in LSB of 16
        srand(2501);
        fillSemiRandom10(&input_data[0], test_width * test_height);

        // Swap endianness
        for (unsigned int i=0;i<test_width * test_height;i++)
            swap(&input_data[i]);

        std::vector<unsigned char> packed_buffer(test_width * test_height * 20 / 16);
        pack10Bits(input_data, &packed_buffer[0]);

        UnpackBitReader<10, unsigned short> reader((const unsigned short *)&packed_buffer[0]);

        std::vector<unsigned char> compressed(test_width * test_height * 2 * 4);
        unsigned int compressed_size = Compress_PY10_To_HY10(test_width, test_height, (const unsigned char *)&packed_buffer[0], &compressed[0]);

        printf("  Compressed size %d/%d (unpacked:%d)\n", compressed_size, packed_buffer.size(), test_width * test_height * 2);

        compressed.resize(compressed_size);

        std::vector<unsigned short> output_data(test_width * test_height);
        Decompress_HY10_To_Y10(compressed_size, test_width, test_height, &compressed[0], (unsigned char *)&output_data[0]);

        for (int i=0;i<test_width * test_height;i++)
        {
            // check packing-unpacking
            unsigned short data = reader.next();
            if ((input_data[i]&0x03FF) != data)
            {
                printf("Error at offset %d, %04X != %04X\n", i, input_data[i], data);
                return 1;
            }

            // check output is only 10 bits
            if ((output_data[i]&0xFC00) != 0)
            {
                printf("Error at offset %d, %04X\n", i, output_data[i]);
                return 1;
            }

            // check output 10 bits equal input 10 bits
            if ((input_data[i]&0x03FF) != (output_data[i]&0x03FF))
            {
                printf("Error at offset %d, %04X != %04X\n", i, input_data[i], output_data[i]);
                return 1;
            }
        }
        printf("  Passed\n");
    }

    printf("Test grayscale 12 bit (packed 12 bit data)\n");
    {
        std::vector<unsigned short> input_data(test_width * test_height); // 12 bits in LSB of 16
        srand(2501);
        fillSemiRandom12(&input_data[0], test_width * test_height);

        // Swap endianness
        for (unsigned int i=0;i<test_width * test_height;i++)
            swap(&input_data[i]);

        std::vector<unsigned char> packed_buffer(test_width * test_height * 2);
        pack12Bits(input_data, &packed_buffer[0]);

        UnpackBitReader<12, unsigned short> reader((const unsigned short *)&packed_buffer[0]);

        std::vector<unsigned char> compressed(test_width * test_height * 2 * 4);
        unsigned int compressed_size = Compress_PY12_To_HY12(test_width, test_height, (const unsigned char *)&packed_buffer[0], &compressed[0]);

        printf("  Compressed size %d/%d (unpacked:%d)\n", compressed_size, packed_buffer.size(), test_width * test_height * 2);

        compressed.resize(compressed_size);

        std::vector<unsigned short> output_data(test_width * test_height);
        Decompress_HY12_To_Y12(compressed_size, test_width, test_height, &compressed[0], (unsigned char *)&output_data[0]);

        for (int i=0;i<test_width * test_height;i++)
        {
            // check packing-unpacking
            unsigned short data = reader.next();
            if ((input_data[i]&0x0FFF) != data)
            {
                printf("Error at offset %d, %04X != %04X\n", i, input_data[i], data);
                return 1;
            }

            // check output is only 12 bits
            if ((output_data[i]&0xF000) != 0)
            {
                printf("Error at offset %d, %04X\n", i, output_data[i]);
                return 1;
            }

            // check output 12 bits equal input 12 bits
            if ((input_data[i]&0x0FFF) != (output_data[i]&0x0FFF))
            {
                printf("Error at offset %d, %04X != %04X\n", i, input_data[i], output_data[i]);
                return 1;
            }
        }
        printf("  Passed\n");
    }

    return 0;
}

