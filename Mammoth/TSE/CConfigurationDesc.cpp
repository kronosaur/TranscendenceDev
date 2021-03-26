//	CConfigurationDesc.cpp
//
//	CConfigurationDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CONFIGURATION_TAG						CONSTLIT("Configuration")

#define AIM_TOLERANCE_ATTRIB					CONSTLIT("aimTolerance")
#define ALTERNATING_ATTRIB						CONSTLIT("alternating")
#define ANGLE_ATTRIB							CONSTLIT("angle")
#define CONFIGURATION_ATTRIB					CONSTLIT("configuration")
#define POS_ANGLE_ATTRIB						CONSTLIT("posAngle")
#define POS_RADIUS_ATTRIB						CONSTLIT("posRadius")

#define CONFIG_TYPE_DUAL						CONSTLIT("dual")
#define CONFIG_TYPE_WALL						CONSTLIT("wall")
#define CONFIG_TYPE_SPREAD2						CONSTLIT("spread2")
#define CONFIG_TYPE_SPREAD3						CONSTLIT("spread3")
#define CONFIG_TYPE_SPREAD5						CONSTLIT("spread5")
#define CONFIG_TYPE_DUAL_ALTERNATING			CONSTLIT("alternating")

int CConfigurationDesc::GetAimTolerance (int iFireDelay) const

//	GetAimTolerance
//
//	Returns the aim tolerance.

	{
	switch (m_iType)
		{
		case ctSingle:
			{
			if (iFireDelay > 10)
				return 2;
			else if (iFireDelay > 4)
				return 4;
			else
				return 6;
			}

		default:
			return m_iAimTolerance;
		}
	}

int CConfigurationDesc::GetShotCount (void) const

//	GetShotCount
//
//	Return the number of shots fired.

	{
	switch (m_iType)
		{
		case ctDual:
		case ctSpread2:
			return 2;

		case ctSpread3:
			return 3;

		case ctWall:
		case ctSpread5:
			return 5;

		case ctCustom:
			if (m_bCustomAlternating)
				return 1;
			else
				return m_Custom.GetCount();

		default:
			return 1;
		}
	}

bool CConfigurationDesc::IncPolarity (int iPolarity, int *retiNewPolarity) const

//	IncPolarity
//
//	If this is an alternating configuration, we return TRUE and initialize the
//	new polarity.

	{
	switch (m_iType)
		{
		case ctDualAlternating:
			if (retiNewPolarity)
				*retiNewPolarity = ((iPolarity + 1) % 2);
			break;

		case CConfigurationDesc::ctCustom:
			if (GetCustomConfigCount() == 0)
				return false;

			if (retiNewPolarity)
				*retiNewPolarity = ((iPolarity + 1) % GetCustomConfigCount());
			break;

		default:
			return false;
		}

	return true;
	}

ALERROR CConfigurationDesc::InitFromWeaponClassXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc, ETypes iDefault)

//	InitFromWeaponClassXML
//
//	Initialize

	{
	m_Custom.DeleteAll();
	m_bCustomAlternating = false;

	CString sConfig = Desc.GetAttribute(CONFIGURATION_ATTRIB);
	if (strEquals(sConfig, CONFIG_TYPE_DUAL))
		{
		m_iType = ctDual;
		m_iAimTolerance = 10;
		}
	else if (strEquals(sConfig, CONFIG_TYPE_WALL))
		{
		m_iType = ctWall;
		m_iAimTolerance = 45;
		}
	else if (strEquals(sConfig, CONFIG_TYPE_SPREAD2))
		{
		m_iType = ctSpread2;
		m_iAimTolerance = 20;
		}
	else if (strEquals(sConfig, CONFIG_TYPE_SPREAD3))
		{
		m_iType = ctSpread3;
		m_iAimTolerance = 30;
		}
	else if (strEquals(sConfig, CONFIG_TYPE_SPREAD5))
		{
		m_iType = ctSpread5;
		m_iAimTolerance = 30;
		}
	else if (strEquals(sConfig, CONFIG_TYPE_DUAL_ALTERNATING))
		{
		m_iType = ctDualAlternating;
		m_iAimTolerance = 10;
		}
	else
		{
		const CXMLElement *pConfig = Desc.GetContentElementByTag(CONFIGURATION_TAG);
		if (pConfig && pConfig->GetContentElementCount())
			{
			m_iType = ctCustom;

			int iCount = pConfig->GetContentElementCount();
			m_Custom.InsertEmpty(iCount);

			for (int i = 0; i < m_Custom.GetCount(); i++)
				{
				const CXMLElement *pShotConfig = pConfig->GetContentElement(i);
				m_Custom[i].Angle.LoadFromXML(pShotConfig->GetAttribute(ANGLE_ATTRIB));
				m_Custom[i].iPosAngle = AngleMod(pShotConfig->GetAttributeInteger(POS_ANGLE_ATTRIB));
				m_Custom[i].rPosRadius = g_KlicksPerPixel * pShotConfig->GetAttributeInteger(POS_RADIUS_ATTRIB);
				}

			//	Empirically we use double the value specified in the XML.

			m_iAimTolerance = 2 * pConfig->GetAttributeIntegerBounded(AIM_TOLERANCE_ATTRIB, 0, -1, 5);

			m_bCustomAlternating = pConfig->GetAttributeBool(ALTERNATING_ATTRIB);
			}
		else
			{
			m_iType = iDefault;
			m_iAimTolerance = 5;
			}
		}

	return NOERROR;
	}

bool CConfigurationDesc::IsDualPointOrigin (void) const

//	IsDualPointOrigin
//
//	Returns TRUE if all shots emanate from two points

	{
	switch (m_iType)
		{
		case ctSingle:
			return false;

		case ctDual:
		case ctDualAlternating:
			return true;

		case ctWall:
			return false;

		case ctSpread2:
		case ctSpread3:
		case ctSpread5:
			return false;

		case ctCustom:
			{
			if (m_Custom.GetCount() != 2)
				return false;
			else
				return (m_Custom[0].iPosAngle != m_Custom[1].iPosAngle);
			}

		default:
			{
			ASSERT(false);
			return false;
			}
		}
	}

bool CConfigurationDesc::IsSinglePointOrigin (void) const

//	IsSinglePointOrigin
//
//	Returns TRUE if all shots emanate from a single point.

	{
	switch (m_iType)
		{
		case ctSingle:
			return true;

		case ctDual:
		case ctDualAlternating:
			return false;

		case ctWall:
			return false;

		case ctSpread2:
		case ctSpread3:
		case ctSpread5:
			return true;

		case ctCustom:
			{
			for (int i = 0; i < m_Custom.GetCount(); i++)
				{
				if (m_Custom[i].rPosRadius != 0.0)
					return false;
				}

			return true;
			}

		default:
			{
			ASSERT(false);
			return false;
			}
		}
	}
