//	CDriveClass.cpp
//
//	CDriveClass class
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_LEVEL 				CONSTLIT("level")
#define FIELD_MAX_SPEED				CONSTLIT("maxSpeed")
#define FIELD_THRUST				CONSTLIT("thrust")
#define FIELD_POWER					CONSTLIT("power")

#define PROPERTY_DRIVE_POWER		CONSTLIT("drivePowerUse")
#define PROPERTY_MAX_SPEED			CONSTLIT("maxSpeed")
#define PROPERTY_POWER				CONSTLIT("power")
#define PROPERTY_THRUST				CONSTLIT("thrust")
#define PROPERTY_THRUST_TO_WEIGHT	CONSTLIT("thrustToWeight")

#define TAG_SCALING                 CONSTLIT("Scaling")

CDriveClass::CDriveClass (void) :
        m_pDesc(NULL),
        m_pDamagedDesc(NULL),
        m_pEnhancedDesc(NULL)

//  CDriveClass constructor

	{
	}

CDriveClass::~CDriveClass (void)

//  CDriveClass destructor

    {
    if (m_pDesc)
        delete[] m_pDesc;

    if (m_pDamagedDesc)
        delete[] m_pDamagedDesc;

    if (m_pEnhancedDesc)
        delete[] m_pEnhancedDesc;
    }

ALERROR CDriveClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDrive)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
    int i;
	CDriveClass *pDrive;

	pDrive = new CDriveClass;
	if (pDrive == NULL)
		return ERR_MEMORY;

	if (error = pDrive->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

    //  Figure out how many levels we need to allocate

    int iBaseLevel = pType->GetLevel();
    pDrive->m_iLevels = (pType->GetMaxLevel() - iBaseLevel) + 1;

    //  Allocate the normal descriptors

    pDrive->m_pDesc = new SScalableStats[pDrive->m_iLevels];

    //  If we've got a scaling element, then we use that.

    CXMLElement *pScalingDesc = pDesc->GetContentElementByTag(TAG_SCALING);
    if (pScalingDesc)
        {
        for (i = 0; i < pScalingDesc->GetContentElementCount(); i++)
            {
            CXMLElement *pStageDesc = pScalingDesc->GetContentElement(i);
            int iLevel = pStageDesc->GetAttributeInteger(FIELD_LEVEL);
            if (iLevel < iBaseLevel || iLevel >= iBaseLevel + pDrive->m_iLevels)
                {
                Ctx.sError = strPatternSubst(CONSTLIT("Invalid level: %d"), iLevel);
                return ERR_FAIL;
                }

            if (error = pDrive->InitStatsFromXML(Ctx, iLevel, pDrive->GetUNID(), pStageDesc, pDrive->m_pDesc[iLevel - iBaseLevel]))
                return error;
            }
        }

    //  Otherwise, we load a single descriptor

    else
        {
        ASSERT(pDrive->m_iLevels >= 1);
        if (error = pDrive->InitStatsFromXML(Ctx, iBaseLevel, pDrive->GetUNID(), pDesc, pDrive->m_pDesc[0]))
            return error;
        }

    //  Loop over all scales and see if all are initialized. If not, we 
    //  interpolate as appropriate.

    int iStartLevel = -1;
    int iEndLevel = 0;
    for (i = 0; i < pDrive->m_iLevels; i++)
        {
        SScalableStats &Stats = pDrive->m_pDesc[i];
        if (Stats.iLevel == -1)
            {
            //  If we don't have a starting level, we can't interpolate.

            if (iStartLevel == -1)
                {
                Ctx.sError = strPatternSubst(CONSTLIT("Unable to interpolate level %d."), i);
                return ERR_FAIL;
                }

            const SScalableStats &StartStats = pDrive->m_pDesc[iStartLevel];

            //  Initialize

            Stats.iLevel = iBaseLevel + i;

            //  Look for the end level, if necessary

            if (iEndLevel != -1 && iEndLevel <= i)
                {
                for (int j = i + 1; j < pDrive->m_iLevels; j++)
                    if (pDrive->m_pDesc[j].iLevel != -1)
                        {
                        iEndLevel = j;
                        break;
                        }

                //  If we couldn't find the end level, then we have none

                if (iEndLevel <= i)
                    iEndLevel = -1;
                }

            //  If we don't have an end level, then this level is the same as 
            //  the starting level (no interpolation).

            if (iEndLevel == -1)
                {
                Stats.DriveDesc = StartStats.DriveDesc;
                Stats.ManeuverDesc = StartStats.ManeuverDesc;
                }

            //  Otherwise, we interpolate between the two levels.

            else
                {
                const SScalableStats &EndStats = pDrive->m_pDesc[iEndLevel];

                //  Compute the number of steps. iSteps has to be > 0 because
                //  iStartLevel is always < i and iEndLevel is always > i.

                int iSteps = iEndLevel - iStartLevel;
                ASSERT(iSteps > 0);

                //  Interpolate

                Metric rInterpolate = (Metric)(i - iStartLevel) / iSteps;
                Stats.DriveDesc.Interpolate(StartStats.DriveDesc, EndStats.DriveDesc, rInterpolate);
                Stats.ManeuverDesc.Interpolate(StartStats.ManeuverDesc, EndStats.ManeuverDesc, rInterpolate);
                }
            }
        else
            iStartLevel = i;
        }

    if (error = pDrive->m_DriveDesc.InitFromXML(Ctx, pDesc, pDrive->GetUNID()))
        return error;

	//	Compute descriptor when damaged

    pDrive->m_DamagedDriveDesc = pDrive->m_DriveDesc;
    pDrive->m_DamagedDriveDesc.AdjMaxSpeed(0.75);
    pDrive->m_DamagedDriveDesc.AdjThrust(0.5);
    pDrive->m_DamagedDriveDesc.AdjPowerUse(1.3);
    pDrive->m_DamagedDriveDesc.SetInertialess(false);

    //  Compute descriptor when enhanced

    pDrive->m_EnhancedDriveDesc = pDrive->m_DriveDesc;
    pDrive->m_EnhancedDriveDesc.AdjMaxSpeed(1.1);
    pDrive->m_EnhancedDriveDesc.AdjThrust(1.2);
    pDrive->m_EnhancedDriveDesc.AdjPowerUse(0.9);

	//	Done

	*retpDrive = pDrive;

	return NOERROR;
	}

