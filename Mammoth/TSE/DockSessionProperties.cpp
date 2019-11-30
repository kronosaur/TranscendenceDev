//	CDockSession.cpp
//
//	CDockSession class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CDockSession> CDockSession::m_PropertyTable = std::array<TPropertyHandler<CDockSession>::SPropertyDef, 1> {
	{	"foo",
		"Test.",
		[](const CDockSession &DockSession, const CString &sProperty) { return ICCItemPtr(ICCItem::Nil); },
		[](CDockSession &DockSession, const CString &sProperty, const ICCItem &Value, CString *retsError) { return false; }
		},
	};

ICCItemPtr CDockSession::GetProperty (const CString &sProperty) const

//	GetProperty
//
//	Returns a property. We guarantee that the result is non-null.

	{
	ICCItemPtr pValue;

	//	First look for built-in properties

	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Next check to see if the screen handles this property.

	else if (pValue = GetUI().GetProperty(sProperty))
		return pValue;

	//	Now look up user-defined properties

	else if (CDesignType *pType = m_DockFrames.GetCurrent().pResolvedRoot)
		{
		CCodeChainCtx CCX(GetUniverse());

		EPropertyType iType;
		ICCItemPtr pResult = pType->GetProperty(CCX, sProperty, &iType);

		//	If the property is a variant or data, then we need to look at screen
		//	data.

		if (iType == EPropertyType::propVariant || iType == EPropertyType::propData)
			return GetData(sProperty);

		//	If this is a dynamic property, we need to evaluate.

		else if (iType == EPropertyType::propDynamicData)
			{
			CCX.SetScreen(m_pDockScreenUI);
			CCX.SaveAndDefineType(pType->GetUNID());
			CCX.SaveAndDefineSourceVar(m_DockFrames.GetCurrent().pLocation);
			CCX.SaveAndDefineDataVar(m_DockFrames.GetCurrent().pInitialData);

			ICCItemPtr pValue = CCX.RunCode(pResult);
			return pValue;
			}

		//	Otherwise we have a valid property.

		else
			return pResult;
		}

	//	Not found

	else
		return ICCItemPtr(ICCItem::Nil);
	}

void CDockSession::InitCustomProperties (void)

//	InitCustomProperties
//
//	Initializes the custom properties for the top of the stack.

	{
	if (!InSession())
		return;

	//	If necessary, create the stored data block

	SDockFrame &Frame = m_DockFrames.GetCurrent();
	if (Frame.pResolvedRoot == NULL)
		return;

	//	Let the type initialize the data because it needs to traverse the
	//	inheritance hierarchy.

	InitCustomProperties(*Frame.pResolvedRoot, Frame);
	}

void CDockSession::InitCustomProperties (const CDesignType &Type, const SDockFrame &Frame)

//	InitCustomProperties
//
//	Initializes any properties defined by the given type. We recursively
//	initialize any properties from parent types.

	{
	//	pResolvedRoot is not always equal to Type because we might be 
	//	initializing an ancestor. But we can always rely on it being non-null.

	ASSERT(Frame.pResolvedRoot);

	//	Initialize our parent, if necessary

	if (CDesignType *pParent = Type.GetInheritFrom())
		InitCustomProperties(*pParent, Frame);

	//	Loop over all properties and initialize as appropriate.

	const CDesignPropertyDefinitions *pProperties = Type.GetPropertyDefs();
	if (pProperties == NULL)
		return;

	CCodeChainCtx CCX(GetUniverse());
	CCX.SetScreen(m_pDockScreenUI);
	CCX.SaveAndDefineType(Frame.pResolvedRoot->GetUNID());
	CCX.SaveAndDefineSourceVar(Frame.pLocation);
	CCX.SaveAndDefineDataVar(Frame.pInitialData);

	for (int i = 0; i < pProperties->GetCount(); i++)
		{
		switch (pProperties->GetType(i))
			{
			case EPropertyType::propData:
			case EPropertyType::propVariant:
				{
				//	Skip if no code.

				ICCItemPtr pCode = pProperties->GetCode(i);
				if (!pCode)
					continue;

				//	gData fields passed in to a screen override the initial 
				//	value of a property, so if such a field is set, we skip
				//	initialization here.

				else if (Frame.pInitialData 
						&& Frame.pInitialData->GetElement(pProperties->GetName(i)))
					continue;

				//	Otherwise, initialize variable

				else
					{
					ICCItemPtr pResult = CCX.RunCode(pCode);
					if (pResult->IsError())
						{
						GetUniverse().LogOutput(strPatternSubst("ERROR: Evaluating property %s: %s", pProperties->GetName(i), pResult->GetStringValue()));
						continue;
						}

					else if (!pResult->IsNil())
						SetData(pProperties->GetName(i), pResult);
					}

				break;
				}
			}
		}
	}

bool CDockSession::SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError)

//	SetProperty
//
//	Sets a property. If we fail, we return FALSE and retsError is the error.

	{
	CString sError;

	//	First look for built-in properies

	if (m_PropertyTable.SetProperty(*this, sProperty, Value, &sError))
		return true;
	else if (!sError.IsBlank())
		{
		if (retsError) *retsError = sError;
		return false;
		}

	//	Next check to see if the screen handles the property.

	else if (GetUI().SetProperty(sProperty, Value))
		return true;

	//	Look up user-defined properties.

	else if (CDesignType *pType = m_DockFrames.GetCurrent().pResolvedRoot)
		{
		ICCItemPtr pDummy;
		EPropertyType iType;
		if (!pType->FindCustomProperty(sProperty, pDummy, &iType))
			return false;

		switch (iType)
			{
			case EPropertyType::propGlobal:
				pType->SetGlobalData(sProperty, &Value);
				return true;

			case EPropertyType::propData:
				SetData(sProperty, &Value);
				return true;

			default:
				{
				if (retsError)
					*retsError = strPatternSubst(CONSTLIT("Cannot set read-only property: %s"), sProperty);
				return false;
				}
			}
		}

	//	Not found

	else
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Unknown property: %s"), sProperty);

		return false;
		}
	}
