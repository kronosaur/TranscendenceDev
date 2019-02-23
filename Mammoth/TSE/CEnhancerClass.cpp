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
#define LEVEL_CHECK_ATTRIB						CONSTLIT("levelCheck")
#define MIN_ACTIVATE_DELAY_ATTRIB				CONSTLIT("minActivateDelay")
#define MAX_ACTIVATE_DELAY_ATTRIB				CONSTLIT("maxActivateDelay")
#define POWER_USE_ATTRIB						CONSTLIT("powerUse")
#define TYPE_ATTRIB								CONSTLIT("type")

#define PROPERTY_ENHANCEMENT_PREFIX				CONSTLIT("enhancement.")
#define PROPERTY_ENHANCEMENT_DAMAGE_TYPE		CONSTLIT("enhancement.damageType")
#define PROPERTY_ENHANCEMENT_HP_BONUS			CONSTLIT("enhancement.hpBonus")
#define PROPERTY_ENHANCEMENT_TYPE				CONSTLIT("enhancement.type")

bool CEnhancerClass::AccumulateOldStyle (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateOldStyle
//
//	Accumulates old-style enhancements.

	{
	CInstalledDevice *pDevice = Device.GetDevice();
	CSpaceObject *pSource = Device.GetSource();

	//	If disabled or damaged then we do not enhance anything

	if (pDevice && !pDevice->IsWorking())
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

void CEnhancerClass::ApplyInherited (SScalableStats &Stats, const SInheritedStats &RootStats)

//	ApplyInherited
//
//	Sets inherited stats.

	{
	if (!RootStats.sType.IsBlank())
		Stats.Enhancements.SetType(-1, RootStats.sType);

	if (!RootStats.sCriteria.IsBlank())
		Stats.Enhancements.SetCriteria(-1, RootStats.Criteria);

	if (RootStats.iPowerUse != -1)
		Stats.iPowerUse = RootStats.iPowerUse;

	if (!RootStats.LevelCheck.IsEmpty())
		Stats.Enhancements.SetLevelCheck(-1, RootStats.LevelCheck);
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

	SInheritedStats RootStats;

	RootStats.sType = pDesc->GetAttribute(TYPE_ATTRIB);
	if (RootStats.sType.IsBlank())
		RootStats.sType = pDesc->GetAttribute(ENHANCEMENT_TYPE_ATTRIB);

	RootStats.sCriteria = pDesc->GetAttribute(CRITERIA_ATTRIB);
	if (!RootStats.sCriteria.IsBlank())
		CItem::ParseCriteria(RootStats.sCriteria, &RootStats.Criteria);

	RootStats.iPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, -1);
	if (error = RootStats.LevelCheck.InitFromXML(Ctx, pDesc->GetAttribute(LEVEL_CHECK_ATTRIB)))
		return error;

	//	If we've got a single <Enhance> element, then we take all data from that.

	CXMLElement *pScaling;
	CXMLElement *pEnhance;
	if (pEnhance = pDesc->GetContentElementByTag(ENHANCE_TAG))
		{
		if (error = pDevice->InitFromEnhanceXML(Ctx, pEnhance, pType, RootStats))
			return error;
		}

	//	Otherwise, if we've got a <Scaling> element then initialize from that.

	else if (pScaling = pDesc->GetContentElementByTag(SCALING_TAG))
		{
		if (error = pDevice->InitFromScalingXML(Ctx, pScaling, pType, RootStats))
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

		pDevice->m_iPowerUse = RootStats.iPowerUse;
		pDevice->m_sEnhancementType = RootStats.sType;
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

ICCItem *CEnhancerClass::FindEnhancementProperty (CItemCtx &Ctx, const CItemEnhancement &Enhancement, const CString &sName) const

//	FindEnhancementProperty
//
//	Returns a property of the given enhancement. Return NULL if not found.

	{
	CCodeChain &CC = GetUniverse().GetCC();

	if (strEquals(sName, PROPERTY_ENHANCEMENT_DAMAGE_TYPE))
		{
		DamageTypes iDamage = Enhancement.GetDamageType();
		if (iDamage == damageError)
			return CC.CreateNil();
		else
			return CC.CreateString(::GetDamageType(iDamage));
		}

	else if (strEquals(sName, PROPERTY_ENHANCEMENT_HP_BONUS))
		return CC.CreateInteger(Enhancement.GetResistHPBonus());

	else
		return NULL;
	}

ICCItem *CEnhancerClass::FindItemProperty (CItemCtx &Ctx, const CString &sName)

//	FindItemProperty
//
//	Returns a property.

	{
	CCodeChain &CC = GetUniverse().GetCC();
	const SScalableStats *pStats = GetStats(Ctx);

	//	We don't support this for old-style definitions.

	if (pStats == NULL)
		return CDeviceClass::FindItemProperty(Ctx, sName);

	//	If this is a property of the enhancement, then get that.

	if (strStartsWith(sName, PROPERTY_ENHANCEMENT_PREFIX))
		{
		if (pStats->Enhancements.GetCount() < 1)
			return NULL;

		//	For now we ignore more than one enhancement.

		return FindEnhancementProperty(Ctx, pStats->Enhancements.GetEnhancement(0), sName);
		}

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::FindItemProperty(Ctx, sName);
	}

int CEnhancerClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the rated power use for the device (regardless of whether it is 
//	enabled or not).

	{
	if (retiIdlePowerUse)
		*retiIdlePowerUse = 0;

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

ALERROR CEnhancerClass::InitFromEnhanceXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, const SInheritedStats &RootStats)

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

	ApplyInherited(m_pDesc[0], RootStats);

	return NOERROR;
	}

ALERROR CEnhancerClass::InitFromScalingXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, const SInheritedStats &RootStats)

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

		ApplyInherited(m_pDesc[iIndex], RootStats);

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

	//	If disabled or damaged then we do not enhance anything

	if (!Device.IsDeviceWorking())
		return false;

	//	New style

	return pStats->Enhancements.Accumulate(Device, *pTarget->GetItem(), EnhancementIDs, pEnhancements);
	}

bool CEnhancerClass::OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	OnAccumulateEnhancements
//
//	Enhances pTarget

	{
	const SScalableStats *pStats = GetStats(Device);
	if (pStats == NULL)
		return AccumulateOldStyle(Device, pTarget, EnhancementIDs, pEnhancements);

	//	If disabled or damaged then we do not enhance anything

	if (!Device.IsDeviceWorking())
		return false;

	//	New style

	return pStats->Enhancements.Accumulate(Device, *pTarget->GetItem(), EnhancementIDs, pEnhancements);
	}
