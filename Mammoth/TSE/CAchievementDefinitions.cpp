//	CAchievementDefinitions.cpp
//
//	CAchievementDefinitions class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

ALERROR CAchievementDefinitions::AddDefinitions (SDesignLoadCtx &Ctx, const CAchievementDataBlock &Def)

//	AddDefinitions
//
//	Adds all definitions from the given block. We return an error if the given
//	definitions are not unique.

	{
	//	Add all achievements

	for (int i = 0; i < Def.GetCount(); i++)
		{
		auto &AchievementDef = Def.GetAchievementDef(i);

		//	For now we only support achievements on registered types.

		if (AchievementDef.GetType().GetExtension() == NULL
				|| !AchievementDef.GetType().GetExtension()->IsRegistered())
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Only registered extensions may define achievements: %s."), AchievementDef.GetType().GetNounPhrase());
			return ERR_FAIL;
			}

		//	Make sure we don't have duplicate achievement IDs.

		bool bNew;
		auto *ppDef = m_List.SetAt(AchievementDef.GetID(), &bNew);
		if (!bNew)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Duplicate achievement ID: %s."), AchievementDef.GetID());
			return ERR_FAIL;
			}

		//	Add it.

		(*ppDef) = &AchievementDef;
		}

	return NOERROR;
	}

const CAchievementDef *CAchievementDefinitions::FindDefinition (const CString &sID) const

//	FindDefinition
//
//	Finds the definition by ID.

	{
	auto *ppDef = m_List.GetAt(sID);
	if (!ppDef)
		return NULL;

	return *ppDef;
	}
