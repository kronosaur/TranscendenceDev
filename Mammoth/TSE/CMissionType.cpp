//	CMissionType.cpp
//
//	CMissionType class

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define ALLOW_ARC_MISSIONS_ATTRIB				CONSTLIT("allowArcMissions")
#define ALLOW_PLAYER_DELETE_ATTRIB				CONSTLIT("allowPlayerDelete")
#define AUTO_ACCEPT_ATTRIB						CONSTLIT("autoAccept")
#define CREATE_CRITERIA_ATTRIB					CONSTLIT("createCriteria")
#define DEBRIEF_AFTER_OUT_OF_SYSTEM_ATTRIB		CONSTLIT("debriefAfterOutOfSystem")
#define DESTROY_ON_DECLINE_ATTRIB				CONSTLIT("destroyOnDecline")
#define EXPIRE_TIME_ATTRIB						CONSTLIT("expireTime")
#define FAILURE_AFTER_OUT_OF_SYSTEM_ATTRIB		CONSTLIT("failureAfterOutOfSystem")
#define FAILURE_ON_RETURN_TO_SYSTEM_ATTRIB		CONSTLIT("failureOnReturnToSystem")
#define FORCE_UNDOCK_AFTER_DEBRIEF_ATTRIB		CONSTLIT("forceUndockAfterDebrief")
#define IGNORE_STATION_LIMIT_ATTRIB				CONSTLIT("ignoreOwnerLimit")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MAX_APPEARING_ATTRIB					CONSTLIT("maxAppearing")
#define MISSION_ARC_ATTRIB						CONSTLIT("missionArc")
#define NAME_ATTRIB								CONSTLIT("name")
#define NO_DEBRIEF_ATTRIB						CONSTLIT("noDebrief")
#define NO_DECLINE_ATTRIB						CONSTLIT("noDecline")
#define NO_FAILURE_ON_OWNER_DESTROYED_ATTRIB	CONSTLIT("noFailureOnOwnerDestroyed")
#define NO_IN_PROGRESS_ATTRIB					CONSTLIT("noInProgress")
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

#define PROPERTY_ARC							CONSTLIT("arc")
#define PROPERTY_ARC_SEQUENCE					CONSTLIT("arcSequence")
#define PROPERTY_ARC_STATUS						CONSTLIT("arcStatus")
#define PROPERTY_AUTO_ACCEPT					CONSTLIT("autoAccept")
#define PROPERTY_CAN_BE_DECLINED				CONSTLIT("canBeDeclined")
#define PROPERTY_CAN_BE_DELETED					CONSTLIT("canBeDeleted")
#define PROPERTY_DESTROY_ON_DECLINE				CONSTLIT("destroyOnDecline")
#define PROPERTY_EXPIRE_TIME					CONSTLIT("expireTime")
#define PROPERTY_FORCE_UNDOCK_AFTER_DEBRIEF		CONSTLIT("forceUndockAfterDebrief")
#define PROPERTY_HAS_DEBRIEF					CONSTLIT("hasDebrief")
#define PROPERTY_HAS_IN_PROGRESS				CONSTLIT("hasInProgress")
#define PROPERTY_IGNORE_STATION_LIMIT			CONSTLIT("ignoreOwnerLimit")
#define PROPERTY_IMAGE_DESC						CONSTLIT("imageDesc")
#define PROPERTY_MAX_APPEARING					CONSTLIT("maxAppearing")
#define PROPERTY_PRIORITY						CONSTLIT("priority")
#define PROPERTY_TOTAL_ACCEPTED					CONSTLIT("totalAccepted")
#define PROPERTY_TOTAL_EXISTING					CONSTLIT("totalExisting")

static const char *CACHED_EVENTS[CMissionType::evtCount] =
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

CMissionType::SArcStatus CMissionType::CalcArcStatus (void) const

