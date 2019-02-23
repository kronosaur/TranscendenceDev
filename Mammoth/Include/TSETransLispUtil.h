//	TSETransLispUtil.h
//
//	Transcendence Lisp Utilities
//	Copyright 2018 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	CCodeChainConvert ----------------------------------------------------------
//
//	Helper class to convert from ICCItem to various types.

class CTLispConvert
	{
	public:
		enum ETypes 
			{
			typeNil,

			typeShipClass,
			typeSpaceObject,
			};

		static ETypes ArgType (ICCItem *pItem, ETypes iDefaultType, ICCItem **retpValue = NULL);
		static DWORD AsImageDesc (ICCItem *pItem, RECT *retrcRect);
		inline static bool AsOption (ICCItem *pItem, const CString &sOption) { return (pItem && pItem->GetBooleanAt(sOption)); }
		static bool AsScreenSelector (ICCItem *pItem, CDockScreenSys::SSelector *retSelector = NULL);
		static ICCItemPtr CreateCurrencyValue (CurrencyValue Value);
		static ICCItemPtr CreatePowerResultMW (int iPower);
		static ICCItemPtr GetElementAt (ICCItem *pItem, const CString &sField);
	};

class CPropertyCompare
	{
	public:
		bool Eval (ICCItem *pPropertyValue) const;
		inline const CString &GetProperty (void) const { return m_sProperty; }
		bool Parse (const CString &sExpression, CString *retsError = NULL);

	private:
		enum EOperator
			{
			opNone,

			opEqual,
			opNotEqual,
			opGreaterThan,
			opLessThan,
			opGreaterThanOrEqual,
			opLessThanOrEqual,
			opNonNil,
			};

		inline bool IsOperatorChar (char chChar) { return (chChar == '=' || chChar == '!' || chChar == '>' || chChar == '<'); }

		CString m_sProperty;
		EOperator m_iOp = opNone;
		ICCItemPtr m_pValue;
	};
