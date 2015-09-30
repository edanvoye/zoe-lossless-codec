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

#pragma once

#include <vector>

namespace OutputProcessing 
{
    enum {Default, interleave_yuyv, gray_to_rgb24, uyvy_to_rgb24, rgb24_to_rgb32, gray_to_rgb32, uyvy_to_rgb32, rgb24_to_rgb32_revY};
}

template <typename T>
class TrivialBitReader
{
public:
    TrivialBitReader(const T * ptr) : cur_ptr(ptr), org_ptr(ptr)
    {}
    T next() 
    {
        T value = *cur_ptr;
        cur_ptr++;
        return value;
    }
    void reset()
    {
        cur_ptr = org_ptr;
    }
    typedef T typeT;
private:
    const T* cur_ptr;
    const T* org_ptr;
};

template <int bitCount, typename outputT>
class UnpackBitReader
{
public:
    UnpackBitReader(const outputT * ptr) : cur_ptr((const char *)ptr), org_ptr((const char *)ptr), bits(8)
    {}
    outputT next() 
    {
        // cur_ptr is the pointer to the current byte
        // bits is the current offset in the current byte (how many bits are already read in this byte)

        // read remaining bits in the current byte
        outputT mask = (1<<(bits)) - 1;
        outputT value = (*cur_ptr)&mask;

        // goto next byte
        cur_ptr++;
        const int remaining_bits = bitCount-bits;
        if (remaining_bits>0)
        {
            // read second part
            outputT mask = ((1<<remaining_bits) - 1) << (8-remaining_bits);
            value = (value<<remaining_bits) | (((*cur_ptr)&mask)>> (8-remaining_bits));
            bits = 8-remaining_bits;
            if (bits==0)
            {
                cur_ptr++;
                bits = 8;
            }
        }

        return value;
    }
    void reset()
    {
        cur_ptr = org_ptr;
    }
    typedef outputT typeT;
private:
    int bits;
    const char* cur_ptr;
    const char* org_ptr;
};

template <typename T, int UsedBits, int Channels >
class ZoeHuffmanCodec
{
public:
	ZoeHuffmanCodec(int width, int height);

    template <typename ReaderT>
	unsigned encode(const T * src, char * dest);
    
    template <typename To, int op>
    bool decode(const char * image_src, To * image_dest);

private:

    static const int BitShift = sizeof(T)*8 - UsedBits;
    static const int BitMask = (1<<UsedBits)-1;

	int image_width;
	int image_height;

    // Encoder only
    struct EncoderData {
        EncoderData() : char_count_used(1<<UsedBits) {}

	    std::pair<int, unsigned> char_count[1<<UsedBits]; // first represents the symbol, or if >0x8000, it is huffNode index+0x8000
        int char_count_used;
	    unsigned huff_bits[1<<UsedBits];
	    unsigned huff_length[1<<UsedBits];
    } encoder_data[Channels];
};

template <typename T>
class BitPacker
{
public:
    BitPacker(char* bufferStart) : next((T*)bufferStart), start((T*)bufferStart), current(0), current_bitcount(0) { }
    __inline void pack(int bitcount, unsigned bits) // relevant bits are in the LSB of bits
    {
        if (current_bitcount+bitcount > (sizeof(T)*8))
        {
            unsigned first_bitcount = (sizeof(T)*8)-current_bitcount;
            unsigned second_bitcount = bitcount-first_bitcount;

            *next = (current<<first_bitcount) | (bits>>second_bitcount);
            next++;

            current = bits;
            current_bitcount = second_bitcount;
        }
        else
        {
            current = (current<<bitcount) | bits;
            current_bitcount += bitcount;
        }
    }
    unsigned flush()
    {
        if (current_bitcount>0)
        {
            *next = current << ((sizeof(T)*8)-current_bitcount);
            next++;
        }

        return (unsigned)((char*)next-(char*)start);
    }
private:
    T * next;
    T * start;
    T current;
    unsigned current_bitcount;
};
