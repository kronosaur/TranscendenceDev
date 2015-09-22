//	CG32bitImage.cpp
//
//	CG32bitImage Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "PixelMacros.h"

#define SMALL_SQUARE_SIZE					2
#define MEDIUM_SQUARE_SIZE					4

CG32bitImage CG32bitImage::m_NullImage;

CG32bitImage::CG32bitImage (void) :
		m_pRGBA(NULL),
		m_bFreeRGBA(false),
		m_iPitch(0),
		m_AlphaType(alphaNone),
		m_pBMI(NULL)

//	CG32bitImage constructor

	{
	}

CG32bitImage::CG32bitImage (const CG32bitImage &Src)

//	CG32bitImage constructor

	{
	Copy(Src);
	}

CG32bitImage::~CG32bitImage (void)

//	CG32bitImage destructor

	{
	CleanUp();
	}

CG32bitImage &CG32bitImage::operator= (const CG32bitImage &Src)

//	CG32bitImage operator =

	{
	CleanUp();
	Copy(Src);
	return *this;
	}

void CG32bitImage::BltToDC (HDC hDC, int x, int y) const

//	BltToDC
//
//	Blt the surface to a DC

	{
	//	Initialize a bitmap info structure

	if (m_pBMI == NULL)
		InitBMI(&m_pBMI);

	//	Blt

	::SetDIBitsToDevice(hDC,
			x,
			y,
			m_cxWidth,
			m_cyHeight,
			0,
			0,
			0,
			m_cyHeight,
			m_pRGBA,
			m_pBMI,
			DIB_RGB_COLORS);
	}

void CG32bitImage::BltToSurface (LPDIRECTDRAWSURFACE7 pSurface, SurfaceTypes iType)

//	BltToSurface
//
//	Blts to a DX surface

	{
	if (IsEmpty())
		return;

	DDSURFACEDESC2 desc;

	//	Lock the surface

	if (!::SurfaceLock(pSurface, &desc))
		return;

	//	Calculate some metrics

	int iLinesToBlt = min(m_cyHeight, (int)desc.dwHeight);
	int iPixelSize = ((iType == r8g8b8) ? sizeof(DWORD) : sizeof(WORD));
	int iDWORDsPerLine = AlignUp(min(m_cxWidth, (int)desc.dwWidth) * iPixelSize / sizeof(DWORD), sizeof(DWORD));
	int iSourcePitch = m_iPitch / sizeof(DWORD);
	int iDestPitch = desc.lPitch / sizeof(DWORD);

	//	Blt the bitmap

	DWORD *pDestLine = (DWORD *)desc.lpSurface;
	CG32bitPixel *pSourceLine = m_pRGBA;

	//	Handle both 16-bit bit patterns

	if (iType == r5g5b5)
		{
		for (int i = 0; i < iLinesToBlt; i++)
			{
			CG32bitPixel *pSource = pSourceLine;
			DWORD *pDest = pDestLine;
			DWORD *pDestEnd = pDestLine + iDWORDsPerLine;

			while (pDest < pDestEnd)
				{
				DWORD dwData1 = pSource->AsR5G5B5();
				pSource++;
				DWORD dwData2 = pSource->AsR5G5B5();
				pSource++;

				*pDest++ = (dwData2 << 16) | dwData1;
				}

			pSourceLine += iSourcePitch;
			pDestLine += iDestPitch;
			}
		}
	else if (iType == r5g6b5)
		{
		for (int i = 0; i < iLinesToBlt; i++)
			{
			CG32bitPixel *pSource = pSourceLine;
			DWORD *pDest = pDestLine;
			DWORD *pDestEnd = pDestLine + iDWORDsPerLine;

			while (pDest < pDestEnd)
				{
				DWORD dwData1 = pSource->AsR5G6B5();
				pSource++;
				DWORD dwData2 = pSource->AsR5G6B5();
				pSource++;

				*pDest++ = (dwData2 << 16) | dwData1;
				}

			pSourceLine += iSourcePitch;
			pDestLine += iDestPitch;
			}
		}
	else if (iType == r8g8b8)
		{
		for (int i = 0; i < iLinesToBlt; i++)
			{
			CG32bitPixel *pSource = pSourceLine;
			DWORD *pDest = pDestLine;
			DWORD *pDestEnd = pDestLine + iDWORDsPerLine;

			while (pDest < pDestEnd)
				{
				*pDest++ = pSource->AsR8G8B8();
				pSource++;
				}

			pSourceLine += iSourcePitch;
			pDestLine += iDestPitch;
			}
		}

	//	Done

	SurfaceUnlock(pSurface);
	}

