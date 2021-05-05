//	TSEPaint.h
//
//	Classes and definitions for painting.
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CPaintOrder
	{
	public:
		enum Types
			{
			error =				0xffffffff,	//	Error parsing
			none =				0x00000000,	//	No specific paint order

			bringToFront =		0x00000001,	//	Paint in front
			sendToBack =		0x00000002,	//	Paint behind
			paintOverhang =		0x00000004,	//	Paint above player ship
			};

		static Types Parse (const CString &sValue);
		static CString GetID (Types iType);
	};

class CLabelPainter
	{
	public:
		void AddLabel (const CString &sText, const CG16bitFont &Font, CG32bitPixel rgbColor, int x, int y);
		void Paint (CG32bitImage &Dest) const;

	private:
		struct SLabelDesc
			{
			CString sText;
			const CG16bitFont *pFont = NULL;
			CG32bitPixel rgbColor;
			int x = 0;
			int y = 0;
			};

		TArray<SLabelDesc> m_Labels;
	};

struct SViewportPaintCtx
	{
	public:
		void Save (void)
			{
			SVariants *pFrame = m_SaveStack.Insert();

			pFrame->bFade = bFade;
			pFrame->bInFront = bInFront;
			pFrame->iDestiny = iDestiny;
			pFrame->iMaxLength = iMaxLength;
			pFrame->iRotation = iRotation;
			pFrame->iTick = iTick;
			pFrame->iVariant = iVariant;
			pFrame->yAnnotations = yAnnotations;
			pFrame->rcObjBounds = rcObjBounds;
			pFrame->byShimmer = byShimmer;
			}

		void Restore (void)
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
				yAnnotations = pFrame->yAnnotations;
				rcObjBounds = pFrame->rcObjBounds;
				byShimmer = pFrame->byShimmer;

				m_SaveStack.Delete(iLastIndex);
				}
			}

		//	Viewport metrics

		CSpaceObject *pCenter = NULL;			//	Center object (viewport perspective)
		CVector vCenterPos;						//	Center of viewport
		RECT rcView;							//	Viewport
		int xCenter = 0;						//	Center of viewport (pixels)
		int yCenter = 0;						//	Center of viewport (pixels)
		ViewportTransform XForm;				//	Converts from object to screen viewport coordinates
												//		Screen viewport coordinates has positive-Y down.
		ViewportTransform XFormRel;				//	In the case of effects, this Xform has been translated
												//		to offset for the effect position

		CVector vDiagonal;						//	Length of 1/2 viewport diagonal (in global coordinates).
		CVector vUR;							//	upper-right and lower-left of viewport in global
		CVector vLL;							//		coordinates.

		CVector vEnhancedDiagonal;				//	Length of 1/2 enhanced viewport diagonal
		CVector vEnhancedUR;
		CVector vEnhancedLL;

		int iPerception = 4;					//	LATER: Same as CSpaceObject::perceptNormal (but we haven't included it yet).
		Metric rIndicatorRadius = 0.0;			//	Radius of circle to show target indicators (in pixels)
		CG32bitPixel rgbStarshineColor = CG32bitPixel::Null();			//	Starshine color
		CSpaceObject *pStar = NULL;				//	Nearest star to POV
		const CG8bitSparseImage *pVolumetricMask = NULL;	//	Volumetric mask for starshine

		CThreadPool *pThreadPool = NULL;		//	Thread pool for painting.

		//	Options

		bool fNoSelection = false;
		bool fNoRecon = false;
		bool fNoDockedShips = false;
		bool bEnhancedDisplay = false;
		bool fNoStarfield = false;
		bool fShowManeuverEffects = false;
		bool fNoStarshine = false;
		bool fNoSpaceBackground = false;
		bool fShowSatellites = false;
		bool bShowUnexploredAnnotation = false;
		bool bShowBounds = false;
		bool bShowFacingsAngle = false;
		bool bNo3DExtras = false;
		bool bInPaintSubordinate = false;
		bool bShowOrderInfo = false;

		CSpaceObject *pObj = NULL;				//	Current object being painted
		RECT rcObjBounds;						//	Object bounds in screen coordinates.
		int yAnnotations = 0;					//	Start of free area for annotations (This start at the
												//		bottom of the object bounds, and each annotation
												//		should increment the value appropriately.

		//	May be modified by callers

		bool bInFront = false;					//	If TRUE, paint elements in front of object (otherwise, behind)
		bool bFade = false;						//	If TRUE, we're painting a fading element
		int iTick = 0;
		int iVariant = 0;
		int iDestiny = 0;
		int iRotation = 0;						//	An angle 0-359
		int iMaxLength = -1;					//	Max length of object (used for projectiles); -1 == no limit
		int iStartFade = 0;						//	If bFade is TRUE this is the tick on which we started fading
		Metric rOffsetScale = 1.0;				//	Scale of Group offsets
		DWORD byShimmer = 0;					//	If 1-255, paint stealth shimmer

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
			int yAnnotations;
			RECT rcObjBounds;
			DWORD byShimmer;
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
		CMapViewportCtx (void) { }
		CMapViewportCtx (CSpaceObject *pCenter, const RECT &rcView, Metric rMapScale);

		CSpaceObject *GetCenterObj (void) const { return m_pCenter; }
		const CVector &GetCenterPos (void) const { return m_vCenter; }
		int GetCenterX (void) const { return m_xCenter; }
		int GetCenterY (void) const { return m_yCenter; }
		CLabelPainter &GetLabelPainter (void) { return m_Labels; }
		const CVector &GetLL () const { return m_vLL; }
		const CVector &GetUR () const { return m_vUR; }
		const RECT &GetViewportRect (void) const { return m_rcView; }
		ViewportTransform &GetXform (void) { return m_Trans; }
		bool IsInViewport (CSpaceObject *pObj) const;
		bool IsPaintStationImagesEnabled (void) const { return m_bPaintStationImages; }
		bool IsSpaceBackgroundEnabled (void) const { return !m_bNoSpaceBackground; }
		void Set3DExtrasEnabled (bool bEnabled = true) { m_bShow3DExtras = bEnabled; }
		void Set3DMapEnabled (bool bEnabled = true) { m_b3DMap = bEnabled; }
		void SetPaintStationImagesEnabled (bool bEnabled = true) { m_bPaintStationImages = bEnabled; }
		void SetSpaceBackgroundEnabled (bool bEnabled = true) { m_bNoSpaceBackground = !bEnabled; }
		bool Show3DExtras (void) const { return m_bShow3DExtras; }
		void Transform (const CVector &vPos, int *retx, int *rety) const;

	private:
		CSpaceObject *m_pCenter = NULL;		//	Center of viewport (may be NULL)
		CVector m_vCenter;					//	Center of viewport in global coordinate
		RECT m_rcView = { 0, 0, 0, 0 };		//	RECT of viewport
		Metric m_rMapScale = g_KlicksPerPixel;	//	Map scale (klicks per pixel)

		CVector m_vUR;						//	Upper-right of viewport bounds (in global coordinates)
		CVector m_vLL;						//	Lower-left of viewport bounds
		int m_xCenter = 0;					//	Center of viewport
		int m_yCenter = 0;					//		(in viewport coordinate)

		ViewportTransform m_Trans;			//	Transform

		CLabelPainter m_Labels;		//	List of labels to paint

		bool m_b3DMap = true;				//	Use standard 3D projection
		bool m_bNoSpaceBackground = false;	//	Do not paint a space background
		bool m_bPaintStationImages = false;	//	Paint station images
		bool m_bShow3DExtras = true;		//	Show 3D extras
	};

