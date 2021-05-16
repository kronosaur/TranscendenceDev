//	COrderDesc.cpp
//
//	COrderDesc class
//	Copyright (c) 2021 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

COrderDesc COrderDesc::Null;

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder) :
		m_dwOrderType((DWORD)iOrder)

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget)

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, int iData) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget),
		m_dwDataType((DWORD)EDataType::Int32),
		m_pData((void *)iData)

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, int iData1, int iData2) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget),
		m_dwDataType((DWORD)EDataType::Int16Pair),
		m_pData((void *)MAKELONG(iData1, iData2))

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const CString &sData) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget),
		m_dwDataType((DWORD)EDataType::String),
		m_pData(new CString(sData))

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const CVector &vData) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget),
		m_dwDataType((DWORD)EDataType::Vector),
		m_pData(new CVector(vData))

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const CItem &Data) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget),
		m_dwDataType((DWORD)EDataType::Item),
		m_pData(new CItem(Data))

//	COrderDesc constructor

	{
	}

COrderDesc::COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const ICCItem &Data) :
		m_dwOrderType((DWORD)iOrder),
		m_pTarget(pTarget),
		m_dwDataType((DWORD)EDataType::CCItem),
		m_pData(Data.Reference())

//	COrderDesc constructor

	{
	}

ICCItemPtr COrderDesc::AsCCItemList () const

//	AsCCItem
//
//	Export order desc as a CCItem.

	{
	if (GetOrder() == IShipController::orderNone)
		return ICCItemPtr::Nil();

	//	Create result list

	ICCItemPtr pList(ICCItem::List);

	//	Add order name

	pList->AppendString(IShipController::GetOrderName(GetOrder()));

	//	Add the target

	if (IShipController::OrderHasTarget(GetOrder()))
		{
		pList->Append(CTLispConvert::CreateObject(GetTarget()));
		}

	//	Add order data

	switch (GetDataType())
		{
		case EDataType::None:
			break;

		case EDataType::Int32:
			pList->AppendInteger(GetDataInteger());
			break;

		case EDataType::Int16Pair:
			pList->AppendInteger(GetDataInteger());
			pList->AppendInteger(GetDataInteger2());
			break;

		case EDataType::Item:
			pList->Append(CTLispConvert::CreateItem(GetDataItem()));
			break;

		case EDataType::String:
			pList->AppendString(GetDataString());
			break;

		case EDataType::Vector:
			pList->Append(CTLispConvert::CreateVector(GetDataVector()));
			break;

		case EDataType::CCItem:
			pList->Append(GetDataCCItem());
			break;

		default:
			throw CException(ERR_FAIL);
		}

	//	Done

	return pList;
	}

void COrderDesc::CleanUp ()

//	CleanUp
//
//	Free all resources.

	{
	switch (GetDataType())
		{
		case EDataType::None:
		case EDataType::Int32:
		case EDataType::Int16Pair:
			break;

		case EDataType::String:
			delete (CString *)m_pData;
			break;

		case EDataType::Vector:
			delete (CVector *)m_pData;
			break;

		case EDataType::Item:
			delete (CItem *)m_pData;
			break;

		case EDataType::CCItem:
			((ICCItem *)m_pData)->Discard();
			break;

		default:
			throw CException(ERR_FAIL);
		}

	m_dwDataType = (DWORD)EDataType::None;
	m_pData = NULL;
	}

void COrderDesc::Copy (const COrderDesc &Src)

//	Copy
//
//	Copy from source

	{
	m_dwOrderType = Src.m_dwOrderType;
	m_dwDataType = Src.m_dwDataType;
	m_pTarget = Src.m_pTarget;

	m_fCancelOnReactionOrder = Src.m_fCancelOnReactionOrder;

	switch (GetDataType())
		{
		case EDataType::None:
		case EDataType::Int32:
		case EDataType::Int16Pair:
			m_pData = Src.m_pData;
			break;

		case EDataType::String:
			m_pData = new CString(*(CString *)Src.m_pData);
			break;

		case EDataType::Vector:
			m_pData = new CVector(*(CVector *)Src.m_pData);
			break;

		case EDataType::Item:
			m_pData = new CItem(*(CItem *)Src.m_pData);
			break;

		case EDataType::CCItem:
			m_pData = ((ICCItem *)Src.m_pData)->Reference();
			break;

		default:
			throw CException(ERR_FAIL);
		}
	}

DiceRange COrderDesc::GetDataDiceRange (const CString &sField, int iDefault, CString *retsSuffix) const

