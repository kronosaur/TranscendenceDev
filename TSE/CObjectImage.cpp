//	CObjectImage.cpp
//
//	CObjectImage class

#include "PreComp.h"

#define UNID_ATTRIB							CONSTLIT("UNID")
#define BITMAP_ATTRIB						CONSTLIT("bitmap")
#define BITMASK_ATTRIB						CONSTLIT("bitmask")
#define BACK_COLOR_ATTRIB					CONSTLIT("backColor")
#define HIT_MASK_ATTRIB						CONSTLIT("hitMask")
#define LOAD_ON_USE_ATTRIB					CONSTLIT("loadOnUse")
#define NO_PM_ATTRIB						CONSTLIT("noPM")
#define SHADOW_MASK_ATTRIB					CONSTLIT("shadowMask")
#define SPRITE_ATTRIB						CONSTLIT("sprite")

#define FIELD_IMAGE_DESC					CONSTLIT("imageDesc")

CObjectImage::CObjectImage (void)

//	CObjectImage constructor

	{
	}

CObjectImage::CObjectImage (CG32bitImage *pBitmap, bool bFreeBitmap, CG32bitImage *pShadowMask) :
		m_pBitmap(pBitmap),
		m_pShadowMask(pShadowMask),
		m_bFreeBitmap(bFreeBitmap),
		m_bLocked(true)

//	CObjectImage constructor

	{
	ASSERT(pBitmap);
	}

CObjectImage::~CObjectImage (void)

//	CObjectImage destructor

	{
	CleanUp();
	}

void CObjectImage::CleanUp (void)

//	CleanUp
//
//	Free all images

	{
	//	LATER: For now we assume that either all or none of the bitmaps are
	//	owned by us.

	if (m_bFreeBitmap)
		{
		if (m_pBitmap)
			delete m_pBitmap;

		if (m_pHitMask)
			delete m_pHitMask;

		if (m_pShadowMask)
			delete m_pShadowMask;
		}

	m_pBitmap = NULL;
	m_pHitMask = NULL;
	m_pShadowMask = NULL;
	m_bLocked = false;
	}

CG32bitImage *CObjectImage::CreateCopy (CString *retsError)

//	CreateCopy
//
//	Creates a copy of the image. Caller must manually free the image when done.
//
//	NOTE: This is safe to call even if we have not bound.

	{
	//	If we have the image, the we need to make a copy

	if (m_pBitmap)
		return new CG32bitImage(*m_pBitmap);

	//	Otherwise, we load a copy

	CG32bitImage *pResult = GetRawImage(NULL_STR, retsError);
	m_pBitmap = NULL;	//	Clear out because we don't keep a copy

	return pResult;
	}

ALERROR CObjectImage::Exists (SDesignLoadCtx &Ctx)

//	Exists
//
//	Returns TRUE if the appropriate resources exist

	{
	if (m_pBitmap)
		return NOERROR;

	if (!m_sBitmap.IsBlank() && !Ctx.pResDb->ImageExists(NULL_STR, m_sBitmap))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find image: '%s'"), m_sBitmap);
		return ERR_FAIL;
		}

	if (!m_sBitmask.IsBlank() && !Ctx.pResDb->ImageExists(NULL_STR, m_sBitmask))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find image: '%s'"), m_sBitmask);
		return ERR_FAIL;
		}

	if (!m_sHitMask.IsBlank() && !Ctx.pResDb->ImageExists(NULL_STR, m_sHitMask))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find image: '%s'"), m_sHitMask);
		return ERR_FAIL;
		}

	if (!m_sShadowMask.IsBlank() && !Ctx.pResDb->ImageExists(NULL_STR, m_sShadowMask))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find image: '%s'"), m_sShadowMask);
		return ERR_FAIL;
		}

	return NOERROR;
	}

bool CObjectImage::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns data about the image

	{
	if (strEquals(sField, FIELD_IMAGE_DESC))
		{
		CG32bitImage *pImage = GetRawImage(CONSTLIT("data field"));
		if (pImage == NULL)
			{
			*retsValue = NULL_STR;
			return true;
			}

		*retsValue = strPatternSubst(CONSTLIT("=(0x%x 0 0 %d %d)"),
				GetUNID(),
				pImage->GetWidth(),
				pImage->GetHeight());
		}
	else
		return false;

	return true;
	}

