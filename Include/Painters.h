//	Painters.h
//
//	Transcendence UI Engine
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#pragma once

class CGalacticMapPainter
	{
	public:
		CGalacticMapPainter (const CVisualPalette &VI, CSystemMap *pMap);
		~CGalacticMapPainter (void);

		void AdjustCenter (const RECT &rcView, int xCenter, int yCenter, int iScale, int *retxCenter, int *retyCenter);
		inline int GetHeight (void) { return m_cyMap; }
		void GetPos (int x, int y, const RECT &rcView, int xCenter, int yCenter, int iScale, int *retx, int *rety);
		inline int GetWidth (void) { return m_cxMap; }
		void Paint (CG32bitImage &Dest, const RECT &rcView, int xCenter, int yCenter, int iScale);

	private:
		void DrawNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, CG32bitPixel rgbColor);
		void Init (void);

		const CVisualPalette &m_VI;

		CSystemMap *m_pMap;
		int m_cxMap;
		int m_cyMap;

		CG32bitImage *m_pImage;
		bool m_bFreeImage;
	};