//	CSystemMapThumbnails.cpp
//
//	CSystemMapThumbnails class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

//  We want a 600 pixel image to be 1/10th the size of the thumbnail.

const Metric SYSTEM_RADIUS =            LIGHT_SECOND * 1200.0;
const Metric IMAGE_SCALE =              3000.0;

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
    for (i = 0; i < m_Cache.GetCount(); i++)
        delete m_Cache[i];

    m_Cache.DeleteAll();

    for (i = 0; i < m_ObjImageCache.GetCount(); i++)
        delete m_ObjImageCache[i];

    m_ObjImageCache.DeleteAll();

    m_pSystemData = NULL;
    }

void CSystemMapThumbnails::DrawThumbnail (CTopologyNode *pNode, CG32bitImage &Dest, int x, int y, Metric rScale) const

//  DrawThumbnail
//
//  Draws the given thumbnail.

    {
    CG32bitImage *pImage = GetThumbnail(pNode);
    if (pImage == NULL)
        return;

    CGDraw::BltTransformed(Dest,
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
    int iRotation;
    const CObjectImageArray &FullImage = ObjEntry.pType->GetTypeImage().GetImage((ObjEntry.pImageSel ? *ObjEntry.pImageSel : ImageSel), Modifiers, &iRotation);
    CG32bitImage *pBmpImage = (FullImage.IsLoaded() ? &FullImage.GetImage(CONSTLIT("System thumbnail")) : NULL);
    if (pBmpImage == NULL)
        return NULL;

    //  Figure out the image size

	RECT rcBmpImage = FullImage.GetImageRect(0, iRotation);
    int iSize = Max(RectWidth(rcBmpImage), RectHeight(rcBmpImage));
    if (iSize <= 0)
        return NULL;

    //  Compute scale

    Metric rScaleSize = Max((Metric)m_iMinImage, Min(m_rImageScale * iSize, (Metric)m_iMaxImage));
    Metric rScale = rScaleSize / (Metric)iSize;

    //  Stars are bigger

    if (ObjEntry.pType->GetScale() == scaleStar)
        rScale *= 3.0;

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

    //  Remember in cache so we don't have to recreat this.

    m_ObjImageCache.SetAt(dwHash, pImage);

    //  Done

    return pImage;
    }

CG32bitImage *CSystemMapThumbnails::GetThumbnail (CTopologyNode *pNode) const

//  GetThumbnail
//
//  Returns an appropriate thumbnail for the system (or NULL if there is some
//  kind of error).

    {
    int i;

    if (m_pSystemData == NULL)
        return NULL;

    //  Look in cache first.

    CG32bitImage *pImage;
    if (m_Cache.Find(pNode, &pImage))
        return pImage;

    //  Otherwise, we create an image

    pImage = new CG32bitImage;
    pImage->Create(m_cxThumb, m_cyThumb, CG32bitImage::alpha8, CG32bitPixel::Null());
    m_Cache.SetAt(pNode, pImage);

    //  Compute a transform

    RECT rcThumb;
    rcThumb.left = 0;
    rcThumb.top = 0;
    rcThumb.right = m_cxThumb;
    rcThumb.bottom = m_cyThumb;
    CMapViewportCtx Ctx(NULL, rcThumb, m_rMapScale);

    //  Paint orbits

    const TArray<COrbit> &Orbits = m_pSystemData->GetSystemOrbits(pNode);
    for (i = 0; i < Orbits.GetCount(); i++)
        Orbits[i].Paint(Ctx, *pImage, RGB_MAP_ORBIT);

    //  Get the list of objects to paint

    TSortMap<Metric, CObjectTracker::SBackgroundObjEntry> Objs(DescendingSort);
    m_pSystemData->GetSystemBackgroundObjects(pNode, Objs);

    //  Paint all objects

    for (i = 0; i < Objs.GetCount(); i++)
        {
        const CObjectTracker::SBackgroundObjEntry &ObjData = Objs[i];
        CG32bitImage *pObjImage = GetObjImage(ObjData);
        if (pImage == NULL)
            continue;

        int xPos, yPos;
        Ctx.Transform(ObjData.vPos, &xPos, &yPos);

        pImage->Composite(0, 0, pObjImage->GetWidth(), pObjImage->GetHeight(), 255, *pObjImage, xPos - (pObjImage->GetWidth() / 2), yPos - (pObjImage->GetHeight() / 2));
        }

    //  Done

    return pImage;
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
