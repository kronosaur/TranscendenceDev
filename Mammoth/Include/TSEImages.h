//	TSEImages.h
//
//	Defines classes and interfaces for images
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCompositeImageModifiers;
class CSystemType;

struct SGetImageCtx
	{
	SGetImageCtx (CUniverse &UniverseArg) :
			m_Universe(UniverseArg)
		{ }

	CUniverse &GetUniverse (void) const { return m_Universe; }

	private:
		CUniverse &m_Universe;
	};

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
		bool IsEmpty (void) const { return (m_Stack.GetCount() == 0); }

	private:
		TArray<CImageFilterDesc> m_Stack;
	};

class CObjectImage : public CDesignType
	{
	public:
		CObjectImage (void);
		CObjectImage (CG32bitImage *pBitmap, bool bFreeBitmap = false, CG32bitImage *pShadowMask = NULL);
		~CObjectImage (void);

		CObjectImage *AddRef (void) { m_dwRefCount++; return this; }
		CG32bitImage *CreateCopy (CString *retsError = NULL);
		ALERROR Exists (SDesignLoadCtx &Ctx);
		bool FreesBitmap (void) const { return m_bFreeBitmap; }
        int GetHeight (void) const { return (m_pBitmap ? m_pBitmap->GetHeight() : 0); }
		CG32bitImage *GetHitMask (void);
		size_t GetMemoryUsage (void) const;
		CG32bitImage *GetRawImage (const CString &sLoadReason, CString *retsError = NULL) const;
		CString GetImageFilename (void) { return m_sBitmap; }
		CG32bitImage *GetShadowMask (void);
        int GetWidth (void) const { return (m_pBitmap ? m_pBitmap->GetWidth() : 0); }
		bool HasAlpha (void) { return (m_pBitmap ? (m_pBitmap->GetAlphaType() == CG32bitImage::alpha8) : false); }

        bool IsMarked (void) const { return m_bMarked; }
        ALERROR Lock (SDesignLoadCtx &Ctx);
		void Mark (void) { GetRawImage(NULL_STR); m_bMarked = true; }

		//	CDesignType overrides
		static CObjectImage *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designImage) ? (CObjectImage *)pType : NULL); }

		virtual void Delete (void) override { if (--m_dwRefCount == 0) delete this; }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designImage; }

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateStats (SStats &Stats) const { Stats.dwGraphicsMemory += GetMemoryUsage(); }
		virtual void OnClearMark (void) override { m_bMarked = false; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const;
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnSweep (void) override;
		virtual void OnUnbindDesign (void) override;

	private:
		void CleanUp (void);
		CG32bitImage *LoadImageFromDb (CResourceDb &ResDb, const CString &sLoadReason, CString *retsError = NULL) const;
		bool LoadMask(const CString &sFilespec, CG32bitImage **retpImage);

		CString m_sResourceDb;					//	Resource db
		CString m_sBitmap;						//	Bitmap resource within db
		CString m_sBitmask;						//	Optional mask to use for alpha
		CString m_sHitMask;						//	Optional mask to use for hit testing
		CString m_sShadowMask;					//	Optional mask to use to generate volumetric shadow
		bool m_bPreMult = false;				//	If TRUE, image needs to be premultiplied with mask on load.
		bool m_bLoadOnUse = false;				//	If TRUE, image is only loaded when needed
		mutable bool m_bFreeBitmap = false;		//	If TRUE, we free the bitmap when done
		ChannelTypes m_iPNGMaskAlphaChannel = channelAlpha;	//	This is the channel that should be loaded from a png bitmask (m_sBitmask) to use for alpha

		//	Runtime

		bool m_bMarked = false;					//	Marked
		bool m_bLocked = false;					//	Image is never unloaded
		DWORD m_dwRefCount = 1;

		//	Cached values

		mutable CG32bitImage *m_pBitmap = NULL;	//	Loaded image (NULL if not loaded)
		CG32bitImage *m_pHitMask = NULL;		//	NULL if not loaded
		CG32bitImage *m_pShadowMask = NULL;		//	NULL if not loaded
		mutable bool m_bLoadError = false;		//	If TRUE, load failed

		//	Property table

		static TPropertyHandler<CObjectImage> m_PropertyTable;
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

		CObjectImageArray (void) { }
		CObjectImageArray (const CObjectImageArray &Source);
		~CObjectImageArray (void);

		explicit operator bool () const { return !IsEmpty(); }
		CObjectImageArray &operator= (const CObjectImageArray &Source);

		ALERROR Init (CUniverse &Universe, DWORD dwBitmapUNID, int iFrameCount, int iTicksPerFrame, bool bResolveNow = false);
		ALERROR Init (CUniverse &Universe, DWORD dwBitmapUNID, const RECT &rcImage, int iFrameCount, int iTicksPerFrame);
		ALERROR Init (CObjectImage *pImage, const RECT &rcImage, int iFrameCount, int iTicksPerFrame);
		ALERROR InitFromBitmap (CG32bitImage *pBitmap, const RECT &rcImage, int iFrameCount, int iTicksPerFrame, bool bFreeBitmap, int xOffset = 0, int yOffset = 0);
		ALERROR InitFromFrame (const CObjectImageArray &Source, int iTick, int iRotationIndex);
		ALERROR InitFromRotated (const CObjectImageArray &Source, int iTick, int iVariant, int iRotation);
		ALERROR InitFromRotated (const CObjectImageArray &Source, const RECT &rcSrc, int iRotation);
		ALERROR InitFromXML (const CXMLElement &Desc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc, bool bResolveNow = false, int iDefaultRotationCount = 1);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);

		bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int iTick, int iRotation, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) const;
		void CleanUp (void);
		void ClearMark (void) { if (m_pImage) m_pImage->ClearMark(); }
		void CopyImage (CG32bitImage &Dest, int x, int y, int iFrame, int iRotation) const;
		DWORD GetBitmapUNID (void) const { return m_dwBitmapUNID; }
		CString GetFilename (void) const;
		int GetFrameCount (void) const { return m_iFrameCount; }
		CG32bitImage &GetImage (const CString &sLoadReason) const { CG32bitImage *pBmp = (m_pImage ? m_pImage->GetRawImage(sLoadReason) : NULL); return (pBmp ? *pBmp : m_NullImage); }
		int GetImageHeight (void) const { return RectHeight(m_rcImage); }
		const RECT &GetImageRect (void) const { return m_rcImage; }
		RECT GetImageRect (int iTick, int iRotation, int *retxCenter = NULL, int *retyCenter = NULL) const;
		RECT GetImageRectAtPoint (int x, int y) const;
		bool GetImageOffset (int iTick, int iRotation, int *retx, int *rety) const;
		int GetImageViewportSize (void) const;
		int GetImageWidth (void) const { return RectWidth(m_rcImage); }
		size_t GetMemoryUsage (void) const;
		int GetRotationCount (void) const { return m_iRotationCount; }
		int GetTicksPerFrame (void) const { return m_iTicksPerFrame; }
		bool HasAlpha (void) const { return (m_pImage ? m_pImage->HasAlpha() : false); }
		bool ImagesIntersect (int iTick, int iRotation, int x, int y, const CObjectImageArray &Image2, int iTick2, int iRotation2) const;
		bool IsAnimated (void) const { return (m_iTicksPerFrame > 0 && m_iFrameCount > 1); }
		bool IsEmpty (void) const { return ((m_pImage == NULL) && (m_dwBitmapUNID == 0)); }
		bool IsLoaded (void) const { return (m_pImage != NULL); }
        bool IsMarked (void) const { return (m_pImage && m_pImage->IsMarked()); }
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
		void SetImage (TSharedPtr<CObjectImage> pImage);
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

		void CalcRequiredImageSize (int &cxRequired, int &cyRequired) const;
		void ComputeRotationOffsets (void);
		void ComputeRotationOffsets (int xOffset, int yOffset);
		void ComputeSourceXY (int iTick, int iRotation, int *retxSrc, int *retySrc) const;
		void ComputeSourceXY (int iTick, int iRotation, LONG *retxSrc, LONG *retySrc) const { ComputeSourceXY(iTick, iRotation, (int *)retxSrc, (int *)retySrc); }
		void CopyFrom (const CObjectImageArray &Source);
		void GenerateGlowImage (int iRotation) const;
		void GenerateScaledImages (int iRotation, int cxWidth, int cyHeight) const;
		CG32bitImage *GetHitMask (void) const;
		bool ValidateImageSize (int cxWidth, int cyHeight) const;

		DWORD m_dwBitmapUNID = 0;				//	UNID of bitmap (0 if none)
		TSharedPtr<CObjectImage> m_pImage;	//	Image
		RECT m_rcImage = { 0 };
		int m_iFrameCount = 0;
		int m_iTicksPerFrame = 0;
		int m_iFlashTicks = 0;
		int m_iRotationCount = 0;
		int m_iRotationOffset = 0;
		OffsetStruct *m_pRotationOffset = NULL;
		int m_iBlending = 0;
		int m_iViewportSize = 0;			//	Size of 3D viewport in pixels (default to image width)
		int m_iFramesPerColumn = 0;			//	Rotation frames spread out over multiple columns
		int m_iFramesPerRow = 0;			//	Animation frames spread out over multiple rows
		bool m_bDefaultSize = false;		//	If TRUE, get size from image.

		//	Cached images

		mutable CG8bitImage *m_pGlowImages = NULL;
		mutable CG32bitImage *m_pScaledImages = NULL;
		mutable int m_cxScaledImage = -1;

		static CObjectImageArray m_Null;
		static CG32bitImage m_NullImage;
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
		void DeleteAll (void) { m_Sel.DeleteAll(); }
		int GetCount (void) const { return m_Sel.GetCount(); }
        DWORD GetHash (void) const;
		CObjectImageArray &GetFlotsamImage (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		CItemType *GetFlotsamType (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		CShipClass *GetShipwreckClass (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		ETypes GetType (DWORD dwID) const;
		int GetVariant (DWORD dwID) const;
		bool IsEmpty (void) const { return (m_Sel.GetCount() == 0); }
		void ReadFromItem (const CDesignCollection &Design, ICCItemPtr pData);
		void ReadFromStream (SLoadCtx &Ctx);
		ICCItemPtr WriteToItem (void) const;
		void WriteToStream (IWriteStream *pStream) const;

        static const CCompositeImageSelector &Null (void) { return g_NullSelector; }

	private:
		struct SEntry
			{
			DWORD dwID;
			int iVariant;					//	If -1 and dwExtra != 0, then this is an ItemType
			DWORD dwExtra;					//	Either 0 or a pointer to CItemType or CShipClass.
			};

		const SEntry *FindEntry (DWORD dwID) const;
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
		DWORD GetID (void) const { return m_dwID; }
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) = 0;
		virtual int GetMaxLifetime (void) const { return 0; }
		virtual CShipClass *GetShipwreckClass (const CCompositeImageSelector &Selector) const { return NULL; }
        virtual CObjectImageArray &GetSimpleImage (void);
		virtual int GetVariantByID (const CString &sID) const { return -1; }
		virtual int GetVariantCount (void) = 0;
		virtual CString GetVariantID (int iVariant) const { return NULL_STR; }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) { return NOERROR; }
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) { }
		virtual bool IsConstant (void) = 0;
		virtual bool IsRotatable (void) const { return false; }
		virtual bool IsShipwreckDesc (void) const { return false; }
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifier) { }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

	protected:
		DWORD m_dwID;
	};

