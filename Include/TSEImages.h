//	TSEImages.h
//
//	Defines classes and interfaces for images
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCompositeImageModifiers;
class CSystemType;

struct SPointInObjectCtx
	{
	SPointInObjectCtx (void) :
			pObjImage(NULL),
			pImage(NULL)
		{ }


	//	Used by CStation
	const CObjectImageArray *pObjImage;

	//	Used by CObjectImageArray
	CG32bitImage *pImage;				//	Image
	RECT rcImage;						//	RECT of valid image
	int xImageOffset;					//	Offset to convert from point coords to image coords
	int yImageOffset;
	};

struct SLightingCtx
	{
	SLightingCtx (int iStarAngleArg) :
			iStarAngle(iStarAngleArg),
			vRight(1.0, 0.0),
			vDown(0.0, 1.0)
		{
		vSl = ::PolarToVector(iStarAngle, 1.0);
		vSw = vSl.Perpendicular();
		vSl.SetY(-vSl.GetY());
		vSw.SetY(-vSw.GetY());

		vIncX = CVector(vSw.Dot(vRight), vSl.Dot(vRight));
		vIncY = CVector(vSw.Dot(vDown), vSl.Dot(vDown));
		}

	int iStarAngle;						//	Angle towards which light is shining

	//	Image space unit vectors

	CVector vRight;
	CVector vDown;

	//	Two orthogonal unit vectors along the lighting path
	//	(In image coordinates)

	CVector vSl;
	CVector vSw;

	//	Two orthogonal vectors to map lighting space to image
	//	space coordinates.

	CVector vIncX;
	CVector vIncY;
	};

class CImageFilterDesc
	{
	public:
		enum ETypes
			{
			filterNone,

			filterBloom,					//	iData1 = clip point (0-255)
			filterColorize,					//	iData1 = hue, iData2 = saturation, iData3 = brightness, byOpacity = fade
			filterHueSaturation,			//	iData1 = hue, iData2 = saturation, iData3 = brightness, byOpactiy = fade
			filterLevels,					//	iData1 = black point, iData2 = white point, rData1 = gamma
			};

		CImageFilterDesc (void);

		bool operator== (const CImageFilterDesc &Val) const;
		bool operator!= (const CImageFilterDesc &Val) const { return (*this == Val ? false : true); }

		void ApplyTo (CG32bitImage &Image) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);

	private:
		void ApplyBloom (CG32bitImage &Dest, int iClipPoint) const;
		void ApplyColorize (CG32bitImage &Dest, int iHue, int iSaturation, int iBrightness, BYTE byOpacity) const;
		void ApplyHueSaturation (CG32bitImage &Dest, int iHue, int iSaturation, int iBrightness, BYTE byOpacity) const;
		void ApplyLevels (CG32bitImage &Dest, int iBlack, int iWhite, Metric rGamma) const;

		ETypes m_iType;
		int m_iData1;
		int m_iData2;
		int m_iData3;
		Metric m_rData1;
		BYTE m_byOpacity;
	};

class CImageFilterStack
	{
	public:
		bool operator== (const CImageFilterStack &Val) const;
		bool operator!= (const CImageFilterStack &Val) const { return (*this == Val ? false : true); }

		void ApplyTo (CG32bitImage &Image) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		inline bool IsEmpty (void) const { return (m_Stack.GetCount() == 0); }

	private:
		TArray<CImageFilterDesc> m_Stack;
	};

