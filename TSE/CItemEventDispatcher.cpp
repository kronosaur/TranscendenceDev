//	CItemEventDispatcher.cpp
//
//	CItemEventDispatcher object

#include "PreComp.h"

#define ON_AI_UPDATE_EVENT						CONSTLIT("OnAIUpdate")
#define ON_UPDATE_EVENT							CONSTLIT("OnUpdate")

CItemEventDispatcher::CItemEventDispatcher (void) : m_pFirstEntry(NULL)

//	CItemEventDispatcher constructor

	{
	}

CItemEventDispatcher::~CItemEventDispatcher (void)

//	CItemEventDispatcher destructor

	{
	RemoveAll();
	}

void CItemEventDispatcher::AddEntry (const CString &sEvent, EItemEventDispatchTypes iType, const SEventHandlerDesc &Event, CItem *pItem, DWORD dwEnhancementID)

//	AddEntry
//
//	Adds an entry

	{
	ECodeChainEvents iEvent;

	if (strEquals(sEvent, ON_AI_UPDATE_EVENT))
		iEvent = eventOnAIUpdate;
	else if (strEquals(sEvent, ON_UPDATE_EVENT))
		iEvent = eventOnUpdate;
	else
		iEvent = eventNone;

	if (iEvent != eventNone)
		{
		SEntry *pEntry = AddEntry();
		pEntry->iType = iType;
		pEntry->iEvent = iEvent;
		pEntry->Event = Event;
		pEntry->pItem = pItem;
		pEntry->dwEnhancementID = dwEnhancementID;
		}
	}

CItemEventDispatcher::SEntry *CItemEventDispatcher::AddEntry (void)

//	AddEntry
//
//	Adds a new entry to the beginning of the list

	{
	SEntry *pEntry = new SEntry;
	pEntry->pNext = m_pFirstEntry;
	m_pFirstEntry = pEntry;
	return pEntry;
	}

void CItemEventDispatcher::Init (CSpaceObject *pSource)

//	Init
//
//	Initializes the dispatcher from the item list

	{
	DEBUG_TRY

	int i;

	RemoveAll();

	CItemListManipulator ItemList(pSource->GetItemList());
	while (ItemList.MoveCursorForward())
		{
		CItem *pItem = ItemList.GetItemPointerAtCursor();

		//	Add entries for update events: OnAIUpdate and OnUpdate

		const CEventHandler *pEvents;
		TSortMap<CString, SEventHandlerDesc> FullEvents;
		pItem->GetType()->GetEventHandlers(&pEvents, &FullEvents);

		if (pEvents)
			{
			SEventHandlerDesc Event;
			Event.pExtension = pItem->GetType()->GetExtension();

			for (i = 0; i < pEvents->GetCount(); i++)
				{
				CString sEvent = pEvents->GetEvent(i, &Event.pCode);

				AddEntry(sEvent, dispatchFireEvent, Event, pItem, OBJID_NULL);
				}
			}
		else
			{
			for (i = 0; i < FullEvents.GetCount(); i++)
				AddEntry(FullEvents.GetKey(i), dispatchFireEvent, FullEvents[i], pItem, OBJID_NULL);
			}

		//	If this item has mods, see if we need to call any mods
		//	Add entries for OnEnhancementUpdate

		CItemType *pModType;
		if (pItem->HasMods()
				&& (pModType = pItem->GetMods().GetEnhancementType()))
			{
			//	If the mod has an expiration time, add a check here

			if (pItem->GetMods().GetExpireTime() != -1)
				{
				SEntry *pEntry = AddEntry();
				pEntry->iType = dispatchCheckEnhancementLifetime;
				pEntry->iEvent = eventNone;
				pEntry->Event.pExtension = NULL;
				pEntry->Event.pCode = NULL;
				pEntry->pItem = pItem;
				pEntry->dwEnhancementID = pItem->GetMods().GetID();
				}
			}
		}

	DEBUG_CATCH
	}

void CItemEventDispatcher::FireEventFull (CSpaceObject *pSource, ECodeChainEvents iEvent)

//	FireEventFull
//
//	Fires the given event

	{
	DEBUG_TRY

	CCodeChainCtx Ctx;
	bool bSavedVars = false;

	//	Fire event for all items that have it

	SEntry *pEntry = m_pFirstEntry;
	while (pEntry)
		{
		//	Skip NULL items (this can happen after a refresh).

		if (pEntry->pItem == NULL)
			{ }

		//	If this is the event, handle it.

		else if (pEntry->iEvent == iEvent)
			{
			//	We don't bother saving the variables unless we've got an event

			if (!bSavedVars)
				{
				Ctx.SaveAndDefineSourceVar(pSource);
				Ctx.SaveItemVar();
				bSavedVars = true;
				}

			//	Set the item

			Ctx.DefineItem(*pEntry->pItem);

			//	Run code

			ICCItem *pResult = Ctx.Run(pEntry->Event);
			if (pResult->IsError())
				pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x Event"), pEntry->pItem->GetType()->GetUNID()), pResult);
			Ctx.Discard(pResult);
			}

		//	Next

		pEntry = pEntry->pNext;

		//	If the item event list has changed, then we need to refresh it. This
		//	can happen if the code changed one of the items.

		if (pEntry && !pSource->IsItemEventListValid())
			Refresh(pSource, pEntry);
		}

	DEBUG_CATCH
	}