//	GetDataDiceRange
//
//	Returns a dice range value.

	{
	if (IsCCItem())
		{
		ICCItemPtr pData = GetDataCCItem();

		if (const ICCItem *pValue = pData->GetElement(sField))
			{
			if (pValue->IsIdentifier())
				{
				DiceRange Value;
				if (Value.LoadFromXML(pValue->GetStringValue(), iDefault, retsSuffix) != NOERROR)
					{
					if (retsSuffix)
						*retsSuffix = NULL_STR;

					return DiceRange(0, 0, iDefault);
					}

				return Value;
				}
			else
				{
				if (retsSuffix)
					*retsSuffix = NULL_STR;

				return DiceRange(0, 0, pValue->GetIntegerValue());
				}
			}
		else
			{
			if (retsSuffix)
				*retsSuffix = NULL_STR;

			return DiceRange(0, 0, iDefault);
			}
		}
	else
		{
		if (retsSuffix)
			*retsSuffix = NULL_STR;

		return DiceRange(0, 0, iDefault);
		}
	}

bool COrderDesc::GetDataBoolean (const CString &sField, bool bDefault) const

//	GetDataBoolean
//
//	Gets a boolean field value.

	{
	if (IsCCItem())
		{
		ICCItemPtr pData = GetDataCCItem();

		if (const ICCItem *pValue = pData->GetElement(sField))
			return !pValue->IsNil();
		else
			return bDefault;
		}
	else
		return bDefault;
	}

Metric COrderDesc::GetDataDouble (const CString &sField, Metric rDefault) const

//	GetDataDouble
//
//	Gets a double value of the given field.

	{
	if (IsCCItem())
		{
		ICCItemPtr pData = GetDataCCItem();

		if (const ICCItem *pValue = pData->GetElement(sField))
			return pValue->GetDoubleValue();
		else
			return rDefault;
		}
	else
		return rDefault;
	}

DWORD COrderDesc::GetDataInteger () const

//	AsInteger
//
//	Returns the first integer.

	{
	switch (GetDataType())
		{
		case EDataType::Int32:
			return (DWORD)m_pData;

		case EDataType::Int16Pair:
			return LOWORD((DWORD)m_pData);

		default:
			return 0;
		}
	}

DWORD COrderDesc::GetDataInteger (const CString &sField, bool bDefaultField, DWORD dwDefault) const

//	GetDataInteger
//
//	Returns an integer.

	{
	switch (GetDataType())
		{
		case EDataType::CCItem:
			{
			ICCItemPtr pData = GetDataCCItem();

			if (const ICCItem *pValue = pData->GetElement(sField))
				return pValue->GetIntegerValue();
			else
				return dwDefault;
			}

		case EDataType::Int16Pair:
		case EDataType::Int32:
			{
			DWORD dwValue = GetDataInteger();
			if (bDefaultField && dwValue)
				return dwValue;
			else
				return dwDefault;
			}

		default:
			return dwDefault;
		}
	}

DWORD COrderDesc::GetDataInteger2 () const

//	AsInteger2
//
//	Returns the second integer.

	{
	switch (GetDataType())
		{
		case EDataType::Int16Pair:
			return HIWORD((DWORD)m_pData);

		default:
			return 0;
		}
	}

DWORD COrderDesc::GetDataIntegerOptional (const CString &sField, DWORD dwDefault) const

//	GetDataIntegerOptional
//
//	Returns an integer.

	{
	switch (GetDataType())
		{
		case EDataType::CCItem:
			{
			ICCItemPtr pData = GetDataCCItem();

			if (const ICCItem *pValue = pData->GetElement(sField))
				return pValue->GetIntegerValue();
			else
				return dwDefault;
			}

		case EDataType::Int16Pair:
			return GetDataInteger();

		case EDataType::Int32:
			return dwDefault;

		default:
			return dwDefault;
		}
	}

CSpaceObject *COrderDesc::GetDataObject (CSpaceObject &SourceObj, const CString &sField) const

//	GetDataObject
//
//	Returns an object stored as an ID. Returns NULL if not found.

	{
	DWORD dwObjID;

	switch (GetDataType())
		{
		case EDataType::CCItem:
			{
			ICCItemPtr pData = GetDataCCItem();

			if (const ICCItem *pValue = pData->GetElement(sField))
				dwObjID = pValue->GetIntegerValue();
			else
				return NULL;
			break;
			}

		default:
			return NULL;
		}

	CSystem *pSystem = SourceObj.GetSystem();
	if (!pSystem)
		return NULL;

	return pSystem->FindObject(dwObjID);
	}

