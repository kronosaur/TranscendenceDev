//	SovereignProperties.cpp
//
//	CSovereign class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_ENEMY_OBJECT_CACHE				CONSTLIT("enemyObjectCache")
#define PROPERTY_NAME							CONSTLIT("name")
#define PROPERTY_PLAYER_THREAT_LEVEL			CONSTLIT("playerThreatLevel")
#define PROPERTY_PLURAL							CONSTLIT("plural")
#define PROPERTY_SHIPS_DESTROYED_BY_PLAYER		CONSTLIT("shipsDestroyedByPlayer")
#define PROPERTY_STATIONS_DESTROYED_BY_PLAYER	CONSTLIT("stationsDestroyedByPlayer")

TPropertyHandler<CSovereign> CSovereign::m_PropertyTable = std::array<TPropertyHandler<CSovereign>::SPropertyDef, 1> {{
		{
		"alignment",		"Sovereign alignment",
		[](const CSovereign &Sovereign, const CString &sProperty) 
			{
			return ICCItemPtr(CSovereign::GetAlignmentID(Sovereign.m_iAlignment));
			},
		NULL,
		},
	}};

ICCItemPtr CSovereign::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns the given property (or NULL if not found)

	{
	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	else if (strEquals(sProperty, PROPERTY_ENEMY_OBJECT_CACHE))
		{
		const CSystem *pSystem = GetUniverse().GetCurrentSystem();
		if (!pSystem)
			return ICCItemPtr::Nil();

		InitEnemyObjectList(pSystem);
		ICCItemPtr pResult(ICCItem::List);
		for (int i = 0; i < m_EnemyObjects.GetCount(); i++)
			pResult->Append(CTLispConvert::CreateObject(m_EnemyObjects.GetObj(i)));

		return pResult;
		}

	else if (strEquals(sProperty, PROPERTY_NAME))
		return ICCItemPtr(GetNounPhrase());

	else if (strEquals(sProperty, PROPERTY_PLAYER_THREAT_LEVEL))
		return ICCItemPtr((int)GetPlayerThreatLevel());

	else if (strEquals(sProperty, PROPERTY_PLURAL))
		return ICCItemPtr(m_bPluralForm);

	else if (strEquals(sProperty, PROPERTY_SHIPS_DESTROYED_BY_PLAYER))
		return ICCItemPtr(m_iShipsDestroyedByPlayer);

	else if (strEquals(sProperty, PROPERTY_STATIONS_DESTROYED_BY_PLAYER))
		return ICCItemPtr(m_iStationsDestroyedByPlayer);

	else
		return NULL;
	}

