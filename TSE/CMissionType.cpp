//	CMissionType.cpp
//
//	CMissionType class

#include "PreComp.h"

#define ALLOW_PLAYER_DELETE_ATTRIB				CONSTLIT("allowPlayerDelete")
#define DEBRIEF_AFTER_OUT_OF_SYSTEM_ATTRIB		CONSTLIT("debriefAfterOutOfSystem")
#define EXPIRE_TIME_ATTRIB						CONSTLIT("expireTime")
#define FAILURE_AFTER_OUT_OF_SYSTEM_ATTRIB		CONSTLIT("failureAfterOutOfSystem")
#define FORCE_UNDOCK_AFTER_DEBRIEF_ATTRIB		CONSTLIT("forceUndockAfterDebrief")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MAX_APPEARING_ATTRIB					CONSTLIT("maxAppearing")
#define NAME_ATTRIB								CONSTLIT("name")
#define NO_DEBRIEF_ATTRIB						CONSTLIT("noDebrief")
#define NO_DECLINE_ATTRIB						CONSTLIT("noDecline")
#define NO_FAILURE_ON_OWNER_DESTROYED_ATTRIB	CONSTLIT("noFailureOnOwnerDestroyed")
#define NO_STATS_ATTRIB							CONSTLIT("noStats")
#define PRIORITY_ATTRIB							CONSTLIT("priority")
#define RECORD_NON_PLAYER_ATTRIB				CONSTLIT("recordNonPlayer")

#define FIELD_LEVEL								CONSTLIT("level")
#define FIELD_MAX_LEVEL							CONSTLIT("maxLevel")
#define FIELD_MIN_LEVEL							CONSTLIT("minLevel")
#define FIELD_NAME								CONSTLIT("name")

#define PROPERTY_CAN_BE_DECLINED				CONSTLIT("canBeDeclined")
#define PROPERTY_CAN_BE_DELETED					CONSTLIT("canBeDeleted")
#define PROPERTY_FORCE_UNDOCK_AFTER_DEBRIEF		CONSTLIT("forceUndockAfterDebrief")
#define PROPERTY_HAS_DEBRIEF					CONSTLIT("hasDebrief")
#define PROPERTY_MAX_APPEARING					CONSTLIT("maxAppearing")
#define PROPERTY_PRIORITY						CONSTLIT("priority")
#define PROPERTY_TOTAL_ACCEPTED					CONSTLIT("totalAccepted")
#define PROPERTY_TOTAL_EXISTING					CONSTLIT("totalExisting")

bool CMissionType::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns the data field.

	{
	if (strEquals(sField, FIELD_LEVEL))
		*retsValue = strFromInt(GetLevel());

	else if (strEquals(sField, FIELD_MAX_LEVEL))
		*retsValue = strFromInt(m_iMaxLevel);

	else if (strEquals(sField, FIELD_MIN_LEVEL))
		*retsValue = strFromInt(m_iMinLevel);

	else if (strEquals(sField, FIELD_NAME))
		*retsValue = m_sName;
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

ALERROR CMissionType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	return NOERROR;
	}

