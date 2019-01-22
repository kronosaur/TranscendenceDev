//	CEconomyType.cpp
//
//	CEconomyType class

#include "PreComp.h"

#define CURRENCY_ATTRIB							CONSTLIT("currency")
#define CONVERSION_ATTRIB						CONSTLIT("conversion")
#define ID_ATTRIB								CONSTLIT("id")

#define FIELD_BALANCED							CONSTLIT("balanced")
#define FIELD_LEVEL								CONSTLIT("level")
#define FIELD_NAME								CONSTLIT("name")
#define FIELD_STANDARD							CONSTLIT("standard")
#define FIELD_X									CONSTLIT("x")

static const CEconomyType *g_pDefaultEconomy = NULL;

const CEconomyType *CEconomyType::Default (void)

//	Default
//
//	Returns the default economy (credits)

	{
	if (g_pDefaultEconomy == NULL)
		g_pDefaultEconomy = CEconomyType::AsType(g_pUniverse->FindDesignType(DEFAULT_ECONOMY_UNID));

	return g_pDefaultEconomy;
	}

CurrencyValue CEconomyType::Exchange (const CEconomyType *pFrom, CurrencyValue iAmount) const

//	Exchange
//
//	Exchange from the given currency to this one.
//	NOTE: If pFrom is NULL, then we assume iAmount is in credits.

	{
	ASSERT(sizeof(CurrencyValue) == sizeof(LONGLONG));

	CurrencyValue iFromConversion = (pFrom ? pFrom->m_iCreditConversion : 100);

	//	If we're the same rate, then done

	if (iFromConversion == m_iCreditConversion)
		return iAmount;

	//	If converting from credits, then it is simpler

	else if (iFromConversion == 100)
		return 100 * iAmount / m_iCreditConversion;

	//	If converting to credits, then it is simpler

	else if (m_iCreditConversion == 100)
		return iFromConversion * iAmount / 100;

	//	Otherwise we need to do both conversion

	else
		{
		//	First convert to Commonwealth credits

		CurrencyValue iCredits = iFromConversion * iAmount / 100;

		//	Now convert back to this currency

		return 100 * iCredits / m_iCreditConversion;
		}
	}

CurrencyValue CEconomyType::ExchangeToCredits (const CEconomyType *pFrom, CurrencyValue iAmount)

//	ExchangeToCredits
//
//	Converts the given amount to credits.

	{
	return pFrom->m_iCreditConversion * iAmount / 100;
	}

CurrencyValue CEconomyType::ExchangeToCredits (const CCurrencyAndValue &Value)

//  ExchangeToCredits
//
//  Converts the given value to credits

    {
    return Value.GetCurrencyType()->m_iCreditConversion * Value.GetValue() / 100;
    }

bool CEconomyType::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Get a data field

	{
	if (strEquals(sField, FIELD_NAME))
		*retsValue = m_sCurrencySingular;
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

ALERROR CEconomyType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	return NOERROR;
	}

ALERROR CEconomyType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Load from XML

	{
	m_sSID = pDesc->GetAttribute(ID_ATTRIB);
	if (m_sSID.IsBlank())
		return ComposeLoadError(Ctx, CONSTLIT("Invalid ID"));

	//	Parse the currency name

	m_sCurrencyName = pDesc->GetAttribute(CURRENCY_ATTRIB);
	if (m_sCurrencyName.IsBlank())
		return ComposeLoadError(Ctx, CONSTLIT("Invalid currency name"));

	m_sCurrencySingular = CLanguage::ParseNounForm(m_sCurrencyName, NULL_STR, 0, false, true);
	if (m_sCurrencySingular.IsBlank())
		return ComposeLoadError(Ctx, CONSTLIT("Invalid singular form of currency name"));

	m_sCurrencyPlural = CLanguage::ParseNounForm(m_sCurrencyName, NULL_STR, 0, true, true);
	if (m_sCurrencyPlural.IsBlank())
		return ComposeLoadError(Ctx, CONSTLIT("Invalid plural form of currency name"));

	//	Get the conversion rate

	m_iCreditConversion = pDesc->GetAttributeIntegerBounded(CONVERSION_ATTRIB, 1, -1, 100);

	return NOERROR;
	}

