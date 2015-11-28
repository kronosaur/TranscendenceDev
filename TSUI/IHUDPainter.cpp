//	IHUDPainter.cpp
//
//	IHUDPainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

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

			pPainter = new CArmorHUDImages;
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