void CItemEventDispatcher::FireOnDocked (CSpaceObject *pSource, CSpaceObject *pDockedAt) const

//	FireOnDocked
//
//	Fire OnDocked event to all items that want it.

	{
	DEBUG_TRY

	int i;

	//	Make a list of all items that have an OnObjDestroyed event.

	TArray<CItem *> Items;
	CItemListManipulator Search(pSource->GetItemList());
	while (Search.MoveCursorForward())
		{
		CItem *pItem = Search.GetItemPointerAtCursor();
		if (pItem->GetType()->FindEventHandlerItemType(CItemType::evtOnDocked))
			Items.Insert(pItem);
		}

	//	Now call the event

	for (i = 0; i < Items.GetCount(); i++)
		{
		//	If items changed, then we skip any invalid pointers (they might have
		//	gotten destroyed).

		if (!pSource->IsItemEventListValid()
				&& !Search.IsItemPointerValid(Items[i]))
			continue;

		//	Fire event

		Items[i]->FireOnDocked(pSource, pDockedAt);
		}

	DEBUG_CATCH
	}

void CItemEventDispatcher::FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const

//	FireOnObjDestroyed
//
//	Fire OnObjDestroyed event.

	{
	DEBUG_TRY

	int i;

	//	Make a list of all items that have an OnObjDestroyed event.

	TArray<CItem *> Items;
	CItemListManipulator Search(pSource->GetItemList());
	while (Search.MoveCursorForward())
		{
		CItem *pItem = Search.GetItemPointerAtCursor();
		if (pItem->GetType()->FindEventHandler(CDesignType::evtOnObjDestroyed))
			Items.Insert(pItem);
		}

	//	Now call the event

	for (i = 0; i < Items.GetCount(); i++)
		{
		//	If items changed, then we skip any invalid pointers (they might have
		//	gotten destroyed).

		if (!pSource->IsItemEventListValid()
				&& !Search.IsItemPointerValid(Items[i]))
			continue;

		//	Fire event

		Items[i]->FireOnObjDestroyed(pSource, Ctx);
		}

	DEBUG_CATCH
	}

void CItemEventDispatcher::FireUpdateEventsFull (CSpaceObject *pSource)

//	FireUpdateEventsFull
//
//	Fires all events at item update time

	{
	DEBUG_TRY

	CCodeChainCtx Ctx;
	bool bSavedVars = false;

	//	Fire event for all items that have it

	SEntry *pEntry = m_pFirstEntry;
	while (pEntry)
		{
		//	Skip NULL items (this can happen after a refresh).

		if (pEntry->pItem == NULL)
			{ }

		//	Fire OnUpdate event

		else if (pEntry->iEvent == eventOnUpdate)
			{
			//	We don't bother saving the variables unless we've got an event

			if (!bSavedVars)
				{
				Ctx.SaveAndDefineSourceVar(pSource);
				Ctx.SaveItemVar();
				bSavedVars = true;
				}

			//	Set the item

			Ctx.DefineItem(*pEntry->pItem);

			//	Run code

			ICCItem *pResult = Ctx.Run(pEntry->Event);
			if (pResult->IsError())
				pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x Event"), pEntry->pItem->GetType()->GetUNID()), pResult);
			Ctx.Discard(pResult);
			}

		//	Check for enhancement expiration

		else if (pEntry->iType == dispatchCheckEnhancementLifetime)
			{
			//	Remove any expired mods on the item
			//	(TRUE flag means, "removed expired only")

			pSource->RemoveItemEnhancement(*pEntry->pItem, pEntry->dwEnhancementID, true);
			}

		//	Next

		pEntry = pEntry->pNext;

		//	If the item event list has changed, then we need to refresh it. This
		//	can happen if the code changed one of the items.

		if (pEntry && !pSource->IsItemEventListValid())
			Refresh(pSource, pEntry);
		}

	DEBUG_CATCH
	}

void CItemEventDispatcher::Refresh (CSpaceObject *pSource, SEntry *pFirst)

//	Refresh
//
//	Refresh the items starting at pFirst.

	{
	CItemListManipulator ItemList(pSource->GetItemList());

	//	Loop over all entries and update the item points.

	SEntry *pEntry = pFirst;
	while (pEntry)
		{
		if (!ItemList.IsItemPointerValid(pEntry->pItem))
			pEntry->pItem = NULL;

		//	Next

		pEntry = pEntry->pNext;
		}
	}

void CItemEventDispatcher::RemoveAll (void)

//	RemoveAll
//
//	Remove all entries

	{
	SEntry *pEntry = m_pFirstEntry;
	while (pEntry)
		{
		SEntry *pDelete = pEntry;
		pEntry = pEntry->pNext;
		delete pDelete;
		}

	m_pFirstEntry = NULL;
	}
