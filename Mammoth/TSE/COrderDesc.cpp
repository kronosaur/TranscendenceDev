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
			m_pData = NULL;
			break;

		case EDataType::Vector:
			delete (CVector *)m_pData;
			m_pData = NULL;
			break;

		case EDataType::Item:
			delete (CItem *)m_pData;
			m_pData = NULL;
			break;

		case EDataType::CCItem:
			((ICCItem *)m_pData)->Discard();
			m_pData = NULL;
			break;

		default:
			throw CException(ERR_FAIL);
		}
	}

void COrderDesc::Copy (const COrderDesc &Src)

//	Copy
//
//	Copy from source

	{
	m_dwOrderType = Src.m_dwOrderType;
	m_dwDataType = Src.m_dwDataType;
	m_pTarget = Src.m_pTarget;

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
			m_pData = ((ICCItem *)m_pData)->Reference();
			break;

		default:
			throw CException(ERR_FAIL);
		}
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

void COrderDesc::Move (COrderDesc &Src)

//	Move
//
//	Move from source.

	{
	m_dwOrderType = Src.m_dwOrderType;
	m_dwDataType = Src.m_dwDataType;
	m_pTarget = Src.m_pTarget;
	m_pData = Src.m_pData;

	Src.m_dwDataType = (DWORD)EDataType::None;
	Src.m_pData = NULL;
	}