CString CEconomyType::RinHackGet (CSpaceObject *pObj)

//	RinHackGet
//
//	In previous version we used to store rin as object data on the player ship
//	This hack returns it from the proper place

	{
	CShip *pPlayerShip = pObj->AsShip();
	IShipController *pController = (pPlayerShip ? pPlayerShip->GetController() : NULL);
	CCurrencyBlock *pMoney = (pController ? pController->GetCurrencyBlock() : NULL);
	return (pMoney ? strFromInt((int)pMoney->GetCredits(CONSTLIT("rin"))) : NULL_STR);
	}

CurrencyValue CEconomyType::RinHackInc (CSpaceObject *pObj, CurrencyValue iInc)

//	RinHackInc
//
//	Increment rin

	{
	CShip *pPlayerShip = pObj->AsShip();
	IShipController *pController = (pPlayerShip ? pPlayerShip->GetController() : NULL);
	CCurrencyBlock *pMoney = (pController ? pController->GetCurrencyBlock() : NULL);
	return (pMoney ? pMoney->IncCredits(CONSTLIT("rin"), iInc) : 0);
	}

void CEconomyType::RinHackSet (CSpaceObject *pObj, const CString &sData)

//	RinHackSet
//
//	Sets rin

	{
	CShip *pPlayerShip = pObj->AsShip();
	IShipController *pController = (pPlayerShip ? pPlayerShip->GetController() : NULL);
	CCurrencyBlock *pMoney = (pController ? pController->GetCurrencyBlock() : NULL);
	if (pMoney)
		pMoney->SetCredits(CONSTLIT("rin"), strToInt(sData, 0));
	}

//	CEconomyTypeRef ------------------------------------------------------------

ALERROR CEconomyTypeRef::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind the design

	{
	if (m_sUNID.IsBlank())
		{
		if (g_pDefaultEconomy == NULL)
			g_pDefaultEconomy = CEconomyType::AsType(g_pUniverse->FindDesignType(DEFAULT_ECONOMY_UNID));

		m_pType = g_pDefaultEconomy;
		}
	else
		{
		m_pType = GetEconomyTypeFromString(m_sUNID);
		if (m_pType == NULL)
			{
			if (Ctx.pType && Ctx.pType->GetAPIVersion() < 26)
				return NOERROR;

			Ctx.sError = strPatternSubst(CONSTLIT("Unable to find economy type: %s"), m_sUNID);
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

void CEconomyTypeRef::Set (DWORD dwUNID)

//	Set
//
//	Set

	{
	m_pType = CEconomyType::AsType(g_pUniverse->FindDesignType(dwUNID));
	}

//	CCurrencyAndValue ----------------------------------------------------------

CCurrencyAndValue::CCurrencyAndValue (CurrencyValue iValue, const CEconomyType *pCurrency)

//	CCurrencyAndValue constructor

	{
	m_iValue = iValue;
	m_pCurrency.Set(pCurrency);
	}

void CCurrencyAndValue::Add (const CCurrencyAndValue &Value)

//	Add
//
//	Adds, converting currency if necessary

	{
	m_iValue += m_pCurrency->Exchange(Value);
	}

ALERROR CCurrencyAndValue::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return m_pCurrency.Bind(Ctx);
	}

CurrencyValue CCurrencyAndValue::GetCreditValue (void) const

//	GetCreditValue
//
//	Returns the value in credits (converting as appropriate).

	{
	return CEconomyType::ExchangeToCredits(*this);
	}

CString CCurrencyAndValue::GetSID (void) const

//	GetSID
//
//	Returns the string ID for the currency type.

	{
	return (m_pCurrency ? m_pCurrency->GetSID() : NULL_STR);
	}

ALERROR CCurrencyAndValue::InitFromXMLAndDefault (SDesignLoadCtx &Ctx, const CString &sDesc, const CCurrencyAndValue &Default, int iDefaultLevel)

//	InitFromXMLAndDefault
//
//	Initializes from a string.

	{
	return CCurrencyValueDesc::Parse(Ctx, sDesc, Default, iDefaultLevel, *this);
	}

//	CCurrencyAndRange ----------------------------------------------------------

ALERROR CCurrencyAndRange::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind

	{
	return m_pCurrency.Bind(Ctx);
	}

ALERROR CCurrencyAndRange::InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;

	//	Handle blank

	if (sDesc.IsBlank())
		{
		m_pCurrency.LoadUNID(NULL_STR);
		m_Value.SetConstant(0);
		return NOERROR;
		}

	//	Look for a colon separator

	char *pPos = sDesc.GetASCIIZPointer();
	char *pStart = pPos;
	while (*pPos != '\0' && *pPos != ':')
		pPos++;

	//	If found, then take the first part as the currency
	//	and the second part as value.

	CString sCurrency;
	CString sValue;
	if (*pPos == ':')
		{
		sCurrency = CString(pStart, pPos - pStart);
		sValue = CString(pPos + 1);
		}

	//	Otherwise, assume credits (blank string means credits)

	else
		sValue = sDesc;

	//	Load the currency type

	m_pCurrency.LoadUNID(sCurrency);

	//	Load the range

	if (error = m_Value.LoadFromXML(sValue))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid dice range: %s"), sValue);
		return ERR_FAIL;
		}

	//	Done

	return NOERROR;
	}

