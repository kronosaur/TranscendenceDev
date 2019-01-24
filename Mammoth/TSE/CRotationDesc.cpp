//	CRotationDesc.cpp
//
//	CRotationDesc class
//	Copyright (c) 2014 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define MANEUVER_TAG							CONSTLIT("Maneuver")
#define THRUSTER_TAG							CONSTLIT("Thruster")
#define THRUST_EFFECT_TAG						CONSTLIT("ThrustEffect")

#define MANEUVER_ATTRIB							CONSTLIT("maneuver")
#define MAX_ROTATION_RATE_ATTRIB				CONSTLIT("maxRotationRate")
#define ROTATION_ATTRIB							CONSTLIT("rotation")
#define ROTATION_ACCEL_ATTRIB					CONSTLIT("rotationAccel")
#define ROTATION_STOP_ACCEL_ATTRIB				CONSTLIT("rotationStopAccel")
#define ROTATION_COUNT_ATTRIB					CONSTLIT("rotationCount")
#define THRUST_EFFECT_ATTRIB					CONSTLIT("thrustEffect")

#define ROTATE_LEFT_MANEUVER					CONSTLIT("rotateLeft")
#define ROTATE_RIGHT_MANEUVER					CONSTLIT("rotateRight")

const Metric MANEUVER_MASS_FACTOR =				1.0;
const Metric MAX_INERTIA_RATIO =				9.0;

void CRotationDesc::Add (const CRotationDesc &Src)

//  Add
//
//  Combine Src with our stats as an enhancement. In general, we take the 
//  highest of the two parameters.

    {
    m_rDegreesPerTick = Max(m_rDegreesPerTick, Src.m_rDegreesPerTick);
    m_rAccelPerTick = Max(m_rAccelPerTick, Src.m_rAccelPerTick);
    m_rAccelPerTickStop = Max(m_rAccelPerTickStop, Src.m_rAccelPerTickStop);
    }

bool CRotationDesc::AdjForShipMass (Metric rHullMass, Metric rItemMass)

//  AdjForShipMass
//
//  Adjusts our metrics to account for ship mass.

    {
	//	If we have no mass, then there is nothing to do

    if (rHullMass == 0.0)
        return false;

    //  Compute the extra mass that we have. If we don't have much extra, then
    //  rotation is not affected

	Metric rExtraMass = (rItemMass - rHullMass) * MANEUVER_MASS_FACTOR;
	if (rExtraMass <= 0.0)
		return false;

	//	Otherwise, we slow down

	Metric rRatio = 1.0f / Min(MAX_INERTIA_RATIO, (1.0f + (rExtraMass / rHullMass)));
    m_rAccelPerTick *= rRatio;
    m_rAccelPerTickStop *= rRatio;
    m_rDegreesPerTick *= pow(rRatio, 0.3);

    //  Success!

    return true;
    }

ALERROR CRotationDesc::Bind (SDesignLoadCtx &Ctx, CObjectImageArray &Image)

//	Bind
//
//	Bind the design

	{
	//	If our rotation count is not set to default, and the image is set to
	//	defaults, then we take our count. [We do this for backwards compatibility,
	//	but newer extensions should always specify a rotation count in the
	//	image.]

	if (m_iCount != STD_ROTATION_COUNT && Image.GetRotationCount() == STD_ROTATION_COUNT)
		{
        //  NOTE: We can guarantee that m_iCount <= 360 because we check during
        //  XML load.

		Image.SetRotationCount(m_iCount);
		InitRotationCount(m_iCount);
		}

	//	Otherwise, make sure we match the image's rotation count.

	else
		{
		//	At bind time we take the rotation count from the image itself

		int iRotationCount = (!Image.IsEmpty() ? Image.GetRotationCount() : 0);
		if (iRotationCount < 0)
			{
			Ctx.sError = CONSTLIT("Image must specify valid rotation count.");
			return ERR_FAIL;
			}
        else if (iRotationCount > 360)
            {
			Ctx.sError = CONSTLIT("More than 360 rotation frames not supported.");
			return ERR_FAIL;
            }

		InitRotationCount(iRotationCount);
		}

	return NOERROR;
	}

