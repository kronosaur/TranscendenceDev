//	DXPath.h
//
//	Implements 2D paths
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGPath
	{
	public:
		void Close (void);
		void CreateSubPathStrokePath (int iSubPath, Metric rLineWidth, CGPath *retPath);
		inline int GetSubPathCount (void) const { return m_SubPaths.GetCount(); }
		int GetSubPathPolygonPoints (int iSubPath, TArray<CVector> *retPoints) const;
		int GetVertexCount (void) const;
		void Init (void);
		void Init (const TArray<CVector> &Points);
		inline bool IsEmpty (void) const { return (m_SubPaths.GetCount() == 0); }
		void LineTo (const CVector &Pos);
		void MoveTo (const CVector &Pos);
		void Rasterize (CGRegion *retRegion, int iAntiAlias = 4);

		static void CalcCornerPoints (const CVector &From, const CVector &Center, const CVector &To, Metric rHalfWidth, CVector *retInner, CVector *retOuter);

	private:
		struct SSubPath
			{
			SSubPath (void) :
					bClosed(false)
				{ }

			TArray<CVector> Lines;
			bool bClosed;					//	The last line in the sub-path connects back
											//		to the first point.
			};

		TArray<SSubPath> m_SubPaths;
	};
