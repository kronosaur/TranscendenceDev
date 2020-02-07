//	CEngineLanguage.cpp
//
//	CEngineLanguage class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ABANDONED_SCREEN_ATTRIB					CONSTLIT("abandonedScreen")

static constexpr DWORD UNID_ENGINE_TEXT = 0x00030004;

const CString &CEngineLanguage::GetAsteroidTypeLabel (EAsteroidType iType) const

//	GetAsteroidTypeLabel
//
//	Returns the label for the given asteroid type.

	{
	//	Translate text if we don't have it.

	if (m_AsteroidTypeLabel[static_cast<int>(iType)].IsBlank())
		{
		m_AsteroidTypeLabel[static_cast<int>(iType)] = Translate(strPatternSubst(CONSTLIT("asteroidType.%s"), CAsteroidDesc::CompositionID(iType)));
		}

	return m_AsteroidTypeLabel[static_cast<int>(iType)];
	}

CString CEngineLanguage::Translate (const CString &sID, ICCItem *pData) const

//	Translate
//
//	Translate engine text.

	{
	if (m_pEngineText == NULL)
		{
		m_pEngineText = m_Design.FindEntry(UNID_ENGINE_TEXT);
		if (!m_pEngineText)
			return CONSTLIT("Error: Can't find engine text type.");
		}

	ICCItemPtr pResult;
	if (!m_pEngineText->Translate(sID, pData, pResult))
		return strPatternSubst(CONSTLIT("Error: Can't find engine text ID %s"), sID);

	if (pResult->IsNil())
		return strPatternSubst(CONSTLIT("Error: Engine text ID %s returned Nil."), sID);

	return pResult->GetStringValue();
	}

void CEngineLanguage::Reinit (void)

//	Reinit
//
//	Flush the cache.

	{
	m_pEngineText = NULL;

	for (int i = 0; i < EAsteroidTypeCount; i++)
		m_AsteroidTypeLabel[i] = NULL_STR;
	}