void CG32bitImage::CleanUp (void)

//	CleanUp
//
//	Clean up the bitmap

	{
	if (m_pRGBA && m_bFreeRGBA)
		{
		delete [] m_pRGBA;
		m_pRGBA = NULL;
		}

	m_cxWidth = 0;
	m_cyHeight = 0;
	m_iPitch = 0;
	m_AlphaType = alphaNone;
	ResetClipRect();

	if (m_pBMI)
		{
		delete [] (BYTE *)m_pBMI;
		m_pBMI = NULL;
		}
	}

void CG32bitImage::Copy (const CG32bitImage &Src)

//	Copy
//
//	Copy from source. We assume that we are already clean.

	{
	if (Src.IsEmpty())
		return;

	//	Copy the buffer

	int iSize = CalcBufferSize(Src.m_iPitch / sizeof(DWORD), Src.m_cyHeight);
	m_pRGBA = new CG32bitPixel [iSize];
	m_bFreeRGBA = true;

	CG32bitPixel *pSrc = Src.m_pRGBA;
	CG32bitPixel *pSrcEnd = pSrc + iSize;
	CG32bitPixel *pDest = m_pRGBA;

	while (pSrc < pSrcEnd)
		*pDest++ = *pSrc++;

	//	Now copy the remaining variable

	m_cxWidth = Src.m_cxWidth;
	m_cyHeight = Src.m_cyHeight;
	m_iPitch = Src.m_iPitch;
	m_AlphaType = Src.m_AlphaType;
	m_rcClip = Src.m_rcClip;
	m_pBMI = NULL;
	}

bool CG32bitImage::CopyToClipboard (void)

//	CopyToClipboard
//
//	Copies to the clipboard

	{
	//	Create an HBITMAP

	HWND hDesktopWnd = ::GetDesktopWindow();
	HDC hDesktopDC = ::GetDC(hDesktopWnd);
	HDC hDC = ::CreateCompatibleDC(hDesktopDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDesktopDC, m_cxWidth, m_cyHeight);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hBitmap);
	BltToDC(hDC, 0, 0);
	::SelectObject(hDC, hOldBitmap);
	::DeleteDC(hDC);
	::ReleaseDC(hDesktopWnd, hDesktopDC);

	//	Copy to the clipboard

	if (!::OpenClipboard(NULL))
		return ERR_FAIL;

	if (!::EmptyClipboard())
		{
		::CloseClipboard();
		return ERR_FAIL;
		}

	if (!::SetClipboardData(CF_BITMAP, hBitmap))
		{
		::CloseClipboard();
		return ERR_FAIL;
		}

	if (!::CloseClipboard())
		return ERR_FAIL;

	return NOERROR;
	}

void CG32bitImage::CopyTransformed (const RECT &rcDest, const CG32bitImage &Src, const RECT &rcSrc, const CXForm &SrcToDest, const CXForm &DestToSrc, const RECT &rcDestXForm)

