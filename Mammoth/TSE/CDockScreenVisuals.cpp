//	CDockScreenVisuals.cpp
//
//	CDockScreenVisuals class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define BACKGROUND_IMAGE_ATTRIB					CONSTLIT("backgroundImage")
#define CONTENT_MASK_ATTRIB		    			CONSTLIT("contentMask")
#define TEXT_COLOR_ATTRIB     					CONSTLIT("textColor")
#define TEXT_BACKGROUND_COLOR_ATTRIB     		CONSTLIT("textBackgroundColor")
#define TITLE_BACKGROUND_ATTRIB					CONSTLIT("titleBackgroundColor")
#define TITLE_TEXT_ATTRIB				    	CONSTLIT("titleTextColor")

#define FONT_MEDIUM_HEAVY_BOLD					CONSTLIT("MediumHeavyBold")

const CG32bitPixel RGB_TEXT =                   CG32bitPixel(0xb0, 0xb8, 0xbf);    //   H:210 S:8   B:75
const CG32bitPixel RGB_TEXT_BACKGROUND =        CG32bitPixel(0x2a, 0x31, 0x38);    //   H:210 S:25  B:22
const CG32bitPixel RGB_TITLE_BACKGROUND =       CG32bitPixel(0x37, 0x3f, 0x47);    //   H:240 S:23  B:28
const CG32bitPixel RGB_TITLE_TEXT =             CG32bitPixel(0xd9, 0xec, 0xff);    //   H:210 S:15  B:100

CDockScreenVisuals CDockScreenVisuals::m_Default;
bool CDockScreenVisuals::m_bDefaultInitialized = false;

void CDockScreenVisuals::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const

//  AddTypesUsed
//
//  Adds the list of types used by this object

    {
    retTypesUsed->SetAt(m_Background.GetBitmapUNID(), true);
    retTypesUsed->SetAt(m_ContentMask.GetBitmapUNID(), true);
    }

ALERROR CDockScreenVisuals::Bind (SDesignLoadCtx &Ctx)

//  Bind
//
//  Bind design

    {
    ALERROR error;

    if (error = m_Background.OnDesignLoadComplete(Ctx))
        return error;

    if (error = m_ContentMask.OnDesignLoadComplete(Ctx))
        return error;

    return NOERROR;
    }

CDockScreenVisuals &CDockScreenVisuals::GetDefault (CUniverse &Universe)

//  GetDefault
//
//  Returns the default visuals

    {
    //  Initialize, if necessary

    if (!m_bDefaultInitialized)
        {
        //  NOTE: We need to resolve the default image now because we don't get
        //  called at bind (because we're a default image).

        m_Default.m_Background.Init(Universe, DEFAULT_DOCK_SCREEN_IMAGE_UNID, 1, 1, true);
        m_Default.m_ContentMask.Init(Universe, DEFAULT_DOCK_SCREEN_MASK_UNID, 1, 1, true);

        //  Default colors

        m_Default.m_rgbText = RGB_TEXT;
        m_Default.m_rgbTextBackground = RGB_TEXT_BACKGROUND;
        m_Default.m_rgbTitleBackground = RGB_TITLE_BACKGROUND;
        m_Default.m_rgbTitleText = RGB_TITLE_TEXT;

		//	Default fonts

		m_Default.m_pStatusFont = &Universe.GetFont(FONT_MEDIUM_HEAVY_BOLD);

        m_bDefaultInitialized = true;
        }

    return m_Default;
    }

ALERROR CDockScreenVisuals::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//  InitFromXML
//
//  Initializes from XML element.

    {
    //  Images

    DWORD dwImageUNID;
	if ((::LoadUNID(Ctx, pDesc->GetAttribute(BACKGROUND_IMAGE_ATTRIB), &dwImageUNID) != NOERROR)
            || (dwImageUNID != 0 
                && (m_Background.Init(Ctx.GetUniverse(), dwImageUNID, 1, 1) != NOERROR)))
        {
        Ctx.sError = strPatternSubst("Unable to initialize image: %08x", dwImageUNID);
        return ERR_FAIL;
        }

	if ((::LoadUNID(Ctx, pDesc->GetAttribute(CONTENT_MASK_ATTRIB), &dwImageUNID) != NOERROR)
            || (dwImageUNID != 0 
                && (m_ContentMask.Init(Ctx.GetUniverse(), dwImageUNID, 1, 1) != NOERROR)))
        {
        Ctx.sError = strPatternSubst("Unable to initialize image: %08x", dwImageUNID);
        return ERR_FAIL;
        }

    //  Colors

    m_rgbText = ::LoadRGBColor(pDesc->GetAttribute(TEXT_COLOR_ATTRIB), RGB_TEXT);
    m_rgbTextBackground = ::LoadRGBColor(pDesc->GetAttribute(TEXT_BACKGROUND_COLOR_ATTRIB), RGB_TEXT_BACKGROUND);

    m_rgbTitleBackground = ::LoadRGBColor(pDesc->GetAttribute(TITLE_BACKGROUND_ATTRIB), RGB_TITLE_BACKGROUND);
    m_rgbTitleText = ::LoadRGBColor(pDesc->GetAttribute(TITLE_TEXT_ATTRIB), m_rgbText);

	//	Fonts

	m_pStatusFont = &Ctx.GetUniverse().GetFont(FONT_MEDIUM_HEAVY_BOLD);

    return NOERROR;
    }

void CDockScreenVisuals::MarkImages (void) const

//  MarkImages
//
//  Mark images

    {
    m_Background.MarkImage();
    m_ContentMask.MarkImage();
    }
