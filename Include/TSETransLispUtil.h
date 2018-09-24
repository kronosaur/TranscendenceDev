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
		static bool AsScreenSelector (ICCItem *pItem, CDockScreenSys::SSelector *retSelector = NULL);
		static ICCItemPtr CreateCurrencyValue (CCodeChain &CC, CurrencyValue Value);
	};