//	CopyTransformed
//
//	Blts the source to the destination. All transformations are from the
//	center of the source.

	{
	int xDest = rcDest.left;
	int yDest = rcDest.top;
	int cxDest = RectWidth(rcDest);
	int cyDest = RectHeight(rcDest);

	int xSrc = rcSrc.left;
	int ySrc = rcSrc.top;
	int cxSrc = RectWidth(rcSrc);
	int cySrc = RectHeight(rcSrc);
	int xSrcEnd = xSrc + cxSrc;
	int ySrcEnd = ySrc + cySrc;

	//	Compute vectors that move us by 1 pixel

	CVector vOrigin = DestToSrc.Transform(CVector(0.0, 0.0));
	CVector vIncX = DestToSrc.Transform(CVector(1.0, 0.0)) - vOrigin;
	CVector vIncY = DestToSrc.Transform(CVector(0.0, 1.0)) - vOrigin;

	int iRowHeight = Src.GetPixelPos(0, 1) - Src.GetPixelPos(0, 0);

	//	Loop over every pixel in the destination

	CVector vSrcRow = DestToSrc.Transform(CVector(rcDestXForm.left, rcDestXForm.top));
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);
	CG32bitPixel *pDestRowEnd = GetPixelPos(xDest, yDest + cyDest);
	while (pDestRow < pDestRowEnd)
		{
		CVector vSrcPos = vSrcRow;
		CG32bitPixel *pDestPos = pDestRow;
		CG32bitPixel *pDestPosEnd = pDestRow + cxDest;
		while (pDestPos < pDestPosEnd)
			{
			int xSrcPos = (int)vSrcPos.GetX();
			int ySrcPos = (int)vSrcPos.GetY();

			if (xSrcPos >= xSrc && xSrcPos + 1 < xSrcEnd
					&& ySrcPos >= ySrc && ySrcPos + 1 < ySrcEnd)
				{
				CG32bitPixel *pSrcPos = Src.GetPixelPos(xSrcPos, ySrcPos);
				BYTE byAlpha = pSrcPos->GetAlpha();

				if (byAlpha == 0x00)
					*pDestPos = CG32bitPixel::Null();

				else
					INTERPOLATE_COPY32(pSrcPos, iRowHeight, vSrcPos, xSrcPos, ySrcPos, byAlpha, *pDestPos);
				}
			else
				*pDestPos = CG32bitPixel::Null();

			//	Next

			vSrcPos = vSrcPos + vIncX;
			pDestPos++;
			}

		//	Next row

		vSrcRow = vSrcRow + vIncY;
		pDestRow = NextRow(pDestRow);
		}
	}

bool CG32bitImage::Create (int cxWidth, int cyHeight, EAlphaTypes AlphaType, CG32bitPixel InitialValue)

//	Create
//
//	Creates a blank image

	{
	CleanUp();
	if (cxWidth <= 0 || cyHeight <= 0)
		return false;

	//	Allocate a new buffer

	m_iPitch = cxWidth * sizeof(DWORD);
	int iSize = CalcBufferSize(m_iPitch / sizeof(DWORD), cyHeight);
	m_pRGBA = new CG32bitPixel [iSize];
	m_bFreeRGBA = true;

	//	Initialize

	CG32bitPixel *pDest = m_pRGBA;
	CG32bitPixel *pDestEnd = pDest + iSize;

	while (pDest < pDestEnd)
		*pDest++ = InitialValue;

	//	Other variables

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	m_AlphaType = AlphaType;
	ResetClipRect();

	return true;
	}

bool CG32bitImage::CreateFromBitmap (HBITMAP hImage, HBITMAP hMask, EBitmapTypes iMaskType, DWORD dwFlags)

