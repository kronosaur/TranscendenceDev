//	CArmorLimits.cpp
//
//	CArmorLimits class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_CRITERIA_ATTRIB					CONSTLIT("armorCriteria")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define MASS_ATTRIB								CONSTLIT("mass")
#define MASS_CLASS_ATTRIB						CONSTLIT("massClass")
#define MAX_ARMOR_ATTRIB						CONSTLIT("maxArmor")
#define MAX_ARMOR_SPEED_ATTRIB					CONSTLIT("maxArmorSpeed")
#define MAX_ARMOR_SPEED_PENALTY_ATTRIB			CONSTLIT("maxArmorSpeedAdj")
#define MIN_ARMOR_SPEED_ATTRIB					CONSTLIT("minArmorSpeed")
#define MIN_ARMOR_SPEED_BONUS_ATTRIB			CONSTLIT("minArmorSpeedAdj")
#define ARMOR_CLASS_ATTRIB						CONSTLIT("armorClass")
#define SIZE_ATTRIB								CONSTLIT("size")
#define SPEED_ADJ_ATTRIB						CONSTLIT("speedAdj")
#define STD_ARMOR_ATTRIB						CONSTLIT("stdArmor")

static constexpr Metric MASS_TO_SPEED_ADJ_KX =	4.0;
static constexpr Metric MASS_TO_SPEED_ADJ_KE =	0.5;

ALERROR CArmorLimits::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind now that we have all design types.

	{
	ASSERT(Ctx.pDesign);
	if (Ctx.pDesign == NULL) return ERR_FAIL;

	//	We need to do some post-processing depending on the type of limits that
	//	we have.

	switch (m_iType)
		{
		case typeNone:
			break;

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeCompatible:
			{
			//	If necessary, look up actual mass values for std and max armor.

			if (!m_sMaxArmorClass.IsBlank())
				{
				m_rMaxArmorSize = Ctx.pDesign->GetArmorMassDefinitions().GetArmorClassSize(m_sMaxArmorClass);
				if (m_rMaxArmorSize == 0)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor class: %s."), m_sMaxArmorClass);
					return ERR_FAIL;
					}
				}

			if (!m_sStdArmorClass.IsBlank())
				{
				m_rStdArmorSize = Ctx.pDesign->GetArmorMassDefinitions().GetArmorClassSize(m_sStdArmorClass);
				if (m_rStdArmorSize == 0)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor class: %s."), m_sStdArmorClass);
					return ERR_FAIL;
					}
				}

			//	Make sure values are in range

			if (m_rMaxArmorSize <= 0)
				{
				m_iType = typeNone;
				m_rStdArmorSize = 0;
				}

			else if (m_rStdArmorSize <= 0)
				m_rStdArmorSize = m_rMaxArmorSize / 2;

			else if (m_rStdArmorSize > m_rMaxArmorSize)
				m_rStdArmorSize = m_rMaxArmorSize;

			break;
			}

		case typeTable:
			{
			//	Calculate and cache armor mass limits

			m_rMaxArmorSize = 0;
			m_rStdArmorSize = 0;
			m_iMaxArmorSpeedPenalty = 0;
			m_iMinArmorSpeedBonus = 0;
			for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
				{
				Metric rSize = m_ArmorLimits[i].rSize;
				if (rSize == 0)
					rSize = Ctx.pDesign->GetArmorMassDefinitions().GetArmorClassSize(m_ArmorLimits[i].sClass);

				//	Max armor mass that we can support.

				if (rSize > m_rMaxArmorSize)
					{
					m_rMaxArmorSize = rSize;
					m_sMaxArmorClass = m_ArmorLimits[i].sClass;
					m_pMaxArmorLimits = &m_ArmorLimits[i];
					}

				//	Standard mass is the armor mass at which we have no bonus or
				//	penalty.

				if (m_ArmorLimits[i].iSpeedAdj == 0)
					{
					if (rSize > m_rStdArmorSize)
						{
						m_rStdArmorSize = rSize;
						m_sStdArmorClass = m_ArmorLimits[i].sClass;
						m_pStdArmorLimits = &m_ArmorLimits[i];
						}
					}

				//	Keep track of the highest penalty/bonus

				else if (m_ArmorLimits[i].iSpeedAdj < 0)
					{
					if (m_ArmorLimits[i].iSpeedAdj < m_iMaxArmorSpeedPenalty)
						m_iMaxArmorSpeedPenalty = m_ArmorLimits[i].iSpeedAdj;
					}
				else
					{
					if (m_ArmorLimits[i].iSpeedAdj > m_iMinArmorSpeedBonus)
						m_iMinArmorSpeedBonus = m_ArmorLimits[i].iSpeedAdj;
					}
				}
			break;
			}

		default:
			ASSERT(false);
			break;
		}

	//	Done

	return NOERROR;
	}