CG32bitImage *CObjectImage::GetHitMask (void)

//	GetHitMask
//
//	Returns the hit mask image, if we have one. Otherwise, we return NULL.

	{
	//	If we have the image, we're done

	if (m_pHitMask)
		return m_pHitMask;

	//	If no shadow mask, nothing to do

	if (m_sHitMask.IsBlank())
		return NULL;

	//	Otherwise, load it

	if (!LoadMask(m_sHitMask, &m_pHitMask))
		return NULL;

	return m_pHitMask;
	}

CG32bitImage *CObjectImage::GetRawImage (const CString &sLoadReason, CString *retsError) const

//	GetRawImage
//
//	Returns the image

	{
	try
		{
		//	If we have the image, we're done

		if (m_pBitmap)
			return m_pBitmap;

		//	If we have a load error, then don't bother trying again (otherwise we'll 
		//	constantly be opening files).

		if (m_bLoadError)
			return NULL;

		//	Open the database

		CResourceDb ResDb(m_sResourceDb, !strEquals(m_sResourceDb, g_pUniverse->GetResourceDb()));
		ResDb.SetDebugMode(g_pUniverse->InDebugMode());
		if (ResDb.Open(DFOPEN_FLAG_READ_ONLY, retsError) != NOERROR)
			{
			::kernelDebugLogPattern("Unable to open resource db: %s", m_sResourceDb);
			m_bLoadError = true;
			return NULL;
			}

		//	Load the image

		CString sError;
		m_pBitmap = LoadImageFromDb(ResDb, sLoadReason, &sError);
		if (m_pBitmap == NULL)
			{
			::kernelDebugLogString(sError);
			m_bLoadError = true;
			if (retsError) *retsError = sError;
			return NULL;
			}

		//	We need to free the bitmap

		m_bFreeBitmap = true;
		m_bLoadError = false;

		//	Done

		return m_pBitmap;
		}
	catch (...)
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Crash loading resource db: %s"), m_sResourceDb);

		return NULL;
		}
	}

CG32bitImage *CObjectImage::GetShadowMask (void)

//	GetShadowMask
//
//	Returns the shadow mask image, if we have one. Otherwise, we return NULL.

	{
	//	If we have the image, we're done

	if (m_pShadowMask)
		return m_pShadowMask;

	//	If no shadow mask, nothing to do

	if (m_sShadowMask.IsBlank())
		return NULL;

	//	Otherwise, load it

	if (!LoadMask(m_sShadowMask, &m_pShadowMask))
		return NULL;

	return m_pShadowMask;
	}

CG32bitImage *CObjectImage::LoadImageFromDb (CResourceDb &ResDb, const CString &sLoadReason, CString *retsError) const

//	LoadImageFromDb
//
//	Returns the image, loading it if necessary.
//	NOTE: Callers must free result.

	{
	ALERROR error;

	//	If necessary we log that we had to load an image (we generally do this
	//	to debug issues with loading images in the middle of play).

	if (g_pUniverse->InDebugMode() && g_pUniverse->LogImageLoad())
		kernelDebugLogPattern("Loading image %s for %s.", m_sBitmap, sLoadReason);

	//	Load the images

	TUniquePtr<CG32bitImage> pBitmap;
	if (error = ResDb.LoadImageFile(m_sBitmap, m_sBitmask, pBitmap, m_bPreMult, retsError))
		return NULL;

	//	Done

	return pBitmap.GetHandoff();
	}

bool CObjectImage::LoadMask(const CString &sFilespec, CG32bitImage **retpImage)

//	LoadMask
//
//	Loads a mask image. Returns TRUE if we succeed.

	{
	CString sError;

	//	Open the database

	CResourceDb ResDb(m_sResourceDb, !strEquals(m_sResourceDb, g_pUniverse->GetResourceDb()));
	ResDb.SetDebugMode(g_pUniverse->InDebugMode());
	if (ResDb.Open(DFOPEN_FLAG_READ_ONLY, &sError) != NOERROR)
		{
		::kernelDebugLogPattern("Unable to load %s: %s", sFilespec, sError);
		return false;
		}

	//	Load the mask

	TUniquePtr<CG32bitImage> pImage;
	if (ResDb.LoadMaskFile(sFilespec, pImage, &sError) != NOERROR)
		{
		::kernelDebugLogString(sError);
		return false;
		}

	//	Done

	*retpImage = pImage.GetHandoff();
	return true;
	}

