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
    m_iCargoSpace = pDesc->GetAttributeIntegerBounded(CARGO_SPACE_ATTRIB, 0, -1, 0);

    return NOERROR;
    }

void CCargoDesc::Interpolate (const CCargoDesc &From, const CCargoDesc &To, Metric rInterpolate)

//  Interpolates
//
//  Initializes based on interpolation between two values.

    {
    m_iCargoSpace = mathRound(mathInterpolate(From.m_iCargoSpace, To.m_iCargoSpace, rInterpolate));
    }