Metric CArmorLimits::CalcArmorSize (const CItemCtx &ArmorItem) const

//	CalcArmorSize
//
//	Returns mass of the given armor item for purposes of calculating speed 
//	adjustment.

	{
	switch (m_iType)
		{
		case typeAuto:
			{
			//	We always use the mass class value so that we get consistent
			//	speed adjustment. Otherwise, different "medium" armor might have
			//	different speed adjustments.

			CArmorClass *pClass = ArmorItem.GetArmorClass();
			if (pClass == NULL)
				return 0;

			CString sID = pClass->GetMassClass(ArmorItem);
			Metric rArmorSize = g_pUniverse->GetDesignCollection().GetArmorMassDefinitions().GetArmorClassSize(sID);
			if (rArmorSize < g_Epsilon)
				return ArmorItem.GetItem().GetVolume();

			return rArmorSize;
			}

		default:
			return ArmorItem.GetItem().GetVolume();
		}
	}

bool CArmorLimits::CalcArmorSpeedBonus (CItemCtx &ArmorItem, int iSegmentCount, int *retiBonus) const

//	CalcArmorSpeedBonus
//
//	Figures out the speed bonus if the ship class had the given armor segment
//	installed. If the armor segment is too heavy for the class, we return FALSE.

	{
	switch (m_iType)
		{
		case typeNone:
			{
			if (retiBonus) *retiBonus = 0;
			return true;
			}

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeCompatible:
			{
			Metric rArmorSize = CalcArmorSize(ArmorItem);

			//	Too heavy?

			if (rArmorSize > GetMaxArmorSize())
				return false;

			//	Add up the total armor mass

			Metric rTotalArmorSize = iSegmentCount * rArmorSize;

			//	Calculate speed bonus

			if (retiBonus)
				*retiBonus = CalcArmorSpeedBonus(iSegmentCount, rTotalArmorSize);

			return true;
			}

		case typeTable:
			{
			//	Look for this armor in the table (by mass class). If not found, then 
			//	it means that we can't mount this kind of armor.

			const SArmorLimits *pLimits;
			if (!FindArmorLimits(ArmorItem, &pLimits))
				return false;

			//	Return the speed bonus (or penalty)

			if (retiBonus)
				*retiBonus = pLimits->iSpeedAdj;

			return true;
			}

		default:
			ASSERT(false);
			return false;
		}
	}

bool CArmorLimits::CalcArmorSpeedBonus (const CString &sArmorClassID, int iSegmentCount, int *retiBonus) const

