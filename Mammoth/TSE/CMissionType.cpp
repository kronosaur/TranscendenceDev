//	CMissionType.cpp
//
//	CMissionType class

#include "PreComp.h"

#define ALLOW_PLAYER_DELETE_ATTRIB				CONSTLIT("allowPlayerDelete")
#define AUTO_ACCEPT_ATTRIB						CONSTLIT("autoAccept")
#define DEBRIEF_AFTER_OUT_OF_SYSTEM_ATTRIB		CONSTLIT("debriefAfterOutOfSystem")
#define DESTROY_ON_DECLINE_ATTRIB				CONSTLIT("destroyOnDecline")
#define EXPIRE_TIME_ATTRIB						CONSTLIT("expireTime")
#define FAILURE_AFTER_OUT_OF_SYSTEM_ATTRIB		CONSTLIT("failureAfterOutOfSystem")
#define FORCE_UNDOCK_AFTER_DEBRIEF_ATTRIB		CONSTLIT("forceUndockAfterDebrief")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MAX_APPEARING_ATTRIB					CONSTLIT("maxAppearing")
#define MISSION_ARC_ATTRIB						CONSTLIT("missionArc")
#define NAME_ATTRIB								CONSTLIT("name")
#define NO_DEBRIEF_ATTRIB						CONSTLIT("noDebrief")
#define NO_DECLINE_ATTRIB						CONSTLIT("noDecline")
#define NO_FAILURE_ON_OWNER_DESTROYED_ATTRIB	CONSTLIT("noFailureOnOwnerDestroyed")
#define NO_STATS_ATTRIB							CONSTLIT("noStats")
#define PRIORITY_ATTRIB							CONSTLIT("priority")
#define RECORD_NON_PLAYER_ATTRIB				CONSTLIT("recordNonPlayer")

#define AUTO_ACCEPT_AND_CONTINUE				CONSTLIT("acceptAndContinue")
#define AUTO_ACCEPT_AND_EXIT					CONSTLIT("acceptAndExit")
#define AUTO_ACCEPT_NONE						CONSTLIT("none")

#define FIELD_LEVEL								CONSTLIT("level")
#define FIELD_MAX_LEVEL							CONSTLIT("maxLevel")
#define FIELD_MIN_LEVEL							CONSTLIT("minLevel")
#define FIELD_NAME								CONSTLIT("name")

#define PROPERTY_AUTO_ACCEPT					CONSTLIT("autoAccept")
#define PROPERTY_CAN_BE_DECLINED				CONSTLIT("canBeDeclined")
#define PROPERTY_CAN_BE_DELETED					CONSTLIT("canBeDeleted")
#define PROPERTY_DESTROY_ON_DECLINE				CONSTLIT("destroyOnDecline")
#define PROPERTY_FORCE_UNDOCK_AFTER_DEBRIEF		CONSTLIT("forceUndockAfterDebrief")
#define PROPERTY_HAS_DEBRIEF					CONSTLIT("hasDebrief")
#define PROPERTY_MAX_APPEARING					CONSTLIT("maxAppearing")
#define PROPERTY_PRIORITY						CONSTLIT("priority")
#define PROPERTY_TOTAL_ACCEPTED					CONSTLIT("totalAccepted")
#define PROPERTY_TOTAL_EXISTING					CONSTLIT("totalExisting")

static char *CACHED_EVENTS[CMissionType::evtCount] =
	{
	"CanCreate",
	};

ICCItemPtr CMissionType::AutoAcceptAsItem (EMissionAutoAccept iAutoAccept)

//	AutoAcceptAsItem
//
//	Returns auto accept setting as an item.

	{
	switch (iAutoAccept)
		{
		case EMissionAutoAccept::acceptAndContinue:
			return ICCItemPtr(AUTO_ACCEPT_AND_CONTINUE);

		case EMissionAutoAccept::acceptAndExit:
			return ICCItemPtr(AUTO_ACCEPT_AND_EXIT);

		default:
			return ICCItemPtr(ICCItem::Nil);
		}
	}

bool CMissionType::CanBeCreated (const CMissionList &AllMissions, CSpaceObject *pOwner, ICCItem *pCreateData) const

//	CanBeCreated
//
//	Returns TRUE if we can create this mission.
//
//	NOTE: We don't check to see if the mission is appropriate to the system 
//	level here because we want to allow explicitly created missions. Instead
//	we check for system level inside CUniverse::CreateRandomMission.

	{
	if (!CanBeEncountered())
		return false;

	//	If this is part of a mission arc, then see if we can create it.

	TArray<CMission *> MissionArc;
	if (!m_sArc.IsBlank() 
			&& !AllMissions.CanCreateMissionInArc(m_sArc, m_iArcSequence))
		return false;

	//	Fire <CanCreate>

	if (!FireCanCreate(pOwner, pCreateData))
		return false;

	return true;
	}