bool CDriveClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	if (strEquals(sField, FIELD_MAX_SPEED))
		*retsValue = strFromInt((int)((100.0 * m_DriveDesc.GetMaxSpeed() / LIGHT_SPEED) + 0.5));
	else if (strEquals(sField, FIELD_THRUST))
		*retsValue = strFromInt(m_DriveDesc.GetThrust());
	else if (strEquals(sField, FIELD_POWER))
		*retsValue = strFromInt(m_DriveDesc.GetPowerUse() * 100);
	else
		return false;

	return true;
	}

const CDriveClass::SScalableStats *CDriveClass::GetDesc (CItemCtx &Ctx) const

//	GetDesc
//
//	Returns the drive descriptor

	{
    if (m_pDesc == NULL)
        return NULL;

    CInstalledDevice *pDevice = Ctx.GetDevice();
    int iBaseLevel = m_pDesc[0].iLevel;

    //  Figure out if we want a scaled item

    int iIndex = Min(Max(0, (Ctx.GetItem().IsEmpty() ? 0 : Ctx.GetItem().GetLevel() - iBaseLevel)), m_iLevels - 1);

    //  If no device, then standard descriptor

	if (pDevice == NULL)
		return &m_pDesc[iIndex];

    //  If the device is damaged, then return damaged descriptor

    else if (pDevice->IsDamaged() || pDevice->IsDisrupted())
        {
        InitDamagedDesc();
		return &m_pDamagedDesc[iIndex];
        }

    //  If enhanced, then return enhanced descriptor

    else if (pDevice->IsEnhanced())
        {
        InitEnhancedDesc();
		return &m_pEnhancedDesc[iIndex];
        }

    //  Otherwise, standard descriptor.

	else
		return &m_pDesc[iIndex];
	}

ICCItem *CDriveClass::GetItemProperty (CItemCtx &Ctx, const CString &sProperty)

//	GetItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	const SScalableStats *pDesc = GetDesc(Ctx);
    if (pDesc == NULL)
        return CC.CreateNil();

	if (strEquals(sProperty, PROPERTY_MAX_SPEED))
		return CC.CreateInteger((int)((100.0 * pDesc->DriveDesc.GetMaxSpeed() / LIGHT_SPEED) + 0.5));

	else if (strEquals(sProperty, PROPERTY_THRUST))
		return CC.CreateInteger(pDesc->DriveDesc.GetThrust());
	
	else if (strEquals(sProperty, PROPERTY_POWER)
			|| strEquals(sProperty, PROPERTY_DRIVE_POWER))
		return CC.CreateInteger(pDesc->DriveDesc.GetPowerUse() * 100);

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::GetItemProperty(Ctx, sProperty);
	}

