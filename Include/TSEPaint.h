//	TSEPaint.h
//
//	Classes and definitions for painting.
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SViewportPaintCtx
	{
	SViewportPaintCtx (void) :
			pCenter(NULL),
			xCenter(0),
			yCenter(0),
			pObj(NULL),
			iPerception(4),				//	LATER: Same as CSpaceObject::perceptNormal (but we haven't included it yet).
			rgbSpaceColor(CG32bitPixel::Null()),
			pStar(NULL),
			pVolumetricMask(NULL),
			pThreadPool(NULL),
			fNoSelection(false),
			fNoRecon(false),
			fNoDockedShips(false),
			fEnhancedDisplay(false),
			fNoStarfield(false),
			fShowManeuverEffects(false),
			fNoStarshine(false),
			fNoSpaceBackground(false),
            fShowSatellites(false),
			bInFront(false),
			bFade(false),
			iTick(0),
			iVariant(0),
			iDestiny(0),
			iRotation(0),
			iMaxLength(-1),
			iStartFade(0)
		{ }

	inline void Save (void)
		{
		SVariants *pFrame = m_SaveStack.Insert();

		pFrame->bFade = bFade;
		pFrame->bInFront = bInFront;
		pFrame->iDestiny = iDestiny;
		pFrame->iMaxLength = iMaxLength;
		pFrame->iRotation = iRotation;
		pFrame->iTick = iTick;
		pFrame->iVariant = iVariant;
		}

	inline void Restore (void)
		{
		int iLastIndex = m_SaveStack.GetCount() - 1;
		if (iLastIndex >= 0)
			{
			SVariants *pFrame = &m_SaveStack[iLastIndex];

			bFade = pFrame->bFade;
			bInFront = pFrame->bInFront;
			iDestiny = pFrame->iDestiny;
			iMaxLength = pFrame->iMaxLength;
			iRotation = pFrame->iRotation;
			iTick = pFrame->iTick;
			iVariant = pFrame->iVariant;

			m_SaveStack.Delete(iLastIndex);
			}
		}

	//	Viewport metrics

	CSpaceObject *pCenter;				//	Center object (viewport perspective)
	CVector vCenterPos;					//	Center of viewport
	RECT rcView;						//	Viewport
	int xCenter;						//	Center of viewport (pixels)
	int yCenter;						//	Center of viewport (pixels)
	ViewportTransform XForm;			//	Converts from object to screen viewport coordinates
										//		Screen viewport coordinates has positive-Y down.
	ViewportTransform XFormRel;			//	In the case of effects, this Xform has been translated
										//		to offset for the effect position

	CVector vDiagonal;					//	Length of 1/2 viewport diagonal (in global coordinates).
	CVector vUR;						//	upper-right and lower-left of viewport in global
	CVector vLL;						//		coordinates.

	CVector vEnhancedDiagonal;			//	Length of 1/2 enhanced viewport diagonal
	CVector vEnhancedUR;
	CVector vEnhancedLL;

	int iPerception;					//	Perception
	Metric rIndicatorRadius;			//	Radius of circle to show target indicators (in pixels)
	CG32bitPixel rgbSpaceColor;			//	Starshine color
	CSpaceObject *pStar;				//	Nearest star to POV
	const CG8bitSparseImage *pVolumetricMask;	//	Volumetric mask for starshine

	CThreadPool *pThreadPool;			//	Thread pool for painting.

	//	Options

	DWORD fNoSelection:1;
	DWORD fNoRecon:1;
	DWORD fNoDockedShips:1;
	DWORD fEnhancedDisplay:1;
	DWORD fNoStarfield:1;
	DWORD fShowManeuverEffects:1;
	DWORD fNoStarshine:1;
	DWORD fNoSpaceBackground:1;

    DWORD fShowSatellites:1;
    DWORD fSpare2:1;
    DWORD fSpare3:1;
    DWORD fSpare4:1;
    DWORD fSpare5:1;
    DWORD fSpare6:1;
    DWORD fSpare7:1;

	DWORD dwSpare:16;

	CSpaceObject *pObj;					//	Current object being painted
	RECT rcObjBounds;					//	Object bounds in screen coordinates.
	int yAnnotations;					//	Start of free area for annotations (This start at the
										//		bottom of the object bounds, and each annotation
										//		should increment the value appropriately.

	//	May be modified by callers

	bool bInFront;						//	If TRUE, paint elements in front of object (otherwise, behind)
	bool bFade;							//	If TRUE, we're painting a fading element
	int iTick;
	int iVariant;
	int iDestiny;
	int iRotation;						//	An angle 0-359
	int iMaxLength;						//	Max length of object (used for projectiles); -1 == no limit
	int iStartFade;						//	If bFade is TRUE this is the tick on which we started fading

	private:
		struct SVariants
			{
			bool bInFront;
			bool bFade;
			int iTick;
			int iVariant;
			int iDestiny;
			int iRotation;
			int iMaxLength;
			};

		//	Stack of modifications

		TArray<SVariants> m_SaveStack;
	};

class CViewportPaintCtxSmartSave
	{
	public:
		CViewportPaintCtxSmartSave (SViewportPaintCtx &Ctx) : m_Ctx(Ctx)
			{
			m_Ctx.Save();
			}

		~CViewportPaintCtxSmartSave (void)
			{
			m_Ctx.Restore();
			}

	private:
		SViewportPaintCtx &m_Ctx;
	};

class CMapViewportCtx
	{
	public:
		CMapViewportCtx (void);
		CMapViewportCtx (CSpaceObject *pCenter, const RECT &rcView, Metric rMapScale);

		inline CSpaceObject *GetCenterObj (void) const { return m_pCenter; }
		inline const CVector &GetCenterPos (void) const { return m_vCenter; }
		inline const RECT &GetViewportRect (void) const { return m_rcView; }
		inline ViewportTransform &GetXform (void) { return m_Trans; }
		bool IsInViewport (CSpaceObject *pObj) const;
		inline bool IsSpaceBackgroundEnabled (void) const { return !m_bNoSpaceBackground; }
		inline void Set3DMapEnabled (bool bEnabled = true) { m_b3DMap = bEnabled; }
		inline void SetSpaceBackgroundEnabled (bool bEnabled = true) { m_bNoSpaceBackground = !bEnabled; }
		void Transform (const CVector &vPos, int *retx, int *rety) const;

	private:
		CSpaceObject *m_pCenter;		//	Center of viewport (may be NULL)
		CVector m_vCenter;				//	Center of viewport in global coordinate
		RECT m_rcView;					//	RECT of viewport
		Metric m_rMapScale;				//	Map scale (klicks per pixel)

		CVector m_vUR;					//	Upper-right of viewport bounds (in global coordinates)
		CVector m_vLL;					//	Lower-left of viewport bounds
		int m_xCenter;					//	Center of viewport
		int m_yCenter;					//		(in viewport coordinate)

		ViewportTransform m_Trans;		//	Transform

		bool m_b3DMap;					//	Use standard 3D projection
		bool m_bNoSpaceBackground;		//	Do not paint a space background
	};

