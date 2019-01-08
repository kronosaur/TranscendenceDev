//	CSystemMapThumbnails.cpp
//
//	CSystemMapThumbnails class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//  We want a 600 pixel image to be 1/10th the size of the thumbnail.

const Metric SYSTEM_RADIUS =            LIGHT_SECOND * 1200.0;
const Metric IMAGE_SCALE =              3000.0;
const Metric STAR_IMAGE_SCALE =         0.1;

const Metric STAR_THUMB_RADIUS =        32.0;   //  Separation of stars, in pixels, at scale = 1.0

const CG32bitPixel RGB_MAP_ORBIT =		CG32bitPixel(115, 149, 229);

CSystemMapThumbnails::CSystemMapThumbnails (void) :
        m_pSystemData(NULL)

//  CSystemMapThumbnails constructor

    {
    }

CSystemMapThumbnails::~CSystemMapThumbnails (void)

//  CSystemMapThumbnails destructor

    {
    CleanUp();
    }

void CSystemMapThumbnails::CleanUp (void)
 
//  CleanUp
//
//  Free all resources

    {
    int i;

    m_Cache.DeleteAll();

    for (i = 0; i < m_ObjImageCache.GetCount(); i++)
        delete m_ObjImageCache[i];

    m_ObjImageCache.DeleteAll();

    m_pSystemData = NULL;
    }

bool CSystemMapThumbnails::CreateThumbnail (CTopologyNode *pNode, bool bFullSystem, CG32bitImage &Result) const

//  CreateThumbnail
//
//  Creates a system thumbnail

    {
    int i;

    if (m_pSystemData == NULL)
        return false;

    //  Create the image

    Result.Create(m_cxThumb, m_cyThumb, CG32bitImage::alpha8, CG32bitPixel::Null());

    //  Compute a transform

    RECT rcThumb;
    rcThumb.left = 0;
    rcThumb.top = 0;
    rcThumb.right = m_cxThumb;
    rcThumb.bottom = m_cyThumb;
    CMapViewportCtx Ctx(NULL, rcThumb, m_rMapScale);

    //  Paint orbits

#ifndef DEBUG
    if (bFullSystem)
        {
        const TArray<COrbit> &Orbits = m_pSystemData->GetSystemOrbits(pNode);
        for (i = 0; i < Orbits.GetCount(); i++)
            Orbits[i].PaintHD(Ctx, Result, RGB_MAP_ORBIT, CGDraw::blendCompositeNormal);
        }
#endif

    //  Get the list of objects to paint

    TSortMap<Metric, CObjectTracker::SBackgroundObjEntry> Objs(DescendingSort);
    m_pSystemData->GetSystemBackgroundObjects(pNode, Objs);

    //  Paint all objects

    for (i = 0; i < Objs.GetCount(); i++)
        {
        const CObjectTracker::SBackgroundObjEntry &ObjData = Objs[i];
        CG32bitImage *pObjImage = GetObjImage(ObjData);
        if (pObjImage == NULL)
            continue;

        //  If we're not painting the full system, then we only paint stars

        if (!bFullSystem && ObjData.pType->GetScale() != scaleStar)
            continue;

        //  Paint

        int xPos, yPos;
        Ctx.Transform(ObjData.vPos, &xPos, &yPos);

        Result.Composite(0, 0, pObjImage->GetWidth(), pObjImage->GetHeight(), 255, *pObjImage, xPos - (pObjImage->GetWidth() / 2), yPos - (pObjImage->GetHeight() / 2));
        }

    //  Done

    return true;
    }

bool CSystemMapThumbnails::CreateThumbnailStars (CTopologyNode *pNode, TArray<SStarDesc> &Result) const

