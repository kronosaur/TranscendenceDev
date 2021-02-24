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
		static DWORD AsImageDesc (const ICCItem *pItem, RECT *retrcRect);
		static CSpaceObject *AsObject (const ICCItem *pItem);
		static bool AsOption (ICCItem *pItem, const CString &sOption) { return (pItem && pItem->GetBooleanAt(sOption)); }
		static CG32bitPixel AsRGB (const ICCItem *pItem, CG32bitPixel rgbDefault = CG32bitPixel(255, 255, 255));
		static bool AsScreenSelector (ICCItem *pItem, CDockScreenSys::SSelector *retSelector = NULL);
		static ICCItemPtr CreateCurrencyValue (CurrencyValue Value);
		static ICCItemPtr CreateItem (const CItem &Value);
		static ICCItemPtr CreateObject (const CSpaceObject *pObj);
		static ICCItemPtr CreateObjectList (const CSpaceObjectList &List);
		static ICCItemPtr CreatePowerResultMW (int iPower);
		static ICCItemPtr CreateIntegerList (const TArray<int> &List);
		static ICCItemPtr CreateStringList (const TArray<CString> &List);
		static ICCItemPtr CreateVector (const CVector &vValue);
		static ICCItemPtr GetElementAt (ICCItem *pItem, const CString &sField);
	};

class CPropertyCompare
	{
	public:
		bool Eval (ICCItem *pPropertyValue) const;
		inline const CString &GetProperty (void) const { return m_sProperty; }
		bool Parse (CCodeChainCtx &CCX, const CString &sExpression, CString *retsError = NULL);

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
			opInRange,
			opNonNil,
			};

		bool IsOperatorChar (char chChar) { return (chChar == '=' || chChar == '!' || chChar == '>' || chChar == '<' || chChar == '['); }
		static bool ParseValue (CCodeChainCtx &CCX, const char *&pPos, ICCItemPtr &pValue, CString *retsError = NULL);

		CString m_sProperty;
		EOperator m_iOp = opNone;
		ICCItemPtr m_pValue;
		ICCItemPtr m_pValue2;
	};
