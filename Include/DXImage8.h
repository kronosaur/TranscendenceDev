//	DXImage8.h
//
//	Implements 8-bit Channels
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CG8bitImage : public CGImagePlane
	{
	public:
		CG8bitImage (void);
		CG8bitImage (const CG8bitImage &Src);
		~CG8bitImage (void);

		CG8bitImage &operator= (const CG8bitImage &Src);

		//	Basic Interface

		bool Create (int cxWidth, int cyHeight, BYTE InitialValue = 0);
		bool CreateChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);
		bool CreateRoundedRect (int cxWidth, int cyHeight, int iRadius, BYTE Foreground = 0xff, BYTE Background = 0xff);
		inline BYTE GetPixel (int x, int y) const { return *GetPixelPos(x, y); }
		inline BYTE *GetPixelPos (int x, int y) const { return m_pChannel + (y * m_cxWidth) + x; }
		inline bool IsEmpty (void) const { return (m_pChannel == NULL); }
		inline BYTE *NextRow (BYTE *pPos) const { return pPos + m_cxWidth; }

		//	Basic Drawing Interface

		void Fill (int x, int y, int cxWidth, int cyHeight, BYTE Value);
		inline void FillColumn (int x, int y, int cyHeight, BYTE Value) { Fill(x, y, 1, cyHeight, Value); }
		inline void FillLine (int x, int y, int cxWidth, BYTE Value) { Fill(x, y, cxWidth, 1, Value); }
		inline void SetPixel (int x, int y, BYTE Value) 
			{ if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) *GetPixelPos(x, y) = Value; }

	private:
		static int CalcBufferSize (int cxWidth, int cyHeight) { return (cxWidth * cyHeight); }
		void CleanUp (void);
		void Copy (const CG8bitImage &Src);

		BYTE *m_pChannel;
	};
