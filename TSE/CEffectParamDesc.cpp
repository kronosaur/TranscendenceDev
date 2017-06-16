//	CEffectParamDesc.cpp
//
//	CEffectParamDesc class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CEffectParamDesc::CEffectParamDesc (EDataTypes iType, int iValue)

//	CEffectParamDesc constructor

	{
	switch (iType)
		{
		case typeColorConstant:
		case typeIntegerConstant:
			m_iType = iType;
			m_dwData = (DWORD)iValue;
			break;

		default:
			ASSERT(false);
			m_iType = typeNull;
			break;
		}
	}

CEffectParamDesc::~CEffectParamDesc (void)

//	CEffectParamDesc destructor

	{
	CleanUp();
	}

void CEffectParamDesc::CleanUp (void)

//	CleanUp
//
//	Clean up value

	{
	switch (m_iType)
		{
		case typeImage:
			delete m_pImage;
			break;

		case typeVectorConstant:
			delete m_pVector;
			break;
		}

	m_iType = typeNull;
	}

void CEffectParamDesc::Copy (const CEffectParamDesc &Src)

//	Copy
//
//	Copies from the source. We assume that we are in a pristine state
//	(otherwise, call CleanUp() first).

	{
	m_iType = Src.m_iType;

	switch (Src.m_iType)
		{
		case typeBoolConstant:
		case typeColorConstant:
		case typeIntegerConstant:
			m_dwData = Src.m_dwData;
			break;

		case typeImage:
			m_pImage = new CObjectImageArray(*Src.m_pImage);
			break;

		case typeIntegerDiceRange:
			m_DiceRange = Src.m_DiceRange;
			break;

		case typeStringConstant:
			m_sData = Src.m_sData;
			break;

		case typeVectorConstant:
			m_pVector = new CVector(*Src.m_pVector);
			break;
		}
	}

CGDraw::EBlendModes CEffectParamDesc::EvalBlendMode (CGDraw::EBlendModes iDefault) const

//	EvalBlendMode
//
//	Returns a blend mode

	{
	switch (m_iType)
		{
		case typeIntegerConstant:
			return (CGDraw::EBlendModes)Max((int)CGDraw::blendNormal, Min((int)m_dwData, (int)CGDraw::blendModeCount - 1));

		case typeIntegerDiceRange:
			return (CGDraw::EBlendModes)Max((int)CGDraw::blendNormal, Min((int)m_DiceRange.Roll(), (int)CGDraw::blendModeCount - 1));

		case typeStringConstant:
			{
			CGDraw::EBlendModes iMode = CGDraw::ParseBlendMode(m_sData);
			if (iMode == CGDraw::blendNone)
				return iDefault;

			return iMode;
			}

		default:
			return iDefault;
		}
	}

bool CEffectParamDesc::EvalBool (void) const

//	EvalBool
//
//	Returns a boolean

	{
	switch (m_iType)
		{
		case typeBoolConstant:
		case typeIntegerConstant:
			return (m_dwData != 0);

		case typeIntegerDiceRange:
			return (m_DiceRange.Roll() != 0);

		case typeStringConstant:
			return strEquals(m_sData, CONSTLIT("true"));

		default:
			return false;
		}
	}

CG32bitPixel CEffectParamDesc::EvalColor (CG32bitPixel rgbDefault) const

//	EvalColor
//
//	Returns the color

	{
	switch (m_iType)
		{
		case typeColorConstant:
			return CG32bitPixel::FromDWORD(m_dwData);

		default:
			return rgbDefault;
		}
	}

DiceRange CEffectParamDesc::EvalDiceRange (int iDefaultCount, int iDefaultSides, int iDefaultBonus) const

//	EvalDiceRange
//
//	Evaluates the value as a dice range.

	{
	switch (m_iType)
		{
		case typeNull:
			return DiceRange(iDefaultCount, iDefaultSides, iDefaultBonus);

		case typeIntegerConstant:
			return DiceRange(0, 0, (int)m_dwData);

		case typeIntegerDiceRange:
			return m_DiceRange;

		case typeStringConstant:
			{
			DiceRange Temp;
			if (Temp.LoadFromXML(m_sData) != NOERROR)
				Temp = DiceRange();

			return Temp;
			}

		default:
			return DiceRange(iDefaultCount, iDefaultSides, iDefaultBonus);
		}
	}

int CEffectParamDesc::EvalIdentifier (LPSTR *pIDMap, int iMax, int iDefault) const

