//	TSEPower.h
//
//	Classes and functions for powers.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CPower : public CDesignType
	{
	public:
		enum class EEvent
			{
			//	This list must match CACHED_EVENTS array in CPower.cpp

			Code					= 0,
			OnShow					= 1,
			OnInvokedByPlayer		= 2,
			OnInvokedByNonPlayer	= 3,
			OnDestroyCheck			= 4,

			count					= 5,
			};

		CPower (void);
		virtual ~CPower (void);

		bool FindEventHandler (EEvent iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[(int)iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[(int)iEvent];
			return true;
			}

		const CObjectImageArray &GetImage (void) const;
		int GetInvokeCost (void) const { return m_iInvokeCost; }
		const CString &GetInvokeKey (void) const { return m_sInvokeKey; }
		const CString &GetName (void) const { return m_sName; }
		void Invoke (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		void InvokeByPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		void InvokeByNonPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnShow (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker);

		//	CDesignType overrides
		static CPower *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designPower) ? (CPower *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designPower; }
		virtual const CObjectImageArray &GetTypeSimpleImage (void) const override { return m_Image; }

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual void OnMarkImages (void) override;

	private:
		void InitOldStyleEvent (EEvent iEvent, ICCItem *pCode);

		CString m_sName;
		int m_iInvokeCost = 0;
		CString m_sInvokeKey;
		CObjectImageArray m_Image;				//	Image of power

		ICCItemPtr m_pCode;						//	Invoke event
		ICCItemPtr m_pOnShow;					//	OnShow event checks if the power should show up in the Invoke menu
		ICCItemPtr m_pOnInvokedByPlayer;		//	OnInvokedByPlayer event checks if a player should be allowed to invoke
		ICCItemPtr m_pOnInvokedByNonPlayer;		//	OnInvokedByNonPlayer event checks if a non-player should be allowed to invoke
		ICCItemPtr m_pOnDestroyCheck;			//	OnDestroyCheck event fires before the player is about to be destroyed

		SEventHandlerDesc m_CachedEvents[(int)EEvent::count];

		//	Property table

		static TPropertyHandler<CPower> m_PropertyTable;
	};