bool CMissionType::FireCanCreate (CSpaceObject *pOwner, ICCItem *pCreateData) const

//	FireCanCreate
//
//	Invokes <CanCreate> to see if the mission is available to be created.

	{
	//	If we cannot find CanCreate event, then we assume we can create this
	//	mission.

	SEventHandlerDesc Handler;
	if (!FindCachedEventHandler(evtCanCreate, &Handler))
		return true;

	//	Fire the event

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineDataVar(pCreateData);
	Ctx.DefineSpaceObject(CONSTLIT("aOwnerObj"), pOwner);

	ICCItemPtr pResult = Ctx.RunCode(Handler);
	if (pResult->IsError())
		{
		::kernelDebugLogPattern("[%08x] <CanCreate>: %s", GetUNID(), pResult->GetStringValue());
		return false;
		}
	else
		return !pResult->IsNil();
	}

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

void CMissionType::IncAccepted (void)

//	IncAccepted
//
//	Mission has been accepted by player.

	{
	m_iAccepted++;
	m_dwLastAcceptedOn = GetUniverse().GetTicks();
	}

ALERROR CMissionType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	m_CachedEvents.Init(this, CACHED_EVENTS);

	//	For missions that are part of an arc, we track the first mission in the
	//	arc. We need to do this because both priority and shuffle (order) must
	//	be the same for all missions in an arc (otherwise sorting does not work
	//	correctly).

	if (!m_sArc.IsBlank())
		{
		if (!Ctx.MissionArcRoots.Find(m_sArc, &m_pArcRoot))
			{
			//	Should never happen.
			ASSERT(false);
			m_sArc = NULL_STR;
			m_iArcSequence = -1;
			m_pArcRoot = NULL;
			}
		}

	return NOERROR;
	}

ALERROR CMissionType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	ALERROR error;
	CString sError;

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_iPriority = pDesc->GetAttributeIntegerBounded(PRIORITY_ATTRIB, 0, -1, 1);
	m_iExpireTime = pDesc->GetAttributeIntegerBounded(EXPIRE_TIME_ATTRIB, 1, -1, -1);
	m_iFailIfOutOfSystem = pDesc->GetAttributeIntegerBounded(FAILURE_AFTER_OUT_OF_SYSTEM_ATTRIB, 0, -1, -1);
	m_fRecordNonPlayer = pDesc->GetAttributeBool(RECORD_NON_PLAYER_ATTRIB);
	m_fNoFailureOnOwnerDestroyed = pDesc->GetAttributeBool(NO_FAILURE_ON_OWNER_DESTROYED_ATTRIB);
	m_fNoDebrief = pDesc->GetAttributeBool(NO_DEBRIEF_ATTRIB);
	m_fNoStats = pDesc->GetAttributeBool(NO_STATS_ATTRIB);
	m_fCloseIfOutOfSystem = pDesc->GetAttributeBool(DEBRIEF_AFTER_OUT_OF_SYSTEM_ATTRIB);
	m_fForceUndockAfterDebrief = pDesc->GetAttributeBool(FORCE_UNDOCK_AFTER_DEBRIEF_ATTRIB);
	m_fAllowDelete = pDesc->GetAttributeBool(ALLOW_PLAYER_DELETE_ATTRIB);
	m_fDestroyOnDecline = pDesc->GetAttributeBool(DESTROY_ON_DECLINE_ATTRIB);

	//	Mission creation

	CString sAttrib;
	if (pDesc->FindAttribute(MISSION_ARC_ATTRIB, &sAttrib))
		{
		if (!ParseMissionArc(Ctx, sAttrib, &m_sArc, &m_iArcSequence, &sError))
			return ERR_FAIL;
		}

	if (pDesc->FindAttribute(MAX_APPEARING_ATTRIB, &sAttrib))
		{
		if (error = m_MaxAppearing.LoadFromXML(sAttrib))
			return ComposeLoadError(Ctx, CONSTLIT("Invalid maxAppearing parameter."));
		}

	m_iMaxAppearing = (m_MaxAppearing.IsEmpty() ? -1 : m_MaxAppearing.Roll());
	m_iAccepted = 0;
	m_iExisting = 0;
	m_dwLastAcceptedOn = 0;

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

	//	Auto accept

	if (pDesc->FindAttribute(AUTO_ACCEPT_ATTRIB, &sAttrib))
		{
		m_iAutoAccept = ParseAutoAccept(Ctx, sAttrib);
		if (m_iAutoAccept == EMissionAutoAccept::error)
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Invalid autoAccept: %s"), sAttrib));
		}
	else if (pDesc->GetAttributeBool(NO_DECLINE_ATTRIB))
		m_iAutoAccept = EMissionAutoAccept::acceptAndExit;

	return NOERROR;
	}

