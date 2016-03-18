//	CDriveClass.cpp
//
//	CDriveClass class
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define UNID_ATTRIB					CONSTLIT("UNID")
#define ITEM_ID_ATTRIB				CONSTLIT("itemID")

#define FIELD_MAX_SPEED				CONSTLIT("maxSpeed")
#define FIELD_THRUST				CONSTLIT("thrust")
#define FIELD_POWER					CONSTLIT("power")

#define PROPERTY_DRIVE_POWER		CONSTLIT("drivePowerUse")
#define PROPERTY_MAX_SPEED			CONSTLIT("maxSpeed")
#define PROPERTY_POWER				CONSTLIT("power")
#define PROPERTY_THRUST				CONSTLIT("thrust")
#define PROPERTY_THRUST_TO_WEIGHT	CONSTLIT("thrustToWeight")

CDriveClass::CDriveClass (void)
	{
	}

ALERROR CDriveClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDrive)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
	CDriveClass *pDrive;

	pDrive = new CDriveClass;
	if (pDrive == NULL)
		return ERR_MEMORY;

	if (error = pDrive->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

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

const CDriveDesc *CDriveClass::GetDriveDesc (CItemCtx &Ctx) const

//	GetDriveDesc
//
//	Returns the drive descriptor

	{
    CInstalledDevice *pDevice = Ctx.GetDevice();

	if (pDevice)
		{
		if (pDevice->IsDamaged() || pDevice->IsDisrupted())
			return &m_DamagedDriveDesc;
		else if (pDevice->IsEnhanced())
			return &m_EnhancedDriveDesc;
		else
			return &m_DriveDesc;
		}
	else
		return &m_DriveDesc;
	}

ICCItem *CDriveClass::GetItemProperty (CItemCtx &Ctx, const CString &sProperty)

//	GetItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	const CDriveDesc &Desc = *GetDriveDesc(Ctx);

	if (strEquals(sProperty, PROPERTY_MAX_SPEED))
		return CC.CreateInteger((int)((100.0 * Desc.GetMaxSpeed() / LIGHT_SPEED) + 0.5));

	else if (strEquals(sProperty, PROPERTY_THRUST))
		return CC.CreateInteger(Desc.GetThrust());
	
	else if (strEquals(sProperty, PROPERTY_POWER)
			|| strEquals(sProperty, PROPERTY_DRIVE_POWER))
		return CC.CreateInteger(Desc.GetPowerUse() * 100);

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::GetItemProperty(Ctx, sProperty);
	}

int CDriveClass::GetPowerRating (CItemCtx &Ctx) const

//	GetPowerRating
//
//	Get minimum reactor output for device

	{
	const CDriveDesc &Desc = *GetDriveDesc(Ctx);
	return Desc.GetPowerUse();
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

void CDriveClass::OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList)

//	OnAccumulateAttributes
//
//	Returns display attributes

	{
	const CDriveDesc &Desc = *GetDriveDesc(ItemCtx);

	//	Inertialess

	if (Desc.IsInertialess())
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("inertialess")));
	}

bool CDriveClass::OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//  OnAccumulatePerformance
//
//  Modifies the performance of the ship.

    {
	const CDriveDesc &Desc = *GetDriveDesc(ItemCtx);

    //  Add our drive metrics to the base metrics (defined by the class).

    Ctx.DriveDesc.Add(Desc);

    return true;
    }

CString CDriveClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
	CString sReference;

	//	Get the drive stats

	const CDriveDesc &Desc = *GetDriveDesc(Ctx);

	//	Max speed

	int iSpeed = (int)(100.0 * Desc.GetMaxSpeed() / LIGHT_SPEED);
	AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("max speed 0.%02dc"), iSpeed));

	//	Thrust

	AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("thrust %d"), Desc.GetThrust()));

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
