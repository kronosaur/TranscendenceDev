//	CAdventureHighScoreList.cpp
//
//	CGameRecord class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

ALERROR CAdventureHighScoreList::InitFromJSON (DWORD dwAdventureUNID, const CJSONValue &Value)

//	InitFromJSON
//
//	Initialize the list from a JSON list of game records.

	{
	int i;

	m_dwAdventure = dwAdventureUNID;

	m_HighScores.DeleteAll();
	m_HighScores.InsertEmpty(Value.GetCount());

	for (i = 0; i < Value.GetCount(); i++)
		{
		
		if (m_HighScores[i].InitFromJSON(Value.GetElement(i)) != NOERROR)
			m_HighScores[i] = CGameRecord();
		}

	return NOERROR;
	}

void CAdventureHighScoreList::SetSelection (const CString &sUsername, int iScore)

//	SetSelection
//
//	Selects the given entry

	{
	int i;

	m_iSelection = -1;

	//	Short-circuit

	if (sUsername.IsBlank())
		return;

	//	Look for the selection

	for (i = 0; i < m_HighScores.GetCount(); i++)
		if (strEquals(sUsername, m_HighScores[i].GetUsername()) 
				&& m_HighScores[i].GetScore() == iScore)
			{
			m_iSelection = i;
			return;
			}
	}
