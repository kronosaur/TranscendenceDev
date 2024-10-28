//	CDifficultyOptions.cpp
//
//	CDifficultyOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CDifficultyOptions::SDesc CDifficultyOptions::m_Table[(int)ELevel::Count] = {
	//	ID				Name			PlayerDamageAdj		EnemyDamageAdj
	{	"story",		"Easy",				0.25,				1.0,	},
	{	"normal",		"Medium",			0.5,				1.0,	},
	{	"challenge",	"Classic",			1.0,				1.0,	},
	{	"permadeath",	"Permadeath",		1.0,				1.0,	},
	};

CString CDifficultyOptions::GetID (ELevel iLevel)

//	GetID
//
//	Returns the ID.

	{
	if (iLevel >= ELevel::Story && iLevel < ELevel::Count)
		return CString(m_Table[(int)iLevel].pID);
	else
		return NULL_STR;
	}

CString CDifficultyOptions::GetLabel (ELevel iLevel)

//	GetLabel
//
//	Returns the human-readable name of the difficulty level.

	{
	if (iLevel >= ELevel::Story && iLevel < ELevel::Count)
		return CString(m_Table[(int)iLevel].pName);
	else
		return NULL_STR;
	}

Metric CDifficultyOptions::GetScoreAdj (void) const

//	GetScoreAdj
//
//	Score adjustment.

	{
	if (m_iLevel >= ELevel::Story && m_iLevel < ELevel::Count)
		return GetPlayerDamageAdj() / GetEnemyDamageAdj();
	else
		return 1.0;
	}

CDifficultyOptions::ELevel CDifficultyOptions::ParseID (const CString &sValue)

//	ParseID
//
//	Parse the value as an ID. If invalid, we return lvlUnknown.

	{
	for (int i = 0; i < (int)ELevel::Count; i++)
		if (strEquals(sValue, CString(m_Table[i].pID)))
			return (ELevel)i;

	return ELevel::Unknown;
	}

void CDifficultyOptions::ReadFromStream (IReadStream &Stream)

//	ReadFromStream
//
//	Reads from a stream.

	{
	DWORD dwLoad;
	Stream.Read(dwLoad);
	if ((int)dwLoad >= 0 && (int)dwLoad < (int)ELevel::Count)
		m_iLevel = (ELevel)dwLoad;
	else
		m_iLevel = ELevel::Unknown;
	}

bool CDifficultyOptions::SaveOnUndock (void) const

//	SaveOnUndock
//
//	Returns TRUE if we should save whenever we undock.

	{
	switch (m_iLevel)
		{
		case ELevel::Story:
		case ELevel::Normal:
			return true;

		default:
			return false;
		}
	}

void CDifficultyOptions::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	Write to a stream.

	{
	DWORD dwSave = (DWORD)m_iLevel;
	Stream.Write(dwSave);
	}