//	CreateFromBitmap
//
//	Creates from an image and (optional) mask

	{
	try
		{
		CleanUp();

		//	Get properties of the image

		int cxImageWidth = 0;
		int cyImageHeight = 0;
		void *pImageBase = NULL;
		int iImageStride;
		if (hImage)
			{
			if (dibGetInfo(hImage, &cxImageWidth, &cyImageHeight, &pImageBase, &iImageStride) != NOERROR)
				return false;
			}

		int cxMaskWidth = 0;
		int cyMaskHeight = 0;
		void *pMaskBase = NULL;
		int iMaskStride;
		if (hMask)
			{
			if (dibGetInfo(hMask, &cxMaskWidth, &cyMaskHeight, &pMaskBase, &iMaskStride) != NOERROR)
				return false;
			}

		//	Figure out the dimensions of the resulting image

		int cxWidth;
		int cyHeight;
		if (hImage)
			{
			cxWidth = cxImageWidth;
			cyHeight = cyImageHeight;

			//	If we also have a mask, it better be at least as big

			if (hMask)
				{
				if (cxImageWidth > cxMaskWidth
						|| cyImageHeight > cyMaskHeight)
					return false;
				}
			}
		else if (hMask)
			{
			cxWidth = cxMaskWidth;
			cyHeight = cyMaskHeight;
			}
		else
			return false;

		//	Do we have an 8-bit mask?

		bool bHas8BitMask = (iMaskType == bitmapAlpha || iMaskType == bitmapRGB);

		//	Allocate the image

		m_iPitch = cxWidth * sizeof(DWORD);
		int iSize = CalcBufferSize(m_iPitch / sizeof(DWORD), cyHeight);
		m_pRGBA = new CG32bitPixel [iSize];
		m_bFreeRGBA = true;
		m_cxWidth = cxWidth;
		m_cyHeight = cyHeight;
		ResetClipRect();

		//	Load the image. If we don't have an image, then initialize to 0.

		if (hImage == NULL)
			{
			CG32bitPixel InitialValue(0);
			CG32bitPixel *pDest = m_pRGBA;
			CG32bitPixel *pDestEnd = pDest + iSize;

			while (pDest < pDestEnd)
				*pDest++ = InitialValue;
			}

		//	If 16-bit image, load it

		else if (dibIs16bit(hImage))
			{
			CG32bitPixel *pDestRow = m_pRGBA;
			CG32bitPixel *pDestRowEnd = pDestRow + iSize;
			WORD *pSrcRow = (WORD *)pImageBase;

			while (pDestRow < pDestRowEnd)
				{
				CG32bitPixel *pDest = pDestRow;
				CG32bitPixel *pDestEnd = pDest + cxWidth;
				WORD *pSrc = pSrcRow;

				while (pDest < pDestEnd)
					{
					BYTE byRed = (BYTE)CG16bitImage::RedValue(*pSrc);
					BYTE byGreen = (BYTE)CG16bitImage::GreenValue(*pSrc);
					BYTE byBlue = (BYTE)CG16bitImage::BlueValue(*pSrc);

					*pDest = CG32bitPixel(byRed, byGreen, byBlue);

					pDest++;
					pSrc++;
					}

				pDestRow = NextRow(pDestRow);
				pSrcRow = (WORD *)(((char *)pSrcRow) + iImageStride);
				}
			}

		//	If 24-bit image

		else if (dibIs24bit(hImage))
			{
			CG32bitPixel *pDestRow = m_pRGBA;
			CG32bitPixel *pDestRowEnd = pDestRow + iSize;
			BYTE *pSrcRow = (BYTE *)pImageBase;

			while (pDestRow < pDestRowEnd)
				{
				CG32bitPixel *pDest = pDestRow;
				CG32bitPixel *pDestEnd = pDest + cxWidth;
				BYTE *pSrc = pSrcRow;

				while (pDest < pDestEnd)
					{
					BYTE byBlue = *pSrc++;
					BYTE byGreen = *pSrc++;
					BYTE byRed = *pSrc++;

					*pDest++ = CG32bitPixel(byRed, byGreen, byBlue);
					}

				pDestRow = NextRow(pDestRow);
				pSrcRow += iImageStride;
				}
			}

		//	Otherwise, we don't know

		else
			{
			ASSERT(false);
			return false;
			}

		//	Now load the mask. If no mask, then we're done

		if (hMask == NULL)
			m_AlphaType = alphaNone;

		//	16-bit mask

		else if (dibIs16bit(hMask))
			{
			//	Handle pre-multiplication

			if ((dwFlags & FLAG_PRE_MULT_ALPHA) && bHas8BitMask)
				{
				CG32bitPixel *pDestRow = m_pRGBA;
				CG32bitPixel *pDestRowEnd = pDestRow + iSize;
				WORD *pSrcRow = (WORD *)pMaskBase;

				while (pDestRow < pDestRowEnd)
					{
					CG32bitPixel *pDest = pDestRow;
					CG32bitPixel *pDestEnd = pDest + cxWidth;
					WORD *pSrc = pSrcRow;

					while (pDest < pDestEnd)
						{
						BYTE byAlpha = (BYTE)CG16bitImage::GreenValue(*pSrc);
						BYTE *pAlpha = CG32bitPixel::AlphaTable(byAlpha);

						BYTE byRed = pAlpha[pDest->GetRed()];
						BYTE byGreen = pAlpha[pDest->GetGreen()];
						BYTE byBlue = pAlpha[pDest->GetBlue()];

						*pDest++ = CG32bitPixel(byRed, byGreen, byBlue, byAlpha);

						pDest++;
						pSrc++;
						}

					pDestRow = NextRow(pDestRow);
					pSrcRow = (WORD *)(((char *)pSrcRow) + iMaskStride);
					}

				m_AlphaType = alpha8;
				}

			//	Otherwise, copy the mask

			else
				{
				CG32bitPixel *pDestRow = m_pRGBA;
				CG32bitPixel *pDestRowEnd = pDestRow + iSize;
				WORD *pSrcRow = (WORD *)pMaskBase;

				while (pDestRow < pDestRowEnd)
					{
					CG32bitPixel *pDest = pDestRow;
					CG32bitPixel *pDestEnd = pDest + cxWidth;
					WORD *pSrc = pSrcRow;

					while (pDest < pDestEnd)
						{
						pDest->SetAlpha((BYTE)CG16bitImage::GreenValue(*pSrc));

						pDest++;
						pSrc++;
						}

					pDestRow = NextRow(pDestRow);
					pSrcRow = (WORD *)(((char *)pSrcRow) + iMaskStride);
					}

				m_AlphaType = (bHas8BitMask ? alpha8 : alpha1);
				}
			}

		//	24-bit mask

		else if (dibIs24bit(hMask))
			{
			//	Handle pre-multiplication

			if ((dwFlags & FLAG_PRE_MULT_ALPHA) && bHas8BitMask)
				{
				CG32bitPixel *pDestRow = m_pRGBA;
				CG32bitPixel *pDestRowEnd = pDestRow + iSize;
				BYTE *pSrcRow = (BYTE *)pMaskBase;

				while (pDestRow < pDestRowEnd)
					{
					CG32bitPixel *pDest = pDestRow;
					CG32bitPixel *pDestEnd = pDest + cxWidth;
					BYTE *pSrc = pSrcRow;

					while (pDest < pDestEnd)
						{
						BYTE byAlpha = *pSrc;
						BYTE *pAlpha = CG32bitPixel::AlphaTable(byAlpha);

						BYTE byRed = pAlpha[pDest->GetRed()];
						BYTE byGreen = pAlpha[pDest->GetGreen()];
						BYTE byBlue = pAlpha[pDest->GetBlue()];

						*pDest++ = CG32bitPixel(byRed, byGreen, byBlue, byAlpha);

						pSrc += 3;
						}

					pDestRow = NextRow(pDestRow);
					pSrcRow += iMaskStride;
					}

				m_AlphaType = alpha8;
				}

			//	Otherwise, copy the mask

			else
				{
				CG32bitPixel *pDestRow = m_pRGBA;
				CG32bitPixel *pDestRowEnd = pDestRow + iSize;
				BYTE *pSrcRow = (BYTE *)pMaskBase;

				while (pDestRow < pDestRowEnd)
					{
					CG32bitPixel *pDest = pDestRow;
					CG32bitPixel *pDestEnd = pDest + cxWidth;
					BYTE *pSrc = pSrcRow;

					while (pDest < pDestEnd)
						{
						BYTE byRed = *pSrc++;
						BYTE byGreen = *pSrc++;
						BYTE byBlue = *pSrc++;

						pDest->SetAlpha(byGreen);

						pDest++;
						}

					pDestRow = NextRow(pDestRow);
					pSrcRow += iMaskStride;
					}

				m_AlphaType = (bHas8BitMask ? alpha8 : alpha1);
				}
			}

		return true;
		}
	catch (...)
		{
		::kernelDebugLogMessage("Crash in CG32bitImage::CreateFromBitmap.");
		return false;
		}
	}

