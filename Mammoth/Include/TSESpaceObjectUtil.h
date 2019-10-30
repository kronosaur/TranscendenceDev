//	TSESpaceObjectUtil.h
//
//	Utilities used by the base CSpaceObject class.
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Item Event Handler

enum EItemEventDispatchTypes
	{
	dispatchFireEvent,
	dispatchCheckEnhancementLifetime,
	};

class CItemEventDispatcher
	{
	public:
		CItemEventDispatcher (void);
		~CItemEventDispatcher (void);

		void FireEvent (CSpaceObject *pSource, ECodeChainEvents iEvent)	{ if (m_pFirstEntry) FireEventFull(pSource, iEvent); }
		void FireOnDocked (CSpaceObject *pSource, CSpaceObject *pDockedAt) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		void FireUpdateEvents (CSpaceObject *pSource) { if (m_pFirstEntry) FireUpdateEventsFull(pSource); }
		void Init (CSpaceObject *pSource);

	private:
		struct SEntry
			{
			EItemEventDispatchTypes iType;				//	Type of entry
			CItem *pItem;								//	Pointer to item

			ECodeChainEvents iEvent;					//	Event (if dispatchFireEvent)
			SEventHandlerDesc Event;					//	Code (if dispatchFireEvent)

			DWORD dwEnhancementID;						//	ID of enhancement (if relevant)

			SEntry *pNext;
			};

		void AddEntry (const CString &sEvent, EItemEventDispatchTypes iType, const SEventHandlerDesc &Event, CItem *pItem, DWORD dwEnhancementID);
		SEntry *AddEntry (void);
		void FireEventFull (CSpaceObject *pSource, ECodeChainEvents iEvent);
		void FireUpdateEventsFull (CSpaceObject *pSource);
		void Refresh (CSpaceObject *pSource, SEntry *pFirst);
		void RemoveAll (void);

		SEntry *m_pFirstEntry;
	};

