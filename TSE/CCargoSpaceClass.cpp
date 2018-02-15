//	CCargoSpaceClass.cpp
//
//	CCargoSpaceClass class

#include "PreComp.h"

#define FIELD_CARGO_SPACE			CONSTLIT("cargoSpace")
#define FIELD_LEVEL                 CONSTLIT("level")

#define TAG_SCALING                 CONSTLIT("Scaling")

CCargoSpaceClass::CCargoSpaceClass (void)
	{
	}

ALERROR CCargoSpaceClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
    int i;
	CCargoSpaceClass *pDevice;

	pDevice = new CCargoSpaceClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

    //  Figure out how many levels we need to allocate

    int iBaseLevel = pType->GetLevel();
    int iLevels = (pType->GetMaxLevel() - iBaseLevel) + 1;

    //  Allocate the normal descriptors

    pDevice->m_Desc.InsertEmpty(iLevels);

    //  If we've got a scaling element, then we use that.

    CXMLElement *pScalingDesc = pDesc->GetContentElementByTag(TAG_SCALING);
    if (pScalingDesc)
        {
        for (i = 0; i < pScalingDesc->GetContentElementCount(); i++)
            {
            CXMLElement *pStageDesc = pScalingDesc->GetContentElement(i);
            int iLevel = pStageDesc->GetAttributeInteger(FIELD_LEVEL);
            if (iLevel < iBaseLevel || iLevel >= iBaseLevel + iLevels)
                {
                Ctx.sError = strPatternSubst(CONSTLIT("Invalid level: %d"), iLevel);
                return ERR_FAIL;
                }

            if (error = pDevice->m_Desc[iLevel - iBaseLevel].InitFromXML(Ctx, pStageDesc))
                return error;
            }
        }

    //  Otherwise, we load a single descriptor

    else
        {
        ASSERT(iLevels >= 1);
        if (error = pDevice->m_Desc[0].InitFromXML(Ctx, pDesc))
            return error;
        }

    //  Loop over all scales and see if all are initialized. If not, we 
    //  interpolate as appropriate.

    int iStartLevel = -1;
    int iEndLevel = 0;
    for (i = 0; i < pDevice->m_Desc.GetCount(); i++)
        {
        CCargoDesc &Stats = pDevice->m_Desc[i];
        if (Stats.IsEmpty())
            {
            //  If we don't have a starting level, we can't interpolate.

            if (iStartLevel == -1)
                {
                Ctx.sError = strPatternSubst(CONSTLIT("Unable to interpolate level %d."), i);
                return ERR_FAIL;
                }

            const CCargoDesc &StartStats = pDevice->m_Desc[iStartLevel];

            //  Initialize

            int iLevel = iBaseLevel + i;

            //  Look for the end level, if necessary

            if (iEndLevel != -1 && iEndLevel <= i)
                {
                for (int j = i + 1; j < pDevice->m_Desc.GetCount(); j++)
                    if (!pDevice->m_Desc[j].IsEmpty())
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
                Stats = StartStats;
                }

            //  Otherwise, we interpolate between the two levels.

            else
                {
                const CCargoDesc &EndStats = pDevice->m_Desc[iEndLevel];

                //  Compute the number of steps. iSteps has to be > 0 because
                //  iStartLevel is always < i and iEndLevel is always > i.

                int iSteps = iEndLevel - iStartLevel;
                ASSERT(iSteps > 0);

                //  Interpolate

                Metric rInterpolate = (Metric)(i - iStartLevel) / iSteps;
                Stats.Interpolate(StartStats, EndStats, rInterpolate);
                }
            }
        else
            iStartLevel = i;
        }

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

bool CCargoSpaceClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
    const CCargoDesc *pDesc = GetDesc(CItemCtx());
    if (pDesc == NULL)
        return false;

	if (strEquals(sField, FIELD_CARGO_SPACE))
		*retsValue = strFromInt(pDesc->GetCargoSpace());
	else
		return false;

	return true;
	}

const CCargoDesc *CCargoSpaceClass::GetDesc (CItemCtx &Ctx) const

//  GetDesc
//
//  Returns the proper descriptor for this item

    {
    CInstalledDevice *pDevice = Ctx.GetDevice();
    int iBaseLevel = GetItemType()->GetLevel();
    int iLevels = m_Desc.GetCount();
    if (iLevels == 0)
        return NULL;

    //  Figure out if we want a scaled item

    int iIndex = Min(Max(0, (Ctx.GetItem().IsEmpty() ? 0 : Ctx.GetItem().GetLevel() - iBaseLevel)), iLevels - 1);

    //  Return standard descriptor for level

	return &m_Desc[iIndex];
    }

bool CCargoSpaceClass::OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//  OnAccumulatePerformance
//
//  Modifies the performance of the ship.

    {
    //  If disabled, then nothing

    if (!ItemCtx.IsDeviceEnabled())
        return false;

    //  Get the stats (this checks for damage)

	const CCargoDesc *pDesc = GetDesc(ItemCtx);
    if (pDesc == NULL)
        return false;

    //  Add our metrics to the base metrics.
	//
	//	NOTE: We don't check for limits (even negative numbers) because there
	//	could be cargo devices with negative cargo space. Instead, we check at
	//	the end when we initialize performance.

    int iNewSpace = Ctx.CargoDesc.GetCargoSpace() + pDesc->GetCargoSpace();
    Ctx.CargoDesc.SetCargoSpace(iNewSpace);

    return true;
    }

CString CCargoSpaceClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
    const CCargoDesc *pDesc = GetDesc(Ctx);
    if (pDesc == NULL)
        return NULL_STR;

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
		const CHullDesc &Hull = pClass->GetHullDesc();

		int iCargoInc = Min(pDesc->GetCargoSpace(), Hull.GetMaxCargoSpace() - Hull.GetCargoSpace());
		if (iCargoInc > 0)
			sReference = strPatternSubst(CONSTLIT("+%d ton capacity"), iCargoInc);
		}

	//	Otherwise, describe the full amount

	else
		sReference = strPatternSubst(CONSTLIT("%d ton capacity"), pDesc->GetCargoSpace());

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
