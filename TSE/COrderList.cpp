//	COrderList.cpp
//
//	COrderList class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

COrderList::SOrderEntry COrderList::m_NullOrder;

COrderList::~COrderList (void)

//	COrderList destructor

	{
	DeleteAll();
	}

void COrderList::Delete (int iIndex)

//	Delete
//
//	Delete the given order

	{
	if (iIndex < m_List.GetCount())
		{
		CleanUp(&m_List[iIndex]);
		m_List.Delete(iIndex);
		}
	}

void COrderList::DeleteAll (void)

//	DeleteAll
//
//	Delete all orders

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		CleanUp(&m_List[i]);

	m_List.DeleteAll();
	}

void COrderList::DeleteCurrent (void)

//	DeleteCurrent
//
//	Delete the current order

	{
	if (m_List.GetCount() > 0)
		{
		CleanUp(&m_List[0]);
		m_List.Delete(0);
		}
	}

void COrderList::CleanUp (SOrderEntry *pEntry)

//	CleanUp
//
//	Clean up the entry

	{
	switch (pEntry->dwDataType)
		{
		case IShipController::dataString:
			if (pEntry->dwData)
				{
				delete (CString *)pEntry->dwData;
				pEntry->dwData = 0;
				}
			break;

		case IShipController::dataVector:
			if (pEntry->dwData)
				{
				delete (CVector *)pEntry->dwData;
				pEntry->dwData = 0;
				}
			break;
		}
	}

IShipController::OrderTypes COrderList::GetOrder (int iIndex, CSpaceObject **retpTarget, IShipController::SData *retData) const

//	GetOrder
//
//	Returns the given order

	{
	const SOrderEntry &Entry = (iIndex < m_List.GetCount() ? m_List[iIndex] : m_NullOrder);

	if (retpTarget)
		*retpTarget = Entry.pTarget;

	if (retData)
		{
		retData->iDataType = (IShipController::EDataTypes)Entry.dwDataType;

		switch (Entry.dwDataType)
			{
			case IShipController::dataInteger:
				retData->dwData1 = Entry.dwData;
				break;

			case IShipController::dataPair:
				retData->dwData1 = LOWORD(Entry.dwData);
				retData->dwData2 = HIWORD(Entry.dwData);
				break;

			case IShipController::dataString:
				retData->sData = (Entry.dwData ? *(CString *)Entry.dwData : NULL_STR);
				break;

			case IShipController::dataVector:
				retData->vData = (Entry.dwData ? *(CVector *)Entry.dwData : NullVector);
				break;
			}
		}

	return (IShipController::OrderTypes)Entry.dwOrderType;
	}

void COrderList::Insert (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const IShipController::SData &Data, bool bAddBefore)

//	Insert
//
//	Adds an order to the list.

	{
	SOrderEntry *pEntry;
	if (bAddBefore)
		pEntry = m_List.InsertAt(0);
	else
		pEntry = m_List.Insert();

	pEntry->dwOrderType = iOrder;
	pEntry->pTarget = pTarget;

	SetEntryData(pEntry, Data);
	}

void COrderList::OnNewSystem (CSystem *pNewSystem, bool *retbCurrentChanged)

//	OnNewSystem
//
//	Delete all orders that have objects that don't belong to this system.

	{
	int i;
	bool bCurrentChanged = false;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SOrderEntry *pEntry = &m_List[i];
		if (pEntry->pTarget 
				&& pEntry->pTarget->GetSystem() != pNewSystem
				&& !pEntry->pTarget->IsPlayer())
			{
			//	Remember if this is the current order (because our caller may
			//	want to know).

			if (i == 0)
				bCurrentChanged = true;

			//	Remove the order

			CleanUp(pEntry);
			m_List.Delete(i);
			i--;
			}
		}

	//	Done

	if (retbCurrentChanged)
		*retbCurrentChanged = bCurrentChanged;
	}

void COrderList::OnObjDestroyed (CSpaceObject *pObj, bool *retbCurrentChanged)

//	OnObjDestroyed
//
//	Delete all orders that have the object that was destroyed.

	{
	int i;
	bool bCurrentChanged = false;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SOrderEntry *pEntry = &m_List[i];
		if (pEntry->pTarget == pObj)
			{
			//	Remember if this is the current order (because our caller may
			//	want to know).

			if (i == 0)
				bCurrentChanged = true;

			//	Remove the order

			CleanUp(pEntry);
			m_List.Delete(i);
			i--;
			}
		}

	//	Done

	if (retbCurrentChanged)
		*retbCurrentChanged = bCurrentChanged;
	}

void COrderList::OnPlayerChangedShips (CSpaceObject *pOldShip, CSpaceObject *pNewShip, SPlayerChangedShipsCtx &Options, bool *retbCurrentChanged)

//	OnPlayerChangedShips
//
//	Alter appropriate orders if player changed ships.

	{
	int i;
	bool bCurrentChanged = false;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SOrderEntry *pEntry = &m_List[i];
		IShipController::OrderTypes iOrder = GetOrder(i);
		DWORD dwFlags = ::GetOrderFlags(iOrder);

		//	If the old ship is waiting, then we keep autons with the old ships
		//	but cancel attacks.

		if (Options.bOldShipWaits)
			{
			if (pEntry->pTarget == pOldShip
					&& (dwFlags & ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS))
				{
				//	Remember if this is the current order (because our caller may
				//	want to know).

				if (i == 0)
					bCurrentChanged = true;

				//	Remove the order

				CleanUp(pEntry);
				m_List.Delete(i);
				i--;
				}
			}

		//	Otherwise, change target if necessary

		else
			{
			if (pEntry->pTarget == pOldShip
					&& (dwFlags & ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP))
				{
				pEntry->pTarget = pNewShip;
				if (i == 0)
					bCurrentChanged = true;
				}
			}
		}

	//	Done

	if (retbCurrentChanged)
		*retbCurrentChanged = bCurrentChanged;
	}

