//	COrderList.cpp
//
//	COrderList class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

COrderDesc COrderList::m_NullOrder;

void COrderList::Delete (int iIndex)

//	Delete
//
//	Delete the given order

	{
	if (iIndex < m_List.GetCount())
		m_List.Delete(iIndex);
	}

void COrderList::DeleteAll (void)

//	DeleteAll
//
//	Delete all orders

	{
	m_List.DeleteAll();
	}

void COrderList::DeleteCurrent (void)

//	DeleteCurrent
//
//	Delete the current order

	{
	if (m_List.GetCount() > 0)
		m_List.Delete(0);
	}

void COrderList::Insert (COrderDesc OrderDesc, bool bAddBefore)

//	Insert
//
//	Adds an order to the list.

	{
	COrderDesc *pEntry;
	if (bAddBefore)
		pEntry = m_List.InsertAt(0);
	else
		pEntry = m_List.Insert();

	*pEntry = std::move(OrderDesc);
	}

void COrderList::OnNewSystem (CSystem *pNewSystem, bool *retbCurrentChanged)

//	OnNewSystem
//
//	Delete all orders that have objects that don't belong to this system.

	{
	bool bCurrentChanged = false;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		COrderDesc &Entry = m_List[i];
		if (Entry.GetTarget()
				&& Entry.GetTarget()->GetSystem() != pNewSystem
				&& !Entry.GetTarget()->IsPlayer())
			{
			//	Remember if this is the current order (because our caller may
			//	want to know).

			if (i == 0)
				bCurrentChanged = true;

			//	Remove the order

			Delete(i);
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
	bool bCurrentChanged = false;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		COrderDesc &Entry = m_List[i];
		if (Entry.GetTarget() == pObj)
			{
			//	Remember if this is the current order (because our caller may
			//	want to know).

			if (i == 0)
				bCurrentChanged = true;

			//	Remove the order

			Delete(i);
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
	bool bCurrentChanged = false;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		COrderDesc &Entry = m_List[i];
		IShipController::OrderTypes iOrder = Entry.GetOrder();
		DWORD dwFlags = IShipController::GetOrderFlags(iOrder);

		//	If the old ship is waiting, then we keep autons with the old ships
		//	but cancel attacks.

		if (Options.bOldShipWaits)
			{
			if (Entry.GetTarget() == pOldShip
					&& (dwFlags & ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS))
				{
				//	Remember if this is the current order (because our caller may
				//	want to know).

				if (i == 0)
					bCurrentChanged = true;

				//	Remove the order

				Delete(i);
				i--;
				}
			}

		//	Otherwise, change target if necessary

		else
			{
			if (Entry.GetTarget() == pOldShip
					&& (dwFlags & ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP))
				{
				Entry.SetTarget(pNewShip);
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
	bool bCurrentChanged = false;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		COrderDesc &Entry = m_List[i];
		IShipController::OrderTypes iOrder = Entry.GetOrder();
		DWORD dwFlags = IShipController::GetOrderFlags(iOrder);

		if (Entry.GetTarget() == pObj
				&& (dwFlags & ORDER_FLAG_DELETE_ON_STATION_DESTROYED))
			{
			//	Remember if this is the current order (because our caller may
			//	want to know).

			if (i == 0)
				bCurrentChanged = true;

			//	Remove the order

			Delete(i);
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
	DWORD dwCount;

	Ctx.pStream->Read(dwCount);
	m_List.InsertEmpty(dwCount);

	for (int i = 0; i < m_List.GetCount(); i++)
		m_List[i].ReadFromStream(Ctx);
	}

void COrderList::SetCurrentOrderDataInteger (DWORD dwData)

//	SetCurrentOrderDataInteger
//
//	Sets the data

	{
	if (m_List.GetCount() > 0)
		m_List[0].SetDataInteger(dwData);
	}

void COrderList::SetCurrentOrderDataInteger (DWORD dwData1, DWORD dwData2)

//	SetCurrentOrderDataInteger
//
//	Sets an integer pair.

	{
	if (m_List.GetCount() > 0)
		m_List[0].SetDataInteger(dwData1, dwData2);
	}

void COrderList::WriteToStream (IWriteStream &Stream, const CShip &Ship)

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
	DWORD dwSave = m_List.GetCount();
	Stream.Write(dwSave);

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		m_List[i].WriteToStream(Stream, Ship);
		}
	}