class CObjectImage : public CDesignType
	{
	public:
		CObjectImage (void);
		CObjectImage (CG32bitImage *pBitmap, bool bFreeBitmap = false, CG32bitImage *pShadowMask = NULL);
		~CObjectImage (void);

		CG32bitImage *CreateCopy (CString *retsError = NULL);
		ALERROR Exists (SDesignLoadCtx &Ctx);
		inline bool FreesBitmap (void) const { return m_bFreeBitmap; }
        inline int GetHeight (void) const { return (m_pBitmap ? m_pBitmap->GetHeight() : 0); }
		CG32bitImage *GetHitMask (void);
		CG32bitImage *GetRawImage (const CString &sLoadReason, CString *retsError = NULL) const;
		inline CString GetImageFilename (void) { return m_sBitmap; }
		CG32bitImage *GetShadowMask (void);
        inline int GetWidth (void) const { return (m_pBitmap ? m_pBitmap->GetWidth() : 0); }
		inline bool HasAlpha (void) { return (m_pBitmap ? (m_pBitmap->GetAlphaType() == CG32bitImage::alpha8) : false); }

        inline bool IsMarked (void) const { return m_bMarked; }
        ALERROR Lock (SDesignLoadCtx &Ctx);
		inline void Mark (void) { GetRawImage(NULL_STR); m_bMarked = true; }

		//	CDesignType overrides
		static CObjectImage *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designImage) ? (CObjectImage *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const;
		virtual DesignTypes GetType (void) const { return designImage; }

	protected:
		//	CDesignType overrides
		virtual void OnClearMark (void) { m_bMarked = false; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx);
		virtual void OnSweep (void);
		virtual void OnUnbindDesign (void);

	private:
		void CleanUp (void);
		CG32bitImage *LoadImageFromDb (CResourceDb &ResDb, const CString &sLoadReason, CString *retsError = NULL) const;
		bool LoadMask(const CString &sFilespec, CG32bitImage **retpImage);

		CString m_sResourceDb;					//	Resource db
		CString m_sBitmap;						//	Bitmap resource within db
		CString m_sBitmask;						//	Bitmask resource within db
		CString m_sHitMask;						//	Optional mask to use for hit testing
		CString m_sShadowMask;					//	Optional mask to use to generate volumetric shadow
		bool m_bPreMult = false;				//	If TRUE, image needs to be premultiplied with mask on load.
		bool m_bLoadOnUse = false;				//	If TRUE, image is only loaded when needed
		mutable bool m_bFreeBitmap = false;		//	If TRUE, we free the bitmap when done

		mutable CG32bitImage *m_pBitmap = NULL;	//	Loaded image (NULL if not loaded)
		CG32bitImage *m_pHitMask = NULL;		//	NULL if not loaded
		CG32bitImage *m_pShadowMask = NULL;		//	NULL if not loaded
		bool m_bMarked = false;					//	Marked
		bool m_bLocked = false;					//	Image is never unloaded
		mutable bool m_bLoadError = false;		//	If TRUE, load failed
	};

