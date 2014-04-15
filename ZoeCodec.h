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

static const DWORD FOURCC_AZCL = mmioFOURCC('A','Z','C','L');   // Zoe Lossless codec

class ZoeCodecInstance
{
public:
    ZoeCodecInstance() {}

    BOOL QueryAbout();
    DWORD About(HWND hwnd);

    BOOL QueryConfigure();
    DWORD Configure(HWND hwnd);

    DWORD GetState(LPVOID pv, DWORD dwSize);
    DWORD SetState(LPVOID pv, DWORD dwSize);

    DWORD GetInfo(ICINFO* icinfo, DWORD dwSize);

    DWORD CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD Compress(ICCOMPRESS* icinfo, DWORD dwSize);
    DWORD CompressEnd();

    DWORD DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD Decompress(ICDECOMPRESS* icinfo, DWORD dwSize);
    DWORD DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
    DWORD DecompressEnd();

    static ZoeCodecInstance* OpenCodec(ICOPEN* icinfo);
    static DWORD CloseCodec(ZoeCodecInstance* pinst);

    static void *operator new(size_t size)
    {
        void *ptr = LocalAlloc(LPTR, size); // Bypass CRT for allocations, because the delete happens in 
                                            // ZoeCodecInstance::CloseCodec and that is called by DRV_CLOSE,
                                            // which is called after the CRT was unloaded.
        return ptr;
    }

    static void operator delete(void *ptr, size_t size)
    {
        LocalFree(ptr);
    }
};