//	CalcArmorSpeedBonus
//	CalcArmorSpeedBonus
//
//	Figures out the speed bonus if the ship class had the given armor segment
//	installed. If the armor segment is too heavy for the class, we return FALSE.

	{
	switch (m_iType)
		{
		case typeNone:
			{
			if (retiBonus) *retiBonus = 0;
			return true;
			}

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeCompatible:
			{
			const CArmorClassDefinitions &Def = g_pUniverse->GetDesignCollection().GetArmorMassDefinitions();
			Metric rArmorSize = Def.GetArmorClassSize(sArmorClassID);
			if (rArmorSize == 0)
				return false;

			//	Too heavy?

			if (rArmorSize > GetMaxArmorSize())
				return false;

			//	Add up the total armor mass

			Metric rTotalArmorSize = iSegmentCount * rArmorSize;

			//	Calculate speed bonus

			if (retiBonus)
				*retiBonus = CalcArmorSpeedBonus(iSegmentCount, rTotalArmorSize);

			return true;
			}

		case typeTable:
			{
			const CArmorClassDefinitions &Def = g_pUniverse->GetDesignCollection().GetArmorMassDefinitions();
			Metric rArmorSize = Def.GetArmorClassSize(sArmorClassID);

			//	Search

			for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
				{
				const SArmorLimits *pLimits = &m_ArmorLimits[i];

				//	Skip if the wrong mass class

				if (pLimits->rSize)
					{
					if (rArmorSize > pLimits->rSize)
						continue;
					}
				else
					{
					if (!strEquals(pLimits->sClass, sArmorClassID))
						continue;
					}

				//	Found

				if (retiBonus)
					*retiBonus = pLimits->iSpeedAdj;
				return true;
				}

			//	If we get this far, then it means that we cannot install this armor 
			//	class.

			return false;
			}

		default:
			ASSERT(false);
			return false;
		}
	}

int CArmorLimits::CalcArmorSpeedBonus (const TArray<CItemCtx> &Armor) const

//	CalcArmorSpeedBonus
//
//	Computes the speed bonus based on the given set of installed armor segments.

	{
	//	Short-circuit. This can happen for virtual classes, etc.

	if (Armor.GetCount() == 0)
		return 0;

	//	Compute based on type

	switch (m_iType)
		{
		case typeNone:
			return 0;

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeCompatible:
			{
			//	Add up the armor mass

			Metric rTotalArmorSize = 0;
			for (int i = 0; i < Armor.GetCount(); i++)
				rTotalArmorSize += CalcArmorSize(Armor[i]);

			//	Calculate speed bonus

			return CalcArmorSpeedBonus(Armor.GetCount(), rTotalArmorSize);
			}

		case typeTable:
			{
			//	Loop over all armor segments and compute the speed adjustment.

			int iTotalSpeedAdj = 0;
			for (int i = 0; i < Armor.GetCount(); i++)
				{
				//	Look for this armor in the table (by mass class). If not found, then 
				//	it means that we can't mount this kind of armor.

				const SArmorLimits *pLimits;
				if (!FindArmorLimits(Armor[i], &pLimits))
					{
					//	This should never happen. It means that an armor segment got
					//	installed that should not have been installed. We ignore it.

					continue;
					}

				iTotalSpeedAdj += pLimits->iSpeedAdj;
				}

			//	Average out the speed bonus

			return mathRound((Metric)iTotalSpeedAdj / (Metric)Armor.GetCount());
			}

		default:
			ASSERT(false);
			return 0;
		}
	}

int CArmorLimits::CalcArmorSpeedBonus (int iSegmentCount, Metric rTotalArmorSize) const

