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
#include "ZoeCodec.h"
#include "codecs.h"

static bool LOG_TO_FILE = false;
static bool LOG_TO_STDOUT = false;

#define VERSION 0x00010000 // 1.0

#if _WIN64
    TCHAR szDescription[] = TEXT("Zoe Lossless Codec (64 bits) v1.0.0");
    TCHAR szName[]        = TEXT("ZoeLosslessCodec64");
#else
    TCHAR szDescription[] = TEXT("Zoe Lossless Codec (32 bits) v1.0.0");
    TCHAR szName[]        = TEXT("ZoeLosslessCodec32");
#endif

const char * fourCCStr(DWORD fourcc)
{
    static char buf[32];
    if (fourcc==BI_RGB)
        strcpy_s(buf,"BI_RGB");
    else if (fourcc==BI_RLE8)
        strcpy_s(buf,"BI_RLE8");
    else if (fourcc==BI_RLE4)
        strcpy_s(buf,"BI_RLE4");
    else if (fourcc==BI_BITFIELDS)
        strcpy_s(buf,"BI_BITFIELDS");
    else if (fourcc==BI_JPEG)
        strcpy_s(buf,"BI_JPEG");
    else if (fourcc==BI_PNG)
        strcpy_s(buf,"BI_PNG");
    else
    {
        buf[0] = (fourcc&0x000000FF);
        buf[1] = (fourcc&0x0000FF00)>>8;
        buf[2] = (fourcc&0x00FF0000)>>16;
        buf[3] = (fourcc&0xFF000000)>>24;
        buf[4] = 0;
    }
    return buf;
}

enum eBufferTypes {
    BTYPE_INVALID = 0xFF,
    BTYPE_NONE = 0,
    BTYPE_RGB24,
    BTYPE_RGB32,
    BTYPE_Y8,
    BTYPE_Y10,
    BTYPE_HY8,
    BTYPE_HY10,
    BTYPE_HRGB24,
    BTYPE_HRGB32,

    BTYPE_COUNT
};

struct ZoeCodecHeader
{
    unsigned char version;
    unsigned char buffer_type;
    unsigned char pad0;
    unsigned char pad1;
};

BOOL IsValidType(int type)
{
    return type>BTYPE_NONE && type<BTYPE_COUNT;
}

void logMessage(const char * format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, 255, format, args);

    if (LOG_TO_FILE)
    {
        std::ofstream logFile("c:\\temp\\ZoeCodec.log", std::ios::app);
        if (logFile.is_open())
        {
            logFile << buffer << std::endl;
        }
    }
    if (LOG_TO_STDOUT)
    {
        std::cout << "ZoeCodec> " << buffer << std::endl;
    }

    va_end(args);
}

bool IsFormatSupported(LPBITMAPINFOHEADER lpbiIn) 
{
    if (!lpbiIn)
        return FALSE;

    // Formats supported by the Compressor

    if (lpbiIn->biCompression == BI_RGB && lpbiIn->biBitCount == 24)
        return TRUE;
    if (lpbiIn->biCompression == BI_RGB && lpbiIn->biBitCount == 32)
        return TRUE;
    if (lpbiIn->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && lpbiIn->biBitCount == 8)
        return TRUE;
    if (lpbiIn->biCompression == mmioFOURCC('Y', '1', '0', ' ') && lpbiIn->biBitCount == 16)
        return TRUE;

    return FALSE;
}

void FillHeaderForInput(const LPBITMAPINFOHEADER lpbiIn, ZoeCodecHeader* header)
{
    header->version = 1;
    header->buffer_type = BTYPE_NONE;

    if (lpbiIn->biCompression == BI_RGB && lpbiIn->biBitCount == 24)
        header->buffer_type = BTYPE_HRGB24; // BTYPE_RGB24;
    else if (lpbiIn->biCompression == BI_RGB && lpbiIn->biBitCount == 32)
        header->buffer_type = BTYPE_HRGB32; // BTYPE_RGB32;
    else if (lpbiIn->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && lpbiIn->biBitCount == 8)
        header->buffer_type = BTYPE_HY8; // BTYPE_Y8
    else if (lpbiIn->biCompression == mmioFOURCC('Y', '1', '0', ' ') && lpbiIn->biBitCount == 16)
        header->buffer_type = BTYPE_HY10; // BTYPE_Y10

    header->pad0 = 0;
    header->pad1 = 0;
}

