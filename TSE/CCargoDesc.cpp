//	CCargoDesc.cpp
//
//	CCargoDesc class
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CARGO_SPACE_ATTRIB			CONSTLIT("cargoSpace")

ALERROR CCargoDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//  InitFromXML
//
//  Initializes from an XML element

    {
	//	NOTE: It is OK to have negative cargo space; it just means that we use
	//	up more cargo space than we provide.

    m_iCargoSpace = pDesc->GetAttributeInteger(CARGO_SPACE_ATTRIB);
	m_bUninitialized = false;

    return NOERROR;
    }

void CCargoDesc::Interpolate (const CCargoDesc &From, const CCargoDesc &To, Metric rInterpolate)

//  Interpolates
//
//  Initializes based on interpolation between two values.

    {
    m_iCargoSpace = mathRound(mathInterpolate(From.m_iCargoSpace, To.m_iCargoSpace, rInterpolate));
	m_bUninitialized = false;
    }

void CCargoDesc::ValidateCargoSpace (int iMaxCargoSpace)

//	ValidateCargoSpace
//
//	Make sure our cargo space is within limits

	{
	if (m_iCargoSpace < 0)
		m_iCargoSpace = 0;
	else if (iMaxCargoSpace > 0 && m_iCargoSpace > iMaxCargoSpace)
		m_iCargoSpace = iMaxCargoSpace;
	}