//	CalcArcStatus
//
//	Computes the progress on this mission arc.

	{
	struct SEntry
		{
		TArray<const CMission *> Missions;
		};

	CUniverse &Universe = GetUniverse();
	SArcStatus Status;

	if (m_sArc.IsBlank())
		return Status;

	//	If there are multiple missions with the same title, then we count them
	//	as the same for "chapter" purposes.

	TSortMap<CString, TSortMap<const CMissionType *, SEntry>> ChapterList;

	int iMissionTypeCount = Universe.GetDesignCollection().GetCount(designMissionType);
	for (int i = 0; i < iMissionTypeCount; i++)
		{
		const CMissionType *pMissionType = CMissionType::AsType(Universe.GetDesignCollection().GetEntry(designMissionType, i));
		if (!strEquals(pMissionType->m_sArc, m_sArc))
			continue;

		CString sTitle = pMissionType->GetTitle();
		auto pList = ChapterList.SetAt(sTitle);
		auto pEntry = pList->SetAt(pMissionType);
		}

	const CMissionList &Missions = Universe.GetMissions();
	for (int i = 0; i < Missions.GetCount(); i++)
		{
		const CMission &Mission = Missions[i];
		if (!strEquals(Mission.GetArc(), m_sArc))
			continue;

		const CMissionType &MissionType = Mission.GetMissionType();
		auto pList = ChapterList.GetAt(MissionType.GetTitle());
		if (!pList)
			continue;

		auto pEntry = pList->GetAt(&MissionType);
		pEntry->Missions.Insert(&Mission);
		}

	//	The total number of chapters in the mission arc is equal to the total
	//	number of unique mission titles.

	Status.iTotal = ChapterList.GetCount();

	//	Find the latest mission that's been created in this arc.

	const CMission *pLatest = Missions.FindByArc(m_sArc);

	//	Now loop over all chapters and count the number that have been completed.

	for (int i = 0; i < ChapterList.GetCount(); i++)
		{
		const auto &MissionTypes = ChapterList[i];

		//	First figure out if all the mission types in this chapter are before
		//	the latest.

		bool bBeforeLatest;
		if (pLatest)
			{
			bBeforeLatest = true;
			for (int j = 0; j < MissionTypes.GetCount(); j++)
				{
				if (MissionTypes.GetKey(j)->GetArcSequence() >= pLatest->GetArcSequence())
					{
					bBeforeLatest = false;
					break;
					}
				}
			}
		else
			bBeforeLatest = false;

		//	Now see if any/all of the missions in this chapter have been 
		//	completed (recorded).

		bool bAtLeastOneNotCompleted = false;
		bool bAtLeastOneCompleted = false;
		for (int j = 0; j < MissionTypes.GetCount(); j++)
			{
			const auto &Entry = MissionTypes[j];
			for (int k = 0; k < Entry.Missions.GetCount(); k++)
				if (Entry.Missions[k]->IsRecorded())
					bAtLeastOneCompleted = true;
				else
					bAtLeastOneNotCompleted = true;
			}

		//	If all missions have been completed, then we count this chapter as
		//	being completed.

		if (bAtLeastOneCompleted && !bAtLeastOneNotCompleted)
			Status.iCompleted++;

		//	If we're already past this chapter and if we've completed any of the
		//	missions, then we count it as completed.

		else if (bAtLeastOneCompleted && bBeforeLatest)
			Status.iCompleted++;

		//	Otherwise, if we're at or ahead of the latest mission, then we count
		//	this as a chapter that's still left.

		else if (!bBeforeLatest)
			Status.iLeft++;

		//	Otherwise, this chapter was skipped

		else
			Status.iSkipped++;
		}

	//	Done

	return Status;
	}

bool CMissionType::CanBeCreated (const CMissionList &AllMissions, SCreateCtx &CreateCtx) const