ZoeCodecInstance* ZoeCodecInstance::OpenCodec(ICOPEN* icinfo)
{
    if (icinfo && icinfo->fccType != ICTYPE_VIDEO)
        return NULL;

    ZoeCodecInstance* pinst = new ZoeCodecInstance();

    if (icinfo) 
        icinfo->dwError = pinst ? ICERR_OK : ICERR_MEMORY;

    logMessage("Open instance %p", pinst);

    return pinst;
}

DWORD ZoeCodecInstance::CloseCodec(ZoeCodecInstance* pinst)
{
    logMessage("Close instance %p", pinst);

    delete pinst;

    return 1;
}

BOOL ZoeCodecInstance::QueryAbout()
{
    return FALSE;
}

DWORD ZoeCodecInstance::About(HWND hwnd)
{
    return ICERR_ERROR;
}

BOOL ZoeCodecInstance::QueryConfigure()
{
    return FALSE;
}

DWORD ZoeCodecInstance::Configure(HWND hwnd)
{
    return ICERR_ERROR;
}

DWORD ZoeCodecInstance::GetState(LPVOID pv, DWORD dwSize)
{
    return 0; // no state information
}

DWORD ZoeCodecInstance::SetState(LPVOID pv, DWORD dwSize)
{
    return 0; // no state information
}

DWORD ZoeCodecInstance::GetInfo(ICINFO* icinfo, DWORD dwSize)
{
    logMessage("GetInfo");

    if (icinfo == NULL)
        return sizeof(ICINFO);

    if (dwSize < sizeof(ICINFO))
        return 0;

    icinfo->dwSize            = sizeof(ICINFO);
    icinfo->fccType           = ICTYPE_VIDEO;
    icinfo->fccHandler        = FOURCC_AZCL;
    icinfo->dwFlags           = 0;

    icinfo->dwVersion         = VERSION;
    icinfo->dwVersionICM      = ICVERSION;
    MultiByteToWideChar(CP_ACP, 0, szDescription, -1, icinfo->szDescription, sizeof(icinfo->szDescription)/sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, szName, -1, icinfo->szName, sizeof(icinfo->szName)/sizeof(WCHAR));

    return sizeof(ICINFO);
}

DWORD ZoeCodecInstance::CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    if (!lpbiIn)
        return ICERR_BADFORMAT;

    logMessage("CompressQuery in FOURCC:%s bitCount:%d", fourCCStr(lpbiIn->biCompression), lpbiIn->biBitCount);

    if (lpbiOut)
        logMessage("CompressQuery out FOURCC:%s bitCount:%d", fourCCStr(lpbiOut->biCompression), lpbiOut->biBitCount);

    return (IsFormatSupported(lpbiIn)) ? ICERR_OK : ICERR_BADFORMAT;
}

DWORD ZoeCodecInstance::CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    logMessage("CompressGetFormat");

    if (!IsFormatSupported(lpbiIn))
        return ICERR_BADFORMAT;

    if (!lpbiOut)
        return sizeof(BITMAPINFOHEADER) + sizeof(ZoeCodecHeader); // size of returned structure

    // Fill output structure
    lpbiOut->biSize = sizeof(BITMAPINFOHEADER) + sizeof(ZoeCodecHeader);
    lpbiOut->biWidth = lpbiIn->biWidth;
    lpbiOut->biHeight = lpbiIn->biHeight;
    lpbiOut->biPlanes = 1;
    lpbiOut->biXPelsPerMeter = lpbiIn->biXPelsPerMeter;
    lpbiOut->biYPelsPerMeter = lpbiIn->biYPelsPerMeter;
    lpbiOut->biClrUsed = 0;
    lpbiOut->biClrImportant = 0;

    lpbiOut->biSizeImage = 0; // will be filled later in Compress
    lpbiOut->biBitCount = 32;
    lpbiOut->biCompression = FOURCC_AZCL;

    ZoeCodecHeader* header = (ZoeCodecHeader*)(&lpbiOut[1]);
    FillHeaderForInput(lpbiIn, header);

    return ICERR_OK;
}

DWORD ZoeCodecInstance::CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    logMessage("CompressBegin");

    // Initialization

    return ICERR_OK;
}

DWORD ZoeCodecInstance::CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    logMessage("CompressGetSize width:%d height:%d bits:%d", lpbiIn->biWidth, lpbiIn->biHeight, lpbiIn->biBitCount);

    // Worst case scenario is full uncompressed size
    return (lpbiIn->biWidth * lpbiIn->biHeight * lpbiIn->biBitCount) / 8;
}