bool CG32bitImage::CreateFromExternalBuffer (void *pBuffer, int cxWidth, int cyHeight, int iPitch, EAlphaTypes AlphaType)

//	CreateFromExternalBuffer
//
//	Creates from an external buffer which we do not control. Callers are
//	responsible for lifetime management.

	{
	CleanUp();
	if (cxWidth <= 0 || cyHeight <= 0)
		return false;

	//	Allocate a new buffer

	m_iPitch = iPitch;
	m_pRGBA = (CG32bitPixel *)pBuffer;
	m_bFreeRGBA = false;

	//	Other variables

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	m_AlphaType = AlphaType;
	ResetClipRect();

	return true;
	}

bool CG32bitImage::CreateFromFile (const CString &sImageFilespec, const CString &sMaskFilespec, DWORD dwFlags)

//	CreateFromFile
//
//	Loads a BMP or JPEG into this image

	{
	try
		{
		//	Load the image (which must exist).

		HBITMAP hImage;
		EBitmapTypes iImageType;
		if (!dxLoadImageFile(sImageFilespec, &hImage, &iImageType)
				|| iImageType == bitmapNone)
			return false;

		//	Load the (optional) mask
		//
		//	NOTE: This returns TRUE even if sMaskFilespec is blank. It just sets
		//	iMaskType to bitmapNone.

		HBITMAP hMask;
		EBitmapTypes iMaskType;
		if (!dxLoadImageFile(sMaskFilespec, &hMask, &iMaskType))
			{
			::DeleteObject(hImage);
			return false;
			}

		//	Create from bitmaps

		bool bSuccess = CreateFromBitmap(hImage, hMask, iMaskType, dwFlags);

		//	Either way, we're done with our bitmaps

		::DeleteObject(hImage);
		if (hMask)
			::DeleteObject(hMask);

		//	Done

		return bSuccess;
		}
	catch (...)
		{
		::kernelDebugLogMessage("Crash loading image: %s.", sImageFilespec);
		return false;
		}
	}