//	CCurrencyValueDesc ---------------------------------------------------------

ALERROR CCurrencyValueDesc::InitFromXMLAndDefault (SDesignLoadCtx &Ctx, const CString &sDesc, const CCurrencyAndValue &Default, int iDefaultLevel)

//	InitFromXMLAndDefault
//
//	Initializes from a string.

	{
	return CCurrencyValueDesc::Parse(Ctx, sDesc, Default, iDefaultLevel, m_Value, &m_iSpecial);
	}

ALERROR CCurrencyValueDesc::Parse (SDesignLoadCtx &Ctx, const CString &sDesc, const CCurrencyAndValue &Default, int iDefaultLevel, CCurrencyAndValue &retValue, ESpecialValues *retiSpecial)

//	InitFromXMLAndDefault
//
//	Initializes from a string. We parse strings of the following forms:
//
//	+10						+10% above default value
//	-10						-10% off default value
//	123						123 credits
//	rin:100					100 rin
//	standard				Standard treasure value for iDefaultLevel
//	standard:level=3		Standard treasure value for level 3
//	standard:x=1.5			1.5 times standard treasure value for iDefaultLevel 
//	standard:level=3:x=1.5	1.5 times standard treasure value for level 3

	{
	//	Pre-initialize

	if (retiSpecial) *retiSpecial = specialNone;

	//	If blank, use the default

	if (sDesc.IsBlank())
		{
		retValue = Default;
		return NOERROR;
		}

	//	See if we start with '+' or '-', which means we are adjusting (in % terms) 
	//	from the default.

	char *pPos = sDesc.GetASCIIZPointer();
	if ((*pPos == '+' || *pPos == '-') && !Default.IsEmpty())
		{
		bool bFailed;
		int iBonus = strToInt(sDesc, 0, &bFailed);
		if (bFailed || iBonus < -100)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid currency adjustment: %s"), sDesc);
			return ERR_FAIL;
			}

		//	Start with default and adjust

		retValue = Default;
		retValue.Adjust(100 + iBonus);

		//	Done

		return NOERROR;
		}

	//	Look for a colon separator

	char *pStart = pPos;
	while (*pPos != '\0' && *pPos != ':')
		pPos++;

	//	If found, then take the first part as the currency
	//	and the second part as value.

	CString sCurrency;
	CString sValue;
	if (*pPos == ':')
		{
		sCurrency = CString(pStart, pPos - pStart);
		sValue = CString(pPos + 1);
		}

	//	Otherwise, assume credits (blank string means credits)

	else
		sValue = sDesc;

	//	If the value is "standard" then we take the standard treasure value
	//	for the level.

	if (strEquals(sValue, FIELD_STANDARD))
		{
		//	If we don't have a default level, then use default

		if (iDefaultLevel == 0)
			retValue = Default;

		//	Otherwise, initialize from standard

		else
			{
			retValue = CCurrencyAndValue();
			retValue.SetValue(CItemType::GetStdStats(iDefaultLevel).TreasureValue);
			if (retiSpecial) *retiSpecial = specialStdTreasure;
			}
		}

	//	If the value is "balanced" then we take the standard treasure value
	//	for the level.

	else if (strEquals(sValue, FIELD_BALANCED))
		{
		//	If we don't have a default level, then use default

		if (iDefaultLevel == 0)
			retValue = Default;

		//	Otherwise, initialize from standard. Callers will see the special
		//	type and adjust the value accordingly.

		else
			{
			retValue = CCurrencyAndValue();
			retValue.SetValue(CItemType::GetStdStats(iDefaultLevel).TreasureValue);
			if (retiSpecial) *retiSpecial = specialBalancedTreasure;
			}
		}

	//	If the currency starts with "standard" then we're specifying a standard 
	//	treasure value.

	else if (strStartsWith(sCurrency, FIELD_STANDARD))
		{
		int iLevel = iDefaultLevel;
		Metric rMultiplier = 1.0;

		//	Parse various modifiers

		char *pPos = sValue.GetASCIIZPointer();
		while (*pPos != '\0')
			{
			//	Skip whitespace

			while (strIsWhitespace(pPos))
				pPos++;

			//	Modifier

			char *pStart = pPos;
			while (*pPos != '=' && *pPos != '\0')
				pPos++;

			if (*pPos != '=')
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Invalid currency syntax: %s"), sDesc);
				return ERR_FAIL;
				}

			CString sModifier(pStart, (int)(pPos - pStart));

			//	Value

			pPos++;
			bool bError;
			Metric rValue = strParseDouble(pPos, 0.0, &pPos, &bError);
			if (bError)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Invalid currency syntax: %s"), sDesc);
				return ERR_FAIL;
				}

			//	See which modifier we have

			if (strEquals(sModifier, FIELD_LEVEL))
				iLevel = (int)rValue;
			else if (strEquals(sModifier, FIELD_X))
				rMultiplier = rValue;
			else
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Invalid currency syntax: %s"), sDesc);
				return ERR_FAIL;
				}

			//	Skip until the next modifier

			while (*pPos != '\0' && !strIsAlpha(pPos))
				pPos++;
			}

		//	Initialize

		retValue = CCurrencyAndValue();
		if (rMultiplier != 1.0)
			retValue.SetValue((CurrencyValue)Max(0.0, rMultiplier * CItemType::GetStdStats(iLevel).TreasureValue));
		else
			retValue.SetValue(CItemType::GetStdStats(iLevel).TreasureValue);

		if (retiSpecial) *retiSpecial = specialStdTreasure;
		}

	//	Otherwise, we have a currency and a value

	else
		{
		//	Load the currency type

		retValue.SetCurrencyType(sCurrency);

		//	Load the value

		bool bFailed;
		retValue.SetValue(strToInt(sValue, 0, &bFailed));
		if (bFailed)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid currency value: %s"), sValue);
			return ERR_FAIL;
			}
		}

	//	Done

	return NOERROR;
	}