CString COrderDesc::GetDataString (const CString &sField) const

//	GetDataString
//
//	Gets a string value of the given field.

	{
	if (IsCCItem())
		{
		ICCItemPtr pData = GetDataCCItem();

		if (const ICCItem *pValue = pData->GetElement(sField))
			return pValue->GetStringValue();
		else
			return NULL_STR;
		}
	else
		return NULL_STR;
	}

int COrderDesc::GetDataTicksLeft () const

//	GetDataTicksLeft
//
//	Returns the ticks left (or -1 if no expiration).

	{
	DWORD dwSecondsLeft = 0;

	switch (GetDataType())
		{
		case EDataType::CCItem:
			{
			ICCItemPtr pData = GetDataCCItem();

			if (const ICCItem *pValue = pData->GetElement(CONSTLIT("timer")))
				dwSecondsLeft = pValue->GetIntegerValue();
			else
				return -1;

			break;
			}

		case EDataType::Int32:
			dwSecondsLeft = GetDataInteger();
			break;

		case EDataType::Int16Pair:
			dwSecondsLeft = GetDataInteger2();
			break;

		default:
			return -1;
		}

	if (dwSecondsLeft)
		return 1 + g_TicksPerSecond * dwSecondsLeft;
	else
		return -1;
	}

CVector COrderDesc::GetDataVector (const CString &sField, bool bDefaultField, const CVector &vDefault) const

//	GetDataVector
//
//	Returns a vector.

	{
	if (IsCCItem())
		{
		ICCItemPtr pData = GetDataCCItem();

		if (const ICCItem *pValue = pData->GetElement(sField))
			return CTLispConvert::AsVector(pValue);
		else
			return vDefault;
		}
	else if (bDefaultField && IsVector())
		return GetDataVector();
	else
		return vDefault;
	}

void COrderDesc::Move (COrderDesc &Src)

//	Move
//
//	Move from source.

	{
	m_dwOrderType = Src.m_dwOrderType;
	m_dwDataType = Src.m_dwDataType;
	m_pTarget = Src.m_pTarget;
	m_pData = Src.m_pData;

	m_fCancelOnReactionOrder = Src.m_fCancelOnReactionOrder;

	Src.m_dwDataType = (DWORD)EDataType::None;
	Src.m_pData = NULL;
	}

COrderDesc COrderDesc::ParseFromCCItem (CCodeChainCtx &CCX, IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const ICCItem &Args, int iFirstArg)

//	ParseFromCCItem
//
//	Parses from arguments to a function.

	{
	//	If we don't have at least one argument, then we have null data.

	if (Args.GetCount() <= iFirstArg)
		{
		return COrderDesc(iOrder, pTarget);
		}

	//	Handle based on the data type we expect.

	else
		{
		char chData = IShipController::GetOrderDataType(iOrder);
		const ICCItem &Value = *Args.GetElement(iFirstArg);

		switch (chData)
			{
			case 'I':
				if (!Value.IsNil())
					return COrderDesc(iOrder, pTarget, CCX.AsItem(&Value));
				else
					return COrderDesc(iOrder, pTarget);

			case 's':
				if (!Value.IsNil())
					return COrderDesc(iOrder, pTarget, Value.GetStringValue());
				else
					return COrderDesc(iOrder, pTarget);

			case 'v':
				if (!Value.IsNil())
					return COrderDesc(iOrder, pTarget, ::CreateVectorFromList(CCX.GetCC(), &Value));
				else
					return COrderDesc(iOrder, pTarget);

			//	Figure out based on arguments

			default:
				{
				//	2 Integers

				if (Args.GetCount() > (iFirstArg + 1))
					{
					const ICCItem &Value2 = *Args.GetElement(iFirstArg + 1);

					if (!Value.IsNil() || !Value2.IsNil())
						return COrderDesc(iOrder, pTarget, Value.GetIntegerValue(), Value2. GetIntegerValue());
					else
						return COrderDesc(iOrder, pTarget);
					}

				//	A symbol table

				else if (Value.IsSymbolTable())
					return COrderDesc(iOrder, pTarget, Value);

				//	A string

				else if (Value.IsIdentifier())
					return COrderDesc(iOrder, pTarget, Value.GetStringValue());

				//	One integer

				else
					{
					if (!Value.IsNil())
						return COrderDesc(iOrder, pTarget, Value.GetIntegerValue());
					else
						return COrderDesc(iOrder, pTarget);
					}
				}
			}
		}
	}

