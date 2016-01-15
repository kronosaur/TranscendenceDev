//	CCargoSpaceClass.cpp
//
//	CCargoSpaceClass class

#include "PreComp.h"

#define CARGO_SPACE_ATTRIB			CONSTLIT("cargoSpace")

#define FIELD_CARGO_SPACE			CONSTLIT("cargoSpace")

CCargoSpaceClass::CCargoSpaceClass (void) : CDeviceClass(NULL)
	{
	}

ALERROR CCargoSpaceClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
	CCargoSpaceClass *pDevice;

	pDevice = new CCargoSpaceClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	pDevice->m_iCargoSpace = pDesc->GetAttributeInteger(CARGO_SPACE_ATTRIB);

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

bool CCargoSpaceClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	if (strEquals(sField, FIELD_CARGO_SPACE))
		*retsValue = strFromInt(m_iCargoSpace);
	else
		return false;

	return true;
	}

CString CCargoSpaceClass::OnGetReference (CItemCtx &Ctx, int iVariant, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
	CString sReference;

	//	If this item is installed on a ship, then take into account the maximum
	//	cargo limits.

	CSpaceObject *pObj;
	CShip *pShip;
	CShipClass *pClass;
	if (Ctx.GetItem().IsInstalled()
			&& (pObj = Ctx.GetSource())
			&& (pShip = pObj->AsShip())
			&& (pClass = pShip->GetClass()))
		{
		int iCargoInc = Min(m_iCargoSpace, pClass->GetMaxCargoSpace() - pClass->GetCargoSpace());
		if (iCargoInc > 0)
			sReference = strPatternSubst(CONSTLIT("+%d ton capacity"), iCargoInc);
		}

	//	Otherwise, describe the full amount

	else
		sReference = strPatternSubst(CONSTLIT("%d ton capacity"), m_iCargoSpace);

	//	Done

	return sReference;
	}

void CCargoSpaceClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Device installed

	{
	//	Identified when installed

	if (pSource->IsPlayer())
		GetItemType()->SetKnown();
	}