//	CalcArmorSpeedBonus
//
//	Compute the speed bonus for the total armor mass.

	{
	switch (m_iType)
		{
		case typeNone:
			return 0;

		case typeAuto:
		case typeAutoSpeedAdj:
			{
			//	In InitFromXML we guarantee that m_iHullMass is greater than 0.
			//	Otherwise, we would have returned an error at load time.

			ASSERT(m_iHullMass > 0);

			//	Compute the armor size delta from the standard size value as a
			//	fraction of the hull mass * massToSizeRatio.
			//	The delta is positive if the total armor mass is lighter than
			//	standard and negative if heavier than standard.

			Metric rSizeDelta = (m_rStdArmorSize * iSegmentCount) - rTotalArmorSize;
			Metric rSizeFrac = Absolute(rSizeDelta) / (m_iHullMass);

			//	Scale the mass fraction to a speed adjustment value.

			int iSpeedAdj = mathRound(MASS_TO_SPEED_ADJ_KX * pow(rSizeFrac, MASS_TO_SPEED_ADJ_KE));

			//	Penalty or bonus

			return (rSizeDelta < 0 ? -iSpeedAdj : iSpeedAdj);
			}

		case typeCompatible:
			{
			Metric rStdTotalArmorSize = m_rStdArmorSize * iSegmentCount;

			//	Speed pentalty

			if (rTotalArmorSize >= rStdTotalArmorSize)
				{
				if (m_iMaxArmorSpeedPenalty < 0 
						&& m_rMaxArmorSize > m_rStdArmorSize)
					{
					Metric rMaxTotalArmorSize = m_rMaxArmorSize * iSegmentCount;
					Metric rRange = rMaxTotalArmorSize - rStdTotalArmorSize;
					Metric rSizePerInc = rRange / (1 - m_iMaxArmorSpeedPenalty);
					if (rSizePerInc <= 0)
						return 0;

					int iIncrements = (int)((rTotalArmorSize - rStdTotalArmorSize) / rSizePerInc);
					return Max(-iIncrements, m_iMaxArmorSpeedPenalty);
					}
				else
					return 0;
				}

			//	Speed bonus

			else
				{
				if (m_iMinArmorSpeedBonus > 0)
					{
					Metric rMinTotalArmorSize = m_rStdArmorSize * iSegmentCount / 2;
					Metric rRange = rStdTotalArmorSize - rMinTotalArmorSize;
					Metric rSizePerInc = rRange / m_iMinArmorSpeedBonus;
					if (rSizePerInc <= 0)
						return 0;

					int iIncrements = (int)((rStdTotalArmorSize - rTotalArmorSize) / rSizePerInc);
					return Min(iIncrements, m_iMinArmorSpeedBonus);
					}
				else
					return 0;
				}
			}

		//	This should never happen because we don't call this function when
		//	we have an explicit table.

		case typeTable:
			ASSERT(false);
			return 0;

		default:
			ASSERT(false);
			return 0;
		}
	}

//	CalcMaxSpeedByArmorSize
//
//	Returns a struct with entries for each value of max speed. Each entry has the
//	smallest armor mass which results in the given speed.
//
//	If there is no variation in speed, we return a single speed value.
//
ICCItemPtr CArmorLimits::CalcMaxSpeedByArmorSize (CCodeChainCtx &Ctx, int iStdSpeed) const

	{
	ICCItemPtr pResult(ICCItem::SymbolTable);

	switch (m_iType)
		{
		case typeNone:
			pResult->SetAt(strFromInt(iStdSpeed), ICCItemPtr::Nil());
			break;

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeCompatible:
			{
			int iMinSpeed = iStdSpeed + m_iMaxArmorSpeedPenalty;
			int iMaxSpeed = iStdSpeed + m_iMinArmorSpeedBonus;

			for (int i = iMinSpeed; i <= iMaxSpeed; i++)
				{
				CString sLine;

				if (i == iMinSpeed)
					sLine = strPatternSubst(CONSTLIT("%r-%r"), CalcMinArmorSizeForSpeed(i, iStdSpeed), m_rMaxArmorSize);
				else if (i == iMaxSpeed)
					{
					if (i == iStdSpeed && i > iMinSpeed)
						sLine = strPatternSubst(CONSTLIT("0-%r"), CalcMinArmorSizeForSpeed(i - 1, iStdSpeed) - 1);
					else
						sLine = strPatternSubst(CONSTLIT("0-%r"), CalcMinArmorSizeForSpeed(i, iStdSpeed));
					}
				else if (i > iStdSpeed)
					sLine = strPatternSubst(CONSTLIT("%r-%r"), CalcMinArmorSizeForSpeed(i + 1, iStdSpeed) + 1, CalcMinArmorSizeForSpeed(i, iStdSpeed));
				else
					sLine = strPatternSubst(CONSTLIT("%r-%r"), CalcMinArmorSizeForSpeed(i, iStdSpeed), CalcMinArmorSizeForSpeed(i - 1, iStdSpeed) - 1);

				pResult->SetStringAt(strFromInt(i), sLine);
				}
			break;
			}

		case typeTable:
			{
			for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
				{
				int iSpeed = iStdSpeed + m_ArmorLimits[i].iSpeedAdj;
				if (!m_ArmorLimits[i].sClass.IsBlank())
					pResult->SetStringAt(strFromInt(iSpeed), m_ArmorLimits[i].sClass);
				else
					pResult->SetStringAt(strFromInt(iSpeed), strFromDouble(m_ArmorLimits[i].rSize));
				}
			break;
			}

		default:
			ASSERT(false);
			break;
		}

	return pResult;
	}

