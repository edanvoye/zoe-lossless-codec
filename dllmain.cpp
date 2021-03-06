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

#include "dllmain.h"
#include "ZoeCodec.h"

BOOL WINAPI DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

ZOECODEC_API LRESULT WINAPI DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2) 
{

    switch (uiMessage) {
    case DRV_LOAD:
        return (LRESULT)1L;

    case DRV_FREE:
        return (LRESULT)1L;

    case DRV_OPEN:
        return (LRESULT)1L;

    case DRV_CLOSE:
        return (LRESULT)1L;

    case DRV_QUERYCONFIGURE:
        return (LRESULT)1L;

    case DRV_CONFIGURE:
        Configure((HWND)lParam1);
        return DRV_OK;

    case ICM_CONFIGURE:
        //  return ICERR_OK if you will do a configure box, error otherwise
        if (lParam1 == -1)
            return QueryConfigure() ? ICERR_OK : ICERR_UNSUPPORTED;
        else
            return Configure((HWND)lParam1);

    case ICM_ABOUT:
        //  return ICERR_OK if you will do a about box, error otherwise
        if (lParam1 == -1)
            return QueryAbout() ? ICERR_OK : ICERR_UNSUPPORTED;
        else
            return About((HWND)lParam1);

    case ICM_GETSTATE:
        return GetState((LPVOID)lParam1, (DWORD)lParam2);

    case ICM_SETSTATE:
        return SetState((LPVOID)lParam1, (DWORD)lParam2);

    case ICM_GETINFO:
        return GetInfo((ICINFO*)lParam1, (DWORD)lParam2);

    case ICM_GETDEFAULTQUALITY:
        if (lParam1) {
            *((LPDWORD)lParam1) = 1000;
            return ICERR_OK;
        }
        break;

    case ICM_COMPRESS:
        return Compress((ICCOMPRESS*)lParam1, (DWORD)lParam2);

    case ICM_COMPRESS_QUERY:
        return CompressQuery((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_COMPRESS_BEGIN:
        return CompressBegin((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_COMPRESS_GET_FORMAT:
        return CompressGetFormat((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_COMPRESS_GET_SIZE:
        return CompressGetSize((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_COMPRESS_END:
        return CompressEnd();

    case ICM_DECOMPRESS_QUERY:
        // The ICM_DECOMPRESS_QUERY message queries a video decompression driver to determine if it supports a specific input format or if it can decompress a specific input format to a specific output format.
        return DecompressQuery((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_DECOMPRESS:
        return Decompress((ICDECOMPRESS*)lParam1, (DWORD)lParam2);

    case ICM_DECOMPRESS_BEGIN:
        return DecompressBegin((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_DECOMPRESS_GET_FORMAT:
        // The ICM_DECOMPRESS_GET_FORMAT message requests the output format of the decompressed data from a video decompression driver.
        return DecompressGetFormat((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_DECOMPRESS_GET_PALETTE:
        return DecompressGetPalette((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

    case ICM_DECOMPRESS_END:
        return DecompressEnd();

        // Driver messages

    case DRV_DISABLE:
    case DRV_ENABLE:
        return (LRESULT)1L;

    case DRV_INSTALL:
    case DRV_REMOVE:
        return (LRESULT)DRV_OK;
    }

    if (uiMessage < DRV_USER)
        return DefDriverProc(dwDriverID, hDriver, uiMessage, lParam1, lParam2);
    else
        return ICERR_UNSUPPORTED;
}
