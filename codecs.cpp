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

DWORD Compress_RGB24_To_RGB24(DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 3;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

DWORD Compress_RGB32_To_RGB32(DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 4;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

BOOL Decompress_RGB24_To_RGB24(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 3;

    if (inSize != len)
        return FALSE;

    memcpy(out_frame, in_frame, len);
    return TRUE;
}

BOOL Decompress_RGB32_To_RGB32(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 4;

    if (inSize != len)
        return FALSE;

    memcpy(out_frame, in_frame, len);
    return TRUE;
}

DWORD Compress_Y8_To_Y8(DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 1;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

DWORD Compress_Y8_To_HY8(DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8> huff(width, height);
    DWORD len = huff.encode((const char *)in_frame, (char*)out_frame);
    return len;
}

DWORD Compress_Y10_To_HY10(DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10> huff(width, height);
    DWORD len = huff.encode((const short *)in_frame, (char*)out_frame);
    return len;
}

DWORD Compress_Y10_To_Y10(DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 2;
    memcpy(out_frame, in_frame, len); // uncompressed
    return len;
}

BOOL Decompress_Y8_To_Y8(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 1;

    if (inSize != len)
        return FALSE;

    memcpy(out_frame, in_frame, len);
    return TRUE;
}

BOOL Decompress_HY8_To_Y8(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

BOOL Decompress_HY8_To_RGB24(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb24>((const char *)in_frame, (char*)out_frame);
}

BOOL Decompress_HY10_To_RGB24(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10> huff(width, height);
    return huff.decode<char, OutputProcessing::gray_to_rgb24>((const char *)in_frame, (char*)out_frame);
}

BOOL Decompress_HY10_To_Y10(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10> huff(width, height);
    return huff.decode<short, OutputProcessing::Default>((const char *)in_frame, (short*)out_frame);
}

BOOL Decompress_HY8_To_UYVY(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<char, 8> huff(width, height);
    return huff.decode<char, OutputProcessing::interleave_yuyv>((const char *)in_frame, (char*)out_frame);
}

BOOL Decompress_HY10_To_UYVY(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10> huff(width, height);
    return huff.decode<short, OutputProcessing::interleave_yuyv>((const char *)in_frame, (short*)out_frame);
}

BOOL Decompress_HY10_To_Y8(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    ZoeHuffmanCodec<short, 10> huff(width, height);
    return huff.decode<char, OutputProcessing::Default>((const char *)in_frame, (char*)out_frame);
}

BOOL Decompress_Y10_To_Y10(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 2;

    if (inSize != len)
        return FALSE;

    memcpy(out_frame, in_frame, len);
    return TRUE;
}

BOOL Decompress_Y10_To_Y8(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
{
    DWORD len = width * height * 2;

    if (inSize != len)
        return FALSE;

    const unsigned short* in_values = (const unsigned short*)(in_frame);

    for (DWORD i=0;i<len;i++)
        out_frame[i] = (in_values[i]>>2)&0x00FF;

    return TRUE;
}

BOOL Decompress_Y8_To_UYVY(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
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

    return TRUE;
}

BOOL Decompress_Y10_To_UYVY(DWORD inSize, DWORD width, DWORD height, const unsigned char* in_frame, unsigned char* out_frame)
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

    return TRUE;
}
