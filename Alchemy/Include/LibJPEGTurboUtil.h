#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define TJERROR							-1
#define JPEG_LFR_DIB					0x00000010

ALERROR JPEGLoadFromFile(CString sFilename, DWORD dwFlags, HPALETTE hPalette, HBITMAP* rethBitmap);
ALERROR JPEGLoadFromMemory(char* pImage, int iSize, DWORD dwFlags, HPALETTE hPalette, HBITMAP* rethBitmap);
ALERROR JPEGLoadFromResource(HINSTANCE hInst, char* pszRes, DWORD dwFlags, HPALETTE hPalette, HBITMAP* rethBitmap);
ALERROR JPEGSaveToMemory(HBITMAP hBitmap, int iQuality, CString* retsData);
