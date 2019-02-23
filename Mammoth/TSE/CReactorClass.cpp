//	CReactorClass.cpp
//
//	CReactorClass class

#include "PreComp.h"

#define MAX_POWER_BONUS_PER_CHARGE_ATTRIB		CONSTLIT("maxPowerBonusPerCharge")

#define PROPERTY_MAX_POWER						CONSTLIT("maxPower")
#define PROPERTY_POWER							CONSTLIT("power")

CReactorClass::CReactorClass (void) :
        m_pDesc(NULL),
        m_pDamagedDesc(NULL),
        m_pEnhancedDesc(NULL)
	{
	}

CReactorClass::~CReactorClass (void)

//  CReactorClass destructor

    {
    if (m_pDesc)
        delete[] m_pDesc;

    if (m_pDamagedDesc)
        delete[] m_pDamagedDesc;

    if (m_pEnhancedDesc)
        delete[] m_pEnhancedDesc;
    }

ALERROR CReactorClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
    int i;

	CReactorClass *pDevice = new CReactorClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

    //  Figure out how many levels we need to allocate

    pDevice->m_iBaseLevel = pType->GetLevel();
    pDevice->m_iLevels = (pType->GetMaxLevel() - pDevice->m_iBaseLevel) + 1;

    //  Allocate the normal descriptors

    pDevice->m_pDesc = new CReactorDesc[pDevice->m_iLevels];

    //  We initialized the first level from XML

    ASSERT(pDevice->m_iLevels >= 1);
    if (error = pDevice->m_pDesc[0].InitFromXML(Ctx, pDesc, pType->GetUNID()))
        return error;

    //  Now initialize the scaled levels

    for (i = 1; i < pDevice->m_iLevels; i++)
        {
        if (error = pDevice->m_pDesc[i].InitScaled(Ctx, pDevice->m_pDesc[0], pDevice->m_iBaseLevel, pDevice->m_iBaseLevel + i))
            return error;
        }

	//	Some properties are not scaled (OK if negative).

	pDevice->m_iExtraPowerPerCharge = pDesc->GetAttributeInteger(MAX_POWER_BONUS_PER_CHARGE_ATTRIB);

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

bool CReactorClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	return m_pDesc[0].FindDataField(sField, retsValue);
	}

ICCItem *CReactorClass::FindItemProperty (CItemCtx &Ctx, const CString &sName)

//	FindItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	CCodeChain &CC = GetUniverse().GetCC();
	const CReactorDesc &Desc = *GetReactorDesc(Ctx);
    ICCItem *pResult;

	//	Some properties we handle ourselves

	if (strEquals(sName, PROPERTY_MAX_POWER))
		{
		if (m_iExtraPowerPerCharge == 0 || Ctx.IsItemNull())
			return CreatePowerResult(100.0 * GetMaxPower(Ctx, Desc));
		else
			{
			int iMaxCharges = Ctx.GetItem().GetType()->GetMaxCharges();
			int iMaxPower = Desc.GetMaxPower() + (iMaxCharges * m_iExtraPowerPerCharge);
			return CreatePowerResult(100.0 * iMaxPower);
			}
		}

	else if (strEquals(sName, PROPERTY_POWER))
		return CreatePowerResult(100.0 * GetMaxPower(Ctx, Desc));

	//	Ask the descriptor

    else if (pResult = Desc.FindProperty(sName))
        return pResult;

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::FindItemProperty(Ctx, sName);
	}

int CReactorClass::GetPowerOutput (CItemCtx &Ctx, DWORD dwFlags) const

//	GetPowerOutput
//
//	Returns the max power output.

	{
	const CReactorDesc *pDesc = GetReactorDesc(Ctx);
	if (pDesc == NULL)
		return 0;

	return GetMaxPower(Ctx, *pDesc);
	}

int CReactorClass::GetMaxPower (CItemCtx &ItemCtx, const CReactorDesc &Desc) const

//	GetMaxPower
//
//	Returns max reactor power

	{
	int iMaxPower = Desc.GetMaxPower();

	//	Adjust for charges

	if (m_iExtraPowerPerCharge > 0)
		iMaxPower += m_iExtraPowerPerCharge * ItemCtx.GetItemCharges();

	//	Done

	return Max(0, iMaxPower);
	}