//  CreateThumbnailStars
//
//  Initializes the result array with the set of stars in this node

    {
    int i;

    if (m_pSystemData == NULL)
        return false;

    //  Get the list of objects to paint

    TArray<CObjectTracker::SBackgroundObjEntry> Objs;
    m_pSystemData->GetSystemStarObjects(pNode, Objs);

    //  Short-circuit

    if (Objs.GetCount() == 0)
        return false;

    //  If we only have a single star, then we always position it at the center.

    else if (Objs.GetCount() == 1)
        {
        const CObjectTracker::SBackgroundObjEntry &ObjData = Objs[0];

        SStarDesc *pStar = Result.Insert();
        pStar->pStar = GetObjImage(ObjData);
        pStar->vPos = CVector();
        }

    //  Otherwise, we scale all the stars so they fit

    else
        {
        Metric rLongestDist = Objs[0].vPos.Length();
        for (i = 1; i < Objs.GetCount(); i++)
            {
            const CObjectTracker::SBackgroundObjEntry &ObjData = Objs[i];
            Metric rDist = ObjData.vPos.Length();
            if (rDist > rLongestDist)
                rLongestDist = rDist;
            }

        //  We scale positions so that the farthest star is at the max radius.

        Metric rScale = (rLongestDist > 0.0 ? (STAR_THUMB_RADIUS / rLongestDist) : 0.0);

        //  Now add all stars

        Result.InsertEmpty(Objs.GetCount());
        for (i = 0; i < Objs.GetCount(); i++)
            {
            const CObjectTracker::SBackgroundObjEntry &ObjData = Objs[i];
            Result[i].pStar = GetObjImage(ObjData);
            Result[i].vPos = rScale * ObjData.vPos;
            }
        }

    //  Done

    return true;
    }

void CSystemMapThumbnails::DrawThumbnail (CTopologyNode *pNode, CG32bitImage &Dest, int x, int y, bool bFullSystem, Metric rScale) const

//  DrawThumbnail
//
//  Draws the given thumbnail.

    {
    int i;

    //  For a full system, get the thumbnail image (from the cache, if possible)
    //  and paint it.

    if (bFullSystem)
        {
        CG32bitImage *pImage = GetThumbnail(pNode, bFullSystem);
        if (pImage == NULL)
            return;

        CGDraw::BltTransformedHD(Dest,
                x,
                y,
                rScale,
                rScale,
                0.0,
                *pImage,
                0,
                0,
                pImage->GetWidth(),
                pImage->GetHeight());
        }

    //  Otherwise, we draw just the stars

    else
        {
        const TArray<SStarDesc> &Stars = GetThumbnailStars(pNode);

        for (i = 0; i < Stars.GetCount(); i++)
            {
			if (Stars[i].pStar == NULL)
				continue;

            CGDraw::BltTransformedHD(Dest,
                    x + rScale * Stars[i].vPos.GetX(),
                    y - rScale * Stars[i].vPos.GetY(),
                    rScale,
                    rScale,
                    0.0,
                    *Stars[i].pStar,
                    0,
                    0,
                    Stars[i].pStar->GetWidth(),
                    Stars[i].pStar->GetHeight(),
                    CGDraw::blendNormal);
            }
        }
    }

CG32bitImage *CSystemMapThumbnails::GetObjImage (const CObjectTracker::SBackgroundObjEntry &ObjEntry) const

//  GetObjImage
//
//  Returns an image for the given object, sized appropriately. If we fail to
//  get an image, we return NULL.

    {
    //  Generate a hash for the object's image and see if we've got it already 
    //  in the cache.

    DWORDLONG dwHash = (((DWORDLONG)ObjEntry.pType->GetUNID()) << 32) | (DWORDLONG)(ObjEntry.pImageSel ? ObjEntry.pImageSel->GetHash() : 0);
    CG32bitImage *pImage;
    if (m_ObjImageCache.Find(dwHash, &pImage))
        return pImage;

    //  If not in cache, we need to create the image. Get it from the object.

    CCompositeImageSelector ImageSel;
	CCompositeImageModifiers Modifiers;
    int iVariant;
    const CObjectImageArray &FullImage = ObjEntry.pType->GetTypeImage().GetImage((ObjEntry.pImageSel ? *ObjEntry.pImageSel : ImageSel), Modifiers, &iVariant);
    CG32bitImage *pBmpImage = (FullImage.IsLoaded() ? &FullImage.GetImage(CONSTLIT("System thumbnail")) : NULL);
    if (pBmpImage == NULL)
        return NULL;

    //  Figure out the image size

	RECT rcBmpImage = FullImage.GetImageRect(0, iVariant);
    int iSize = Max(RectWidth(rcBmpImage), RectHeight(rcBmpImage));
    if (iSize <= 0)
        return NULL;

    //  Compute scale

    Metric rScaleSize = Max((Metric)m_iMinImage, Min(m_rImageScale * iSize, (Metric)m_iMaxImage));
    Metric rScale = rScaleSize / (Metric)iSize;

    //  Stars are a consistent scale

    if (ObjEntry.pType->GetScale() == scaleStar)
        rScale = STAR_IMAGE_SCALE;

    //  Create a small version

    pImage = new CG32bitImage;
	if (!pImage->CreateFromImageTransformed(*pBmpImage,
			rcBmpImage.left,
			rcBmpImage.top,
			RectWidth(rcBmpImage),
			RectHeight(rcBmpImage),
			rScale,
			rScale,
			0.0))
        {
        delete pImage;
        return NULL;
        }

    //  Remember in cache so we don't have to recreate this.

    m_ObjImageCache.SetAt(dwHash, pImage);

    //  Done

    return pImage;
    }