class CObjectImageArray
	{
	public:
		enum EFlags
			{
			FLAG_UPPER_LEFT =	0x00000001,	//	x,y coordinates are upper-left of image (not center)
			FLAG_GRAYED =		0x00000002,	//	Grayed out
			FLAG_CACHED =		0x00000004,	//	Cache scaled image
			};

		CObjectImageArray (void);
		CObjectImageArray (const CObjectImageArray &Source);
		~CObjectImageArray (void);
		CObjectImageArray &operator= (const CObjectImageArray &Source);

		ALERROR Init (CG32bitImage *pBitmap, const RECT &rcImage, int iFrameCount, int iTicksPerFrame, bool bFreeBitmap, int xOffset = 0, int yOffset = 0);
		ALERROR Init (DWORD dwBitmapUNID, int iFrameCount, int iTicksPerFrame, bool bResolveNow = false);
		ALERROR Init (DWORD dwBitmapUNID, const RECT &rcImage, int iFrameCount, int iTicksPerFrame);
		ALERROR Init (CObjectImage *pImage, const RECT &rcImage, int iFrameCount, int iTicksPerFrame);
		ALERROR InitFromFrame (const CObjectImageArray &Source, int iTick, int iRotationIndex);
		ALERROR InitFromRotated (const CObjectImageArray &Source, int iTick, int iVariant, int iRotation);
		ALERROR InitFromXML (CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bResolveNow = false, int iDefaultRotationCount = 1);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);

		bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int iTick, int iRotation, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) const;
		void CleanUp (void);
		inline void ClearMark (void) { if (m_pImage) m_pImage->ClearMark(); }
		void CopyImage (CG32bitImage &Dest, int x, int y, int iFrame, int iRotation) const;
		inline DWORD GetBitmapUNID (void) const { return m_dwBitmapUNID; }
		CString GetFilename (void) const;
		inline int GetFrameCount (void) const { return m_iFrameCount; }
		inline CG32bitImage &GetImage (const CString &sLoadReason) const { CG32bitImage *pBmp = (m_pImage ? m_pImage->GetRawImage(sLoadReason) : NULL); return (pBmp ? *pBmp : m_NullImage); }
		inline int GetImageHeight (void) const { return RectHeight(m_rcImage); }
		inline const RECT &GetImageRect (void) const { return m_rcImage; }
		RECT GetImageRect (int iTick, int iRotation, int *retxCenter = NULL, int *retyCenter = NULL) const;
		RECT GetImageRectAtPoint (int x, int y) const;
		bool GetImageOffset (int iTick, int iRotation, int *retx, int *rety) const;
		inline int GetRotationCount (void) const { return m_iRotationCount; }
		inline int GetTicksPerFrame (void) const { return m_iTicksPerFrame; }
		int GetImageViewportSize (void) const;
		inline int GetImageWidth (void) const { return RectWidth(m_rcImage); }
		inline bool HasAlpha (void) const { return (m_pImage ? m_pImage->HasAlpha() : false); }
		bool ImagesIntersect (int iTick, int iRotation, int x, int y, const CObjectImageArray &Image2, int iTick2, int iRotation2) const;
		inline bool IsEmpty (void) const { return ((m_pImage == NULL) && (m_dwBitmapUNID == 0)); }
		inline bool IsLoaded (void) const { return (m_pImage != NULL); }
        inline bool IsMarked (void) const { return (m_pImage && m_pImage->IsMarked()); }
		void MarkImage (void) const;
		void PaintImage (CG32bitImage &Dest, int x, int y, int iTick, int iRotation, bool bComposite = false) const;
		void PaintImageGrayed (CG32bitImage &Dest, int x, int y, int iTick, int iRotation) const;
		void PaintImageShimmering (CG32bitImage &Dest,
								   int x,
								   int y,
								   int iTick,
								   int iRotation,
								   DWORD byOpacity) const;
		void PaintImageUL (CG32bitImage &Dest, int x, int y, int iTick, int iRotation) const;
		void PaintImageWithGlow (CG32bitImage &Dest,
								 int x,
								 int y,
								 int iTick,
								 int iRotation,
								 CG32bitPixel rgbGlowColor) const;
		void PaintRotatedImage (CG32bitImage &Dest,
								int x,
								int y,
								int iTick,
								int iRotation,
								bool bComposite = false) const;
		void PaintScaledImage (CG32bitImage &Dest, int x, int y, int iTick, int iRotation, int cxWidth, int cyHeight, DWORD dwFlags = 0) const;
		void PaintSilhoutte (CG32bitImage &Dest,
							 int x,
							 int y,
							 int iTick,
							 int iRotation,
							 CG32bitPixel rgbColor) const;
		bool PointInImage (int x, int y, int iTick, int iRotation) const;
		bool PointInImage (SPointInObjectCtx &Ctx, int x, int y) const;
		void PointInImageInit (SPointInObjectCtx &Ctx, int iTick, int iRotation) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void SetImage (CObjectImage *pImage);
		void SetRotationCount (int iRotationCount);
		void TakeHandoff (CObjectImageArray &Source);
		void WriteToStream (IWriteStream *pStream) const;

		static const CObjectImageArray &Null (void) { return m_Null; }

	private:
		enum BlendingTypes
			{
			blendNormal,
			blendLighten,
			};

		struct OffsetStruct
			{
			int x;
			int y;
			};

		void ComputeRotationOffsets (void);
		void ComputeRotationOffsets (int xOffset, int yOffset);
		void ComputeSourceXY (int iTick, int iRotation, int *retxSrc, int *retySrc) const;
		inline void ComputeSourceXY (int iTick, int iRotation, LONG *retxSrc, LONG *retySrc) const { ComputeSourceXY(iTick, iRotation, (int *)retxSrc, (int *)retySrc); }
		void CopyFrom (const CObjectImageArray &Source);
		void GenerateGlowImage (int iRotation) const;
		void GenerateScaledImages (int iRotation, int cxWidth, int cyHeight) const;
		CG32bitImage *GetHitMask (void) const;

		DWORD m_dwBitmapUNID;				//	UNID of bitmap (0 if none)
		CObjectImage *m_pImage;				//	Image (if UNID is 0, we own this structure)
		RECT m_rcImage;
		int m_iFrameCount;
		int m_iTicksPerFrame;
		int m_iFlashTicks;
		int m_iRotationCount;
		int m_iRotationOffset;
		OffsetStruct *m_pRotationOffset;
		int m_iBlending;
		int m_iViewportSize;				//	Size of 3D viewport in pixels (default to image width)
		int m_iFramesPerColumn;				//	Rotation frames spread out over multiple columns
		int m_iFramesPerRow;				//	Animation frames spread out over multiple rows
		bool m_bDefaultSize;				//	If TRUE, get size from image.

		//	Cached images

		mutable CG8bitImage *m_pGlowImages;
		mutable CG32bitImage *m_pScaledImages;
		mutable int m_cxScaledImage;

		static CObjectImageArray m_Null;
		static CG32bitImage m_NullImage;

	friend CObjectClass<CObjectImageArray>;
	};

