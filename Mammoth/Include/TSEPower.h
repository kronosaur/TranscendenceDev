//	TSEPower.h
//
//	Classes and functions for powers.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CPower : public CDesignType
	{
	public:
		enum ECachedHandlers
			{
			//	This list must match CACHED_EVENTS array in CPower.cpp

			evtCode						= 0,
			evtOnShow					= 1,
			evtOnInvokedByPlayer		= 2,
			evtOnInvokedByNonPlayer		= 3,
			evtOnDestroyCheck			= 4,

			evtCount					= 5,
			};

		CPower (void);
		virtual ~CPower (void);

		inline bool FindEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

//		inline ICCItem *GetCode (void) { return m_pCode; }
		inline int GetInvokeCost (void) const { return m_iInvokeCost; }
		inline const CString &GetInvokeKey (void) const { return m_sInvokeKey; }
		inline const CString &GetName (void) const { return m_sName; }
//		inline ICCItem *GetOnInvokedByPlayer (void) { return m_pOnInvokedByPlayer; }
//		inline ICCItem *GetOnInvokedByNonPlayer (void) { return m_pOnInvokedByNonPlayer; }
//		inline ICCItem *GetOnShow (void) { return m_pOnShow; }
		void Invoke (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		void InvokeByPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		void InvokeByNonPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnShow (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker);

		//	CDesignType overrides
		static CPower *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designPower) ? (CPower *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designPower; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;

	private:
		void InitOldStyleEvent (ECachedHandlers iEvent, ICCItem *pCode);

		CString m_sName;
		int m_iInvokeCost;
		CString m_sInvokeKey;

		ICCItemPtr m_pCode;						//	Invoke event
		ICCItemPtr m_pOnShow;					//	OnShow event checks if the power should show up in the Invoke menu
		ICCItemPtr m_pOnInvokedByPlayer;		//	OnInvokedByPlayer event checks if a player should be allowed to invoke
		ICCItemPtr m_pOnInvokedByNonPlayer;		//	OnInvokedByNonPlayer event checks if a non-player should be allowed to invoke
		ICCItemPtr m_pOnDestroyCheck;			//	OnDestroyCheck event fires before the player is about to be destroyed

		SEventHandlerDesc m_CachedEvents[evtCount];
	};