CG32bitImage *CSystemMapThumbnails::GetThumbnail (CTopologyNode *pNode, bool bFullSystem) const

//  GetThumbnail
//
//  Returns an appropriate thumbnail for the system (or NULL if there is some
//  kind of error).

    {
    //  Look in cache first.

    SThumbnailCacheEntry *pCache = m_Cache.GetAt(pNode);
    if (pCache)
        {
        //  If we have a full entry, then we can return it (it is always valid).

        if (bFullSystem && pCache->pFullSystem)
            return pCache->pFullSystem;

        //  Otherwise, if we have a partial image, return that.

        else if (!bFullSystem && pCache->pStarsOnly)
            return pCache->pStarsOnly;
        }

    //  Otherwise, we create an image

    CG32bitImage *pImage = new CG32bitImage;
    pImage->Create(m_cxThumb, m_cyThumb, CG32bitImage::alpha8, CG32bitPixel::Null());
    if (!CreateThumbnail(pNode, bFullSystem, *pImage))
        {
        delete pImage;
        return NULL;
        }

    //  Store in cache

    pCache = m_Cache.SetAt(pNode);
    if (bFullSystem)
        {
        ASSERT(pCache->pFullSystem == NULL);
        pCache->pFullSystem = pImage;
        }
    else
        {
        ASSERT(pCache->pStarsOnly == NULL);
        pCache->pStarsOnly = pImage;
        }

    //  Done

    return pImage;
    }

const TArray<CSystemMapThumbnails::SStarDesc> &CSystemMapThumbnails::GetThumbnailStars (CTopologyNode *pNode) const

//  GetThumbnailStars
//
//  Returns an array of stars to draw for the thumbnails

    {
    //  If we don't have it in the cache, we need to create it.

    SThumbnailCacheEntry *pCache = m_Cache.SetAt(pNode);
    if (pCache->Stars.GetCount() == 0)
        CreateThumbnailStars(pNode, pCache->Stars);

    //  Done

    return pCache->Stars;
    }

void CSystemMapThumbnails::Init (CObjectTracker &SystemData, int cxThumb, int cyThumb)

//  Init
//
//  Initializes

    {
    CleanUp();

    m_pSystemData = &SystemData;
    m_cxThumb = cxThumb;
    m_cyThumb = cyThumb;

    Metric rRadiusPixels = Max(m_cxThumb, m_cyThumb) / 2.0;
    m_rMapScale = SYSTEM_RADIUS / rRadiusPixels;
    m_Trans = ViewportTransform(CVector(), m_rMapScale, m_cxThumb / 2, m_cyThumb / 2);

    //  Compute the size of images on the thumbnail. We want a 600 pixel image 
    //  to be 1/10th the size of the thumbnail.

    m_rImageScale = m_cxThumb / (Metric)IMAGE_SCALE;
    m_iMaxImage = m_cxThumb / 10;
    m_iMinImage = Max(2, m_cxThumb / 80);
    }