COrderDesc COrderDesc::ParseFromString (const CString &sValue)

//	ParseFromString
//
//	Parses from a string.

	{
	const char *pPos = sValue.GetASCIIZPointer();

	//	Parse the order name

	const char *pStart = pPos;
	while (*pPos != '\0' && *pPos != ':')
		pPos++;

	CString sOrder(pStart, (int)(pPos - pStart));

	//	For backwards compatibility we handle some special names.

	IShipController::OrderTypes iOrder;
	if (strEquals(sOrder, CONSTLIT("trade route")))
		iOrder = IShipController::orderTradeRoute;
	else
		{
		iOrder = IShipController::GetOrderType(sOrder);

		//	Check for error

		if (iOrder == IShipController::orderNone && !sOrder.IsBlank())
			return COrderDesc();
		}

	//	Get additional data

	if (*pPos == ':')
		{
		pPos++;
		while (strIsWhitespace(pPos))
			pPos++;

		//	If nothing, then no parameter

		if (*pPos == '\0')
			return COrderDesc(iOrder);

		//	If this is a number, then we expect 1 or 2 numbers

		else if (strIsDigit(pPos))
			{
			DWORD dwData1 = strParseInt(pPos, 0, &pPos);

			while (*pPos != '\0' && *pPos != ':')
				pPos++;

			if (*pPos == ':')
				{
				pPos++;
				DWORD dwData2 = strParseInt(pPos, 0);
				return COrderDesc(iOrder, NULL, dwData1, dwData2);
				}
			else
				{
				return COrderDesc(iOrder, NULL, dwData1);
				}
			}

		//	Otherwise, we expect fields.

		else
			{
			ICCItemPtr pData(ICCItem::SymbolTable);

			while (*pPos != '\0')
				{
				while (strIsWhitespace(pPos))
					pPos++;

				pStart = pPos;
				while (*pPos != '\0' && *pPos != '=')
					pPos++;

				CString sField(pStart, pPos - pStart);
				if (*pPos != '=')
					return COrderDesc();

				pPos++;
				pStart = pPos;
				while (*pPos != '\0' && *pPos != ':' && !strIsWhitespace(pPos))
					pPos++;

				CString sValue(pStart, pPos - pStart);
				pData->SetAt(sField, CCodeChain::CreateLiteral(sValue));

				while (strIsWhitespace(pPos))
					pPos++;

				if (*pPos == ':')
					pPos++;
				}

			return COrderDesc(iOrder, NULL, *pData);
			}
		}
	else
		return COrderDesc(iOrder);
	}

