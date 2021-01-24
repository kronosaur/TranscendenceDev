//	MissileProperties.cpp
//
//	CMissile class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_LIFE_LEFT						CONSTLIT("lifeLeft")
#define PROPERTY_ROTATION						CONSTLIT("rotation")
#define PROPERTY_SOURCE							CONSTLIT("source")
#define PROPERTY_TARGET							CONSTLIT("target")

TPropertyHandler<CMissile> CMissile::m_PropertyTable = std::array<TPropertyHandler<CMissile>::SPropertyDef, 5> {{
		{
		"lifeLeft",			"ticks",
		[](const CMissile &Obj, const CString &sProperty) 
			{
			return (Obj.m_fDestroyOnAnimationDone ? ICCItemPtr(0) : ICCItemPtr(Obj.m_iLifeLeft));
			},
		NULL,
		},

		{
		"rotation",			"degrees",
		[](const CMissile &Obj, const CString &sProperty) { return ICCItemPtr(Obj.GetRotation()); },
		NULL,
		},

		{
		"source",			"damage source",
		[](const CMissile &Obj, const CString &sProperty) { return ICCItemPtr(::CreateDamageSource(Obj.GetUniverse().GetCC(), Obj.m_Source)); },
		NULL,
		},

		{
		"target",			"target object",
		[](const CMissile &Obj, const CString &sProperty) { return ICCItemPtr(::CreateObjPointer(Obj.GetUniverse().GetCC(), Obj.m_pTarget)); },
		NULL,
		},

		{
		"tracking",			"missile is tracking",
		[](const CMissile &Obj, const CString &sProperty) { return ICCItemPtr(Obj.m_pDesc->IsTracking()); },
		NULL,
		}
	}};

ICCItemPtr CMissile::OnFindProperty (CCodeChainCtx &CCX, const CString &sProperty) const

//	GetProperty
//
//	Returns a property

	{
	//	First look for a property in our table.

	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Check the weapon fire descriptor

	else if (ICCItem *pValue = m_pDesc->FindProperty(sProperty))
		{
		return ICCItemPtr(pValue);
		}

	//	Otherwise, not found

	else
		return ICCItemPtr();
	}

bool CMissile::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	CCodeChain &CC = GetUniverse().GetCC();

    if (strEquals(sName, PROPERTY_LIFE_LEFT))
        {
        if (!m_fDestroyOnAnimationDone)
            m_iLifeLeft = Max(0, pValue->GetIntegerValue());
        return true;
        }

	else if (strEquals(sName, PROPERTY_ROTATION))
		{
		m_iRotation = AngleMod(pValue->GetIntegerValue());
		return true;
		}

	else if (strEquals(sName, PROPERTY_SOURCE))
		{
		//	NOTE: CDamageSource handles the case where any objects are destroyed.
		//	so we don't need to check anything here.

		m_Source = ::GetDamageSourceArg(CC, pValue);
		return true;
		}

	else if (strEquals(sName, PROPERTY_TARGET))
		{
		m_pTarget = ::CreateObjFromItem(pValue, CCUTIL_FLAG_CHECK_DESTROYED);
		return true;
		}

	else
		return CSpaceObject::SetProperty(sName, pValue, retsError);
	}