//	EvalIdentifier
//
//	Evalues the value as an identifer (either a constant integer or a string,
//	which is looked up in the ID map).

	{
	switch (m_iType)
		{
		case typeIntegerConstant:
			return Max(0, Min((int)m_dwData, iMax));

		case typeIntegerDiceRange:
			return Max(0, Min((int)m_DiceRange.Roll(), iMax));

		case typeStringConstant:
			{
			DWORD dwID;
			if (!FindIdentifier(m_sData, pIDMap, &dwID))
				return iDefault;

			return (int)dwID;
			}

		default:
			return iDefault;
		}
	}

const CObjectImageArray &CEffectParamDesc::EvalImage (void) const

//	EvalImage
//
//	Returns an image

	{
	switch (m_iType)
		{
		case typeImage:
			return *m_pImage;

		default:
			return CObjectImageArray::Null();
		}
	}

int CEffectParamDesc::EvalInteger (void) const

//	EvalInteger
//
//	Returns the integer

	{
	switch (m_iType)
		{
		case typeIntegerConstant:
			return m_dwData;

		case typeIntegerDiceRange:
			return m_DiceRange.Roll();

		default:
			return 0;
		}
	}

int CEffectParamDesc::EvalIntegerBounded (int iMin, int iMax, int iDefault) const

//	EvalIntegerBounded
//
//	Returns the integer within a certain range

	{
	int iValue;

	switch (m_iType)
		{
		case typeBoolConstant:
		case typeIntegerConstant:
			iValue = (int)m_dwData;
			break;

		case typeIntegerDiceRange:
			iValue = m_DiceRange.Roll();
			break;

		default:
			return iDefault;
		}

	if (iValue == iDefault)
		return iDefault;

	else if (iMax < iMin)
		return Max(iValue, iMin);

	else
		return Max(Min(iValue, iMax), iMin);
	}

CString CEffectParamDesc::EvalString (void) const

//	EvalString
//
//	Returns a string

	{
	switch (m_iType)
		{
		case typeStringConstant:
			return m_sData;

		default:
			return NULL_STR;
		}
	}

CVector CEffectParamDesc::EvalVector (void) const

//	EvalVector
//
//	Returns a vector

	{
	switch (m_iType)
		{
		case typeVectorConstant:
			return *m_pVector;

		default:
			return CVector();
		}
	}

ALERROR CEffectParamDesc::InitColorFromXML (SDesignLoadCtx &Ctx, const CString &sValue)

//	InitColorFromXML
//
//	Initializes a color value

	{
	ASSERT(m_iType == typeNull);

	if (!sValue.IsBlank())
		{
		m_iType = typeColorConstant;
		m_dwData = ::LoadRGBColor(sValue).AsDWORD();
		}

	return NOERROR;
	}

bool CEffectParamDesc::FindIdentifier (const CString &sValue, LPSTR *pIDMap, DWORD *retdwID)

//	FindIdentifier
//
//	Finds the identifier in the table

	{
	//	Loop through the ID table looking for the identifier

	DWORD dwID = 0;
	LPSTR *pID = pIDMap;
	while (*pID != NULL)
		{
		if ((*pID)[0] != '\0' && strEquals(sValue, CString(*pID, -1, TRUE)))
			{
			if (retdwID)
				*retdwID = dwID;

			return true;
			}

		//	Next string in the table

		dwID++;
		pID++;
		}

	//	Not found

	return false;
	}

ALERROR CEffectParamDesc::InitBlendModeFromXML (SDesignLoadCtx &Ctx, const CString &sValue)

//	InitBlendModeFromXML
//
//	Initializes a blend mode

	{
	ASSERT(m_iType == typeNull);

	//	If the value is blank, then we leave as defaults

	if (sValue.IsBlank())
		{
		m_iType = typeNull;
		return NOERROR;
		}

	//	Convert.

	CGDraw::EBlendModes iMode = CGDraw::ParseBlendMode(sValue);
	if (iMode == CGDraw::blendNone)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid effect param blend mode: %s"), sValue);
		return ERR_FAIL;
		}

	m_iType = typeIntegerConstant;
	m_dwData = iMode;

	return NOERROR;
	}

ALERROR CEffectParamDesc::InitIdentifierFromXML (SDesignLoadCtx &Ctx, const CString &sValue, LPSTR *pIDMap)

