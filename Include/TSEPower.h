//	TSEPower.h
//
//	Classes and functions for powers.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CPower : public CDesignType
	{
	public:
		CPower (void);
		virtual ~CPower (void);

		inline ICCItem *GetCode (void) { return m_pCode; }
		inline int GetInvokeCost (void) { return m_iInvokeCost; }
		inline const CString &GetInvokeKey (void) { return m_sInvokeKey; }
		inline const CString &GetName (void) { return m_sName; }
		inline ICCItem *GetOnInvokedByPlayer (void) { return m_pOnInvokedByPlayer; }
		inline ICCItem *GetOnInvokedByNonPlayer (void) { return m_pOnInvokedByNonPlayer; }
		inline ICCItem *GetOnShow (void) { return m_pOnShow; }
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
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		CString m_sName;
		int m_iInvokeCost;
		CString m_sInvokeKey;

		ICCItem *m_pCode;					//	Invoke event
		ICCItem *m_pOnShow;					//	OnShow event checks if the power should show up in the Invoke menu
		ICCItem *m_pOnInvokedByPlayer;		//	OnInvokedByPlayer event checks if a player should be allowed to invoke
		ICCItem *m_pOnInvokedByNonPlayer;	//	OnInvokedByNonPlayer event checks if a non-player should be allowed to invoke
		ICCItem *m_pOnDestroyCheck;			//	OnDestroyCheck event fires before the player is about to be destroyed
	};

