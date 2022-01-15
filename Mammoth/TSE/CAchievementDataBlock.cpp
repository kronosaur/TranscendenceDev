//	CAchievementDataBlock.cpp
//
//	CAchievementDataBlock class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include <utility>

#define ACHIEVEMENT_TAG							CONSTLIT("Achievement")

CAchievementDataBlock CAchievementDataBlock::m_Null;

ALERROR CAchievementDataBlock::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Bind design elements.

	{
	for (int i = 0; i < m_Achievements.GetCount(); i++)
		{
		if (ALERROR error = m_Achievements[i]->BindDesign(Ctx))
			return error;
		}

	return NOERROR;
	}

ALERROR CAchievementDataBlock::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Initialize from an <Achievements> element.

	{
	if (!Ctx.pType)
		throw CException(ERR_FAIL);

	for (int i = 0; i < Desc.GetContentElementCount(); i++)
		{
		const CXMLElement &Entry = *Desc.GetContentElement(i);
		if (strEquals(Entry.GetTag(), ACHIEVEMENT_TAG))
			{
			TUniquePtr<CAchievementDef> pDef(new CAchievementDef(*Ctx.pType));
			if (ALERROR error = pDef->InitFromXML(Ctx, Entry))
				return error;

			auto *ppEntry = m_Achievements.Insert();
			*ppEntry = std::move(pDef);
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid achievement element: %s"), Entry.GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}
