//	MissionProperties.cpp
//
//	CMission class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_ACCEPTED_ON					CONSTLIT("acceptedOn")
#define PROPERTY_COMPLETED_ON					CONSTLIT("completedOn")
#define PROPERTY_DEBRIEFER_ID					CONSTLIT("debrieferID")
#define PROPERTY_IN_PROGRESS					CONSTLIT("inProgress")
#define PROPERTY_IS_ACTIVE						CONSTLIT("isActive")
#define PROPERTY_IS_COMPLETED					CONSTLIT("isCompleted")
#define PROPERTY_IS_DEBRIEFED					CONSTLIT("isDebriefed")
#define PROPERTY_IS_DECLINED					CONSTLIT("isDeclined")
#define PROPERTY_IS_FAILURE						CONSTLIT("isFailure")
#define PROPERTY_IS_INTRO_SHOWN					CONSTLIT("isIntroShown")
#define PROPERTY_IS_OPEN						CONSTLIT("isOpen")
#define PROPERTY_IS_RECORDED					CONSTLIT("isRecorded")
#define PROPERTY_IS_SUCCESS						CONSTLIT("isSuccess")
#define PROPERTY_IS_UNAVAILABLE					CONSTLIT("isUnavailable")
#define PROPERTY_NAME							CONSTLIT("name")
#define PROPERTY_NODE_ID						CONSTLIT("nodeID")
#define PROPERTY_OWNER_ID						CONSTLIT("ownerID")
#define PROPERTY_SUMMARY						CONSTLIT("summary")
#define PROPERTY_UNID							CONSTLIT("unid")

#define REASON_DEBRIEFED						CONSTLIT("debriefed")

TPropertyHandler<CMission> CMission::m_PropertyTable = std::array<TPropertyHandler<CMission>::SPropertyDef, 20> {{
		{
		"acceptedOn",		"ticks",
		[](const CMission &Obj, const CString &sProperty) 
			{
			return (Obj.m_fAcceptedByPlayer ? ICCItemPtr(Obj.m_dwAcceptedOn) : ICCItemPtr(ICCItem::Nil));
			},
		NULL,
		},

		{
		"arcName",			"Name of arc",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_sArcTitle); },
		NULL,
		},

		{
		"completedOn",		"ticks",
		[](const CMission &Obj, const CString &sProperty) 
			{
			return (Obj.IsCompleted() ? ICCItemPtr(Obj.m_dwCompletedOn) : ICCItemPtr(ICCItem::Nil));
			},
		NULL,
		},

		{
		"debrieferID",		"objID",
		[](const CMission &Obj, const CString &sProperty) 
			{
			if (Obj.m_pDebriefer.GetID() != OBJID_NULL)
				return ICCItemPtr(Obj.m_pDebriefer.GetID());
			else if (Obj.m_pOwner.GetID() != OBJID_NULL)
				return ICCItemPtr(Obj.m_pOwner.GetID());
			else
				return ICCItemPtr(ICCItem::Nil);
			},
		NULL,
		},

		{
		"inProgress",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsActive() && !Obj.IsCompleted()); },
		NULL,
		},

		{
		"isActive",			"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsActive()); },
		NULL,
		},

		{
		"isCompleted",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsCompleted()); },
		NULL,
		},

		{
		"isDebriefed",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_fDebriefed ? true : false); },
		NULL,
		},

		{
		"isDeclined",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_fDeclined ? true : false); },
		NULL,
		},

		{
		"isFailure",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsFailure()); },
		NULL,
		},

		{
		"isIntroShown",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_fIntroShown ? true : false); },
		NULL,
		},

		{
		"isOpen",			"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_iStatus == Status::open); },
		NULL,
		},

		{
		"isRecorded",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsRecorded()); },
		NULL,
		},

		{
		"isSuccess",		"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsSuccess()); },
		NULL,
		},

		{
		"isUnavailable",	"True|Nil",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.IsUnavailable()); },
		NULL,
		},

		{
		"missionNumber",	"Ordinal of mission of type",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_iMissionNumber); },
		NULL,
		},

		{
		"name",				"Name of mission",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_sTitle); },
		NULL,
		},

		{
		"nodeID",			"nodeID",
		[](const CMission &Obj, const CString &sProperty)
			{
			return (Obj.m_sNodeID.IsBlank() ? ICCItemPtr(ICCItem::Nil) : ICCItemPtr(Obj.m_sNodeID));
			},
		NULL,
		},

		{
		"ownerID",			"objID",
		[](const CMission &Obj, const CString &sProperty)
			{
			if (Obj.m_pOwner.GetID() == OBJID_NULL)
				return ICCItemPtr(ICCItem::Nil);
			else
				return ICCItemPtr(Obj.m_pOwner.GetID());
			},
		NULL,
		},

		{
		"summary",			"Summary of mission",
		[](const CMission &Obj, const CString &sProperty) {	return ICCItemPtr(Obj.m_sInstructions); },
		NULL,
		}
	}};

ICCItemPtr CMission::OnFindProperty (CCodeChainCtx &CCX, const CString &sProperty) const

//	OnFindProperty
//
//	Returns a property, if found.

	{
	//	First look for a property in our table.

	ICCItemPtr pValue;

	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Otherwise, not found

	else
		return ICCItemPtr();
	}

bool CMission::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	if (strEquals(sName, PROPERTY_IS_DEBRIEFED))
		{
		if (m_iStatus == Status::playerSuccess || m_iStatus == Status::playerFailure)
			{
			if (!pValue->IsNil())
				{
				if (!m_fDebriefed)
					{
					m_fDebriefed = true;

					FireOnSetPlayerTarget(REASON_DEBRIEFED);
					CloseMission();
					}
				}
			else
				m_fDebriefed = false;
			}
		}

	else if (strEquals(sName, PROPERTY_DEBRIEFER_ID))
		{
		if (pValue->IsNil())
			m_pDebriefer.SetID(OBJID_NULL);
		else
			m_pDebriefer.SetID(pValue->GetIntegerValue());
		}

	else if (strEquals(sName, PROPERTY_IS_DECLINED))
		{
		if (m_iStatus == Status::open)
			m_fDeclined = !pValue->IsNil();
		}

	else if (strEquals(sName, PROPERTY_IS_INTRO_SHOWN))
		{
		if (m_iStatus == Status::open)
			m_fIntroShown = !pValue->IsNil();
		}

	else if (strEquals(sName, PROPERTY_NAME))
		{
		if (IsActive())
			m_sTitle = pValue->GetStringValue();
		}

	else if (strEquals(sName, PROPERTY_SUMMARY))
		{
		if (IsActive())
			m_sInstructions = pValue->GetStringValue();
		}

	else
		return CSpaceObject::SetProperty(sName, pValue, retsError);

	return true;
	}

