//	CEnhancerClass.cpp
//
//	CEnhancerClass class

#include "PreComp.h"

#define ENHANCE_TAG								CONSTLIT("Enhance")
#define SCALING_TAG								CONSTLIT("Scaling")

#define ACTIVATE_ADJ_ATTRIB						CONSTLIT("activateAdj")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define DAMAGE_ADJ_ATTRIB						CONSTLIT("damageAdj")
#define ENHANCEMENT_TYPE_ATTRIB					CONSTLIT("enhancementType")
#define HP_BONUS_ATTRIB							CONSTLIT("hpBonus")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MIN_ACTIVATE_DELAY_ATTRIB				CONSTLIT("minActivateDelay")
#define MAX_ACTIVATE_DELAY_ATTRIB				CONSTLIT("maxActivateDelay")
#define POWER_USE_ATTRIB						CONSTLIT("powerUse")
#define TYPE_ATTRIB								CONSTLIT("type")

bool CEnhancerClass::AccumulateOldStyle (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateOldStyle
//
//	Accumulates old-style enhancements.

	{
	CInstalledDevice *pDevice = Device.GetDevice();
	CSpaceObject *pSource = Device.GetSource();

	//	If disabled or damaged then we do not enhance anything

	if (pDevice 
			&& (!pDevice->IsEnabled() || pDevice->IsDamaged()))
		return false;

	//	If the target item does not match our criteria, then no enhancement

	if (pSource 
			&& pTarget
			&& !pSource->GetItemForDevice(pTarget).MatchesCriteria(m_Criteria))
		return false;

	//	If this enhancement type has already been applied, then nothing

	if (!m_sEnhancementType.IsBlank()
			&& EnhancementIDs.Find(m_sEnhancementType))
		return false;

	//	Keep track of enhancements.

	bool bEnhanced = false;

	//	Add HP bonus enhancements

	int iBonus;
	if (m_bUseArray)
		{
		//	NOTE: We enter an empty item context because we want to hard-code to 
		//	the first variant (otherwise we would default to the selected
		//	variant)

		int iType = pTarget->GetDamageType(CItemCtx());
		iBonus = (iType != -1 ? m_iDamageAdjArray[iType] : 0);
		}
	else
		iBonus = m_iDamageAdj;

	if (iBonus != 0)
		{
		pEnhancements->InsertHPBonus(iBonus);
		bEnhanced = true;
		}

	//	Add speed enhancements

	if (m_iActivateAdj != 100)
		{
		pEnhancements->InsertActivateAdj(m_iActivateAdj, m_iMinActivateDelay, m_iMaxActivateDelay);
		bEnhanced = true;
		}

	//	Remember that we added this enhancement type

	if (!m_sEnhancementType.IsBlank())
		EnhancementIDs.Insert(m_sEnhancementType);

	//	Done

	return bEnhanced;
	}

int CEnhancerClass::CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Returns the amount of power used per tick

	{
	if (!pDevice->IsEnabled())
		return 0;

	const SScalableStats *pStats = GetStats(CItemCtx(pSource, pDevice));
	if (pStats == NULL)
		return m_iPowerUse;

	return pStats->iPowerUse;
	}

ALERROR CEnhancerClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Load device data from XML

	{
	ALERROR error;
	CEnhancerClass *pDevice;

	pDevice = new CEnhancerClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	//	Some elements can optionally come from the root.

	CString sRootType = pDesc->GetAttribute(TYPE_ATTRIB);
	if (sRootType.IsBlank())
		sRootType = pDesc->GetAttribute(ENHANCEMENT_TYPE_ATTRIB);

	CString sRootCriteria = pDesc->GetAttribute(CRITERIA_ATTRIB);
	int iRootPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, -1);

	//	If we've got a single <Enhance> element, then we take all data from that.

	CXMLElement *pScaling;
	CXMLElement *pEnhance;
	if (pEnhance = pDesc->GetContentElementByTag(ENHANCE_TAG))
		{
		if (error = pDevice->InitFromEnhanceXML(Ctx, pEnhance, pType, sRootType, sRootCriteria, iRootPowerUse))
			return error;
		}

	//	Otherwise, if we've got a <Scaling> element then initialize from that.

	else if (pScaling = pDesc->GetContentElementByTag(SCALING_TAG))
		{
		if (error = pDevice->InitFromScalingXML(Ctx, pScaling, pType, sRootType, sRootCriteria, iRootPowerUse))
			return error;
		}

	//	Otherwise we assume old-style format

	else
		{
		//	The old style is to have an array of damage adj; the new way is to just
		//	have a single damage adj and a criteria

		int iDamageAdjCount;
		LoadDamageAdj(pDesc, DAMAGE_ADJ_ATTRIB, pDevice->m_iDamageAdjArray, &iDamageAdjCount);
		if (iDamageAdjCount)
			pDevice->m_bUseArray = true;
		else
			{
			pDevice->m_iDamageAdj = pDesc->GetAttributeInteger(HP_BONUS_ATTRIB);
			pDevice->m_bUseArray = false;
			}

		pDevice->m_iPowerUse = iRootPowerUse;
		pDevice->m_sEnhancementType = sRootType;
		pDevice->m_iActivateAdj = pDesc->GetAttributeIntegerBounded(ACTIVATE_ADJ_ATTRIB, 1, -1, 100);
		pDevice->m_iMinActivateDelay = pDesc->GetAttributeIntegerBounded(MIN_ACTIVATE_DELAY_ATTRIB, 0, -1, 0);
		pDevice->m_iMaxActivateDelay = pDesc->GetAttributeIntegerBounded(MAX_ACTIVATE_DELAY_ATTRIB, 0, -1, 0);

		//	Load the item criteria

		CString sCriteria;
		if (!pDesc->FindAttribute(CRITERIA_ATTRIB, &sCriteria))
			sCriteria = CONSTLIT("w");

		CItem::ParseCriteria(sCriteria, &pDevice->m_Criteria);
		}

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

int CEnhancerClass::GetPowerRating (CItemCtx &Ctx) const

//	GetPowerRating
//
//	Returns the rated power use for the device (regardless of whether it is 
//	enabled or not).

	{
	const SScalableStats *pStats = GetStats(Ctx);
	if (pStats == NULL)
		return m_iPowerUse;

	return pStats->iPowerUse;
	}

const CEnhancerClass::SScalableStats *CEnhancerClass::GetStats (CItemCtx &Ctx) const

//	GetStats
//
//	Returns the appropriate stats block. If we're using old-style definitions, 
//	then we return NULL.

	{
	if (!m_pDesc)
		return NULL;

	else if (Ctx.GetItem().IsEmpty() || m_iLevels == 1)
		return &m_pDesc[0];

	else
		{
		int iIndex = Ctx.GetItem().GetLevel() - m_pDesc[0].iLevel;
		if (iIndex < 0 || iIndex >= m_iLevels)
			return NULL;

		return &m_pDesc[iIndex];
		}
	}

ALERROR CEnhancerClass::InitFromEnhanceXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, const CString &sRootType, const CString &sRootCriteria, int iRootPowerUse)

