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
#define DYNAMIC_DATA_TAG						CONSTLIT("DynamicData")
#define DYNAMIC_GLOBAL_TAG						CONSTLIT("DynamicGlobal")
#define GLOBAL_TAG								CONSTLIT("Global")
#define ITEM_DATA_TAG							CONSTLIT("ItemData")
#define OBJ_DATA_TAG							CONSTLIT("ObjData")
#define VARIANT_TAG								CONSTLIT("Variant")

#define ID_ATTRIB								CONSTLIT("id")

ALERROR CDesignPropertyDefinitions::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Evaluate any properties that need to be evaluated at bind-time.

	{
	for (int i = 0; i < m_InitOrder.GetCount(); i++)
		{
		SDef &Def = m_Defs[m_InitOrder[i]];
		if (Def.iType == EPropertyType::propDefinitionEvalNeeded)
			{
			CCodeChainCtx CCCtx(Ctx.GetUniverse());

			ICCItemPtr pResult = CCCtx.RunCode(Def.pCode);
			if (pResult->IsError())
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Property %s: %s"), m_Defs.GetKey(m_InitOrder[i]), pResult->GetStringValue());
				return ERR_FAIL;
				}

			Def.iType = EPropertyType::propDefinition;
			Def.pCode = pResult;
			}
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

CString CDesignPropertyDefinitions::GetPropertyTypeID (EPropertyType iType)

//	GetPropertyTypeID
//
//	Returns the property type as an ID.

	{
	switch (iType)
		{
		case EPropertyType::propConstant:
			return CONSTLIT("constant");

		case EPropertyType::propData:
		case EPropertyType::propItemData:
		case EPropertyType::propObjData:
			return CONSTLIT("data");

		case EPropertyType::propDefinition:
			return CONSTLIT("definition");

		case EPropertyType::propDynamicData:
			return CONSTLIT("dynamicData");

		case EPropertyType::propDynamicGlobal:
			return CONSTLIT("dynamicGlobal");

		case EPropertyType::propEngine:
			return CONSTLIT("engine");

		case EPropertyType::propGlobal:
			return CONSTLIT("global");

		case EPropertyType::propVariant:
			return CONSTLIT("variant");

		default:
			return NULL_STR;
		}
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
//	<DynamicGlobal...>: Evaluate at get-time on type
//	<DynamicData...>: Evaluate at get-time on object

	{
	//	Item types can define properties for themselves (the item) and in some
	//	cases, for objects (e.g., shot objects created by a weapon). Because of
	//	this we sometimes need to distinguish between item and object.

	bool bIsItemType = (Ctx.pType->GetType() == designItemType);

	//	Load all properties

	TArray<CString> InitOrder;
	for (int i = 0; i < Desc.GetContentElementCount(); i++)
		{
		const CXMLElement &Item = *Desc.GetContentElement(i);

		//	Figure out the type

		EPropertyType iType;
		if (strEquals(Item.GetTag(), CONSTANT_TAG))
			iType = EPropertyType::propConstant;
		else if (strEquals(Item.GetTag(), DATA_TAG))
			{
			if (bIsItemType)
				iType = EPropertyType::propItemData;
			else
				iType = EPropertyType::propData;
			}
		else if (strEquals(Item.GetTag(), DEFINITION_TAG))
			iType = EPropertyType::propDefinition;
		else if (strEquals(Item.GetTag(), DYNAMIC_DATA_TAG))
			iType = EPropertyType::propDynamicData;
		else if (strEquals(Item.GetTag(), DYNAMIC_GLOBAL_TAG))
			iType = EPropertyType::propDynamicGlobal;
		else if (strEquals(Item.GetTag(), GLOBAL_TAG))
			iType = EPropertyType::propGlobal;
		else if (strEquals(Item.GetTag(), ITEM_DATA_TAG) && bIsItemType)
			iType = EPropertyType::propItemData;
		else if (strEquals(Item.GetTag(), OBJ_DATA_TAG) && bIsItemType)
			iType = EPropertyType::propObjData;
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

			pCode = CCodeChain::LinkCode(sData, Options);

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

		InitOrder.Insert(sID);
		}

	//	We evaluate properties in the order that they are defined, in case some
	//	properties refer to others.

	m_InitOrder.InsertEmpty(InitOrder.GetCount());
	for (int i = 0; i < m_InitOrder.GetCount(); i++)
		{
		if (!m_Defs.FindPos(InitOrder[i], &m_InitOrder[i]))
			{
			Ctx.sError = CONSTLIT("This cannot have happened.");
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

void CDesignPropertyDefinitions::InitItemData (CUniverse &Universe, CItem &Item) const

//	InitItemData
//
//	Initializes item-level properties.

	{
	CCodeChainCtx CCX(Universe);
	bool bVarsDefined = false;

	for (int i = 0; i < m_InitOrder.GetCount(); i++)
		{
		const SDef &Def = m_Defs[m_InitOrder[i]];
		switch (Def.iType)
			{
			case EPropertyType::propData:
			case EPropertyType::propItemData:
			case EPropertyType::propVariant:
				{
				if (Def.pCode)
					{
					if (!bVarsDefined)
						{
						CCX.DefineContainingType(Item);
						CCX.SaveAndDefineItemVar(Item);
						bVarsDefined = true;
						}

					ICCItemPtr pResult = CCX.RunCode(Def.pCode);
					if (pResult->IsError())
						{
						::kernelDebugLogPattern("ERROR: Evaluating property %s: %s", m_Defs.GetKey(m_InitOrder[i]), pResult->GetStringValue());
						continue;
						}

					if (!pResult->IsNil())
						Item.SetData(m_Defs.GetKey(m_InitOrder[i]), pResult);
					}

				break;
				}
			}
		}
	}

void CDesignPropertyDefinitions::InitObjectData (CUniverse &Universe, CSpaceObject &Obj, CAttributeDataBlock &Dest) const

//	InitObjectData
//
//	Initializes object-level properties.

	{
	CCodeChainCtx CCCtx(Universe);
	bool bInitialized = false;

	for (int i = 0; i < m_InitOrder.GetCount(); i++)
		{
		const SDef &Def = m_Defs[m_InitOrder[i]];
		switch (Def.iType)
			{
			case EPropertyType::propData:
			case EPropertyType::propObjData:
			case EPropertyType::propVariant:
				{
				if (Def.pCode)
					{
					if (!bInitialized)
						{
						CCCtx.DefineContainingType(&Obj);
						CCCtx.SaveAndDefineSourceVar(&Obj);
						bInitialized = true;
						}

					ICCItemPtr pResult = CCCtx.RunCode(Def.pCode);
					if (pResult->IsError())
						{
						::kernelDebugLogPattern("ERROR: Evaluating property %s: %s", m_Defs.GetKey(m_InitOrder[i]), pResult->GetStringValue());
						continue;
						}

					if (!pResult->IsNil())
						Dest.SetData(m_Defs.GetKey(m_InitOrder[i]), pResult);
					}

				break;
				}
			}
		}
	}

void CDesignPropertyDefinitions::InitTypeData (CUniverse &Universe, CDesignType &Type) const

//	InitTypeData
//
//	Evaluates properties for the type and stores them in Dest.

	{
	CCodeChainCtx CCCtx(Universe);
	CCCtx.DefineContainingType(&Type);

	for (int i = 0; i < m_InitOrder.GetCount(); i++)
		{
		const SDef &Def = m_Defs[m_InitOrder[i]];
		switch (Def.iType)
			{
			case EPropertyType::propConstant:
			case EPropertyType::propGlobal:
				{
				if (Def.pCode)
					{
					ICCItemPtr pResult = CCCtx.RunCode(Def.pCode);
					if (pResult->IsError())
						{
						::kernelDebugLogPattern("ERROR: Evaluating property %s: %s", m_Defs.GetKey(m_InitOrder[i]), pResult->GetStringValue());
						continue;
						}

					if (!pResult->IsNil())
						Type.SetGlobalData(m_Defs.GetKey(m_InitOrder[i]), pResult);
					}

				break;
				}
			}
		}
	}