bool CG32bitImage::CreateFromImageTransformed (const CG32bitImage &Source, int xSrc, int ySrc, int cxSrc, int cySrc, Metric rScaleX, Metric rScaleY, Metric rRotation)

//	CreateFromImageTransformed
//
//	Creates a new image from the source after a transformation.

	{
	CleanUp();

	//	Get some metrics

	CXForm SrcToDest;
	CXForm DestToSrc;
	RECT rcDestXForm;
	if (!CalcBltTransform(0.0, 0.0, rScaleX, rScaleY, rRotation, xSrc, ySrc, cxSrc, cySrc, &SrcToDest, &DestToSrc, &rcDestXForm))
		{
		return true;
		}

	//	Set some rects

	RECT rcSrc;
	rcSrc.left = xSrc;
	rcSrc.top = ySrc;
	rcSrc.right = xSrc + cxSrc;
	rcSrc.bottom = ySrc + cySrc;

	RECT rcDest;
	rcDest.left = 0;
	rcDest.top = 0;
	rcDest.right = RectWidth(rcDestXForm);
	rcDest.bottom = RectHeight(rcDestXForm);

	//	Create the destination image (no need to initialize, since we will copy
	//	over it).

	m_cxWidth = RectWidth(rcDest);
	m_cyHeight = RectHeight(rcDest);
	m_iPitch = m_cxWidth * sizeof(DWORD);
	int iSize = CalcBufferSize(m_iPitch / sizeof(DWORD), m_cyHeight);
	m_pRGBA = new CG32bitPixel [iSize];
	m_bFreeRGBA = true;
	m_AlphaType = Source.GetAlphaType();
	ResetClipRect();

	//	Copy

	CopyTransformed(rcDest,
			Source,
			rcSrc,
			SrcToDest,
			DestToSrc,
			rcDestXForm);

	return true;
	}

void CG32bitImage::DrawDot (int x, int y, CG32bitPixel rgbColor, MarkerTypes iMarker)