ALERROR CObjectImage::Lock (SDesignLoadCtx &Ctx)

//	Locks the image so that it is always loaded

	{
	//	Note: Since this is called on PrepareBindDesign, we can't
	//	assume that Ctx has the proper resource database. Thus
	//	we have to open it ourselves.
	//
	//	NOTE 2: Locked images never get filter adjustments from the system, 
	//	since they are likely to be utility images, like damage textures.

	CG32bitImage *pImage = GetRawImage(NULL_STR, &Ctx.sError);
	if (pImage == NULL)
		return ERR_FAIL;

	m_bLocked = true;

	return NOERROR;
	}

ALERROR CObjectImage::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	CreateFromXML
//
//	Creates the object from and XML element

	{
	ALERROR error;

	//	Load on use

	if (!pDesc->FindAttributeBool(LOAD_ON_USE_ATTRIB, &m_bLoadOnUse))
		m_bLoadOnUse = true;

	//	Get the file paths

	m_sResourceDb = Ctx.sResDb;
	if (Ctx.sFolder.IsBlank())
		{
		m_sBitmap = pDesc->GetAttribute(BITMAP_ATTRIB);
		m_sBitmask = pDesc->GetAttribute(BITMASK_ATTRIB);
		m_sHitMask = pDesc->GetAttribute(HIT_MASK_ATTRIB);
		m_sShadowMask = pDesc->GetAttribute(SHADOW_MASK_ATTRIB);
		}
	else
		{
		CString sFilespec;
		if (pDesc->FindAttribute(BITMAP_ATTRIB, &sFilespec))
			m_sBitmap = pathAddComponent(Ctx.sFolder, sFilespec);

		if (pDesc->FindAttribute(BITMASK_ATTRIB, &sFilespec))
			m_sBitmask = pathAddComponent(Ctx.sFolder, sFilespec);

		if (pDesc->FindAttribute(HIT_MASK_ATTRIB, &sFilespec))
			m_sHitMask = pathAddComponent(Ctx.sFolder, sFilespec);

		if (pDesc->FindAttribute(SHADOW_MASK_ATTRIB, &sFilespec))
			m_sShadowMask = pathAddComponent(Ctx.sFolder, sFilespec);
		}

	//	Transparent color

	CString sTransColor;
	if (pDesc->FindAttribute(BACK_COLOR_ATTRIB, &sTransColor))
		::kernelDebugLogPattern("backColor not supported: %s.", m_sBitmap);

	//	Sprite

	if (pDesc->GetAttributeBool(SPRITE_ATTRIB))
		::kernelDebugLogPattern("Sprites not yet supported.");

	//	Pre-multiply transparency.
	//
	//	NOTE: The noPM="true" parameter means that the image has not been
	//	pre-multiplied. Thus we need to premultiply it when we load it.

	m_bPreMult = pDesc->GetAttributeBool(NO_PM_ATTRIB);

	//	Initialize

	m_bMarked = false;
	m_bLocked = false;
	m_pBitmap = NULL;
	m_pHitMask = NULL;
	m_pShadowMask = NULL;

	//	If we're loading on use, make sure the image exists. For other
	//	images we don't bother checking because we will load them
	//	soon enough.

	if (m_bLoadOnUse)
		if (error = Exists(Ctx))
			return error;

	//	Done

	return NOERROR;
	}

ALERROR CObjectImage::OnPrepareBindDesign (SDesignLoadCtx &Ctx)

//	OnPrepareBindDesign
//
//	Load the image, if necessary

	{
	ALERROR error;

	//	If we're not loading resources, then this doesn't matter

	if (Ctx.bNoResources)
		NULL;

	//	If we're supposed to load the image, load it now

	else if (!m_bLoadOnUse)
		{
		if (error = Lock(Ctx))
			return error;
		}

	//	Done

	return NOERROR;
	}

void CObjectImage::OnSweep (void)

//	OnSweep
//
//	Garbage collect the image, if it is not marked (i.e., in use)

	{
	DEBUG_TRY

	if (!m_bLocked && !m_bMarked)
		CleanUp();

	DEBUG_CATCH
	}

void CObjectImage::OnUnbindDesign (void)

//	OnUnbindDesign
//
//	Free our image

	{
	if (m_bLoadOnUse)
		CleanUp();
	}
