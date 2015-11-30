//	IHUDPainter.cpp
//
//	IHUDPainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define STYLE_ATTRIB							CONSTLIT("style")

#define STYLE_DEFAULT							CONSTLIT("default")
#define STYLE_RING_SEGMENTS						CONSTLIT("ringSegments")

IHUDPainter *IHUDPainter::Create (SDesignLoadCtx &Ctx, CShipClass *pClass, ETypes iType)

//	Create
//
//	Creates a new painter of the given type. The caller is responsible for 
//	freeing the result. We return NULL in case of error.

	{
	const CPlayerSettings *pSettings = pClass->GetPlayerSettings();

	//	Create

	IHUDPainter *pPainter;
	switch (iType)
		{
		case hudArmor:
			{
			CXMLElement *pDesc = pSettings->GetArmorDesc();
			if (pDesc == NULL)
				return NULL;

			CString sStyle = pDesc->GetAttribute(STYLE_ATTRIB);
			if (sStyle.IsBlank() || strEquals(sStyle, STYLE_DEFAULT))
				pPainter = new CArmorHUDImages;
			else if (strEquals(sStyle, STYLE_RING_SEGMENTS))
				pPainter = new CArmorHUDRingSegments;
			else
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor display style: %s."), sStyle);
				return NULL;
				}

			if (pPainter->InitFromXML(Ctx, pClass, pDesc) != NOERROR)
				{
				delete pPainter;
				return NULL;
				}

			break;
			}

		case hudShields:
			{
			CXMLElement *pDesc = pSettings->GetShieldDesc();
			if (pDesc == NULL)
				return NULL;

			pPainter = new CShieldHUDDefault;
			if (pPainter->InitFromXML(Ctx, pClass, pDesc) != NOERROR)
				{
				delete pPainter;
				return NULL;
				}

			break;
			}

		default:
			ASSERT(false);
			return NULL;
		}

	//	Bind

	if (pPainter->Bind(Ctx) != NOERROR)
		{
		delete pPainter;
		return NULL;
		}

	//	Success

	return pPainter;
	}