//	CalcMinArmorSizeForSpeed
//
//	Returns the smallest armor size that is compatible with the given speed.
//
Metric CArmorLimits::CalcMinArmorSizeForSpeed (int iSpeed, int iStdSpeed) const

	{
	int iMinSpeed = iStdSpeed + m_iMaxArmorSpeedPenalty;
	int iMaxSpeed = iStdSpeed + m_iMinArmorSpeedBonus;

	Metric rPenaltyRange = m_rMaxArmorSize - m_rStdArmorSize;
	Metric rPenaltySizePerPoint = rPenaltyRange / (1 - m_iMaxArmorSpeedPenalty);

	Metric rMinArmorSize = m_rStdArmorSize / 2;
	Metric rBonusRange = m_rStdArmorSize - rMinArmorSize;
	Metric rBonusSizePerPoint = (m_iMinArmorSpeedBonus > 0 ? rBonusRange / m_iMinArmorSpeedBonus : 0);

	if (iSpeed < iStdSpeed)
		{
		int iDiff = iStdSpeed - iSpeed;
		return m_rStdArmorSize + (rPenaltySizePerPoint * iDiff);
		}
	else if (iSpeed == iStdSpeed)
		{
		if (iMinSpeed == iMaxSpeed)
			return m_rStdArmorSize;
		else
			return (m_rStdArmorSize - rBonusSizePerPoint) + 1;
		}
	else
		{
		int iDiff = iSpeed - iStdSpeed;
		return m_rStdArmorSize - (rBonusSizePerPoint * iDiff);
		}
	}

