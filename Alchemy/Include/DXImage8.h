//	DXImage8.h
//
//	Implements 8-bit Channels
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CG8bitImage : public TImagePlane<CG8bitImage>
	{
	public:
		CG8bitImage (void);
		CG8bitImage (const CG8bitImage &Src);
		~CG8bitImage (void);

		CG8bitImage &operator= (const CG8bitImage &Src);

		//	Basic Interface

		void CleanUp (void);
		bool Create (int cxWidth, int cyHeight, BYTE InitialValue = 0);
		bool CreateChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);
		bool CreateRoundedRect (int cxWidth, int cyHeight, int iRadius, BYTE Foreground = 0xff, BYTE Background = 0xff);
		BYTE GetPixel (int x, int y) const { return *GetPixelPos(x, y); }
		BYTE *GetPixelPos (int x, int y) const { return m_pChannel + (y * m_cxWidth) + x; }
		void IntersectChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xDest = 0, int yDest = 0, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);
		bool IsEmpty (void) const { return (m_pChannel == NULL); }
		bool IsMarked (void) const { return m_bMarked; }
		BYTE *NextRow (BYTE *pPos) const { return pPos + m_cxWidth; }
		void SetMarked (bool bMarked = true) { m_bMarked = bMarked; }

		//	Basic Drawing Interface

		void Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CGRealChannel &Src, int xDest, int yDest);
		void Copy (const CG8bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xDest = 0, int yDest = 0);
		void CopyChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xDest = 0, int yDest = 0);
		void Fill (int x, int y, int cxWidth, int cyHeight, BYTE Value);
		void FillColumn (int x, int y, int cyHeight, BYTE Value) { Fill(x, y, 1, cyHeight, Value); }
		void FillLine (int x, int y, int cxWidth, BYTE Value) { Fill(x, y, cxWidth, 1, Value); }
		void SetPixel (int x, int y, BYTE Value) 
			{ if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) *GetPixelPos(x, y) = Value; }

		//	Debug

		size_t GetMemoryUsage (void) const { return (size_t)CalcBufferSize(GetWidth(), GetHeight()); }

	private:
		static int CalcBufferSize (int cxWidth, int cyHeight) { return (cxWidth * cyHeight); }
		void Copy (const CG8bitImage &Src);

		BYTE *m_pChannel;
		bool m_bMarked;						//	Mark/sweep flag (for use by caller)
	};