//	InitFromEnhanceXML
//
//	Initializes from a single <Enhance> element

	{
	ALERROR error;

	m_iLevels = 1;
	m_pDesc.Set(new SScalableStats[m_iLevels]);

	//	Initialize basics

	m_pDesc[0].iLevel = pType->GetLevel();
	m_pDesc[0].iPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, 0);

	//	Initialize the enhancements

	if (error = m_pDesc[0].Enhancements.InitFromXML(Ctx, pDesc))
		return error;

	//	If necessary, we inherit some values from the root element

	if (!sRootType.IsBlank())
		m_pDesc[0].Enhancements.SetType(-1, sRootType);

	if (!sRootCriteria.IsBlank())
		{
		CItemCriteria RootCriteria;
		CItem::ParseCriteria(sRootCriteria, &RootCriteria);
		m_pDesc[0].Enhancements.SetCriteria(-1, RootCriteria);
		}

	if (iRootPowerUse != -1)
		m_pDesc[0].iPowerUse = iRootPowerUse;

	return NOERROR;
	}

ALERROR CEnhancerClass::InitFromScalingXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, const CString &sRootType, const CString &sRootCriteria, int iRootPowerUse)

//	InitFromScalingXML
//
//	Initializes from a <Scaling> element

	{
	ALERROR error;
	int i;

	//	We need to fill all levels

    int iBaseLevel = pType->GetLevel();
    m_iLevels = (pType->GetMaxLevel() - iBaseLevel) + 1;

	//	Allocate the structure and initialize iLevel to -1 to indicate that we have 
	//	not yet initialized it.

	m_pDesc.Set(new SScalableStats[m_iLevels]);
	for (i = 0; i < m_iLevels; i++)
		m_pDesc[i].iLevel = -1;

	//	Parse root criteria so we don't have to parse it for each entry.

	CItemCriteria RootCriteria;
	if (!sRootCriteria.IsBlank())
		CItem::ParseCriteria(sRootCriteria, &RootCriteria);

	//	Now loop over all entries and initialize the appropriate level.

	int iFirstLevel = -1;
	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);
		int iLevel = pEntry->GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_ITEM_LEVEL, pType->GetLevel());
		int iIndex = iLevel - iBaseLevel;

		//	Skip if this is not one of our levels.

		if (iIndex < 0 || iIndex >= m_iLevels)
			continue;

		//	Initialize basics

		m_pDesc[iIndex].iLevel = iLevel;
		m_pDesc[iIndex].iPowerUse = pEntry->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, 0);

		//	Initialize the enhancements

		if (error = m_pDesc[iIndex].Enhancements.InitFromXML(Ctx, pEntry))
			return error;

		//	If necessary, we inherit some values from the root element

		if (!sRootType.IsBlank())
			m_pDesc[iIndex].Enhancements.SetType(-1, sRootType);

		if (!sRootCriteria.IsBlank())
			m_pDesc[iIndex].Enhancements.SetCriteria(-1, RootCriteria);

		if (iRootPowerUse != -1)
			m_pDesc[iIndex].iPowerUse = iRootPowerUse;

		//	Keep track of the first entry (by level)

		if (iFirstLevel == -1 || iFirstLevel > iLevel)
			iFirstLevel = iLevel;
		}

	//	Now initialize any levels that haven't been initialized by copying from the 
	//	previous level.

	int iCopyFrom = iFirstLevel - iBaseLevel;
	for (i = 0; i < m_iLevels; i++)
		{
		if (m_pDesc[i].iLevel == -1)
			{
			m_pDesc[i] = m_pDesc[iCopyFrom];
			m_pDesc[i].iLevel = iBaseLevel + i;
			}
		else
			iCopyFrom = i;
		}

	return NOERROR;
	}

bool CEnhancerClass::OnAccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	OnAccumulateEnhancements
//
//	Enhances pTarget

	{
	const SScalableStats *pStats = GetStats(Device);
	if (pStats == NULL)
		return false;

	//	New style

	return pStats->Enhancements.Accumulate(*pTarget->GetItem(), EnhancementIDs, pEnhancements);
	}

bool CEnhancerClass::OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	OnAccumulateEnhancements
//
//	Enhances pTarget

	{
	const SScalableStats *pStats = GetStats(Device);
	if (pStats == NULL)
		return AccumulateOldStyle(Device, pTarget, EnhancementIDs, pEnhancements);

	//	New style

	return pStats->Enhancements.Accumulate(*pTarget->GetItem(), EnhancementIDs, pEnhancements);
	}
