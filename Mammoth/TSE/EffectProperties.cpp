//	CEffect.cpp
//
//	CEffect class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CEffect> CEffect::m_PropertyTable = std::array<TPropertyHandler<CEffect>::SPropertyDef, 1> {
		{
		"lifetime",		"ticks",
		[](const CEffect &Obj, const CString &sProperty) { return ICCItemPtr(Obj.GetLifetime()); },
		[](CEffect &Obj, const CString &sProperty, const ICCItem &Value, CString *retsError) { Obj.SetLifetime(Value.GetIntegerValue()); return true; }
		}
	};

ICCItem *CEffect::GetProperty (CCodeChainCtx &Ctx, const CString &sProperty)

//	GetProperty
//
//	Returns a property

	{
	//	First look for a property of the CEffect object.

	ICCItemPtr pValue;
	CEffectParamDesc ParamValue;

	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue->Reference();

	//	Otherwise, see if this is a property of the effect painter.

	else if (m_pPainter && m_pPainter->GetParam(sProperty, &ParamValue))
		{
		pValue = ParamValue.AsItem();
		return pValue->Reference();
		}

	//	Lastly, get the base class properties

	else
		return CSpaceObject::GetProperty(Ctx, sProperty);
	}

bool CEffect::SetProperty (const CString &sProperty, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets a property

	{
	if (pValue == NULL)
		return SetProperty(sProperty, ICCItemPtr(ICCItem::Nil), retsError);

	//	Try CEffect object properties

	if (m_PropertyTable.SetProperty(*this, sProperty, *pValue, retsError))
		return true;

	//	Next try painter parameters

	if (m_pPainter)
		{
		CCreatePainterCtx ParamCtx;
		CEffectParamDesc ParamValue(pValue);

		if (m_pPainter->SetParam(ParamCtx, sProperty, ParamValue))
			return true;
		}

	//	Lastly, try base class properties

	return CSpaceObject::SetProperty(sProperty, pValue, retsError);
	}