ALERROR CRotationDesc::InitFromManeuverXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, Metric rDefaultDegreesPerTick)

//  InitFromManeuverXML
//
//  Initializes from a <Maneuver> element (or equivalent).

    {
	m_iManeuverability = 0;
	m_iCount = pDesc->GetAttributeIntegerBounded(ROTATION_COUNT_ATTRIB, 1, -1, STD_ROTATION_COUNT);

	//	Max rotation rate is in degrees per tick. Later we convert that to rotation frames per tick
	//	(but not until we figure out the number of rotation frames).

	m_rDegreesPerTick = pDesc->GetAttributeDoubleBounded(MAX_ROTATION_RATE_ATTRIB, 0.01, -1.0, rDefaultDegreesPerTick);

	//	Also init rotation acceleration

	m_rAccelPerTick = pDesc->GetAttributeDoubleBounded(ROTATION_ACCEL_ATTRIB, 0.01, -1.0, m_rDegreesPerTick);
	m_rAccelPerTickStop = pDesc->GetAttributeDoubleBounded(ROTATION_STOP_ACCEL_ATTRIB, 0.01, -1.0, m_rAccelPerTick);

    return NOERROR;
    }

ALERROR CRotationDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from an XML descriptor

	{
    ALERROR error;

	//	If we have a Maneuver element, then use that (and ignore other attributes)

	CXMLElement *pManeuver = pDesc->GetContentElementByTag(MANEUVER_TAG);
	if (pManeuver)
		{
        //  NOTE: For backwards compatibility we default degrees per tick to 18.0.
        //  But all <Maneuver> elements should define max rotation rate instead of
        //  relying on this.

        if (error = InitFromManeuverXML(Ctx, pManeuver, 360.0 / STD_ROTATION_COUNT))
            return error;
		}

	//	Otherwise we look for attributes on the root (this is backwards compatible
	//	with version prior to API 20

	else
		{
		m_iCount = pDesc->GetAttributeIntegerBounded(ROTATION_COUNT_ATTRIB, 1, -1, STD_ROTATION_COUNT);

		//	The original maneuverability value is the number of half-ticks that 
		//	we take per rotation frame.
		//
		//	NOTE: For compatibility we don't allow maneuverability less than 2, which was the 
		//	limit using the old method (1 tick delay).

		m_iManeuverability = pDesc->GetAttributeIntegerBounded(MANEUVER_ATTRIB, 2, -1, 2);

		//	Convert that to degrees per tick

		m_rDegreesPerTick = (m_iCount > 0 ? (STD_SECONDS_PER_UPDATE * 360.0) / (m_iCount * m_iManeuverability) : 0.0);

		//	Default acceleration is equal to rotation rate

		m_rAccelPerTick = m_rDegreesPerTick;
		m_rAccelPerTickStop = m_rDegreesPerTick;
		}

    if (m_iCount > 360)
        {
        Ctx.sError = CONSTLIT("More than 360 rotation frames not supported.");
        return ERR_FAIL;
        }

	return NOERROR;
	}

void CRotationDesc::InitRotationCount (int iCount)

//	InitRotationCount
//
//	Initialize count

	{
    ASSERT(iCount >= 0 && iCount <= 360);

	//	If we're in backwards compatibility mode and if we've got a different
	//	count, then we need to recompute our degrees per tick.

	if (m_iManeuverability && iCount != m_iCount && iCount > 0)
		{
		m_rDegreesPerTick = (STD_SECONDS_PER_UPDATE * 360.0) / (iCount * m_iManeuverability);
		m_rAccelPerTick = m_rDegreesPerTick;
		m_rAccelPerTickStop = m_rDegreesPerTick;
		}

	//	Initialize count

	m_iCount = iCount;
	}

void CRotationDesc::Interpolate (const CRotationDesc &From, const CRotationDesc &To, Metric rInterpolate)

//  Interpolate
//
//  Initializes with interpolation between From and To.

    {
    *this = From;
    }