class CCompositeImageModifiers
	{
	public:
		CCompositeImageModifiers (void)
			{ }

		bool operator== (const CCompositeImageModifiers &Val) const;

		void Apply (const SGetImageCtx &Ctx, CObjectImageArray *retImage) const;
		const CImageFilterStack *GetFilters (void) const { return m_pFilters; }
		int GetRotation (void) const { return m_iRotation; }
		bool IsEmpty (void) const { return (m_wFadeOpacity == 0 && !m_bStationDamage && m_pFilters == NULL && m_iRotateImage == 0); }
		bool ReturnFullImage (void) const { return m_bFullImage; }
		void SetFadeColor (CG32bitPixel rgbColor, DWORD dwOpacity) { m_rgbFadeColor = rgbColor; m_wFadeOpacity = (WORD)dwOpacity; }
		void SetFilters (const CImageFilterStack *pFilters) { m_pFilters = pFilters; }
		void SetFullImage (bool bValue = true) { m_bFullImage = bValue; }
		void SetRotateImage (int iRotation) { m_iRotateImage = iRotation; }
		void SetRotation (int iRotation) { m_iRotation = AngleMod(iRotation); }
		void SetStationDamage (bool bValue = true) { m_bStationDamage = bValue; }

	private:
		static void PaintDamage (CG32bitImage &Dest, const RECT &rcDest, int iCount, IEffectPainter *pPainter);

		CG32bitImage *CreateCopy (CObjectImageArray *pImage, RECT *retrcNewImage) const;

		int m_iRotation = 0;				//	Rotation
		int m_iRotateImage = 0;				//	Rotate image
		const CImageFilterStack *m_pFilters = NULL;
		CG32bitPixel m_rgbFadeColor = 0;	//	Apply a wash on top of image
		WORD m_wFadeOpacity = 0;			//		0 = no wash

		bool m_bStationDamage = false;		//	Apply station damage to image
		bool m_bFullImage = false;			//	Return full image even if we have rotations
	};

