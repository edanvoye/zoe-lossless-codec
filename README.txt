



  Zoe Lossless Codec

Zoe Lossless Codec is a video compression/decompression module to be used in AVI files under 
the Video for Windows (VfW) framework. The encoded data will typically be stored inside AVI files. The 
current version allows lossless compression of grayscale video data in the "Y8  " or "Y10 " formats, 
as well as RGB24, RGB32 and UYVY. RGB formats are encoded per-channel without any color space conversion.
"Y10 " is a new FOURCC code representing 10 bits of grayscale data stored in the LSB of 16 bit words.



 Revision History:
  2014-04-15 E. Danvoye    Initial Release
  2014-04-15 E. Danvoye    1.0.1 Fix crash with CRT allocator
  2014-05-02 E. Danvoye    1.0.2 Multi-channel, Support RGB24 and RGB32
  2014-05-05 E. Danvoye    1.0.4 Add UYVY decompression directly to RGB24
  2014-05-09 E. Danvoye    1.0.5 Fix playback in Windows Media Player and Adobe Premiere (by supporting negative height and RGB32 output)
  2014-05-12 E. Danvoye    1.0.6 Hack for Motionbuilder, forge RGB output for YUV formats
  2014-05-12 E. Danvoye    1.0.7 Fixed playback in Sony Vegas
  2015-07-07 E. Danvoye    1.1.1 Add 10 bit grayscale, 12 bit grayscale and 12bit packed grayscale

=========

Copyright (c) 2014,2015 Activision
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided 
that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and 
   the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and 
the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or 
promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.