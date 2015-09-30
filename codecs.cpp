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
//
// Revision History:
//  2014-03-28 E. Danvoye    Initial Release
//

#include "stdafx.h"
#include "codecs.h"
#include "huffman.h"

unsigned Compress_RGB24_To_RGB24(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 3;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

unsigned Compress_RGB32_To_RGB32(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 4;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

bool Decompress_RGB24_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 3;

    if (inSize != len)
        return false;

    memcpy(out_frame, in_frame, len);
    return true;
}

bool Decompress_RGB32_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 4;

    if (inSize != len)
        return false;

    memcpy(out_frame, in_frame, len);
    return true;
}

unsigned Compress_Y8_To_Y8(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 1;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

unsigned Compress_Y8_To_HY8(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 1> huff(width, height);
    unsigned len = huff.encode<TrivialBitReader<char> >((const char *)in_frame, (char*)out_frame);
    return len;
}

unsigned Compress_Y10_To_HY10(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    unsigned len = huff.encode<TrivialBitReader<short> >((const short *)in_frame, (char*)out_frame);
    return len;
}

unsigned Compress_PY10_To_HY10(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    unsigned len = huff.encode<UnpackBitReader<10, short> >((const short *)in_frame, (char*)out_frame);
    return len;
}

unsigned Compress_PY12_To_HY12(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    unsigned len = huff.encode<UnpackBitReader<12, short> >((const short *)in_frame, (char*)out_frame);
    return len;
}

unsigned Compress_Y10_To_Y10(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 2;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

bool Decompress_Y8_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 1;

    if (inSize != len)
        return false;

    memcpy(out_frame, in_frame, len);
    return true;
}

bool Decompress_HY8_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HY8_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb24>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HY10_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb24>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HY10_To_Y10(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    return huff.decode<short, OutputProcessing::Default>((const char *)in_frame, (short*)out_frame);
}

bool Decompress_HY8_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::interleave_yuyv>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HY10_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    return huff.decode<short, OutputProcessing::interleave_yuyv>((const char *)in_frame, (short*)out_frame);
}

bool Decompress_HY10_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_Y10_To_Y10(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 2;

    if (inSize != len)
        return false;

    memcpy(out_frame, in_frame, len);
    return true;
}

bool Decompress_Y10_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 2;

    if (inSize != len)
        return false;

    const unsigned short* in_values = (const unsigned short*)(in_frame);

    for (unsigned i=0;i<len;i++)
        out_frame[i] = (in_values[i]>>2)&0x00FF;

    return true;
}

bool Decompress_Y8_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    int* destination = (int *)(out_frame);

    // Convert MONO to YUV422 (U and V will be zero)
    for (unsigned i=0;i<width*height/2;i++)
    {
        unsigned char pixel1 = in_frame[i*2+0];
        unsigned char pixel2 = in_frame[i*2+1];
        unsigned int yuv = (pixel1<<8) | (pixel2<<24) | 0x00800080;
        destination[i] = yuv;
    }

    return true;
}

bool Decompress_Y10_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    // LOSSY, we drop the two LSB of the Y10 data

    const unsigned short * img_16bit = (const unsigned short *)(in_frame);
    int* destination = (int *)(out_frame);

    for (unsigned i=0;i<width*height/2;i++)
    {
        unsigned char pixel1 = img_16bit[i*2+0]>>2;
        unsigned char pixel2 = img_16bit[i*2+1]>>2;
        unsigned int yuv = (pixel1<<8) | (pixel2<<24) | 0x00800080;
        destination[i] = yuv;
    }

    return true;
}

unsigned Compress_RGB24_To_HRGB24(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 3> huff(width, height);
    unsigned len = huff.encode<TrivialBitReader<char> >((const char *)in_frame, (char*)out_frame);
    return len;
}

bool Decompress_HRGB24_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 3> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

unsigned Compress_RGB32_To_HRGB32(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 4> huff(width, height);
    unsigned len = huff.encode<TrivialBitReader<char> >((const char *)in_frame, (char*)out_frame);
    return len;
}

bool Decompress_HRGB32_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 4> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

unsigned Compress_UYVY_To_HUYVY(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 2> huff(width, height);
    unsigned len = huff.encode<TrivialBitReader<char> >((const char *)in_frame, (char*)out_frame);
    return len;
}

bool Decompress_HUYVY_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 2> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HUYVY_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 2> huff(width, height);
    return huff.decode<char, OutputProcessing::uyvy_to_rgb24>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HRGB24_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame, bool reverse_y)
{
    ZoeHuffmanCodec<char, 8, 3> huff(width, height);
    if (reverse_y)
        return huff.decode<char, OutputProcessing::rgb24_to_rgb32_revY>((const char *)in_frame, (char*)out_frame);
    else
        return huff.decode<char, OutputProcessing::rgb24_to_rgb32>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HY8_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb32>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HY10_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb32>((const char *)in_frame, (char*)out_frame);
}

bool Decompress_HUYVY_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8, 2> huff(width, height);
    return huff.decode<char, OutputProcessing::uyvy_to_rgb32>((const char *)in_frame, (char*)out_frame);
}

unsigned Compress_Y12_To_Y12(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 2;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}
bool Decompress_Y12_To_Y12(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 2;

    if (inSize != len)
        return false;

    memcpy(out_frame, in_frame, len);
    return true;
}
bool Decompress_Y12_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    // LOSSY, we drop the four LSB of the Y12 data

    const unsigned short * img_16bit = (const unsigned short *)(in_frame);
    int* destination = (int *)(out_frame);

    for (unsigned i=0;i<width*height/2;i++)
    {
        unsigned char pixel1 = img_16bit[i*2+0]>>4;
        unsigned char pixel2 = img_16bit[i*2+1]>>4;
        unsigned int yuv = (pixel1<<8) | (pixel2<<24) | 0x00800080;
        destination[i] = yuv;
    }

    return true;
}
unsigned Compress_Y12_To_HY12(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    unsigned len = huff.encode<TrivialBitReader<short> >((const short *)in_frame, (char*)out_frame);
    return len;
}
bool Decompress_HY12_To_Y12(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    return huff.decode<short, OutputProcessing::Default>((const char *)in_frame, (short*)out_frame);
}
bool Decompress_HY12_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    return huff.decode<short, OutputProcessing::interleave_yuyv>((const char *)in_frame, (short*)out_frame);
}
bool Decompress_HY12_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb24>((const char *)in_frame, (char*)out_frame);
}
bool Decompress_Y12_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    unsigned len = width * height * 2;

    if (inSize != len)
        return false;

    const unsigned short* in_values = (const unsigned short*)(in_frame);

    for (unsigned i=0;i<len;i++)
        out_frame[i] = (in_values[i]>>4)&0x00FF;

    return true;
}
bool Decompress_HY12_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}
bool Decompress_HY12_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 12, 1> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb32>((const char *)in_frame, (char*)out_frame);
}