class CCompositeImageDesc
	{
	public:
		CCompositeImageDesc (void);
		CCompositeImageDesc (const CCompositeImageDesc &Src);
		~CCompositeImageDesc (void);

		CCompositeImageDesc &operator= (const CCompositeImageDesc &Src);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { if (m_pRoot) m_pRoot->AddTypesUsed(retTypesUsed); }
		int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers = CCompositeImageModifiers()) const { return (m_pRoot ? m_pRoot->GetActualRotation(Selector, Modifiers) : 0); }
		CObjectImageArray &GetImage (const SGetImageCtx &Ctx, const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers = CCompositeImageModifiers(), int *retiFrameIndex = NULL) const;
		int GetMaxLifetime (void) const;
		size_t GetMemoryUsage (void) const;
		IImageEntry *GetRoot (void) const { return m_pRoot; }
        const CObjectImageArray &GetSimpleImage (void) const { return (m_pRoot ? m_pRoot->GetSimpleImage() : CObjectImageArray::Null()); }
        CObjectImageArray &GetSimpleImage (void);
		int GetVariantByID (const CString &sID) const;
		int GetVariantCount (void) const { return (m_pRoot ? m_pRoot->GetVariantCount() : 0); }
		bool HasShipwreckClass (const CCompositeImageSelector &Selector, CShipClass **retpClass = NULL) const;
		ALERROR InitAsShipwreck (SDesignLoadCtx &Ctx);
		static ALERROR InitEntryFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CIDCounter &IDGen, IImageEntry **retpEntry);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector);
		ALERROR InitSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bResolveNow = false, int iDefaultRotationCount = 1);
		bool IsConstant (void) const { return m_bConstant; }
		bool IsEmpty (void) const { return (GetVariantCount() == 0); }
		bool IsRotatable (void) const { return (m_pRoot ? m_pRoot->IsRotatable() : false); }
		void MarkImage (void);
		void MarkImage (const SGetImageCtx &Ctx, const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers = CCompositeImageModifiers());
		bool NeedsShipwreckClass (void) const;
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

		CXMLElement *GetDesc (void) const { return m_pDesc; }

		//	CDesignType overrides
		static CCompositeImageType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designImageComposite) ? (CCompositeImageType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designImageComposite; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CXMLElement *m_pDesc;
	};

