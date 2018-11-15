//	TSEEconomyType.h
//
//	Classes and functions for economy type.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CEconomyType : public CDesignType
	{
	public:
		CEconomyType (void) { }

		CurrencyValue Exchange (CEconomyType *pFrom, CurrencyValue iAmount);
		inline CurrencyValue Exchange (const CCurrencyAndValue &Value) { return Exchange(Value.GetCurrencyType(), Value.GetValue()); }
		inline const CString &GetCurrencyNamePlural (void) { return m_sCurrencyPlural; }
		inline const CString &GetCurrencyNameSingular (void) { return m_sCurrencySingular; }
		inline const CString &GetSID (void) { return m_sSID; }
		inline bool IsCreditEquivalent (void) { return (m_iCreditConversion == 100); }

		static CEconomyType *Default (void);
		static CurrencyValue ExchangeToCredits (CEconomyType *pFrom, CurrencyValue iAmount);
		static CurrencyValue ExchangeToCredits (const CCurrencyAndValue &Value);
		static CString RinHackGet (CSpaceObject *pObj);
		static CurrencyValue RinHackInc (CSpaceObject *pObj, CurrencyValue iInc);
		static void RinHackSet (CSpaceObject *pObj, const CString &sData);

		//	CDesignType overrides
		static CEconomyType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEconomyType) ? (CEconomyType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const { if (retdwFlags) *retdwFlags = 0; return m_sCurrencyName; }
		virtual DesignTypes GetType (void) const override { return designEconomyType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		CString m_sSID;									//	String ID (e.g., "credit")
		CString m_sCurrencyName;						//	Annotated name
		CString m_sCurrencySingular;					//	Singular form: "1 credit"
		CString m_sCurrencyPlural;						//	Plural form: "10 credits"; "You don't have enough credits"

		CurrencyValue m_iCreditConversion;				//	Commonwealth Credits that 100 units of the currency is worth
	};