//	DrawDot
//
//	Draws a marker

	{
	switch (iMarker)
		{
		case markerPixel:
			{
			SetPixel(x, y, rgbColor);
			break;
			}

		case markerSmallRound:
			{
			int xStart, xEnd, yStart, yEnd;

			x = x - 1;
			y = y - 1;

			xStart = max(m_rcClip.left, x);
			xEnd = min(m_rcClip.right, x + 3);
			yStart = max(m_rcClip.top, y);
			yEnd = min(m_rcClip.bottom, y + 3);

			for (int yPos = yStart; yPos < yEnd; yPos++)
				{
				CG32bitPixel *pPos = GetPixelPos(xStart, yPos);

				for (int xPos = xStart; xPos < xEnd; xPos++)
					{
					BYTE byTrans = g_wSmallRoundMask[(yPos - y) * 3 + xPos - x];

					if (byTrans == 0xff)
						*pPos = rgbColor;
					else if (byTrans == 0x00)
						NULL;
					else
						*pPos = CG32bitPixel::Blend(*pPos, rgbColor, byTrans);

					pPos++;
					}

				pPos = NextRow(pPos);
				}

			break;
			}

		case markerSmallCross:
			{
			int yTop = y - SMALL_SQUARE_SIZE;
			int yBottom = y + SMALL_SQUARE_SIZE;
			int xLeft = x - SMALL_SQUARE_SIZE;
			int xRight = x + SMALL_SQUARE_SIZE;

			//	Draw the horizontal line

			if (y >= m_rcClip.top && y < m_rcClip.bottom)
				{
				CG32bitPixel *pRowStart = GetPixelPos(0, y);
				CG32bitPixel *pPos = pRowStart + Max((int)m_rcClip.left, xLeft);
				CG32bitPixel *pPosEnd = pRowStart + Min(xRight+1, (int)m_rcClip.right);

				while (pPos < pPosEnd)
					*pPos++ = rgbColor;
				}

			//	Draw the vertical line

			if (x >= m_rcClip.left && x < m_rcClip.right)
				{
				CG32bitPixel *pPos = GetPixelPos(x, Max(yTop+1, (int)m_rcClip.top));
				CG32bitPixel *pPosEnd = GetPixelPos(x, Min(yBottom, (int)m_rcClip.bottom));

				while (pPos < pPosEnd)
					{
					*pPos = rgbColor;
					pPos = NextRow(pPos);
					}
				}

			break;
			}

		case markerMediumCross:
			{
			int yTop = y - MEDIUM_SQUARE_SIZE;
			int yBottom = y + MEDIUM_SQUARE_SIZE;
			int xLeft = x - MEDIUM_SQUARE_SIZE;
			int xRight = x + MEDIUM_SQUARE_SIZE;

			//	Draw the horizontal line

			if (y >= m_rcClip.top && y < m_rcClip.bottom)
				{
				CG32bitPixel *pRowStart = GetPixelPos(0, y);
				CG32bitPixel *pPos = pRowStart + Max((int)m_rcClip.left, xLeft);
				CG32bitPixel *pPosEnd = pRowStart + Min(xRight+1, (int)m_rcClip.right);

				while (pPos < pPosEnd)
					*pPos++ = rgbColor;
				}

			//	Draw the vertical line

			if (x >= m_rcClip.left && x < m_rcClip.right)
				{
				CG32bitPixel *pPos = GetPixelPos(x, Max(yTop, (int)m_rcClip.top));
				CG32bitPixel *pPosEnd = GetPixelPos(x, Min(yBottom+1, (int)m_rcClip.bottom));

				while (pPos < pPosEnd)
					{
					*pPos = rgbColor;
					pPos = NextRow(pPos);
					}
				}

			break;
			}

		case markerSmallFilledSquare:
			{
			int xLeft = x - SMALL_SQUARE_SIZE;
			int yTop = y - SMALL_SQUARE_SIZE;
			CG32bitPixel rgbInsideColor = CG32bitPixel::Blend(0, rgbColor, (BYTE)192);

			DrawDot(x, y, rgbColor, markerSmallSquare);
			Fill(xLeft + 1, yTop + 1, 2 * SMALL_SQUARE_SIZE - 1, 2 * SMALL_SQUARE_SIZE - 1, rgbInsideColor);
			break;
			}

		case markerSmallCircle:
			{
			CG32bitPixel rgbFade1 = CG32bitPixel::Blend(0, rgbColor, (BYTE)192);
			CG32bitPixel rgbFade2 = CG32bitPixel::Blend(0, rgbColor, (BYTE)128);

			SetPixel(x - 2, y, rgbColor);
			SetPixel(x + 2, y, rgbColor);
			SetPixel(x, y - 2, rgbColor);
			SetPixel(x, y + 2, rgbColor);

			SetPixel(x - 1, y - 1, rgbFade2);
			SetPixel(x - 1, y + 1, rgbFade2);
			SetPixel(x + 1, y - 1, rgbFade2);
			SetPixel(x + 1, y + 1, rgbFade2);

			SetPixel(x - 1, y - 2, rgbFade1);
			SetPixel(x + 1, y - 2, rgbFade1);
			SetPixel(x - 1, y + 2, rgbFade1);
			SetPixel(x + 1, y + 2, rgbFade1);
			SetPixel(x + 2, y - 1, rgbFade1);
			SetPixel(x + 2, y + 1, rgbFade1);
			SetPixel(x - 2, y - 1, rgbFade1);
			SetPixel(x - 2, y + 1, rgbFade1);
			break;
			}

		case markerTinyCircle:
			{
			SetPixel(x - 1, y, rgbColor);
			SetPixel(x - 1, y + 1, rgbColor);
			SetPixel(x + 2, y, rgbColor);
			SetPixel(x + 2, y + 1, rgbColor);

			SetPixel(x, y - 1, rgbColor);
			SetPixel(x, y + 2, rgbColor);
			SetPixel(x + 1, y - 1, rgbColor);
			SetPixel(x + 1, y + 2, rgbColor);
			break;
			}

		case markerSmallSquare:
			{
			int yTop = y - SMALL_SQUARE_SIZE;
			int yBottom = y + SMALL_SQUARE_SIZE;
			int xLeft = x - SMALL_SQUARE_SIZE;
			int xRight = x + SMALL_SQUARE_SIZE;

			//	Draw the top line

			if (yTop >= m_rcClip.top && yTop < m_rcClip.bottom)
				{
				CG32bitPixel *pRowStart = GetPixelPos(0, yTop);
				CG32bitPixel *pPos = pRowStart + Max((int)m_rcClip.left, xLeft);
				CG32bitPixel *pPosEnd = pRowStart + Min(xRight+1, (int)m_rcClip.right);

				while (pPos < pPosEnd)
					*pPos++ = rgbColor;
				}

			//	Draw the bottom line

			if (yBottom >= m_rcClip.top && yBottom < m_rcClip.bottom)
				{
				CG32bitPixel *pRowStart = GetPixelPos(0, yBottom);
				CG32bitPixel *pPos = pRowStart + Max((int)m_rcClip.left, xLeft);
				CG32bitPixel *pPosEnd = pRowStart + Min(xRight+1, (int)m_rcClip.right);

				while (pPos < pPosEnd)
					*pPos++ = rgbColor;
				}

			//	Draw the left line

			if (xLeft >= m_rcClip.left && xLeft < m_rcClip.right)
				{
				CG32bitPixel *pPos = GetPixelPos(xLeft, Max(yTop+1, (int)m_rcClip.top));
				CG32bitPixel *pPosEnd = GetPixelPos(xLeft, Min(yBottom, (int)m_rcClip.bottom));

				while (pPos < pPosEnd)
					{
					*pPos = rgbColor;
					pPos = NextRow(pPos);
					}
				}

			//	Draw the right line

			if (xRight >= m_rcClip.left && xRight < m_rcClip.right)
				{
				CG32bitPixel *pPos = GetPixelPos(xRight, Max(yTop+1, (int)m_rcClip.top));
				CG32bitPixel *pPosEnd = GetPixelPos(xRight, Min(yBottom, (int)m_rcClip.bottom));

				while (pPos < pPosEnd)
					{
					*pPos = rgbColor;
					pPos = NextRow(pPos);
					}
				}

			break;
			}
		}
	}