void COrderDesc::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads an order from stream.

	{
	if (Ctx.dwVersion >= 195)
		{
		DWORD dwLoad;
		Ctx.pStream->Read(dwLoad);
		m_dwOrderType = LOWORD(dwLoad);
		m_dwDataType = HIWORD(dwLoad);

		if (Ctx.dwVersion >= 205)
			{
			DWORD dwFlags;
			Ctx.pStream->Read(dwFlags);

			m_fCancelOnReactionOrder = ((dwFlags & 0x00000001) ? true : false);
			}

		CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);

		switch (GetDataType())
			{
			case EDataType::None:
				break;

			case EDataType::Int32:
			case EDataType::Int16Pair:
				Ctx.pStream->Read(dwLoad);
				m_pData = (void *)dwLoad;
				break;

			case EDataType::String:
				{
				CString *pString = new CString;
				pString->ReadFromStream(Ctx.pStream);
				m_pData = pString;
				break;
				}

			case EDataType::Vector:
				{
				CVector *pVector = new CVector;
				pVector->ReadFromStream(*Ctx.pStream);
				m_pData = pVector;
				break;
				}

			case EDataType::Item:
				{
				CItem *pItem = new CItem;
				pItem->ReadFromStream(Ctx);
				m_pData = pItem;
				break;
				}

			case EDataType::CCItem:
				{
				CString sData;
				sData.ReadFromStream(Ctx.pStream);
				m_pData = CCodeChain::LinkCode(sData)->Reference();
				break;
				}
			}
		}
	else if (Ctx.dwVersion >= 87)
		{
		DWORD dwLoad;
		Ctx.pStream->Read(dwLoad);
		m_dwOrderType = LOWORD(dwLoad);

		CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);

		switch (HIWORD(dwLoad))
			{
			case COMPATIBLE_DATA_TYPE_NONE:
				break;

			case COMPATIBLE_DATA_TYPE_INTEGER:
				Ctx.pStream->Read(dwLoad);
				m_dwDataType = (DWORD)EDataType::Int32;
				m_pData = (void *)dwLoad;
				break;

			case COMPATIBLE_DATA_TYPE_PAIR:
				Ctx.pStream->Read(dwLoad);
				m_dwDataType = (DWORD)EDataType::Int16Pair;
				m_pData = (void *)dwLoad;
				break;

			case COMPATIBLE_DATA_TYPE_ITEM:
				{
				CItem *pItem = new CItem;
				pItem->ReadFromStream(Ctx);
				m_dwDataType = (DWORD)EDataType::Item;
				m_pData = pItem;
				break;
				}

			case COMPATIBLE_DATA_TYPE_STRING:
				{
				CString *pString = new CString;
				pString->ReadFromStream(Ctx.pStream);
				m_dwDataType = (DWORD)EDataType::String;
				m_pData = pString;
				break;
				}

			case COMPATIBLE_DATA_TYPE_VECTOR:
				{
				CVector *pVector = new CVector;
				Ctx.pStream->Read((char *)pVector, sizeof(CVector));
				m_dwDataType = (DWORD)EDataType::Vector;
				m_pData = pVector;
				break;
				}

			case COMPATIBLE_DATA_TYPE_ORBIT_EXACT:
				{
				DWORD dwLoad;
				Ctx.pStream->Read(dwLoad);
				DWORD dwAngle = HIWORD(dwLoad);
				DWORD dwRadius = LOWORD(dwLoad);

				Metric rSpeed;
				Ctx.pStream->Read(rSpeed);

				Metric rEccentricity;
				Ctx.pStream->Read(rEccentricity);

				DWORD dwTimer;
				Ctx.pStream->Read(dwTimer);

				ICCItemPtr pData(ICCItem::SymbolTable);
				pData->SetIntegerAt(CONSTLIT("radius"), dwRadius);

				if (dwAngle != COrbitExactOrder::AUTO_ANGLE)
					pData->SetIntegerAt(CONSTLIT("angle"), dwAngle);

				pData->SetDoubleAt(CONSTLIT("speed"), rSpeed);
				if (rEccentricity != 0.0)
					pData->SetDoubleAt(CONSTLIT("eccentricity"), rEccentricity);

				if (dwTimer)
					pData->SetIntegerAt(CONSTLIT("timer"), dwTimer);

				m_dwDataType = (DWORD)EDataType::CCItem;
				m_pData = pData->Reference();
				break;
				}
			}
		}

	//	Backwards compatible

	else
		{
		DWORD dwLoad;
		Ctx.pStream->Read(dwLoad);
		m_dwOrderType = dwLoad;

		CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);

		Ctx.pStream->Read(dwLoad);
		m_dwDataType = (DWORD)EDataType::Int32;
		m_pData = (void *)dwLoad;
		}
	}

void COrderDesc::SetDataInteger (DWORD dwData)

//	SetDataInteger
//
//	Sets as an integer.

	{
	CleanUp();
	m_dwDataType = (DWORD)EDataType::Int32;
	m_pData = (void *)dwData;
	}

void COrderDesc::SetDataInteger (DWORD dwData1, DWORD dwData2)

//	SetDataInteger
//
//	Sets an integer pair

	{
	CleanUp();
	m_dwDataType = (DWORD)EDataType::Int16Pair;
	m_pData = (void *)MAKELONG(dwData1, dwData2);
	}

void COrderDesc::WriteToStream (IWriteStream &Stream, const CShip &Ship) const

//	WriteToStream
//
//	Writes to a stream.

	{
	DWORD dwSave = MAKELONG(m_dwOrderType, m_dwDataType);
	Stream.Write(dwSave);

	DWORD dwFlags = 0;
	dwFlags |= (m_fCancelOnReactionOrder ? 0x00000001 : 0);
	Stream.Write(dwFlags);

	Ship.WriteObjRefToStream(m_pTarget, &Stream);

	switch (GetDataType())
		{
		case EDataType::Int32:
		case EDataType::Int16Pair:
			Stream.Write((DWORD)m_pData);
			break;

		case EDataType::Item:
			GetDataItem().WriteToStream(&Stream);
			break;

		case EDataType::String:
			GetDataString().WriteToStream(&Stream);
			break;

		case EDataType::Vector:
			GetDataVector().WriteToStream(Stream);
			break;

		case EDataType::CCItem:
			{
			CString sData = ::CreateDataFromItem(GetDataCCItem());
			sData.WriteToStream(&Stream);
			break;
			}
		}
	}