const CReactorDesc *CReactorClass::GetReactorDesc (CItemCtx &Ctx, DWORD dwFlags) const

//	GetReactorDesc
//
//	Returns the reactor descriptor

	{
    CInstalledDevice *pDevice = Ctx.GetDevice();
	bool bNormalOnly = ((dwFlags & GPO_FLAG_NORMAL_POWER) ? true : false);

    //  Figure out if we want a scaled item

    int iIndex = Min(Max(0, (Ctx.GetItem().IsEmpty() ? 0 : Ctx.GetItem().GetLevel() - m_iBaseLevel)), m_iLevels - 1);

    //  If no device, then standard descriptor

	if (pDevice == NULL)
		return &m_pDesc[iIndex];

    //  If the device is damaged, then return damaged descriptor

    else if (!bNormalOnly && (pDevice->IsDamaged() || pDevice->IsDisrupted()))
        {
        InitDamagedDesc();
		return &m_pDamagedDesc[iIndex];
        }

    //  If enhanced, then return enhanced descriptor

    else if (!bNormalOnly && pDevice->IsEnhanced())
        {
        InitEnhancedDesc();
		return &m_pEnhancedDesc[iIndex];
        }

    //  Otherwise, standard descriptor.

	else
		return &m_pDesc[iIndex];
	}

void CReactorClass::InitDamagedDesc (void) const

//  InitDamagedDesc
//
//  Makes sure that the damaged descriptor is initialized.

    {
    int i;

    if (m_pDamagedDesc == NULL)
        {
        m_pDamagedDesc = new CReactorDesc[m_iLevels];
        for (i = 0; i < m_iLevels; i++)
            {
            m_pDamagedDesc[i] = m_pDesc[i];
            m_pDamagedDesc[i].AdjMaxPower(0.8);
            m_pDamagedDesc[i].AdjEfficiency(0.8);
            }
        }
    }

void CReactorClass::InitEnhancedDesc (void) const

//  InitEnhancedDesc
//
//  Make sure that the enhanced descriptor is initialzied.

    {
    int i;

    if (m_pEnhancedDesc == NULL)
        {
        m_pEnhancedDesc = new CReactorDesc[m_iLevels];
        for (i = 0; i < m_iLevels; i++)
            {
            m_pEnhancedDesc[i] = m_pDesc[i];
            m_pEnhancedDesc[i].AdjMaxPower(1.2);
            m_pEnhancedDesc[i].AdjEfficiency(1.5);
            }
        }
    }

CString CReactorClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
	CString sReference;

	//	Get reactor stats

	const CReactorDesc &Desc = *GetReactorDesc(Ctx);
	int iMaxPower = GetMaxPower(Ctx, Desc);

	//	Power output

	sReference = strPatternSubst(CONSTLIT("%s max output"),	
			CLanguage::ComposeNumber(CLanguage::numberPower, iMaxPower * 100.0));

	//	Fuel level

	int iMinLevel;
	int iMaxLevel;
    Desc.GetFuelLevel(&iMinLevel, &iMaxLevel);

	if (iMinLevel == -1 && iMaxLevel == -1)
		;
	else if (iMinLevel == iMaxLevel)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level %d"), iMinLevel));
	else if (iMaxLevel == -1)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level %d-25"), iMinLevel));
	else if (iMinLevel == -1)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level 1-%d"), iMaxLevel));
	else
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level %d-%d"), iMinLevel, iMaxLevel));

	//	Efficiency

	int iBonus = Desc.GetEfficiencyBonus();
	if (iBonus > 0)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("+%d%% efficiency"), iBonus));
	else if (iBonus < 0)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d%% efficiency"), iBonus));

	//	Done

	return sReference;
	}

bool CReactorClass::OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//	OnAccumulatePerformance
//
//	Accumulate performance characteristics

	{
	const CReactorDesc *pDesc = GetReactorDesc(ItemCtx);
	if (pDesc == NULL)
		return false;

	//	We don't allow multiple reactors, so we always override.

	Ctx.ReactorDesc = *pDesc;

	//	For reactors with charges, increase power based on charges

	Ctx.ReactorDesc.SetMaxPower(GetMaxPower(ItemCtx, *pDesc));

	//	Done

	return true;
	}

void CReactorClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Device installed

	{
	//	Identified when installed

	if (pSource->IsPlayer())
		GetItemType()->SetKnown();
	}