//	CalcSummary
//
//	Calculate some summary values about the limits.
//
void CArmorLimits::CalcSummary (const CArmorClassDefinitions &Defs, SSummary &Summary) const

	{
	switch (m_iType)
		{
		case typeNone:
			Summary.rMaxArmorSize = 100000;
			Summary.rStdArmorSize = 100000;
			Summary.iMaxSpeedBonus = 0;
			Summary.iMaxSpeedPenalty = 0;
			Summary.rMaxArmorFrequency = 1.0;
			Summary.rStdArmorFrequency = 1.0;
			break;

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeTable:
			{
			Summary.rMaxArmorSize = GetMaxArmorSize();
			Summary.rStdArmorSize = GetStdArmorSize();
			Summary.iMaxSpeedBonus = GetMinArmorSpeedBonus();
			Summary.iMaxSpeedPenalty = GetMaxArmorSpeedPenalty();
			Summary.rMaxArmorFrequency = Defs.GetFrequencyMax(m_sMaxArmorClass);
			Summary.rStdArmorFrequency = Defs.GetFrequencyMax(m_sStdArmorClass);
			break;
			}

		case typeCompatible:
			{
			Summary.rMaxArmorSize = GetMaxArmorSize();
			Summary.rStdArmorSize = GetStdArmorSize();
			Summary.iMaxSpeedBonus = GetMinArmorSpeedBonus();
			Summary.iMaxSpeedPenalty = GetMaxArmorSpeedPenalty();

			int iTotalArmor = 0;
			int iTotalMaxArmor = 0;
			int iTotalStdArmor = 0;
			for (int i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
				{
				CItemType *pItemType = g_pUniverse->GetItemType(i);
				if (!pItemType->IsArmor())
					continue;

				CItem Item(pItemType, 1);
				Metric rSize = Item.GetVolume();
				iTotalArmor++;

				if (rSize <= Summary.rStdArmorSize)
					iTotalStdArmor++;

				if (rSize <= Summary.rMaxArmorSize)
					iTotalMaxArmor++;
				}

			if (iTotalArmor > 0)
				{
				Summary.rMaxArmorFrequency = (Metric)iTotalMaxArmor / (Metric)iTotalArmor;
				Summary.rStdArmorFrequency = (Metric)iTotalStdArmor / (Metric)iTotalArmor;
				}
			else
				{
				Summary.rMaxArmorFrequency = 0.0;
				Summary.rStdArmorFrequency = 0.0;
				}
			break;
			}

		default:
			ASSERT(false);
			break;
		}
	}

CArmorLimits::EResults CArmorLimits::CanInstallArmor (const CItem &Item) const

//	CanInstallArmor
//
//	Determines whether we can install the armor.

	{
	//	If we don't match the criteria, then we fail.

	if (!Item.MatchesCriteria(m_ArmorCriteria))
		return resultIncompatible;

	//	Check mass

	switch (m_iType)
		{
		case typeNone:
			return resultOK;

		case typeAuto:
		case typeAutoSpeedAdj:
		case typeCompatible:
			{
			Metric rArmorSize = CalcArmorSize(Item);
			if (rArmorSize > GetMaxArmorSize())
				return resultTooLarge;

			return resultOK;
			}

		case typeTable:
			{
			//	Look up this armor segment. If we don't find a match, then it means
			//	the armor is incompatible.

			bool bFoundClass;
			if (!FindArmorLimits(CItemCtx(Item), NULL, &bFoundClass))
				{
				//	If we found an entry for the mass class, then it means that we 
				//	normally could handle the mass class but that we failed for a 
				//	different (custom) reason.

				if (bFoundClass)
					return resultIncompatible;

				//	Otherwise, it means that we're too heavy.

				else
					return resultTooLarge;
				}

			return resultOK;
			}

		default:
			ASSERT(false);
			return resultIncompatible;
		}
	}

//	FindArmorLimits
//
//	Finds the armor limit descriptor for this armor item.
//
bool CArmorLimits::FindArmorLimits (const CItemCtx &ItemCtx, const SArmorLimits **retpLimits, bool *retbClassFound) const

	{
	if (retbClassFound) *retbClassFound = false;

	const CItem &ArmorItem = ItemCtx.GetItem();
	CArmorClass *pArmor = ItemCtx.GetArmorClass();
	if (pArmor == NULL)
		return false;

	const CString &sMassClass = pArmor->GetMassClass(ItemCtx);
	Metric rSize = ArmorItem.GetVolume();

	for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
		{
		const SArmorLimits *pLimits = &m_ArmorLimits[i];

		//	Skip if the wrong class

		if (pLimits->rSize)
			{
			if (rSize > pLimits->rSize)
				continue;
			}
		else
			{
			if (!strEquals(pLimits->sClass, sMassClass))
				continue;
			}

		//	We at least found an entry for this class. We need this to tell 
		//	whether we failed due to incompatibility or beacuse it was too heavy.

		if (retbClassFound)
			*retbClassFound = true;

		//	Make sure we match criteria

		if (pLimits->pCriteria && !ArmorItem.MatchesCriteria(*pLimits->pCriteria))
			continue;

		//	Found

		if (retpLimits)
			*retpLimits = pLimits;

		return true;
		}

	//	If we get this far, then not found.

	return false;
	}

//	InitDefaultArmorLimits
//
//	If no armor limits are specified, we initialize them here based on mass, 
//	speed, and thrust
//
void CArmorLimits::InitDefaultArmorLimits (int iMass, int iMaxSpeed, Metric rThrustRatio)

	{
	//	If we're 1000 tons or more, then no limits

	if (iMass == 0 || iMass >= 1000)
		return;

	//	Compute the heaviest segment of armor we can install.

	const Metric MAX_ARMOR_POWER = 0.7;
	const Metric MAX_ARMOR_FACTOR = 0.6;
	const Metric STD_THRUST_RATIO = 7.0;
	const Metric MAX_ARMOR_MAX = 50;
	Metric rHullSize = iMass * g_pUniverse->GetEngineOptions().GetItemXMLMassToVolumeRatio();

	m_rMaxArmorSize = Min(MAX_ARMOR_MAX, MAX_ARMOR_FACTOR * pow(rHullSize, MAX_ARMOR_POWER) * Max(1.0, rThrustRatio / STD_THRUST_RATIO));

	//	Compute the mass of standard armor

	const Metric STD_ARMOR_POWER = 0.8;
	const Metric STD_ARMOR_FACTOR = 0.8;

	m_rStdArmorSize = STD_ARMOR_FACTOR * pow(m_rMaxArmorSize, STD_ARMOR_POWER);

	//	Compute the max speed at maximum armor

	const Metric MAX_ARMOR_SPEED_ADJ = 0.1;

	int iSpeedDec = mathRound((Metric)iMaxSpeed * MAX_ARMOR_SPEED_ADJ);
	m_iMaxArmorSpeedPenalty = -iSpeedDec;

	//	Compute the max speed at minimum armor

	const int MIN_ARMOR_SPEED_OFFSET = 26;
	const Metric MIN_ARMOR_SPEED_ADJ = 0.25;
	const Metric MIN_ARMOR_THRUST_ADJ = 0.5;

	Metric rThrustRatioLimit = Max(1.0, MIN_ARMOR_THRUST_ADJ * rThrustRatio);
	int iSpeedInc = Max(0, mathRound(Min(rThrustRatioLimit, (MIN_ARMOR_SPEED_OFFSET - iMaxSpeed) * MIN_ARMOR_SPEED_ADJ)));
	m_iMinArmorSpeedBonus = iSpeedInc;
	}

ALERROR CArmorLimits::InitArmorLimitsFromXML (SDesignLoadCtx &Ctx, CXMLElement *pLimits)

//	InitArmorLimitsFromXML
//
//	Adds an <ArmorLimits> element. CHullDesc is responsible for calling us here.
//	We don't parse this inside CArmorLimits::InitFromXML because the <Hull> 
//	element might have other sub-elements that we don't understand.

	{
	CString sArmorClass = pLimits->GetAttribute(MASS_CLASS_ATTRIB);
	Metric rSize = pLimits->GetAttributeDoubleBounded(SIZE_ATTRIB, 1, -1, 0);

	//	If the new size value isnt loaded or we are on an old API, try using mass

	if (rSize < g_Epsilon || Ctx.GetAPIVersion() < 59)
		{
		int iMass = pLimits->GetAttributeIntegerBounded(MASS_ATTRIB, 1, -1, 0);
		rSize = iMass * g_pUniverse->GetEngineOptions().GetItemXMLMassToVolumeRatio();
		}

	//	Either massClass or mass must be defined.

	if (sArmorClass.IsBlank() && rSize < g_Epsilon)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor class: %s"), sArmorClass);
		return ERR_FAIL;
		}

	//	Criteria

	CString sCriteria = pLimits->GetAttribute(CRITERIA_ATTRIB);

	//	Limits

	int iSpeedAdj = pLimits->GetAttributeInteger(SPEED_ADJ_ATTRIB);

	//	Now that we have all elements, create the entry.

	SArmorLimits &NewLimits = *m_ArmorLimits.Insert();
	NewLimits.sClass = sArmorClass;
	NewLimits.rSize = (sArmorClass.IsBlank() ? rSize : 0);

	if (!sCriteria.IsBlank())
		{
		NewLimits.pCriteria.Set(new CItemCriteria(sCriteria));
		}

	NewLimits.iSpeedAdj = iSpeedAdj;

	//	Done

	return NOERROR;
	}

