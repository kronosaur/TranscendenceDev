//	CDriveClass.cpp
//
//	CDriveClass class

#include "PreComp.h"


#define UNID_ATTRIB					CONSTLIT("UNID")
#define ITEM_ID_ATTRIB				CONSTLIT("itemID")
#define MAX_SPEED_ATTRIB			CONSTLIT("maxSpeed")
#define THRUST_ATTRIB				CONSTLIT("thrust")
#define POWER_USED_ATTRIB			CONSTLIT("powerUsed")
#define FUEL_USED_ATTRIB			CONSTLIT("fuelUsed")
#define INERTIALESS_DRIVE_ATTRIB	CONSTLIT("inertialessDrive")

#define FIELD_MAX_SPEED				CONSTLIT("maxSpeed")
#define FIELD_THRUST				CONSTLIT("thrust")
#define FIELD_POWER					CONSTLIT("power")

#define PROPERTY_DRIVE_POWER		CONSTLIT("drivePowerUse")
#define PROPERTY_MAX_SPEED			CONSTLIT("maxSpeed")
#define PROPERTY_POWER				CONSTLIT("power")
#define PROPERTY_THRUST				CONSTLIT("thrust")
#define PROPERTY_THRUST_TO_WEIGHT	CONSTLIT("thrustToWeight")

CDriveClass::CDriveClass (void) : CDeviceClass(NULL)
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

	pDrive->m_DriveDesc.dwUNID = pDrive->GetUNID();
	pDrive->m_DriveDesc.rMaxSpeed = (double)pDesc->GetAttributeInteger(MAX_SPEED_ATTRIB) * LIGHT_SPEED / 100;
	pDrive->m_DriveDesc.iThrust = pDesc->GetAttributeInteger(THRUST_ATTRIB);
	pDrive->m_DriveDesc.iPowerUse = pDesc->GetAttributeInteger(POWER_USED_ATTRIB);
	pDrive->m_DriveDesc.fInertialess = pDesc->GetAttributeBool(INERTIALESS_DRIVE_ATTRIB);

	//	Compute descriptor when damaged

	pDrive->m_DamagedDriveDesc.dwUNID = pDrive->GetUNID();
	pDrive->m_DamagedDriveDesc.rMaxSpeed = pDrive->m_DriveDesc.rMaxSpeed * 0.75;
	pDrive->m_DamagedDriveDesc.iThrust = pDrive->m_DriveDesc.iThrust / 2;
	pDrive->m_DamagedDriveDesc.iPowerUse = (pDrive->m_DriveDesc.iPowerUse * 130) / 100;
	pDrive->m_DamagedDriveDesc.fInertialess = false;

	//	Compute descriptor when enhanced

	pDrive->m_EnhancedDriveDesc.dwUNID = pDrive->GetUNID();
	pDrive->m_EnhancedDriveDesc.rMaxSpeed = pDrive->m_DriveDesc.rMaxSpeed * 1.1;
	pDrive->m_EnhancedDriveDesc.iThrust = (pDrive->m_DriveDesc.iThrust * 120) / 100;
	pDrive->m_EnhancedDriveDesc.iPowerUse = (pDrive->m_DriveDesc.iPowerUse * 90) / 100;
	pDrive->m_EnhancedDriveDesc.fInertialess = pDrive->m_DriveDesc.fInertialess;

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
		*retsValue = strFromInt((int)((100.0 * m_DriveDesc.rMaxSpeed / LIGHT_SPEED) + 0.5));
	else if (strEquals(sField, FIELD_THRUST))
		*retsValue = strFromInt(m_DriveDesc.iThrust);
	else if (strEquals(sField, FIELD_POWER))
		*retsValue = strFromInt(m_DriveDesc.iPowerUse * 100);
	else
		return false;

	return true;
	}

const DriveDesc *CDriveClass::GetDriveDesc (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	GetDriveDesc
//
//	Returns the drive descriptor

	{
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
	const DriveDesc &Desc = *GetDriveDesc(Ctx.GetDevice(), Ctx.GetSource());

	if (strEquals(sProperty, PROPERTY_MAX_SPEED))
		return CC.CreateInteger((int)((100.0 * Desc.rMaxSpeed / LIGHT_SPEED) + 0.5));

	else if (strEquals(sProperty, PROPERTY_THRUST))
		return CC.CreateInteger(Desc.iThrust);
	
	else if (strEquals(sProperty, PROPERTY_POWER)
			|| strEquals(sProperty, PROPERTY_DRIVE_POWER))
		return CC.CreateInteger(Desc.iPowerUse * 100);

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::GetItemProperty(Ctx, sProperty);
	}

int CDriveClass::GetPowerRating (CItemCtx &Ctx)

//	GetPowerRating
//
//	Get minimum reactor output for device

	{
	const DriveDesc &Desc = *GetDriveDesc(Ctx.GetDevice(), Ctx.GetSource());
	return Desc.iPowerUse;
	}

ICCItem *CDriveClass::GetDriveProperty (const DriveDesc &Desc, const CString &sProperty)

//	GetDriveProperty
//
//	Returns property for a built-in drive.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_MAX_SPEED))
		return CC.CreateInteger((int)((100.0 * Desc.rMaxSpeed / LIGHT_SPEED) + 0.5));

	else if (strEquals(sProperty, PROPERTY_THRUST))
		return CC.CreateInteger(Desc.iThrust);
	
	else if (strEquals(sProperty, PROPERTY_POWER)
			|| strEquals(sProperty, PROPERTY_DRIVE_POWER))
		return CC.CreateInteger(Desc.iPowerUse * 100);
	else
		return CC.CreateNil();
	}

void CDriveClass::OnAccumulateAttributes (CItemCtx &ItemCtx, int iVariant, TArray<SDisplayAttribute> *retList)

//	OnAccumulateAttributes
//
//	Returns display attributes

	{
	const DriveDesc &Desc = *GetDriveDesc(ItemCtx.GetDevice(), ItemCtx.GetSource());

	//	Inertialess

	if (Desc.fInertialess)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("inertialess")));
	}

CString CDriveClass::OnGetReference (CItemCtx &Ctx, int iVariant, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
	CString sReference;

	//	Get the drive stats

	const DriveDesc &Desc = *GetDriveDesc(Ctx.GetDevice(), Ctx.GetSource());

	//	Max speed

	int iSpeed = (int)(100.0 * Desc.rMaxSpeed / LIGHT_SPEED);
	AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("max speed 0.%02dc"), iSpeed));

	//	Thrust

	AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("thrust %d"), Desc.iThrust));

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