const DWORD DEFAULT_SELECTOR_ID = 0;

class CCompositeImageSelector
	{
	public:
		enum ETypes
			{
			typeNone,

			typeVariant,
			typeShipClass,
			typeItemType,
			};

		CCompositeImageSelector (void) { }

		bool operator== (const CCompositeImageSelector &Val) const;

		void AddFlotsam (DWORD dwID, CItemType *pItemType);
		void AddShipwreck (DWORD dwID, CShipClass *pWreckClass, int iVariant = -1);
		void AddVariant (DWORD dwID, int iVariant);
		inline void DeleteAll (void) { m_Sel.DeleteAll(); }
		inline int GetCount (void) const { return m_Sel.GetCount(); }
        DWORD GetHash (void) const;
		CObjectImageArray &GetFlotsamImage (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		CItemType *GetFlotsamType (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		CShipClass *GetShipwreckClass (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		CObjectImageArray &GetShipwreckImage (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		ETypes GetType (DWORD dwID) const;
		int GetVariant (DWORD dwID) const;
		inline bool HasShipwreckImage (DWORD dwID = DEFAULT_SELECTOR_ID) const { return (GetShipwreckClass(dwID) != NULL); }
		void ReadFromItem (ICCItemPtr pData);
		void ReadFromStream (SLoadCtx &Ctx);
		ICCItemPtr WriteToItem (void) const;
		void WriteToStream (IWriteStream *pStream) const;

        inline static const CCompositeImageSelector &Null (void) { return g_NullSelector; }

	private:
		struct SEntry
			{
			DWORD dwID;
			int iVariant;					//	If -1 and dwExtra != 0, then this is an ItemType
			DWORD dwExtra;					//	Either 0 or a pointer to CItemType or CShipClass.
			};

		SEntry *FindEntry (DWORD dwID) const;
		ETypes GetEntryType (const SEntry &Entry) const;
        DWORD GetHash (const SEntry &Entry) const;

		TArray<SEntry> m_Sel;

        static CCompositeImageSelector g_NullSelector;
	};

struct SSelectorInitCtx
	{
	SSelectorInitCtx (void) :
			pSystem(NULL)
		{ }

	CSystem *pSystem;
	CVector vObjPos;
	CString sLocAttribs;
	};

class IImageEntry
	{
	public:
		virtual ~IImageEntry (void) { }

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
        virtual IImageEntry *Clone (void) = 0;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const { return 0; }
		inline DWORD GetID (void) const { return m_dwID; }
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) = 0;
		virtual int GetMaxLifetime (void) const { return 0; }
        virtual CObjectImageArray &GetSimpleImage (void);
		virtual int GetVariantCount (void) = 0;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) { return NOERROR; }
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) { }
		virtual bool IsConstant (void) = 0;
		virtual bool IsRotatable (void) const { return false; }
		virtual void MarkImage (void) { }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

	protected:
		DWORD m_dwID;
	};

class CCompositeImageModifiers
	{
	public:
		CCompositeImageModifiers (void) :
				m_fStationDamage(false),
				m_fFullImage(false)
			{ }

		bool operator== (const CCompositeImageModifiers &Val) const;

		void Apply (CObjectImageArray *retImage) const;
		inline const CImageFilterStack *GetFilters (void) const { return m_pFilters; }
		inline int GetRotation (void) const { return m_iRotation; }
		inline bool IsEmpty (void) const { return (m_wFadeOpacity == 0 && !m_fStationDamage && m_pFilters == NULL); }
		inline bool ReturnFullImage (void) const { return (m_fFullImage ? true : false); }
		inline void SetFadeColor (CG32bitPixel rgbColor, DWORD dwOpacity) { m_rgbFadeColor = rgbColor; m_wFadeOpacity = (WORD)dwOpacity; }
		inline void SetFilters (const CImageFilterStack *pFilters) { m_pFilters = pFilters; }
		inline void SetFullImage (bool bValue = true) { m_fFullImage = bValue; }
		inline void SetRotation (int iRotation) { m_iRotation = AngleMod(iRotation); }
		inline void SetStationDamage (bool bValue = true) { m_fStationDamage = bValue; }

		static void Reinit (void);

	private:
		static void InitDamagePainters (void);
		static void PaintDamage (CG32bitImage &Dest, const RECT &rcDest, int iCount, IEffectPainter *pPainter);

		CG32bitImage *CreateCopy (CObjectImageArray *pImage, RECT *retrcNewImage) const;

		int m_iRotation = 0;				//	Rotation
		const CImageFilterStack *m_pFilters = NULL;
		CG32bitPixel m_rgbFadeColor = 0;	//	Apply a wash on top of image
		WORD m_wFadeOpacity = 0;			//		0 = no wash

		DWORD m_fStationDamage:1;			//	Apply station damage to image
		DWORD m_fFullImage:1;				//	Return full image even if we have rotations
		DWORD m_dwSpare:30;
	};

class CCompositeImageDesc
	{
	public:
		CCompositeImageDesc (void);
		CCompositeImageDesc (const CCompositeImageDesc &Src);
		~CCompositeImageDesc (void);

		CCompositeImageDesc &operator= (const CCompositeImageDesc &Src);

		inline void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { if (m_pRoot) m_pRoot->AddTypesUsed(retTypesUsed); }
		inline int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers = CCompositeImageModifiers()) const { return (m_pRoot ? m_pRoot->GetActualRotation(Selector, Modifiers) : 0); }
		CObjectImageArray &GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers = CCompositeImageModifiers(), int *retiFrameIndex = NULL) const;
		int GetMaxLifetime (void) const;
		inline IImageEntry *GetRoot (void) const { return m_pRoot; }
        inline const CObjectImageArray &GetSimpleImage (void) const { return (m_pRoot ? m_pRoot->GetSimpleImage() : CObjectImageArray::Null()); }
        CObjectImageArray &GetSimpleImage (void);
		inline int GetVariantCount (void) { return (m_pRoot ? m_pRoot->GetVariantCount() : 0); }
		static ALERROR InitEntryFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CIDCounter &IDGen, IImageEntry **retpEntry);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector);
		ALERROR InitSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bResolveNow = false, int iDefaultRotationCount = 1);
		inline bool IsConstant (void) const { return m_bConstant; }
		inline bool IsEmpty (void) { return (GetVariantCount() == 0); }
		inline bool IsRotatable (void) const { return (m_pRoot ? m_pRoot->IsRotatable() : false); }
		void MarkImage (void);
		void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers = CCompositeImageModifiers());
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void Reinit (void);

        static const CCompositeImageDesc &Null (void) { return g_Null; }

	private:
		struct SCacheEntry
			{
			CCompositeImageSelector Selector;
			CCompositeImageModifiers Modifiers;
			CObjectImageArray Image;
			};

        void CleanUp (void);
        void Copy (const CCompositeImageDesc &Src);
		SCacheEntry *FindCacheEntry (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const;

		CXMLElement *m_pDesc;
		IImageEntry *m_pRoot;
		bool m_bConstant;
		mutable TArray<SCacheEntry> m_Cache;

        static CCompositeImageDesc g_Null;
	};

class CCompositeImageType : public CDesignType
	{
	public:
		CCompositeImageType (void);
		~CCompositeImageType (void);

		inline CXMLElement *GetDesc (void) const { return m_pDesc; }

		//	CDesignType overrides
		static CCompositeImageType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designImageComposite) ? (CCompositeImageType *)pType : NULL); }
		virtual DesignTypes GetType (void) const { return designImageComposite; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CXMLElement *m_pDesc;
	};