ALERROR CMissionType::OnPrepareBindDesign (SDesignLoadCtx &Ctx)

//	OnPrepareBindDesign
//
//	First pass of BindDesign.

	{
	//	If we're part of a mission arc, we figure out which is the root mission
	//	type.

	if (!m_sArc.IsBlank())
		{
		bool bNew;
		CMissionType **ppRoot = Ctx.MissionArcRoots.SetAt(m_sArc, &bNew);
		if (bNew || m_iArcSequence < (*ppRoot)->m_iArcSequence)
			*ppRoot = this;
		}

	//	Done

	return NOERROR;
	}

ICCItemPtr CMissionType::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns a property (or NULL if not found).

	{
	CCodeChain &CC = GetUniverse().GetCC();

	if (strEquals(sProperty, PROPERTY_AUTO_ACCEPT))
		return AutoAcceptAsItem(m_iAutoAccept);

	else if (strEquals(sProperty, PROPERTY_CAN_BE_DECLINED))
		return ICCItemPtr(CanBeDeclined());

	else if (strEquals(sProperty, PROPERTY_CAN_BE_DELETED))
		return ICCItemPtr(CanBeDeleted());

	else if (strEquals(sProperty, PROPERTY_DESTROY_ON_DECLINE))
		return ICCItemPtr(m_fDestroyOnDecline ? true : false);

	else if (strEquals(sProperty, PROPERTY_FORCE_UNDOCK_AFTER_DEBRIEF))
		return ICCItemPtr(ForceUndockAfterDebrief());

	else if (strEquals(sProperty, PROPERTY_HAS_DEBRIEF))
		return ICCItemPtr(HasDebrief());

	else if (strEquals(sProperty, PROPERTY_PRIORITY))
		return ICCItemPtr(GetPriority());

	else if (strEquals(sProperty, PROPERTY_MAX_APPEARING))
		{
		if (m_iMaxAppearing != -1)
			return ICCItemPtr(m_iMaxAppearing);
		else
			return ICCItemPtr(ICCItem::Nil);
		}

	else if (strEquals(sProperty, PROPERTY_TOTAL_ACCEPTED))
		return ICCItemPtr(m_iAccepted);

	else if (strEquals(sProperty, PROPERTY_TOTAL_EXISTING))
		return ICCItemPtr(m_iExisting);

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

	if (Ctx.dwVersion >= 37)
		Ctx.pStream->Read(m_dwLastAcceptedOn);
	}

void CMissionType::OnReinit (void)

//	OnReinit
//
//	Reinitialize

	{
	m_iMaxAppearing = (m_MaxAppearing.IsEmpty() ? -1 : m_MaxAppearing.Roll());
	m_iAccepted = 0;
	m_iExisting = 0;
	m_dwLastAcceptedOn = 0;
	}

void CMissionType::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write mission type data

	{
	pStream->Write(m_iMaxAppearing);
	pStream->Write(m_iAccepted);
	pStream->Write(m_iExisting);
	pStream->Write(m_dwLastAcceptedOn);
	}

EMissionAutoAccept CMissionType::ParseAutoAccept (SDesignLoadCtx &Ctx, const CString &sValue)

//	ParseAutoAccept
//
//	Parses an auto accept value. If invalid, we return EMissionAutoAccept::error

	{
	if (sValue.IsBlank())
		return EMissionAutoAccept::none;
	else if (strEquals(sValue, AUTO_ACCEPT_AND_CONTINUE))
		return EMissionAutoAccept::acceptAndContinue;
	else if (strEquals(sValue, AUTO_ACCEPT_AND_EXIT))
		return EMissionAutoAccept::acceptAndExit;
	else if (strEquals(sValue, AUTO_ACCEPT_NONE))
		return EMissionAutoAccept::none;
	else
		return EMissionAutoAccept::error;
	}

bool CMissionType::ParseMissionArc (SDesignLoadCtx &Ctx, const CString &sValue, CString *retsArc, int *retiSequence, CString *retsError)

//	ParseMissionArc
//
//	Parses an returns a mission arc and sequence.

	{
	if (sValue.IsBlank())
		{
		if (retsArc) *retsArc = NULL_STR;
		if (retiSequence) *retiSequence = -1;
		return true;
		}

	char *pPos = sValue.GetASCIIZPointer();
	char *pStart = pPos;
	while (*pPos != ':' && *pPos != '\0')
		pPos++;

	if (retsArc)
		*retsArc = CString(pStart, (pPos - pStart));

	if (*pPos == '\0')
		{
		if (retiSequence) *retiSequence = -1;
		return true;
		}

	pPos++;
	if (retiSequence)
		*retiSequence = strParseInt(pPos, -1);

	return true;
	}