ALERROR CArmorLimits::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, int iHullMass, int iMaxSpeed)

//	InitFromXML
//
//	Initializes from <Hull> element. NOTE: This is guaranteed to be called AFTER
//	any calls to InitArmorLimitsFromXML.

	{
	m_iHullMass = iHullMass;

	//	Armor criteria is valid for all types

	m_ArmorCriteria.Init(pDesc->GetAttribute(ARMOR_CRITERIA_ATTRIB), CItemCriteria::ALL);

	//	We never exclude virtual items because it is common for (e.g., biotech)
	//	classes to have virtual armor.

	m_ArmorCriteria.SetExcludeVirtual(false);

	//	If we've already got a table of armor limits then we're using that method.

	if (m_ArmorLimits.GetCount() > 0)
		{
		m_iType = typeTable;
		}

	//	Otherwise, load the older method.

	else
		{
		//	Standard and maximum armor mass can be specified either by mass 
		//	class ID or by kilograms.
		//
		//	NOTE: If we specify an ID then we have to wait until Bind to resolve
		//	it, because all types are not loaded yet.

		CString sValue;
		if (pDesc->FindAttribute(MAX_ARMOR_ATTRIB, &sValue))
			{
			m_rMaxArmorSize = strToDouble(sValue, -1.0);
			if (m_rMaxArmorSize < 0 || IS_NAN(m_rMaxArmorSize))
				m_sMaxArmorClass = sValue;
			}
		else
			m_rMaxArmorSize = 0;

		if (pDesc->FindAttribute(STD_ARMOR_ATTRIB, &sValue))
			{
			m_rStdArmorSize = strToDouble(sValue, -1.0);
			if (m_rStdArmorSize < 0 || IS_NAN(m_rMaxArmorSize))
				m_sStdArmorClass = sValue;
			}
		else
			m_rStdArmorSize = 0;

		//	Speed bonus/penalty

		int iValue;
		if (pDesc->FindAttributeInteger(MAX_ARMOR_SPEED_PENALTY_ATTRIB, &iValue))
			m_iMaxArmorSpeedPenalty = Max(Min(iValue, 0), -iMaxSpeed);
		else
			m_iMaxArmorSpeedPenalty = pDesc->GetAttributeIntegerBounded(MAX_ARMOR_SPEED_ATTRIB, 0, iMaxSpeed, iMaxSpeed) - iMaxSpeed;

		if (pDesc->FindAttributeInteger(MIN_ARMOR_SPEED_BONUS_ATTRIB, &iValue))
			m_iMinArmorSpeedBonus = Min(Max(0, iValue), 100 - iMaxSpeed);
		else
			m_iMinArmorSpeedBonus = pDesc->GetAttributeIntegerBounded(MIN_ARMOR_SPEED_ATTRIB, iMaxSpeed, 100, iMaxSpeed) - iMaxSpeed;

		//	Based on what attributes we specified, we can now determine how we 
		//	should compute armor limits.

		if (m_iMaxArmorSpeedPenalty || m_iMinArmorSpeedBonus)
			m_iType = typeCompatible;
		else if (m_rMaxArmorSize || !m_sMaxArmorClass.IsBlank())
			{
			if (m_iHullMass == 0)
				{
				Ctx.sError = CONSTLIT("Cannot compute armor limits if hull mass is 0.");
				return ERR_FAIL;
				}

			if (!m_sMaxArmorClass.IsBlank())
				m_iType = typeAuto;
			else
				m_iType = typeAutoSpeedAdj;
			}
		else
			m_iType = typeNone;
		}

	return NOERROR;
	}