void CG32bitImage::InitBMI (BITMAPINFO **retpbi) const

//	InitBMP
//
//	Initializes the m_pBMI structure

	{
	BITMAPINFO *pbmi = (BITMAPINFO *)(new BYTE [sizeof(BITMAPINFO) + 2 * sizeof(DWORD)]);
	utlMemSet(pbmi, sizeof(BITMAPINFO), 0);

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = m_cxWidth;
	pbmi->bmiHeader.biHeight = -m_cyHeight;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = 32;
	pbmi->bmiHeader.biCompression = BI_RGB;

	//	Done

	*retpbi = pbmi;
	}

bool CG32bitImage::SaveAsWindowsBMP (const CString &sFilespec)

//	SaveAsWindowsBMP
//
//	Save to a file

	{
	CFileWriteStream OutputFile(sFilespec);
	if (OutputFile.Create() != NOERROR)
		return false;

	bool bSuccess = WriteToWindowsBMP(&OutputFile);
	OutputFile.Close();

	return bSuccess;
	}

void CG32bitImage::SwapBuffers (CG32bitImage &Other)

//	SwapBuffers
//
//	Swaps the RGBA buffers
	
	
	{
	Swap(m_pRGBA, Other.m_pRGBA);
	}

bool CG32bitImage::WriteToWindowsBMP (IWriteStream *pStream)

//	WriteToWindowsBMP
//
//	Writes out a Windows BMP file format

	{
	//	Compute the total size of the image data (in bytes)

	int iColorTable = 0;
	int iImageDataSize = m_cyHeight * m_cxWidth * sizeof(DWORD);

	BITMAPFILEHEADER header;
	header.bfType = 'MB';
	header.bfOffBits = sizeof(header) + sizeof(BITMAPINFOHEADER) + iColorTable;
	header.bfSize = header.bfOffBits + iImageDataSize;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	pStream->Write((char *)&header, sizeof(header));

	BITMAPINFO *pbmi;
	InitBMI(&pbmi);

	//	Bottom up
	pbmi->bmiHeader.biHeight = -pbmi->bmiHeader.biHeight;

	pStream->Write((char *)pbmi, sizeof(BITMAPINFOHEADER) + iColorTable);

	//	Write the bits bottom-up

	for (int y = m_cyHeight - 1; y >= 0; y--)
		{
		DWORD *pRow = (DWORD *)GetPixelPos(0, y);
		pStream->Write((char *)pRow, m_cxWidth * sizeof(DWORD));
		}

	delete pbmi;
	return true;
	}