//	InitIdentifierFromXML
//
//	Initializes an identifier

	{
	ASSERT(m_iType == typeNull);

	//	If the value is blank, then we leave as defaults

	if (sValue.IsBlank())
		{
		m_iType = typeNull;
		return NOERROR;
		}

	//	Loop through the ID table looking for the identifier

	if (!FindIdentifier(sValue, pIDMap, &m_dwData))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid effect param identifier: %s"), sValue);
		return ERR_FAIL;
		}

	m_iType = typeIntegerConstant;

	return NOERROR;
	}

ALERROR CEffectParamDesc::InitImageFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitImageFromXML
//
//	Initializes an image

	{
	ALERROR error;

	ASSERT(m_iType == typeNull);
	if (pDesc == NULL)
		return NOERROR;

	m_iType = typeImage;
	m_pImage = new CObjectImageArray;
	if (error = m_pImage->InitFromXML(Ctx, pDesc))
		{
		delete m_pImage;
		m_iType = typeNull;
		return error;
		}

	return NOERROR;
	}

ALERROR CEffectParamDesc::InitIntegerFromXML (SDesignLoadCtx &Ctx, const CString &sValue)

//	InitIntegerFromXML
//
//	Initializes an integer

	{
	ASSERT(m_iType == typeNull);

	if (!sValue.IsBlank())
		{
		if (m_DiceRange.LoadFromXML(sValue) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unable to parse dice range: %s"), sValue);
			return ERR_FAIL;
			}

		//	If the dice range is constant, then just use that

		if (m_DiceRange.IsConstant())
			{
			m_iType = typeIntegerConstant;
			m_dwData = m_DiceRange.Roll();
			}

		//	Otherwise we are variant

		else
			m_iType = typeIntegerDiceRange;
		}

	return NOERROR;
	}

ALERROR CEffectParamDesc::InitStringFromXML (SDesignLoadCtx &Ctx, const CString &sValue)

//	InitStringFromXML
//
//	Initializes a string

	{
	ASSERT(m_iType == typeNull);

	if (!sValue.IsBlank())
		{
		m_iType = typeStringConstant;
		m_sData = sValue;
		}

	return NOERROR;
	}

bool CEffectParamDesc::IsConstant (void)

//	IsConstant
//
//	Returns TRUE if the value is a constant

	{
	switch (m_iType)
		{
		case typeNull:
		case typeBoolConstant:
		case typeColorConstant:
		case typeImage:
		case typeIntegerConstant:
		case typeStringConstant:
		case typeVectorConstant:
			return true;

		default:
			return false;
		}
	}

void CEffectParamDesc::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read the parameter. NOTE: This only works for constant values.

	{
	CleanUp();

	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iType = (EDataTypes)dwLoad;

	switch (m_iType)
		{
		case typeNull:
			break;

		case typeBoolConstant:
			if (Ctx.dwVersion >= 99)
				Ctx.pStream->Read((char *)&m_dwData, sizeof(DWORD));
			else
				m_dwData = 0;
			break;

		case typeColorConstant:
		case typeIntegerConstant:
			Ctx.pStream->Read((char *)&m_dwData, sizeof(DWORD));
			break;

		case typeImage:
			m_pImage = new CObjectImageArray;
			m_pImage->ReadFromStream(Ctx);
			break;

		case typeIntegerDiceRange:
			if (Ctx.dwVersion >= 99)
				m_DiceRange.ReadFromStream(Ctx);
			break;

		case typeStringConstant:
			m_sData.ReadFromStream(Ctx.pStream);
			break;

		case typeVectorConstant:
			m_pVector = new CVector;
			Ctx.pStream->Read((char *)m_pVector, sizeof(CVector));
			break;
		}
	}

void CEffectParamDesc::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write parameter
//
//	DWORD			m_iType
//
//	DWORD
//	or
//	CString

	{
	DWORD dwSave = (DWORD)m_iType;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	switch (m_iType)
		{
		case typeNull:
			break;

		case typeBoolConstant:
		case typeColorConstant:
		case typeIntegerConstant:
			pStream->Write((char *)&m_dwData, sizeof(DWORD));
			break;

		case typeImage:
			m_pImage->WriteToStream(pStream);
			break;

		case typeIntegerDiceRange:
			m_DiceRange.WriteToStream(pStream);
			break;

		case typeStringConstant:
			m_sData.WriteToStream(pStream);
			break;

		case typeVectorConstant:
			pStream->Write((char *)m_pVector, sizeof(CVector));
			break;
		}
	}
