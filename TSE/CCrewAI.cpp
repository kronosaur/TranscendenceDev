//	CCrewAI.cpp
//
//	CCrewAI class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARCHETYPE_BROTHERHOOD					CONSTLIT("brotherhood")
#define ARCHETYPE_ORDER							CONSTLIT("order")
#define ARCHETYPE_VENGEANCE						CONSTLIT("vengeance")

#define FIELD_ARCHETYPE							CONSTLIT("archetype")
#define FIELD_BELIEF							CONSTLIT("belief")
#define FIELD_COHESION							CONSTLIT("cohesion")
#define FIELD_LOYALTY							CONSTLIT("loyalty")

CCrewAI::CCrewAI (void)

//	CCrewAI constructor

	{
	}

CString CCrewAI::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Output crash information

	{
	CString sResult;

	sResult.Append(CONSTLIT("CCrewAI\r\n"));

	return sResult;
	}

bool CCrewAI::OnGetAISettingString (const CString &sSetting, CString *retsValue)

//	OnGetAISettingString
//
//	Returns the value of the given setting.

	{
	if (strEquals(sSetting, FIELD_ARCHETYPE))
		{
		switch (m_Psyche.GetArchetype())
			{
			case archetypeBrotherhood:
				*retsValue = ARCHETYPE_BROTHERHOOD;
				break;

			case archetypeOrder:
				*retsValue = ARCHETYPE_ORDER;
				break;

			case archetypeVengeance:
				*retsValue = ARCHETYPE_VENGEANCE;
				break;

			default:
				return false;
			}
		}

	else if (strEquals(sSetting, FIELD_BELIEF))
		*retsValue = strFromInt(m_Psyche.GetBelief());

	else if (strEquals(sSetting, FIELD_COHESION))
		*retsValue = strFromInt(m_Psyche.GetCohesion());

	else if (strEquals(sSetting, FIELD_LOYALTY))
		*retsValue = strFromInt(m_Psyche.GetLoyalty());

	else
		return false;

	return true;
	}

void CCrewAI::OnObjDestroyedNotify (const SDestroyCtx &Ctx)

//	OnObjDestroyedNotify
//
//	An object has been destroyed.

	{
	}

void CCrewAI::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream

	{
	}

bool CCrewAI::OnSetAISettingString (const CString &sSetting, const CString &sValue)

//	OnSetAISettingString
//
//	Sets the value of the given setting.

	{
	if (strEquals(sSetting, FIELD_ARCHETYPE))
		{
		if (strEquals(sValue, ARCHETYPE_BROTHERHOOD))
			m_Psyche.SetArchetype(archetypeBrotherhood);

		else if (strEquals(sValue, ARCHETYPE_ORDER))
			m_Psyche.SetArchetype(archetypeOrder);

		else if (strEquals(sValue, ARCHETYPE_VENGEANCE))
			m_Psyche.SetArchetype(archetypeVengeance);

		else
			return false;
		}

	else if (strEquals(sSetting, FIELD_BELIEF))
		m_Psyche.SetBelief(strToInt(sValue, 0));

	else if (strEquals(sSetting, FIELD_COHESION))
		m_Psyche.SetCohesion(strToInt(sValue, 0));

	else if (strEquals(sSetting, FIELD_LOYALTY))
		m_Psyche.SetLoyalty(strToInt(sValue, 0));

	else
		return false;

	return true;
	}

void CCrewAI::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream

	{
	}
