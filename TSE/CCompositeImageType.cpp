//	CCompositeImageType.cpp
//
//	CCompositeImageType class
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define COMPOSITE_TAG							CONSTLIT("ImageComposite")
#define IMAGE_TAG								CONSTLIT("Image")
#define IMAGE_EFFECT_TAG						CONSTLIT("ImageEffect")
#define IMAGE_VARIANTS_TAG						CONSTLIT("ImageVariants")

#define ERR_NO_IMAGE_FOUND						CONSTLIT("Unable to find image descriptor")

CCompositeImageType::CCompositeImageType (void) :
		m_pDesc(NULL)

//	CCompositeImageType constructor

	{
	}

CCompositeImageType::~CCompositeImageType (void)

//	CCompositeImageType destructor

	{
	if (m_pDesc)
		delete m_pDesc;
	}

ALERROR CCompositeImageType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Load from XML

	{
	//	Look for the appropriate image tag

	CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(IMAGE_VARIANTS_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(COMPOSITE_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(IMAGE_EFFECT_TAG);
	if (pImage == NULL)
		return ComposeLoadError(Ctx, ERR_NO_IMAGE_FOUND);

	//	Keep a copy

	m_pDesc = pImage->OrphanCopy();

	return NOERROR;
	}

