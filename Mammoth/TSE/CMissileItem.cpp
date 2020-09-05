//	CMissileItem.cpp
//
//	CMissileItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TArray<CItem> CMissileItem::GetLaunchWeapons (void) const

//	GetLaunchWeapons
//
//	Returns a list of weapons that can launch this shot.

	{
	const TArray<CDeviceClass *> &Launchers = GetType().GetLaunchWeapons();

	TArray<CItem> Result;
	Result.InsertEmpty(Launchers.GetCount());
	for (int i = 0; i < Launchers.GetCount(); i++)
		Result[i] = CItem(Launchers[i]->GetItemType(), 1);

	return Result;
	}

CString CMissileItem::GetLaunchedByText (const CItem &Launcher) const

//	GetLaunchedByText
//
//	Returns text of the form:
//
//	"Missile for NAMI missile launcher"

	{
	CString sLauncherName = Launcher.GetNounPhrase();

	ICCItemPtr pData(ICCItem::SymbolTable);
	pData->SetStringAt(CONSTLIT("launcherName"), sLauncherName);

	CString sText;
	if (GetType().TranslateText(*this, LANGID_CORE_LAUNCHED_BY, pData, &sText))
		return sText;
	else if (GetType().IsAmmunition())
		return strPatternSubst(CONSTLIT("Fired by %s"), sLauncherName);
	else
		return strPatternSubst(CONSTLIT("Launched by %s"), sLauncherName);
	}
