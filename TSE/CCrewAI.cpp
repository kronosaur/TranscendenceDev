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
#define FIELD_COMMAND_GROUP						CONSTLIT("commandGroup")
#define FIELD_COMMAND_RANK						CONSTLIT("commandRank")
#define FIELD_LOYALTY							CONSTLIT("loyalty")

CCrewAI::CCrewAI (void) :
		m_iCommandRank(0)

//	CCrewAI constructor

	{
	}

void CCrewAI::AccumulateCrewMetrics (SCrewMetrics &Metrics)

//	AccumulateCrewMetrics
//
//	Accumulates crew metrics

	{
	Metrics.iCrewCount++;
	Metrics.iBelief += m_Psyche.GetBelief();
	Metrics.iCohesion += m_Psyche.GetCohesion();
	Metrics.iLoyalty += m_Psyche.GetLoyalty();
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

bool CCrewAI::OnGetAISettingInteger (const CString &sSetting, int *retiValue)

//	OnGetAISettingInteger
//
//	Returns the value of the given setting.

	{
	if (strEquals(sSetting, FIELD_COMMAND_RANK))
		*retiValue = m_iCommandRank;

	else if (strEquals(sSetting, FIELD_BELIEF))
		*retiValue = m_Psyche.GetBelief();

	else if (strEquals(sSetting, FIELD_COHESION))
		*retiValue = m_Psyche.GetCohesion();

	else if (strEquals(sSetting, FIELD_LOYALTY))
		*retiValue = m_Psyche.GetLoyalty();

	else
		return false;

	return true;
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

	else if (strEquals(sSetting, FIELD_COMMAND_GROUP))
		*retsValue = m_sCommandGroup;

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
//	Read

	{
	m_Psyche.ReadFromStream(Ctx);

	m_sCommandGroup.ReadFromStream(Ctx.pStream);
	Ctx.pStream->Read((char *)&m_iCommandRank, sizeof(DWORD));
	}

bool CCrewAI::OnSetAISettingInteger (const CString &sSetting, int iValue)

//	OnSetAISettingInteger
//
//	Sets the value of the given setting.

	{
	if (strEquals(sSetting, FIELD_COMMAND_RANK))
		m_iCommandRank = iValue;

	else if (strEquals(sSetting, FIELD_BELIEF))
		m_Psyche.SetBelief(iValue);

	else if (strEquals(sSetting, FIELD_COHESION))
		m_Psyche.SetCohesion(iValue);

	else if (strEquals(sSetting, FIELD_LOYALTY))
		m_Psyche.SetLoyalty(iValue);

	else
		return false;

	return true;
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

	else if (strEquals(sSetting, FIELD_COMMAND_GROUP))
		m_sCommandGroup = sValue;

	else
		return false;

	return true;
	}

void CCrewAI::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	CCrewPsyche				m_Psyche
//	CString					m_sCommandGroup
//	DWORD					m_iCommandRank

	{
	m_Psyche.WriteToStream(pStream);

	m_sCommandGroup.WriteToStream(pStream);
	pStream->Write((char *)&m_iCommandRank, sizeof(DWORD));
	}