ALERROR CMissionType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	ALERROR error;

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_iPriority = pDesc->GetAttributeIntegerBounded(PRIORITY_ATTRIB, 0, -1, 1);
	m_iExpireTime = pDesc->GetAttributeIntegerBounded(EXPIRE_TIME_ATTRIB, 1, -1, -1);
	m_iFailIfOutOfSystem = pDesc->GetAttributeIntegerBounded(FAILURE_AFTER_OUT_OF_SYSTEM_ATTRIB, 0, -1, -1);
	m_fRecordNonPlayer = pDesc->GetAttributeBool(RECORD_NON_PLAYER_ATTRIB);
	m_fNoFailureOnOwnerDestroyed = pDesc->GetAttributeBool(NO_FAILURE_ON_OWNER_DESTROYED_ATTRIB);
	m_fNoDebrief = pDesc->GetAttributeBool(NO_DEBRIEF_ATTRIB);
	m_fNoDecline = pDesc->GetAttributeBool(NO_DECLINE_ATTRIB);
	m_fNoStats = pDesc->GetAttributeBool(NO_STATS_ATTRIB);
	m_fCloseIfOutOfSystem = pDesc->GetAttributeBool(DEBRIEF_AFTER_OUT_OF_SYSTEM_ATTRIB);
	m_fForceUndockAfterDebrief = pDesc->GetAttributeBool(FORCE_UNDOCK_AFTER_DEBRIEF_ATTRIB);
	m_fAllowDelete = pDesc->GetAttributeBool(ALLOW_PLAYER_DELETE_ATTRIB);

	CString sAttrib;
	if (pDesc->FindAttribute(MAX_APPEARING_ATTRIB, &sAttrib))
		{
		if (error = m_MaxAppearing.LoadFromXML(sAttrib))
			{
			Ctx.sError = CONSTLIT("Invalid maxAppearing parameter.");
			return ERR_FAIL;
			}
		}

	m_iMaxAppearing = (m_MaxAppearing.IsEmpty() ? -1 : m_MaxAppearing.Roll());
	m_iAccepted = 0;
	m_iExisting = 0;

	//	Level

	if (pDesc->FindAttribute(LEVEL_ATTRIB, &sAttrib))
		{
		//	Parse this value

		char *pPos = sAttrib.GetASCIIZPointer();
		m_iMinLevel = Max(1, Min(strParseInt(pPos, 1, &pPos), MAX_SYSTEM_LEVEL));

		while (*pPos == ' ')
			pPos++;

		if (*pPos == '-')
			{
			pPos++;
			m_iMaxLevel = Max(m_iMinLevel, Min(strParseInt(pPos, MAX_SYSTEM_LEVEL), MAX_SYSTEM_LEVEL));
			}
		else
			m_iMaxLevel = m_iMinLevel;
		}
	else
		{
		m_iMinLevel = 1;
		m_iMaxLevel = MAX_SYSTEM_LEVEL;
		}

	return NOERROR;
	}

ICCItemPtr CMissionType::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns a property (or NULL if not found).

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_CAN_BE_DECLINED))
		return ICCItemPtr(CC.CreateBool(CanBeDeclined()));

	else if (strEquals(sProperty, PROPERTY_CAN_BE_DELETED))
		return ICCItemPtr(CC.CreateBool(CanBeDeleted()));

	else if (strEquals(sProperty, PROPERTY_FORCE_UNDOCK_AFTER_DEBRIEF))
		return ICCItemPtr(CC.CreateBool(ForceUndockAfterDebrief()));

	else if (strEquals(sProperty, PROPERTY_HAS_DEBRIEF))
		return ICCItemPtr(CC.CreateBool(HasDebrief()));

	else if (strEquals(sProperty, PROPERTY_PRIORITY))
		return ICCItemPtr(CC.CreateInteger(GetPriority()));

	else if (strEquals(sProperty, PROPERTY_MAX_APPEARING))
		{
		if (m_iMaxAppearing != -1)
			return ICCItemPtr(CC.CreateInteger(m_iMaxAppearing));
		else
			return ICCItemPtr(CC.CreateNil());
		}

	else if (strEquals(sProperty, PROPERTY_TOTAL_ACCEPTED))
		return ICCItemPtr(CC.CreateInteger(m_iAccepted));

	else if (strEquals(sProperty, PROPERTY_TOTAL_EXISTING))
		return ICCItemPtr(CC.CreateInteger(m_iExisting));

	else
		return NULL;
	}

void CMissionType::OnReadFromStream (SUniverseLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read mission type data

	{
	if (Ctx.dwVersion >= 22)
		{
		Ctx.pStream->Read(m_iMaxAppearing);
		Ctx.pStream->Read(m_iAccepted);
		}

	if (Ctx.dwVersion >= 35)
		Ctx.pStream->Read(m_iExisting);
	else
		m_iExisting = m_iAccepted;
	}

void CMissionType::OnReinit (void)

//	OnReinit
//
//	Reinitialize

	{
	m_iMaxAppearing = (m_MaxAppearing.IsEmpty() ? -1 : m_MaxAppearing.Roll());
	m_iAccepted = 0;
	m_iExisting = 0;
	}

void CMissionType::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write mission type data

	{
	pStream->Write(m_iMaxAppearing);
	pStream->Write(m_iAccepted);
	pStream->Write(m_iExisting);
	}
