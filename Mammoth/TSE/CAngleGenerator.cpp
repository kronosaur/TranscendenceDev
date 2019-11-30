//	CAngleGenerator.cpp
//
//	CAngleGenerator class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ALIGNED_RANDOM_ANGLE			CONSTLIT("alignedRandom")
#define RANDOM_ANGLE					CONSTLIT("random")
#define EQUIDISTANT_ANGLE				CONSTLIT("equidistant")
#define INCREMENTING_ANGLE				CONSTLIT("incrementing")
#define MIN_SEPARATION_ANGLE			CONSTLIT("minSeparation")

#define INCLINATION_ATTRIB				CONSTLIT("inclination")
#define ROTATION_ATTRIB					CONSTLIT("rotation")

ALERROR CAngleGenerator::Generate (SSystemCreateCtx &Ctx, const CString &sAngle, int iCount, Metric *pAngles, int iJitter)

//	Generate
//
//	Generates random angles based on the angle type. Jitter is a value from 0-100
//	indicating a jitter in angle generation. Our caller will increase jitter to
//	try to find non-overlapping patterns.
//
//	SYNTAX
//
//	"n"							Position object at given angle
//	"{dice}"					Roll angle to determine position
//
//	"equidistant"				Start at random angle and distribute evenly.
//	"equidistant:n"				Start at angle n and distribute objects evenly.
//	"equidistant:{dice}"		Start at random angle and offset each position
//									by given param.
//
//	"incrementing:{dice}"		Start at random angle and increment angle by
//									param for each subsequent object.
//
//	"minSeparation:{dice}"		Random, but not closer than param (in degrees)

	{
	ALERROR error;
	int i;

	//	Separate value after colon

	char *pString = sAngle.GetASCIIZPointer();
	char *pColon = pString;
	while (pColon && *pColon != ':' && *pColon != '\0')
		pColon++;

	CString sKeyword;
	CString sValue;
	if (pColon && *pColon == ':')
		{
		sKeyword = CString(pString, pColon - pString);
		sValue = CString(pColon+1);
		}
	else
		sKeyword = sAngle;

	//	Generate list of angles

	if (strEquals(sKeyword, ALIGNED_RANDOM_ANGLE))
		{
		Metric rValue = COrbit::RandomAngle();
		for (i = 0; i < iCount; i++)
			pAngles[i] = rValue;
		}
	else if (strEquals(sKeyword, RANDOM_ANGLE))
		{
		for (i = 0; i < iCount; i++)
			pAngles[i] = COrbit::RandomAngle();
		}
	else if (strEquals(sKeyword, MIN_SEPARATION_ANGLE))
		{
		DiceRange SepRange;
		if (error = SepRange.LoadFromXML(sValue))
			{
			Ctx.sError = CONSTLIT("Invalid minSeparation range in orbitals");
			return error;
			}

		Metric rMinSep = mathDegreesToRadians(SepRange.Roll());

		for (i = 0; i < iCount; i++)
			{
			bool bAngleIsOK;
			int iTries = RANDOM_ANGLE_MAX_TRIES;

			do 
				{
				pAngles[i] = COrbit::RandomAngle();
				bAngleIsOK = true;

				for (int k = 0; k < i; k++)
					{
					if (Absolute(pAngles[i] - pAngles[k]) < rMinSep
							|| pAngles[i] + (2.0 * PI) - pAngles[k] < rMinSep
							|| pAngles[k] + (2.0 * PI) - pAngles[i] < rMinSep)
						{
						bAngleIsOK = false;
						break;
						}
					}
				}
			while (iTries-- > 0 && !bAngleIsOK);
			}
		}
	else if (strEquals(sKeyword, EQUIDISTANT_ANGLE))
		{
		DiceRange OffsetRange(0, 0, 0);
		int iStart;

		//	If we have a parameter, then treat that as an angle start

		if (!sValue.IsBlank())
			{
			if (error = OffsetRange.LoadFromXML(sValue))
				{
				Ctx.sError = CONSTLIT("Invalid equidistant range in orbitals");
				return error;
				}

			//	If the offset is constant, the we assume it is a start angle.
			//	Otherwise, it is just an offset.

			if (OffsetRange.IsConstant())
				iStart = 0;
			else
				iStart = mathRandom(0, 3599);
			}

		//	Otherwise, we randomize the start angle.

		else
			iStart = mathRandom(0, 3599);

		int iSeparation = 3600 / iCount;
		for (i = 0; i < iCount; i++)
			{
			int iAngleJitter = (iJitter > 0 ? iJitter * mathRandom(-iSeparation / 2, iSeparation / 2) / 100 : 0);
			pAngles[i] = mathDegreesToRadians((Metric)OffsetRange.Roll() + ((3600 + iStart + iAngleJitter + iSeparation * i) % 3600) / 10.0);
			}
		}
	else if (strEquals(sKeyword, INCREMENTING_ANGLE))
		{
		DiceRange IncRange;
		if (error = IncRange.LoadFromXML(sValue))
			{
			Ctx.sError = CONSTLIT("Invalid increment range in orbitals");
			return error;
			}

		int iAngle = mathRandom(0, 359);

		for (i = 0; i < iCount; i++)
			{
			pAngles[i] = mathDegreesToRadians(iAngle % 360);
			iAngle += IncRange.Roll();
			}
		}
	else
		{
		DiceRange AngleRange;
		if (error = AngleRange.LoadFromXML(sAngle))
			{
			Ctx.sError = CONSTLIT("Invalid angle in orbitals");
			return error;
			}

		int iAngleJitter = (iJitter > 0 && AngleRange.IsConstant() ? iJitter * mathRandom(-20, 20) / 100 : 0);

		for (i = 0; i < iCount; i++)
			pAngles[i] = mathDegreesToRadians(iAngleJitter + AngleRange.Roll());
		}

	return NOERROR;
	}

