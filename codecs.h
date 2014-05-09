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

unsigned Compress_RGB24_To_RGB24(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_RGB24_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_RGB32_To_RGB32(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_RGB32_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_Y8_To_Y8(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_Y8_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_Y8_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_Y10_To_Y10(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_Y10_To_Y10(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_Y10_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

// Huffman
unsigned Compress_Y8_To_HY8(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY8_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY8_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY8_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_Y10_To_HY10(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY10_To_Y10(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY10_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY10_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

bool Decompress_Y10_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY10_To_Y8(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_RGB24_To_HRGB24(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HRGB24_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_RGB32_To_HRGB32(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HRGB32_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

unsigned Compress_UYVY_To_HUYVY(unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HUYVY_To_UYVY(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HUYVY_To_RGB24(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);

bool Decompress_HRGB24_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY8_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HY10_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
bool Decompress_HUYVY_To_RGB32(unsigned inSize, unsigned width, unsigned height, const unsigned char* in_frame, unsigned char* out_frame);
