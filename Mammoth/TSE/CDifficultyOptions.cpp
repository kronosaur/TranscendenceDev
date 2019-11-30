//	CDifficultyOptions.cpp
//
//	CDifficultyOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CDifficultyOptions::SDesc CDifficultyOptions::m_Table[lvlCount] = {
	//	ID				Name			PlayerDamageAdj		EnemyDamageAdj
	{	"story",		"Story Mode",		0.1,				10.0	},
	{	"normal",		"Standard",			0.5,				2.0,	},
	{	"challenge",	"Challenge",		1.0,				1.0,	},
	{	"permadeath",	"Permadeath",		1.0,				1.0		},
	};

CString CDifficultyOptions::GetID (ELevels iLevel)

//	GetID
//
//	Returns the ID.

	{
	if (iLevel >= 0 && iLevel < lvlCount)
		return CString(m_Table[iLevel].pID);
	else
		return NULL_STR;
	}

CString CDifficultyOptions::GetLabel (ELevels iLevel)

//	GetLabel
//
//	Returns the human-readable name of the difficulty level.

	{
	if (iLevel >= 0 && iLevel < lvlCount)
		return CString(m_Table[iLevel].pName);
	else
		return NULL_STR;
	}

CDifficultyOptions::ELevels CDifficultyOptions::ParseID (const CString &sValue)

//	ParseID
//
//	Parse the value as an ID. If invalid, we return lvlUnknown.

	{
	for (int i = 0; i < lvlCount; i++)
		if (strEquals(sValue, CString(m_Table[i].pID)))
			return (ELevels)i;

	return lvlUnknown;
	}

void CDifficultyOptions::ReadFromStream (IReadStream &Stream)

//	ReadFromStream
//
//	Reads from a stream.

	{
	DWORD dwLoad;
	Stream.Read(dwLoad);
	if ((int)dwLoad >= 0 && (int)dwLoad < lvlCount)
		m_iLevel = (ELevels)dwLoad;
	else
		m_iLevel = lvlUnknown;
	}

bool CDifficultyOptions::SaveOnUndock (void) const

//	SaveOnUndock
//
//	Returns TRUE if we should save whenever we undock.

	{
	switch (m_iLevel)
		{
		case lvlStory:
		case lvlNormal:
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
	DWORD dwSave = m_iLevel;
	Stream.Write(dwSave);
	}
