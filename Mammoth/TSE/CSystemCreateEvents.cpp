//	CSystemCreateEvents.cpp
//
//	CSystemCreateEvents class

#include "PreComp.h"

void CSystemCreateEvents::AddDeferredEvent (CSpaceObject *pObj, CExtension *pExtension, CXMLElement *pEventCode)

//	AddDeferredEvent
//
//	Adds a deferred event. The caller must guarantee the lifetime of pObj and pEventCode

	{
	SEventDesc *pEvent = m_Events.Insert();
	pEvent->pObj = pObj;
	pEvent->pExtension = pExtension;
	pEvent->pEventCode = pEventCode;
	}

ALERROR CSystemCreateEvents::FireDeferredEvent (const CString &sEvent, CString *retsError)

//	FireDeferredEvent
//
//	Fires the given event for all objects in the deferred list

	{
	int i;
	CCodeChainCtx Ctx(*g_pUniverse);

	for (i = 0; i < m_Events.GetCount(); i++)
		{
		if (strEquals(sEvent, m_Events[i].pEventCode->GetTag()))
			{
			//	Link the code

			ICCItemPtr pCode = Ctx.LinkCode(m_Events[i].pEventCode->GetContentText(0));
			if (pCode->IsError())
				{
				if (retsError)
					*retsError = pCode->GetStringValue();

				return ERR_FAIL;
				}

			//	Execute

			Ctx.SetExtension(m_Events[i].pExtension);
			Ctx.DefineContainingType(m_Events[i].pObj);
			Ctx.SaveAndDefineSourceVar(m_Events[i].pObj);

			ICCItemPtr pResult = Ctx.RunCode(pCode);

			//	Check error

			if (pResult->IsError())
				{
				if (retsError)
					*retsError = pResult->GetStringValue();

				return ERR_FAIL;
				}
			}
		}

	return NOERROR;
	}
