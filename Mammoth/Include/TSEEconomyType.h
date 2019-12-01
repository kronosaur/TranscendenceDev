//	TSEEconomyType.h
//
//	Classes and functions for economy type.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CEconomyType : public CDesignType
	{
	public:
		CEconomyType (void) { }

		CurrencyValue Exchange (const CEconomyType *pFrom, CurrencyValue iAmount) const;
		inline CurrencyValue Exchange (const CCurrencyAndValue &Value) const { return Exchange(Value.GetCurrencyType(), Value.GetValue()); }
		inline CCurrencyAndValue ExchangeFrom (const CEconomyType *pFrom, CurrencyValue iAmount) const { return CCurrencyAndValue(Exchange(pFrom, iAmount), this); }
		inline CCurrencyAndValue ExchangeFrom (const CCurrencyAndValue &Value) const { return CCurrencyAndValue(Exchange(Value), this); }
		inline CurrencyValue GetCreditConversion (void) const { return m_iCreditConversion; }
		inline const CString &GetCurrencyNamePlural (void) const { return m_sCurrencyPlural; }
		inline const CString &GetCurrencyNameSingular (void) const { return m_sCurrencySingular; }
		inline const CString &GetSID (void) const { return m_sSID; }
		inline bool IsCreditEquivalent (void) const { return (m_iCreditConversion == 100); }

		static CurrencyValue ExchangeToCredits (const CEconomyType *pFrom, CurrencyValue iAmount);
		static CurrencyValue ExchangeToCredits (const CCurrencyAndValue &Value) { return ExchangeToCredits(Value.GetCurrencyType(), Value.GetValue()); }
		static CString RinHackGet (CSpaceObject *pObj);
		static CurrencyValue RinHackInc (CSpaceObject *pObj, CurrencyValue iInc);
		static void RinHackSet (CSpaceObject *pObj, const CString &sData);

		//	CDesignType overrides
		static CEconomyType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEconomyType) ? (CEconomyType *)pType : NULL); }
		static const CEconomyType *AsType (const CDesignType *pType) { return ((pType && pType->GetType() == designEconomyType) ? (const CEconomyType *)pType : NULL); }
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

