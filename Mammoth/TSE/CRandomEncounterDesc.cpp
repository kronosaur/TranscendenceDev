//	CRandomEncounterDesc.cpp
//
//	CRandomEncounterDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CRandomEncounterDesc::CRandomEncounterDesc (const IShipGenerator &Table, CSpaceObject &BaseObj) :
		m_pTable(&Table),
		m_pType(BaseObj.GetType()),
		m_pBase(&BaseObj)

//	CRandomEncounterDesc constructor

	{
	}

CRandomEncounterDesc::CRandomEncounterDesc (const IShipGenerator &Table, CDesignType &Type, CSovereign *pBaseSovereign) :
		m_pTable(&Table),
		m_pType(&Type),
		m_pBaseSovereign(pBaseSovereign)

//	CRandomEncounterDesc constructor

	{
	}

CRandomEncounterDesc::CRandomEncounterDesc (CSpaceObject &BaseObj) :
		m_pType(BaseObj.GetType()),
		m_pBase(&BaseObj)

//	CRandomEncounterDesc constructor

	{
	}

CRandomEncounterDesc::CRandomEncounterDesc (CDesignType &Type, CSovereign *pBaseSovereign) :
		m_pType(&Type),
		m_pBaseSovereign(pBaseSovereign)

//	CRandomEncounterDesc constructor

	{
	}

void CRandomEncounterDesc::Create (CSystem &System, CSpaceObject *pTarget, CSpaceObject *pGate) const

//	Create
//
//	Create the encounter

	{
	if (m_pTable)
		{
		SShipCreateCtx Ctx;
		Ctx.pSystem = &System;
		Ctx.pBase = m_pBase;
		Ctx.pBaseSovereign = m_pBaseSovereign;
		Ctx.pTarget = pTarget;

		//	Figure out where the encounter will come from

		if (pGate && pGate->IsActiveStargate())
			Ctx.pGate = pGate;
		else if (pGate)
			{
			Ctx.vPos = pGate->GetPos();
			Ctx.PosSpread = DiceRange(2, 1, 2);
			}
		else if (pTarget)
			//	Exclude uncharted stargates
			Ctx.pGate = pTarget->GetNearestStargate(true);

		//	Generate ship

		m_pTable->CreateShips(Ctx);
		}
	else if (m_pType)
		{
		m_pType->FireOnRandomEncounter(m_pBase);
		}
	}

