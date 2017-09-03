//	TSECurrency.h
//
//	Classes and definitions for currency.
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCurrencyAndValue
	{
	public:
		CCurrencyAndValue (CurrencyValue iValue = 0, CEconomyType *pCurrency = NULL);

		void Add (const CCurrencyAndValue &Value);
		inline void Adjust (int iAdj) { m_iValue = iAdj * m_iValue / 100; }
		ALERROR Bind (SDesignLoadCtx &Ctx);
		CurrencyValue GetCreditValue (void) const;
		inline CEconomyType *GetCurrencyType (void) const { return m_pCurrency; }
		inline CurrencyValue GetValue (void) const { return m_iValue; }
		inline void Init (CurrencyValue iValue, const CString &sUNID = NULL_STR) { m_iValue = iValue; m_pCurrency.LoadUNID(sUNID); }
		inline ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc) { return InitFromXMLAndDefault(Ctx, sDesc, CCurrencyAndValue()); }
		ALERROR InitFromXMLAndDefault (SDesignLoadCtx &Ctx, const CString &sDesc, const CCurrencyAndValue &Default);
		inline bool IsEmpty (void) const { return (m_pCurrency.IsEmpty() && m_iValue == 0); }
		inline void SetCurrencyType (CEconomyType *pType) { m_pCurrency.Set(pType); }
		inline void SetValue (CurrencyValue iValue) { m_iValue = iValue; }

	private:
		CurrencyValue m_iValue;
		CEconomyTypeRef m_pCurrency;
	};

class CCurrencyAndRange
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		inline CEconomyType *GetCurrencyType (void) const { return m_pCurrency; }
		inline const DiceRange &GetDiceRange (void) const { return m_Value; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc);
		inline CurrencyValue Roll (void) const { return m_Value.Roll(); }

	private:
		DiceRange m_Value;
		CEconomyTypeRef m_pCurrency;
	};

