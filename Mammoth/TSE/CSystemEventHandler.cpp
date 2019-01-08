//	CSystemEventHandler.cpp
//
//	CSystemEventHandler class

#include "PreComp.h"

void CSystemEventHandler::Create (CSpaceObject *pObj, Metric rMaxRange, CSystemEventHandler **retpHandler)

//	Create
//
//	Create a new object

	{
	if (pObj == NULL)
		throw CException(ERR_FAIL);

	CSystemEventHandler *pHandler = new CSystemEventHandler;

	pHandler->m_pHandler = pObj;
	pHandler->m_rMaxRange2 = rMaxRange * rMaxRange;

	*retpHandler = pHandler;
	}

bool CSystemEventHandler::InRange (const CVector &vPos) const

//	InRange
//
//	Returns TRUE if this event handler is in range of an event at the given pos

	{
	if (m_pHandler == NULL)
		return false;

	return (m_pHandler->GetPos() - vPos).Length2() < m_rMaxRange2; 
	}

bool CSystemEventHandler::OnObjDestroyed (CSpaceObject *pObjDestroyed)

//	OnObjDestroyed
//
//	Notification that an object has been destroyed. Returns TRUE if we referred
//	to the deleted object.

	{
	//	We clear out the object instead of deleting the entry because we might
	//	be inside a traversal of the linked list.
	//
	//	Callers must be ready to deal with a NULL object and the system must
	//	clean up these NULL entries at some point.

	if (pObjDestroyed == m_pHandler)
		{
		m_pHandler = NULL;
		return true;
		}
	else
		return false;
	}

void CSystemEventHandler::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	CSpaceObject	m_pHandler
//	Metric			m_rMaxRange2

	{
	CSystem::ReadObjRefFromStream(Ctx, &m_pHandler);
	Ctx.pStream->Read((char *)&m_rMaxRange2, sizeof(Metric));
	}

bool CSystemEventHandler::OnUnregister (CSpaceObject *pObj)

//	OnUnregister
//
//	Unregister ourselves. We behave as if the object was destroyed for similar
//	reasons: If we're traversing the linked-list, then we don't want to touch
//	the list until later.

	{
	if (pObj == m_pHandler)
		{
		m_pHandler = NULL;
		return true;
		}
	else
		return false;
	}

void CSystemEventHandler::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) const

//	OnWriteToStream
//
//	CSpaceObject	m_pHandler
//	Metric			m_rMaxRange2

	{
	pSystem->WriteObjRefToStream(m_pHandler, pStream);
	pStream->Write((char *)&m_rMaxRange2, sizeof(Metric));
	}
