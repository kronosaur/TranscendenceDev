//	Achievements.cpp
//
//	Generate achievements table.
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void GenerateAchievementsTable (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	auto &Definitions = Universe.GetDesignCollection().GetAchievementDefinitions();

	printf("ID\tName\tSteamID\tExtension\n");

	for (int i = 0; i < Definitions.GetCount(); i++)
		{
		auto &Def = Definitions.GetDefinition(i);

		CString sSteamID = Def.GetSteamID();
		const CExtension *pExtension = Def.GetType().GetExtension();
		DWORD dwExtensionUNID = (pExtension ? pExtension->GetUNID() : 0);

		if (sSteamID.IsBlank())
			{
			printf("%s\t%s\t\t%08x\n",
				(LPSTR)Def.GetID(),
				(LPSTR)Def.GetName(),
				dwExtensionUNID
				);
			}
		else
			{
			printf("%s\t%s\t%s (%d)\t%08x\n",
				(LPSTR)Def.GetID(),
				(LPSTR)Def.GetName(),
				(LPSTR)sSteamID,
				Def.GetSteamIDCode(),
				dwExtensionUNID
				);
			}
		}
	}

