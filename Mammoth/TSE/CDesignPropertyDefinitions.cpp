//	CDesignPropertyDefintions.cpp
//
//	CDesignPropertyDefintions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.
//
//	This class defines custom properties for design types. Different types of
//	properties have different behavior:
//
//	DEFINITION: A definition property is an immutable constant that is computed
//		at bind time. There is no need to store this value in a save file; 
//		instead, we store the value in this class.
//
//		When we ask a type for a definition, we check up the inheritance 
//		hierarchy for the value.

#include "PreComp.h"

#define CONSTANT_TAG							CONSTLIT("Constant")
#define DATA_TAG								CONSTLIT("Data")
#define DEFINITION_TAG							CONSTLIT("Definition")
#define GLOBAL_TAG								CONSTLIT("Global")
#define VARIANT_TAG								CONSTLIT("Variant")

#define ID_ATTRIB								CONSTLIT("id")

ALERROR CDesignPropertyDefinitions::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Evaluate any properties that need to be evaluated at bind-time.

	{
	for (int i = 0; i < m_Defs.GetCount(); i++)
		if (m_Defs[i].iType == EPropertyType::propDefinitionEvalNeeded)
			{
			CCodeChainCtx CCCtx(Ctx.GetUniverse());

			ICCItemPtr pResult = CCCtx.RunCode(m_Defs[i].pCode);
			if (pResult->IsError())
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Property %s: %s"), m_Defs.GetKey(i), pResult->GetStringValue());
				return ERR_FAIL;
				}

			m_Defs[i].iType = EPropertyType::propDefinition;
			m_Defs[i].pCode = pResult;
			}

	return NOERROR;
	}

bool CDesignPropertyDefinitions::Find (const CString &sProperty, ICCItemPtr &pData, EPropertyType *retiType) const

//	Find
//
//	Returns the property, if it exists.
//	
//	NOTE: We guarantee that if we return TRUE, pData will be non-null (though it
//	might be Nil).

	{
	const SDef *pDef = m_Defs.GetAt(sProperty);
	if (pDef == NULL)
		return false;

	if (pDef->pCode)
		pData = pDef->pCode;
	else
		pData = ICCItemPtr(ICCItem::Nil);

	if (retiType)
		*retiType = pDef->iType;

	return true;
	}

ALERROR CDesignPropertyDefinitions::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	We support the following kinds of property definitions:
//
//	<Definition...>: Evaluate at load time, no need to save
//	<Constant...>: Evaluate at game-create time, store in type
//	<Global...>: Evaluate at game-create time, store in type, allow changes
//	<Variant...>: Evaluate at object-create time, store in object
//	<Data...>: Evaluate at object-create time, store in object, allow changes

	{
	for (int i = 0; i < Desc.GetContentElementCount(); i++)
		{
		const CXMLElement &Item = *Desc.GetContentElement(i);

		//	Figure out the type

		EPropertyType iType;
		if (strEquals(Item.GetTag(), CONSTANT_TAG))
			iType = EPropertyType::propConstant;
		else if (strEquals(Item.GetTag(), DATA_TAG))
			iType = EPropertyType::propData;
		else if (strEquals(Item.GetTag(), DEFINITION_TAG))
			iType = EPropertyType::propDefinition;
		else if (strEquals(Item.GetTag(), GLOBAL_TAG))
			iType = EPropertyType::propGlobal;
		else if (strEquals(Item.GetTag(), VARIANT_TAG))
			iType = EPropertyType::propVariant;
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown property type: %s."), Item.GetTag());
			return ERR_FAIL;
			}

		//	Get the ID

		CString sID = Item.GetAttribute(ID_ATTRIB);
		if (sID.IsBlank())
			{
			Ctx.sError = CONSTLIT("Property must have an ID.");
			return ERR_FAIL;
			}

		//	Parse the code

		ICCItemPtr pCode;
		const CString &sData = Item.GetContentText(0);
		if (!sData.IsBlank())
			{
			CCodeChain::SLinkOptions Options;
			Options.bNullIfEmpty = true;

			pCode = ICCItemPtr(CCodeChain::Link(sData, Options));

			//	If no code, then nothing to do.

			if (!pCode)
				;

			//	If we got an error parsing, then we need to report it.

			else if (pCode->IsError())
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Property %s: %s"), sID, pCode->GetStringValue());
				return ERR_FAIL;
				}

			//	If the code is constant, and we don't expect it to change, then
			//	we turn it into a definition, so that we don't have to store it.

			else if (pCode->IsConstant())
				{
				if (iType == EPropertyType::propConstant || iType == EPropertyType::propVariant)
					iType = EPropertyType::propDefinition;
				}

			//	If the code is a definition, but not a constant, then we 
			//	need to evaluate it at bind time.

			else if (iType == EPropertyType::propDefinition)
				{
				iType = EPropertyType::propDefinitionEvalNeeded;
				}
			}

		//	Create an entry

		bool bNew;
		SDef *pNewDef = m_Defs.SetAt(sID, &bNew);
		if (!bNew)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Duplicate property %s"), sID);
			return ERR_FAIL;
			}

		pNewDef->iType = iType;
		pNewDef->pCode = pCode;
		}

	return NOERROR;
	}

void CDesignPropertyDefinitions::InitObjectData (CUniverse &Universe, CSpaceObject &Obj, CAttributeDataBlock &Dest) const

//	InitObjectData
//
//	Initializes object-level properties.

	{
	CCodeChainCtx CCCtx(Universe);

	for (int i = 0; i < m_Defs.GetCount(); i++)
		{
		switch (m_Defs[i].iType)
			{
			case EPropertyType::propData:
			case EPropertyType::propVariant:
				{
				if (m_Defs[i].pCode)
					{
					ICCItemPtr pResult = CCCtx.RunCode(m_Defs[i].pCode);
					if (pResult->IsError())
						{
						::kernelDebugLogPattern("ERROR: Evaluating property %s: %s", m_Defs.GetKey(i), pResult->GetStringValue());
						continue;
						}

					Dest.SetData(m_Defs.GetKey(i), pResult);
					}

				break;
				}
			}
		}
	}

void CDesignPropertyDefinitions::InitTypeData (CUniverse &Universe, CAttributeDataBlock &Dest) const

//	InitTypeData
//
//	Evaluates properties for the type and stores them in Dest.

	{
	CCodeChainCtx CCCtx(Universe);

	for (int i = 0; i < m_Defs.GetCount(); i++)
		{
		switch (m_Defs[i].iType)
			{
			case EPropertyType::propConstant:
			case EPropertyType::propGlobal:
				{
				if (m_Defs[i].pCode)
					{
					ICCItemPtr pResult = CCCtx.RunCode(m_Defs[i].pCode);
					if (pResult->IsError())
						{
						::kernelDebugLogPattern("ERROR: Evaluating property %s: %s", m_Defs.GetKey(i), pResult->GetStringValue());
						continue;
						}

					Dest.SetData(m_Defs.GetKey(i), pResult);
					}

				break;
				}
			}
		}
	}