//	CanBeCreated
//
//	Returns TRUE if we can create this mission.

	{
	if (!CanBeEncountered())
		return false;

	//	Skip if this mission is not appropriate for this system level.

	if (!CreateCtx.bNoSystemLevelCheck)
		{
		int iMinLevel, iMaxLevel;
		GetLevel(&iMinLevel, &iMaxLevel);
		if (CreateCtx.iLevel < iMinLevel || CreateCtx.iLevel > iMaxLevel)
			return false;
		}

	//	If we have create criteria, then make sure we match

	CMissionCriteria CreateCriteria;
	if (!m_sCreateCriteria.IsBlank())
		{
		CreateCriteria = CMissionCriteria(m_sCreateCriteria);

		if (!AllMissions.Matches(CreateCtx.pOwner, CreateCriteria))
			return false;
		}

	//	If this is part of a mission arc, then see if we can create it.

	if (!CreateCtx.bNoMissionArcCheck)
		{
		if (!m_sArc.IsBlank() 
				&& !AllMissions.CanCreateMissionInArc(*this, CreateCtx.pOwner, CreateCriteria))
			return false;
		}

	//	Fire <CanCreate>

	if (!FireCanCreate(CreateCtx.pOwner, CreateCtx.pCreateData))
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

const CObjectImageArray &CMissionType::GetImage (void) const

//	GetImage
//
//	Returns mission image.

	{
	if (!m_Image.IsEmpty())
		return m_Image;
	else if (GetInheritFrom())
		return GetInheritFrom()->GetTypeSimpleImage();
	else
		return CObjectImageArray::Null();
	}

CString CMissionType::GetTitle (void) const

//	GetTitle
//
//	Returns the mission title.

	{
	CString sTitle;
	if (TranslateText(CONSTLIT("Name"), NULL, &sTitle))
		return sTitle;

	return GetName();
	}

void CMissionType::IncAccepted (void)

//	IncAccepted
//
//	Mission has been accepted by player.

	{
	m_iAccepted++;
	m_dwLastAcceptedOn = GetUniverse().GetTicks();
	}

void CMissionType::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Returns flags to determine how we merge from inherited types.

	{
	//	We know how to handle these tags through the inheritance hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(IMAGE_TAG), CXMLElement::MERGE_OVERRIDE);
	}

ALERROR CMissionType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	if (ALERROR error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

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
	m_iFailOnReturnToSystem = pDesc->GetAttributeIntegerBounded(FAILURE_ON_RETURN_TO_SYSTEM_ATTRIB, 0, -1, -1);
	m_fRecordNonPlayer = pDesc->GetAttributeBool(RECORD_NON_PLAYER_ATTRIB);
	m_fNoFailureOnOwnerDestroyed = pDesc->GetAttributeBool(NO_FAILURE_ON_OWNER_DESTROYED_ATTRIB);
	m_fNoDebrief = pDesc->GetAttributeBool(NO_DEBRIEF_ATTRIB);
	m_fNoInProgress = pDesc->GetAttributeBool(NO_IN_PROGRESS_ATTRIB);
	m_fNoStats = pDesc->GetAttributeBool(NO_STATS_ATTRIB);
	m_fCloseIfOutOfSystem = pDesc->GetAttributeBool(DEBRIEF_AFTER_OUT_OF_SYSTEM_ATTRIB);
	m_fForceUndockAfterDebrief = pDesc->GetAttributeBool(FORCE_UNDOCK_AFTER_DEBRIEF_ATTRIB);
	m_fAllowDelete = pDesc->GetAttributeBool(ALLOW_PLAYER_DELETE_ATTRIB);
	m_fDestroyOnDecline = pDesc->GetAttributeBool(DESTROY_ON_DECLINE_ATTRIB);
	m_fIgnoreStationLimit = pDesc->GetAttributeBool(IGNORE_STATION_LIMIT_ATTRIB);

	//	Image

	if (const CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG))
		{
		if (error = m_Image.InitFromXML(Ctx, *pImage))
			return ComposeLoadError(Ctx, CONSTLIT("Unable to load image"));
		}

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

	m_sCreateCriteria = pDesc->GetAttribute(CREATE_CRITERIA_ATTRIB);
	m_bAllowOtherArcMissions = pDesc->GetAttribute(ALLOW_ARC_MISSIONS_ATTRIB);

	m_iMaxAppearing = (m_MaxAppearing.IsEmpty() ? -1 : m_MaxAppearing.Roll());
	m_iAccepted = 0;
	m_iExisting = 0;
	m_dwLastAcceptedOn = 0;

	//	Level

	if (pDesc->FindAttribute(LEVEL_ATTRIB, &sAttrib))
		{
		//	Parse this value

		const char *pPos = sAttrib.GetASCIIZPointer();
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

	if (strEquals(sProperty, PROPERTY_ARC))
		return (m_sArc.IsBlank() ? ICCItemPtr::Nil() : ICCItemPtr(m_sArc));

	else if (strEquals(sProperty, PROPERTY_ARC_SEQUENCE))
		return (m_sArc.IsBlank() ? ICCItemPtr::Nil() : ICCItemPtr(m_iArcSequence));

	else if (strEquals(sProperty, PROPERTY_ARC_STATUS))
		{
		if (m_sArc.IsBlank())
			return ICCItemPtr::Nil();
		else
			{
			SArcStatus Status = CalcArcStatus();

			ICCItemPtr pResult(ICCItem::SymbolTable);
			pResult->SetIntegerAt(CONSTLIT("total"), Status.iTotal);
			pResult->SetIntegerAt(CONSTLIT("completed"), Status.iCompleted);
			pResult->SetIntegerAt(CONSTLIT("left"), Status.iLeft);

			return pResult;
			}
		}

	else if (strEquals(sProperty, PROPERTY_AUTO_ACCEPT))
		return AutoAcceptAsItem(m_iAutoAccept);

	else if (strEquals(sProperty, PROPERTY_CAN_BE_DECLINED))
		return ICCItemPtr(CanBeDeclined());

	else if (strEquals(sProperty, PROPERTY_CAN_BE_DELETED))
		return ICCItemPtr(CanBeDeleted());

	else if (strEquals(sProperty, PROPERTY_DESTROY_ON_DECLINE))
		return ICCItemPtr(m_fDestroyOnDecline ? true : false);

	else if (strEquals(sProperty, PROPERTY_EXPIRE_TIME))
		return (m_iExpireTime == -1 ? ICCItemPtr::Nil() : ICCItemPtr(m_iExpireTime));

	else if (strEquals(sProperty, PROPERTY_FORCE_UNDOCK_AFTER_DEBRIEF))
		return ICCItemPtr(ForceUndockAfterDebrief());

	else if (strEquals(sProperty, PROPERTY_HAS_DEBRIEF))
		return ICCItemPtr(HasDebrief());

	else if (strEquals(sProperty, PROPERTY_HAS_IN_PROGRESS))
		return ICCItemPtr(HasInProgress());

	else if (strEquals(sProperty, PROPERTY_IGNORE_STATION_LIMIT))
		return ICCItemPtr(m_fIgnoreStationLimit ? true : false);

	else if (strEquals(sProperty, PROPERTY_IMAGE_DESC))
		return ICCItemPtr(CreateListFromImage(CC, GetImage()));

	else if (strEquals(sProperty, PROPERTY_PRIORITY))
		return ICCItemPtr(GetPriority());

	else if (strEquals(sProperty, PROPERTY_MAX_APPEARING))
		{
		if (m_iMaxAppearing == -1)
			return ICCItemPtr(ICCItem::Nil);
		else if (m_MaxAppearing.IsConstant())
			return ICCItemPtr(m_iMaxAppearing);
		else
			return ICCItemPtr(strPatternSubst(CONSTLIT("%d-%d"), m_MaxAppearing.GetMinValue(), m_MaxAppearing.GetMaxValue()));
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

	const char *pPos = sValue.GetASCIIZPointer();
	const char *pStart = pPos;
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