ALERROR CAngleGenerator::Init (SSystemCreateCtx &Ctx, int iCount, const CString &sValue, Metric rDefault, int iJitter)

//	Init
//
//	Initializes with the given value.

	{
	//	If already initialized, nothing to do (this should never happen).

	if (m_Values.GetCount() > 0 || iCount <= 0)
		{ }

	//	If we have no value, then take defaults.

	else if (sValue.IsBlank())
		m_rDefault = rDefault;

	//	Otherwise, initialize from the value.

	else
		{
		m_Values.InsertEmpty(iCount);
		if (ALERROR error = Generate(Ctx, sValue, iCount, &m_Values[0], iJitter))
			return error;
		}

	return NOERROR;
	}

ALERROR CAngleGenerator::InitFromInclination (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc, int iCount, Metric rDefault)

//	InitFromInclination
//
//	Initializes from an inclination parameter.

	{
	CString sInclination;

	if (!Ctx.ZAdjust.sInclination.IsBlank())
		{
		if (ALERROR error = Init(Ctx, iCount, Ctx.ZAdjust.sInclination, rDefault))
			return error;

		//	Since we handled the adjustment here, we don't pass it down to our
		//	children. Otherwise, it might get applied twice (e.g., if we have
		//	sibling directives).

		Ctx.ZAdjust.sInclination = NULL_STR;
		}
	else if (XMLDesc.FindAttribute(INCLINATION_ATTRIB, &sInclination))
		{
		if (ALERROR error = Init(Ctx, iCount, sInclination, rDefault))
			return error;
		}
	else
		m_rDefault = rDefault;

	return NOERROR;
	}

ALERROR CAngleGenerator::InitFromRotation (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc, int iCount, Metric rDefault)

//	InitFromRotation
//
//	Initializes from a rotation parameter.

	{
	CString sAngleDesc;

	if (!Ctx.ZAdjust.sRotation.IsBlank())
		{
		if (ALERROR error = Init(Ctx, iCount, Ctx.ZAdjust.sRotation, rDefault))
			return error;

		//	Since we handled the adjustment here, we don't pass it down to our
		//	children. Otherwise, it might get applied twice (e.g., if we have
		//	sibling directives).

		Ctx.ZAdjust.sRotation = NULL_STR;
		}
	else if (XMLDesc.FindAttribute(ROTATION_ATTRIB, &sAngleDesc))
		{
		if (ALERROR error = Init(Ctx, iCount, sAngleDesc, rDefault))
			return error;
		}
	else
		m_rDefault = rDefault;

	return NOERROR;
	}
