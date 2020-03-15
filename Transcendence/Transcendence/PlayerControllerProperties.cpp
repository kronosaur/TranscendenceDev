//	PlayerControllerProperties.cpp
//
//	Implements class to control player's ship
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

TPropertyHandler<CPlayerShipController> CPlayerShipController::m_PropertyTable = std::array<TPropertyHandler<CPlayerShipController>::SPropertyDef, 1> {{
		{
		"target",			"Target object",
		[](const CPlayerShipController &Player, const CString &sProperty) 
			{
			return CTLispConvert::CreateObject(Player.GetTarget(NULL, IShipController::FLAG_ACTUAL_TARGET));
			},

		[](CPlayerShipController &Player, const CString &sProperty, const ICCItem &Value, CString *retsError)
			{
			if (!Player.GetShip()->HasTargetingComputer())
				{
				if (retsError) *retsError = CONSTLIT("No targeting computer installed.");
				return false;
				}

			CSpaceObject *pTarget = CTLispConvert::AsObject(&Value);
			Player.SetTarget(pTarget);
			return true;
			},
		},
	}};

ESetPropertyResult CPlayerShipController::SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError)

//	SetProperty
//
//	Sets the property.

	{
	CString sError;

	if (!m_PropertyTable.SetProperty(*this, sProperty, Value, &sError))
		{
		if (sError.IsBlank())
			return ESetPropertyResult::notFound;
		else
			{
			if (retsError) *retsError = sError;
			return ESetPropertyResult::error;
			}
		}

	return ESetPropertyResult::set;
	}
