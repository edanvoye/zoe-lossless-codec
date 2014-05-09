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

template <typename T, int UsedBits, int Channels>
class ZoeHuffmanCodec
{
public:
	ZoeHuffmanCodec(int width, int height);
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