int CDriveClass::GetPowerRating (CItemCtx &Ctx) const

//	GetPowerRating
//
//	Get minimum reactor output for device

	{
	const SScalableStats *pDesc = GetDesc(Ctx);
    if (pDesc == NULL)
        return 0;

	return pDesc->DriveDesc.GetPowerUse();
	}

ICCItem *CDriveClass::GetDriveProperty (const CDriveDesc &Desc, const CString &sProperty)

//	GetDriveProperty
//
//	Returns property for a built-in drive.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_MAX_SPEED))
		return CC.CreateInteger((int)((100.0 * Desc.GetMaxSpeed() / LIGHT_SPEED) + 0.5));

	else if (strEquals(sProperty, PROPERTY_THRUST))
		return CC.CreateInteger(Desc.GetThrust());
	
	else if (strEquals(sProperty, PROPERTY_POWER)
			|| strEquals(sProperty, PROPERTY_DRIVE_POWER))
		return CC.CreateInteger(Desc.GetPowerUse() * 100);
	else
		return CC.CreateNil();
	}

void CDriveClass::InitDamagedDesc (void) const

//  InitDamagedDesc
//
//  Makes sure that the damaged descriptor is initialized.

    {
    int i;

    if (m_pDamagedDesc == NULL)
        {
        m_pDamagedDesc = new SScalableStats[m_iLevels];
        for (i = 0; i < m_iLevels; i++)
            {
            m_pDamagedDesc[i] = m_pDesc[i];
            m_pDamagedDesc[i].DriveDesc.AdjMaxSpeed(0.75);
            m_pDamagedDesc[i].DriveDesc.AdjThrust(0.5);
            m_pDamagedDesc[i].DriveDesc.AdjPowerUse(1.3);
            m_pDamagedDesc[i].DriveDesc.SetInertialess(false);
            }
        }
    }

void CDriveClass::InitEnhancedDesc (void) const

//  InitEnhancedDesc
//
//  Makes sure that the enhanced descriptor is initialized.

    {
    int i;

    if (m_pEnhancedDesc == NULL)
        {
        m_pEnhancedDesc = new SScalableStats[m_iLevels];
        for (i = 0; i < m_iLevels; i++)
            {
            m_pEnhancedDesc[i] = m_pDesc[i];
            m_pEnhancedDesc[i].DriveDesc.AdjMaxSpeed(1.1);
            m_pEnhancedDesc[i].DriveDesc.AdjThrust(1.2);
            m_pEnhancedDesc[i].DriveDesc.AdjPowerUse(0.9);
            }
        }
    }

ALERROR CDriveClass::InitStatsFromXML (SDesignLoadCtx &Ctx, int iLevel, DWORD dwUNID, CXMLElement *pDesc, SScalableStats &retStats)

//  InitStatsFromXML
//
//  Initialize stats

    {
    ALERROR error;

    retStats.iLevel = iLevel;

    //  Load drive desc

    if (error = retStats.DriveDesc.InitFromXML(Ctx, pDesc, dwUNID))
        return error;

    //  Load maneuverability desc

    //  Done

    return NOERROR;
    }

void CDriveClass::OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList)

//	OnAccumulateAttributes
//
//	Returns display attributes

	{
	const SScalableStats *pDesc = GetDesc(ItemCtx);
    if (pDesc == NULL)
        return;

	//	Inertialess

	if (pDesc->DriveDesc.IsInertialess())
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("inertialess")));
	}

bool CDriveClass::OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//  OnAccumulatePerformance
//
//  Modifies the performance of the ship.

    {
	const SScalableStats *pDesc = GetDesc(ItemCtx);
    if (pDesc == NULL)
        return false;

    //  Add our drive metrics to the base metrics (defined by the class).

    Ctx.DriveDesc.Add(pDesc->DriveDesc);

    return true;
    }

CString CDriveClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
	CString sReference;

	//	Get the drive stats

	const SScalableStats *pDesc = GetDesc(Ctx);
    if (pDesc == NULL)
        return NULL_STR;

	//	Max speed

	int iSpeed = (int)(100.0 * pDesc->DriveDesc.GetMaxSpeed() / LIGHT_SPEED);
	AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("max speed 0.%02dc"), iSpeed));

	//	Thrust

	AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("thrust %d"), pDesc->DriveDesc.GetThrust()));

	//	Done

	return sReference;
	}

void CDriveClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Device installed

	{
	//	Drives are identified when installed

	if (pSource->IsPlayer())
		GetItemType()->SetKnown();
	}