void COrderList::OnStationDestroyed (CSpaceObject *pObj, bool *retbCurrentChanged)

//	OnStationDestroyed
//
//	Delete all orders that have the object that was destroyed.

	{
	int i;
	bool bCurrentChanged = false;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SOrderEntry *pEntry = &m_List[i];
		IShipController::OrderTypes iOrder = GetOrder(i);
		DWORD dwFlags = ::GetOrderFlags(iOrder);

		if (pEntry->pTarget == pObj
				&& (dwFlags & ORDER_FLAG_DELETE_ON_STATION_DESTROYED))
			{
			//	Remember if this is the current order (because our caller may
			//	want to know).

			if (i == 0)
				bCurrentChanged = true;

			//	Remove the order

			CleanUp(pEntry);
			m_List.Delete(i);
			i--;
			}
		}

	//	Done

	if (retbCurrentChanged)
		*retbCurrentChanged = bCurrentChanged;
	}

void COrderList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read

	{
	int i;
	DWORD dwLoad;
	DWORD dwCount;

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	m_List.InsertEmpty(dwCount);

	//	Read new version

	if (Ctx.dwVersion >= 87)
		{
		for (i = 0; i < (int)dwCount; i++)
			{
			SOrderEntry *pEntry = &m_List[i];

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			pEntry->dwOrderType = LOWORD(dwLoad);
			pEntry->dwDataType = HIWORD(dwLoad);

			CSystem::ReadObjRefFromStream(Ctx, &pEntry->pTarget);

			switch (pEntry->dwDataType)
				{
				case IShipController::dataInteger:
				case IShipController::dataPair:
					Ctx.pStream->Read((char *)&pEntry->dwData, sizeof(DWORD));
					break;

				case IShipController::dataString:
					{
					CString *pString = new CString;
					pString->ReadFromStream(Ctx.pStream);
					pEntry->dwData = (DWORD)pString;
					break;
					}

				case IShipController::dataVector:
					{
					CVector *pVector = new CVector;
					Ctx.pStream->Read((char *)pVector, sizeof(CVector));
					pEntry->dwData = (DWORD)pVector;
					break;
					}
				}
			}
		}

	//	Backwards compatible

	else
		{
		for (i = 0; i < (int)dwCount; i++)
			{
			SOrderEntry *pEntry = &m_List[i];

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			pEntry->dwOrderType = dwLoad;
			pEntry->dwDataType = IShipController::dataInteger;

			CSystem::ReadObjRefFromStream(Ctx, &pEntry->pTarget);
			Ctx.pStream->Read((char *)&pEntry->dwData, sizeof(DWORD));
			}
		}
	}

void COrderList::SetCurrentOrderData (const IShipController::SData &Data)

//	SetCurrentOrderData
//
//	Sets the data

	{
	SetEntryData(&GetCurrentEntryActual(), Data);
	}

void COrderList::SetEntryData (SOrderEntry *pEntry, const IShipController::SData &Data)

//	SetEntryData
//
//	Sets the data

	{
	CleanUp(pEntry);

	pEntry->dwDataType = Data.iDataType;
	switch (Data.iDataType)
		{
		case IShipController::dataInteger:
			pEntry->dwData = Data.dwData1;
			break;

		case IShipController::dataPair:
			pEntry->dwData = MAKELONG(Data.dwData1, Data.dwData2);
			break;

		case IShipController::dataString:
			pEntry->dwData = (DWORD)(new CString(Data.sData));
			break;

		case IShipController::dataVector:
			pEntry->dwData = (DWORD)(new CVector(Data.vData));
			break;

		default:
			pEntry->dwData = 0;
		}
	}

void COrderList::WriteToStream (IWriteStream *pStream, CSystem *pSystem)

//	WriteToStream
//
//	Writes to stream
//
//	DWORD			No. of orders
//
//	For each order:
//	DWORD			LOWORD = dwOrderType; HIWORD = dwDataType
//	DWORD			Target reference
//
//	DWORD or		(Depends on dwDataType)
//	CString

	{
	int i;
	DWORD dwSave;

	dwSave = m_List.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SOrderEntry *pEntry = &m_List[i];

		dwSave = MAKELONG(pEntry->dwOrderType, pEntry->dwDataType);
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		pSystem->WriteObjRefToStream(pEntry->pTarget, pStream);

		switch (pEntry->dwDataType)
			{
			case IShipController::dataInteger:
			case IShipController::dataPair:
				pStream->Write((char *)&pEntry->dwData, sizeof(DWORD));
				break;

			case IShipController::dataString:
				{
				CString *pString = (CString *)pEntry->dwData;
				if (pString)
					pString->WriteToStream(pStream);
				else
					{
					CString sNull;
					sNull.WriteToStream(pStream);
					}
				break;
				}

			case IShipController::dataVector:
				{
				CVector *pVector = (CVector *)pEntry->dwData;
				if (pVector)
					pStream->Write((char *)pVector, sizeof(CVector));
				else
					pStream->Write((char *)&NullVector, sizeof(CVector));
				break;
				}
			}
		}
	}
