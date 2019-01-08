//	DXPath.h
//
//	Implements 2D paths
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGPath
	{
	public:
		void AddPolygonHandoff (TArray<CVector> &Points);
		void Close (void);
		void CreateSubPathStrokePath (int iSubPath, Metric rLineWidth, CGPath *retPath);
		inline void DeleteAll (void) { m_SubPaths.DeleteAll(); }
		inline int GetSubPathCount (void) const { return m_SubPaths.GetCount(); }
		int GetSubPathPolygonPoints (int iSubPath, TArray<CVector> *retPoints) const;
		int GetVertexCount (void) const;
		void Init (void);
		void Init (const TArray<CVector> &Points, bool bLeaveOpen = false);
		void InitTakeHandoff (TArray<CVector> &Points);
		inline bool IsEmpty (void) const { return (m_SubPaths.GetCount() == 0); }
		void LineTo (const CVector &Pos);
		void MoveTo (const CVector &Pos);
		void Rasterize (CGRegion *retRegion, int iAntiAlias = 4);
		void Scale (Metric rScale);
		void Translate (const CVector &vOffset);

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

class CGShape
	{
	public:
		enum EFlags
			{
			//	Arc

			ARC_INNER_RADIUS =			0x00000001,	//	Radius is inner arc radius, not center.

			//	Text

			TEXT_ALIGN_LEFT =			0x00000000,
			TEXT_ALIGN_CENTER =			0x00000001,
			TEXT_ALIGN_RIGHT =			0x00000002,
			};

		//	Arcs

		static bool Arc (const CVector &vCenter, Metric rRadius, Metric rStartAngle, Metric rEndAngle, Metric rArcWidth, CGPath *retPath, Metric rSpacing = 0, DWORD dwFlags = 0);
		static bool ArcSegment (const CVector &vCenter, Metric rRadius, Metric rAngle, Metric rWidth, CGPath *retPath);
		static bool ArcQuadrilateral (const CVector &vCenter, const CVector &vInnerPos, const CVector &vOuterPos, Metric rWidth, CGPath *retPath);

		//	Helpers for regions

		inline static bool Arc (const CVector &vCenter, Metric rRadius, Metric rStartAngle, Metric rEndAngle, Metric rArcWidth, CGRegion *retRegion, Metric rSpacing = 0, DWORD dwFlags = 0)
			{
			CGPath Path; 
			if (!Arc(vCenter, rRadius, rStartAngle, rEndAngle, rArcWidth, &Path, rSpacing, dwFlags))
				return false;

			Path.Rasterize(retRegion);
			return true;
			}

		inline static bool ArcSegment (const CVector &vCenter, Metric rRadius, Metric rAngle, Metric rWidth, CGRegion *retRegion)
			{
			CGPath Path;
			if (!ArcSegment(vCenter, rRadius, rAngle, rWidth, &Path))
				return false;

			Path.Rasterize(retRegion);
			return true;
			}

		inline static bool ArcQuadrilateral (const CVector &vCenter, const CVector &vInnerPos, const CVector &vOuterPos, Metric rWidth, CGRegion *retRegion)
			{
			CGPath Path;
			if (!ArcQuadrilateral(vCenter, vInnerPos, vOuterPos, rWidth, &Path))
				return false;

			Path.Rasterize(retRegion);
			return true;
			}
	};