DWORD ZoeCodecInstance::Compress(ICCOMPRESS* icinfo, DWORD dwSize)
{
    logMessage("Compress");

    if (!IsFormatSupported(icinfo->lpbiInput))
        return ICERR_BADFORMAT;

    const ZoeCodecHeader* header = (ZoeCodecHeader*)(&icinfo->lpbiOutput[1]);

    if (icinfo->lpckid)
        *icinfo->lpckid = FOURCC_AZCL;

    if (header->version == 1)
    {
        const unsigned char* in_frame = (unsigned char*)icinfo->lpInput;
        unsigned char* out_frame = (unsigned char*)icinfo->lpOutput;

        if (header->buffer_type == BTYPE_RGB24)
        {
            if (icinfo->lpbiInput->biCompression != BI_RGB || icinfo->lpbiInput->biBitCount != 24)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_RGB24_To_RGB24(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_RGB32)
        {
            if (icinfo->lpbiInput->biCompression != BI_RGB || icinfo->lpbiInput->biBitCount != 32)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_RGB32_To_RGB32(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_Y8)
        {
            if (icinfo->lpbiInput->biCompression != mmioFOURCC('Y', '8', ' ', ' ') || icinfo->lpbiInput->biBitCount != 8)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_Y8_To_Y8(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_HY8)
        {
            if (icinfo->lpbiInput->biCompression != mmioFOURCC('Y', '8', ' ', ' ') || icinfo->lpbiInput->biBitCount != 8)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_Y8_To_HY8(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_HY10)
        {
            if (icinfo->lpbiInput->biCompression != mmioFOURCC('Y', '1', '0', ' ') || icinfo->lpbiInput->biBitCount != 16)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_Y10_To_HY10(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_Y10)
        {
            if (icinfo->lpbiInput->biCompression != mmioFOURCC('Y', '1', '0', ' ') || icinfo->lpbiInput->biBitCount != 16)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_Y10_To_Y10(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_HRGB24)
        {
            if (icinfo->lpbiInput->biCompression != BI_RGB || icinfo->lpbiInput->biBitCount != 24)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_RGB24_To_HRGB24(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
        else if (header->buffer_type == BTYPE_HRGB32)
        {
            if (icinfo->lpbiInput->biCompression != BI_RGB || icinfo->lpbiInput->biBitCount != 32)
                return ICERR_BADFORMAT;

            *icinfo->lpdwFlags = AVIIF_KEYFRAME;

            DWORD size = Compress_RGB32_To_HRGB32(icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight, in_frame, out_frame);
            icinfo->lpbiOutput->biSizeImage = size;

            return ICERR_OK;
        }
    }

    return ICERR_ERROR;
}

DWORD ZoeCodecInstance::CompressEnd()
{
    logMessage("CompressEnd");

    // Cleanup

    return ICERR_OK;
}

DWORD ZoeCodecInstance::DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    if (!lpbiIn)
        return ICERR_BADFORMAT;

    logMessage("DecompressQuery in FOURCC:%s bitCount:%d", fourCCStr(lpbiIn->biCompression), lpbiIn->biBitCount);
    
    if (lpbiOut)
        logMessage("DecompressQuery out FOURCC:%s bitCount:%d", fourCCStr(lpbiOut->biCompression), lpbiOut->biBitCount);

    if (lpbiIn->biCompression!=FOURCC_AZCL)
    {
        logMessage("Can only decompress AZCL");
        return ICERR_BADFORMAT;
    }

    if (lpbiIn->biSize < sizeof(BITMAPINFOHEADER) + sizeof(ZoeCodecHeader))
    {
        logMessage("Bad header");
        return ICERR_BADFORMAT;
    }

    const ZoeCodecHeader* header = (const ZoeCodecHeader*)(&lpbiIn[1]);

    if (header->version != 1 || !IsValidType(header->buffer_type))
    {
        logMessage("Bad header");
        return ICERR_BADFORMAT;
    }

    if (lpbiOut)
    {
        // Identify all possible output formats for each BTYPE

        switch (header->buffer_type)
        {
        case BTYPE_RGB24:
        case BTYPE_HRGB24:
            if (lpbiOut->biCompression == BI_RGB && lpbiOut->biBitCount==24)
                return ICERR_OK;
            break;
        case BTYPE_RGB32:
        case BTYPE_HRGB32:
            if (lpbiOut->biCompression == BI_RGB && lpbiOut->biBitCount==32)
                return ICERR_OK;
            break;
        case BTYPE_Y8:
        case BTYPE_HY8:
            if (lpbiOut->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && lpbiOut->biBitCount==8)
                return ICERR_OK;
            if (lpbiOut->biCompression == mmioFOURCC('U', 'Y', 'V', 'Y') && lpbiOut->biBitCount==16)
                return ICERR_OK;
            if (lpbiOut->biCompression == BI_RGB && lpbiOut->biBitCount==24)
                return ICERR_OK;
            break;
        case BTYPE_Y10:
        case BTYPE_HY10:
            if (lpbiOut->biCompression == mmioFOURCC('Y', '1', '0', ' ') && lpbiOut->biBitCount==16)
                return ICERR_OK;
            if (lpbiOut->biCompression == mmioFOURCC('U', 'Y', 'V', 'Y') && lpbiOut->biBitCount==16) // lossy (10->8)
                return ICERR_OK;
            if (lpbiOut->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && lpbiOut->biBitCount==8) // lossy (10->8)
                return ICERR_OK;
            if (lpbiOut->biCompression == BI_RGB && lpbiOut->biBitCount==24)
                return ICERR_OK;
            break;
        }

        logMessage("Output not supported");
        return ICERR_BADFORMAT;
    }

    return ICERR_OK;
}

DWORD ZoeCodecInstance::DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    logMessage("DecompressGetFormat");

    if (lpbiIn->biCompression != FOURCC_AZCL)
    {
        logMessage("DecompressGetFormat: only AZCL is supported");
        return ICERR_BADFORMAT;
    }

    if (!lpbiOut)
    {
        logMessage("DecompressGetFormat: lpbiOut is null");
        return sizeof(BITMAPINFOHEADER); // size of returned structure
    }

    const ZoeCodecHeader* header = (const ZoeCodecHeader*)(&lpbiIn[1]);

    // Fill output structure
    lpbiOut->biSize = sizeof(BITMAPINFOHEADER);
    lpbiOut->biWidth = lpbiIn->biWidth;
    lpbiOut->biHeight = lpbiIn->biHeight;
    lpbiOut->biPlanes = 1;
    lpbiOut->biXPelsPerMeter = lpbiIn->biXPelsPerMeter;
    lpbiOut->biYPelsPerMeter = lpbiIn->biYPelsPerMeter;
    lpbiOut->biClrUsed = 0;
    lpbiOut->biClrImportant = 0;

    logMessage("DecompressGetFormat: Header version:%d type:%d", header->version, header->buffer_type);

    if (header->version == 1)
    {
        // Identify default output format for each BTYPE

        if (header->buffer_type == BTYPE_RGB24 || header->buffer_type == BTYPE_HRGB24)
        {
            lpbiOut->biBitCount = 24;
            lpbiOut->biCompression = BI_RGB;
        }
        else if (header->buffer_type == BTYPE_RGB32 || header->buffer_type == BTYPE_HRGB32)
        {
            lpbiOut->biBitCount = 32;
            lpbiOut->biCompression = BI_RGB;
        }
        else if (header->buffer_type == BTYPE_Y8 || header->buffer_type == BTYPE_HY8)
        {
            lpbiOut->biBitCount = 8;
            lpbiOut->biCompression = mmioFOURCC('Y', '8', ' ', ' ');
        }
        else if (header->buffer_type == BTYPE_Y10 || header->buffer_type == BTYPE_HY10)
        {
            lpbiOut->biBitCount = 16;
            lpbiOut->biCompression = mmioFOURCC('Y', '1', '0', ' ');
        }
        else 
            return ICERR_BADFORMAT;

        lpbiOut->biSizeImage = (lpbiOut->biWidth * lpbiOut->biHeight * lpbiOut->biBitCount) / 8;

        return ICERR_OK;
    }

    return ICERR_BADFORMAT;
}

DWORD ZoeCodecInstance::DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    logMessage("DecompressBegin");

    if (lpbiIn)
        logMessage("DecompressBegin in FOURCC:%s bitCount:%d", fourCCStr(lpbiIn->biCompression), lpbiIn->biBitCount);
    if (lpbiOut)
        logMessage("DecompressBegin out FOURCC:%s bitCount:%d", fourCCStr(lpbiOut->biCompression), lpbiOut->biBitCount);

    // Initialization

    return ICERR_OK;
}

DWORD ZoeCodecInstance::Decompress(ICDECOMPRESS* icinfo, DWORD dwSize)
{
    logMessage("Decompress outW:%d outH:%d outBpp:%d outputsize:%d", icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, icinfo->lpbiOutput->biBitCount, icinfo->lpbiOutput->biSizeImage);

    if (icinfo->lpbiInput->biCompression != FOURCC_AZCL)
        return ICERR_BADFORMAT;

    const ZoeCodecHeader* header = (const ZoeCodecHeader*)(&icinfo->lpbiInput[1]);

    logMessage("version:%d type:%d", header->version, header->buffer_type);

    if (header->version == 1)
    {
        icinfo->lpbiOutput->biSizeImage = (icinfo->lpbiOutput->biWidth * icinfo->lpbiOutput->biHeight * icinfo->lpbiOutput->biBitCount) >> 3;

        const unsigned char* in_frame = (unsigned char*)icinfo->lpInput;
        unsigned char* out_frame = (unsigned char*)icinfo->lpOutput;

        if (header->buffer_type == BTYPE_RGB24)
        {
            if (icinfo->lpbiOutput->biCompression == BI_RGB && icinfo->lpbiOutput->biBitCount == 24)
            {
                if (Decompress_RGB24_To_RGB24(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        else if (header->buffer_type == BTYPE_RGB32)
        {
            if (icinfo->lpbiOutput->biCompression == BI_RGB && icinfo->lpbiOutput->biBitCount == 32)
            {
                if (Decompress_RGB32_To_RGB32(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        if (header->buffer_type == BTYPE_HRGB24)
        {
            if (icinfo->lpbiOutput->biCompression == BI_RGB && icinfo->lpbiOutput->biBitCount == 24)
            {
                if (Decompress_HRGB24_To_RGB24(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        else if (header->buffer_type == BTYPE_HRGB32)
        {
            if (icinfo->lpbiOutput->biCompression == BI_RGB && icinfo->lpbiOutput->biBitCount == 32)
            {
                if (Decompress_HRGB32_To_RGB32(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        else if (header->buffer_type == BTYPE_Y8)
        {
            if (icinfo->lpbiOutput->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && icinfo->lpbiOutput->biBitCount == 8)
            {
                if (Decompress_Y8_To_Y8(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == mmioFOURCC('U', 'Y', 'V', 'Y') && icinfo->lpbiOutput->biBitCount == 16)
            {
                if (Decompress_Y8_To_UYVY(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        else if (header->buffer_type == BTYPE_HY8)
        {
            if (icinfo->lpbiOutput->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && icinfo->lpbiOutput->biBitCount == 8)
            {
                if (Decompress_HY8_To_Y8(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == mmioFOURCC('U', 'Y', 'V', 'Y') && icinfo->lpbiOutput->biBitCount == 16)
            {
                if (Decompress_HY8_To_UYVY(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == BI_RGB && icinfo->lpbiOutput->biBitCount == 24)
            {
                if (Decompress_HY8_To_RGB24(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        else if (header->buffer_type == BTYPE_HY10)
        {
            if (icinfo->lpbiOutput->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && icinfo->lpbiOutput->biBitCount == 8)
            {
                if (Decompress_HY10_To_Y8(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == mmioFOURCC('Y', '1', '0', ' ') && icinfo->lpbiOutput->biBitCount == 16)
            {
                if (Decompress_HY10_To_Y10(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == mmioFOURCC('U', 'Y', 'V', 'Y') && icinfo->lpbiOutput->biBitCount == 16)
            {
                if (Decompress_HY10_To_UYVY(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == BI_RGB && icinfo->lpbiOutput->biBitCount == 24)
            {
                if (Decompress_HY10_To_RGB24(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
        else if (header->buffer_type == BTYPE_Y10)
        {
            if (icinfo->lpbiOutput->biCompression == mmioFOURCC('Y', '8', ' ', ' ') && icinfo->lpbiOutput->biBitCount == 8)
            {
                if (Decompress_Y10_To_Y8(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == mmioFOURCC('Y', '1', '0', ' ') && icinfo->lpbiOutput->biBitCount == 16)
            {
                if (Decompress_Y10_To_Y10(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
            else if (icinfo->lpbiOutput->biCompression == mmioFOURCC('U', 'Y', 'V', 'Y') && icinfo->lpbiOutput->biBitCount == 16)
            {
                if (Decompress_Y10_To_UYVY(icinfo->lpbiInput->biSizeImage, icinfo->lpbiOutput->biWidth, icinfo->lpbiOutput->biHeight, in_frame, out_frame))
                    return ICERR_OK;
            }
        }
    }

    return ICERR_BADFORMAT;
}

DWORD ZoeCodecInstance::DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    logMessage("DecompressGetPalette");

    return ICERR_BADFORMAT;
}

DWORD ZoeCodecInstance::DecompressEnd()
{
    logMessage("DecompressEnd");

    // Cleanup

    return ICERR_OK;
}

