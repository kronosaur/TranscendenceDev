//	CArmorLimits.cpp
//
//	CArmorLimits class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_CRITERIA_ATTRIB					CONSTLIT("armorCriteria")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define MASS_CLASS_ATTRIB						CONSTLIT("massClass")
#define MAX_ARMOR_ATTRIB						CONSTLIT("maxArmor")
#define MAX_ARMOR_SPEED_ATTRIB					CONSTLIT("maxArmorSpeed")
#define MAX_ARMOR_SPEED_PENALTY_ATTRIB			CONSTLIT("maxArmorSpeedAdj")
#define MIN_ARMOR_SPEED_ATTRIB					CONSTLIT("minArmorSpeed")
#define MIN_ARMOR_SPEED_BONUS_ATTRIB			CONSTLIT("minArmorSpeedAdj")
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
		case typeCompatible:
			{
			//	If necessary, look up actual mass values for std and max armor.

			if (!m_sMaxArmorClass.IsBlank())
				{
				m_iMaxArmorMass = Ctx.pDesign->GetArmorMassDefinitions().GetMassClassMass(m_sMaxArmorClass);
				if (m_iMaxArmorMass == 0)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor mass class: %s."), m_sMaxArmorClass);
					return ERR_FAIL;
					}
				}

			if (!m_sStdArmorClass.IsBlank())
				{
				m_iStdArmorMass = Ctx.pDesign->GetArmorMassDefinitions().GetMassClassMass(m_sStdArmorClass);
				if (m_iStdArmorMass == 0)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor mass class: %s."), m_sStdArmorClass);
					return ERR_FAIL;
					}
				}

			//	Make sure values are in range

			if (m_iMaxArmorMass <= 0)
				{
				m_iType = typeNone;
				m_iStdArmorMass = 0;
				}

			else if (m_iStdArmorMass <= 0)
				m_iStdArmorMass = m_iMaxArmorMass / 2;

			else if (m_iStdArmorMass > m_iMaxArmorMass)
				m_iStdArmorMass = m_iMaxArmorMass;

			break;
			}

		case typeTable:
			{
			//	Calculate and cache armor mass limits

			m_iMaxArmorMass = 0;
			m_iStdArmorMass = 0;
			m_iMaxArmorSpeedPenalty = 0;
			m_iMinArmorSpeedBonus = 0;
			for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
				{
				int iMass = Ctx.pDesign->GetArmorMassDefinitions().GetMassClassMass(m_ArmorLimits[i].sClass);

				//	Max armor mass that we can support.

				if (iMass > m_iMaxArmorMass)
					{
					m_iMaxArmorMass = iMass;
					m_sMaxArmorClass = m_ArmorLimits[i].sClass;
					m_pMaxArmorLimits = &m_ArmorLimits[i];
					}

				//	Standard mass is the armor mass at which we have no bonus or
				//	penalty.

				if (m_ArmorLimits[i].iSpeedAdj == 0)
					{
					if (iMass > m_iStdArmorMass)
						{
						m_iStdArmorMass = iMass;
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

int CArmorLimits::CalcArmorMass (const CItemCtx &ArmorItem) const

//	CalcArmorMass
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
			int iArmorMass = g_pUniverse->GetDesignCollection().GetArmorMassDefinitions().GetMassClassMass(sID);
			if (iArmorMass == 0)
				return ArmorItem.GetItem().GetMassKg();

			return iArmorMass;
			}

		default:
			return ArmorItem.GetItem().GetMassKg();
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
		case typeCompatible:
			{
			int iArmorMass = CalcArmorMass(ArmorItem);

			//	Too heavy?

			if (iArmorMass > GetMaxArmorMass())
				return false;

			//	Add up the total armor mass

			int iTotalArmorMass = iSegmentCount * iArmorMass;

			//	Calculate speed bonus

			if (retiBonus)
				*retiBonus = CalcArmorSpeedBonus(iSegmentCount, iTotalArmorMass);

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
		case typeCompatible:
			{
			const CArmorMassDefinitions &Def = g_pUniverse->GetDesignCollection().GetArmorMassDefinitions();
			int iArmorMass = Def.GetMassClassMass(sArmorClassID);
			if (iArmorMass == 0)
				return false;

			//	Too heavy?

			if (iArmorMass > GetMaxArmorMass())
				return false;

			//	Add up the total armor mass

			int iTotalArmorMass = iSegmentCount * iArmorMass;

			//	Calculate speed bonus

			if (retiBonus)
				*retiBonus = CalcArmorSpeedBonus(iSegmentCount, iTotalArmorMass);

			return true;
			}

		case typeTable:
			{
			//	Search

			for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
				{
				const SArmorLimits *pLimits = &m_ArmorLimits[i];

				//	Skip if the wrong mass class

				if (!strEquals(pLimits->sClass, sArmorClassID))
					continue;

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
		case typeCompatible:
			{
			//	Add up the armor mass

			int iTotalArmorMass = 0;
			for (int i = 0; i < Armor.GetCount(); i++)
				iTotalArmorMass += CalcArmorMass(Armor[i]);

			//	Calculate speed bonus

			return CalcArmorSpeedBonus(Armor.GetCount(), iTotalArmorMass);
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

int CArmorLimits::CalcArmorSpeedBonus (int iSegmentCount, int iTotalArmorMass) const

//	CalcArmorSpeedBonus
//
//	Compute the speed bonus for the total armor mass.

	{
	switch (m_iType)
		{
		case typeNone:
			return 0;

		case typeAuto:
			{
			//	In InitFromXML we guarantee that m_iHullMass is greater than 0.
			//	Otherwise, we would have returned an error at load time.

			ASSERT(m_iHullMass > 0);

			//	Compute the armor mass delta from the standard mass value as a
			//	fraction of the hull mass. The delta is positive if the total 
			//	armor mass is lighter than standard and negative if heavier than
			//	standard.

			int iMassDelta = (m_iStdArmorMass * iSegmentCount) - iTotalArmorMass;
			Metric rMassFrac = Absolute(iMassDelta) / (1000.0 * m_iHullMass);

			//	Scale the mass fraction to a speed adjustment value.

			int iSpeedAdj = mathRound(MASS_TO_SPEED_ADJ_KX * pow(rMassFrac, MASS_TO_SPEED_ADJ_KE));

			//	Penalty or bonus

			return (iMassDelta < 0 ? -iSpeedAdj : iSpeedAdj);
			}

		case typeCompatible:
			{
			int iStdTotalArmorMass = m_iStdArmorMass * iSegmentCount;

			//	Speed pentalty

			if (iTotalArmorMass >= iStdTotalArmorMass)
				{
				if (m_iMaxArmorSpeedPenalty < 0 
						&& m_iMaxArmorMass > m_iStdArmorMass)
					{
					int iMaxTotalArmorMass = m_iMaxArmorMass * iSegmentCount;
					int iRange = iMaxTotalArmorMass - iStdTotalArmorMass;
					int iMassPerTick = iRange / (1 - m_iMaxArmorSpeedPenalty);
					if (iMassPerTick <= 0)
						return 0;

					int iTicks = (iTotalArmorMass - iStdTotalArmorMass) / iMassPerTick;
					return Max(-iTicks, m_iMaxArmorSpeedPenalty);
					}
				else
					return 0;
				}

			//	Speed bonus

			else
				{
				if (m_iMinArmorSpeedBonus > 0)
					{
					int iMinTotalArmorMass = m_iStdArmorMass * iSegmentCount / 2;
					int iRange = iStdTotalArmorMass - iMinTotalArmorMass;
					int iMassPerTick = iRange / m_iMinArmorSpeedBonus;
					if (iMassPerTick <= 0)
						return 0;

					int iTicks = (iStdTotalArmorMass - iTotalArmorMass) / iMassPerTick;
					return Min(iTicks, m_iMinArmorSpeedBonus);
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

ICCItem *CArmorLimits::CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx, int iStdSpeed) const

//	CalcMaxSpeedByArmorMass
//
//	Returns a struct with entries for each value of max speed. Each entry has the
//	smallest armor mass which results in the given speed.
//
//	If there is no variation in speed, we return a single speed value.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult = CC.CreateSymbolTable();

	switch (m_iType)
		{
		case typeNone:
			pResult->SetAt(CC, strFromInt(iStdSpeed), CC.CreateNil());
			break;

		case typeAuto:
		case typeCompatible:
			{
			int iMinSpeed = iStdSpeed + m_iMaxArmorSpeedPenalty;
			int iMaxSpeed = iStdSpeed + m_iMinArmorSpeedBonus;

			for (int i = iMinSpeed; i <= iMaxSpeed; i++)
				{
				CString sLine;

				if (i == iMinSpeed)
					sLine = strPatternSubst(CONSTLIT("%d-%d"), CalcMinArmorMassForSpeed(i, iStdSpeed), m_iMaxArmorMass);
				else if (i == iMaxSpeed)
					{
					if (i == iStdSpeed && i > iMinSpeed)
						sLine = strPatternSubst(CONSTLIT("0-%d"), CalcMinArmorMassForSpeed(i - 1, iStdSpeed) - 1);
					else
						sLine = strPatternSubst(CONSTLIT("0-%d"), CalcMinArmorMassForSpeed(i, iStdSpeed));
					}
				else if (i > iStdSpeed)
					sLine = strPatternSubst(CONSTLIT("%d-%d"), CalcMinArmorMassForSpeed(i + 1, iStdSpeed) + 1, CalcMinArmorMassForSpeed(i, iStdSpeed));
				else
					sLine = strPatternSubst(CONSTLIT("%d-%d"), CalcMinArmorMassForSpeed(i, iStdSpeed), CalcMinArmorMassForSpeed(i - 1, iStdSpeed) - 1);

				pResult->SetStringAt(CC, strFromInt(i), sLine);
				}
			break;
			}

		case typeTable:
			{
			for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
				{
				int iSpeed = iStdSpeed + m_ArmorLimits[i].iSpeedAdj;
				pResult->SetAt(CC, strFromInt(iSpeed), CC.CreateString(m_ArmorLimits[i].sClass));
				}
			break;
			}

		default:
			ASSERT(false);
			break;
		}

	return pResult;
	}

int CArmorLimits::CalcMinArmorMassForSpeed (int iSpeed, int iStdSpeed) const

//	CalcMinArmorMassForSpeed
//
//	Returns the smallest armor mass that is compatible with the given speed.

	{
	int iMinSpeed = iStdSpeed + m_iMaxArmorSpeedPenalty;
	int iMaxSpeed = iStdSpeed + m_iMinArmorSpeedBonus;

	int iPenaltyRange = m_iMaxArmorMass - m_iStdArmorMass;
	int iPenaltyMassPerPoint = iPenaltyRange / (1 - m_iMaxArmorSpeedPenalty);

	int iMinArmorMass = m_iStdArmorMass / 2;
	int iBonusRange = m_iStdArmorMass - iMinArmorMass;
	int iBonusMassPerPoint = (m_iMinArmorSpeedBonus > 0 ? iBonusRange / m_iMinArmorSpeedBonus : 0);

	if (iSpeed < iStdSpeed)
		{
		int iDiff = iStdSpeed - iSpeed;
		return m_iStdArmorMass + (iPenaltyMassPerPoint * iDiff);
		}
	else if (iSpeed == iStdSpeed)
		{
		if (iMinSpeed == iMaxSpeed)
			return m_iStdArmorMass;
		else
			return (m_iStdArmorMass - iBonusMassPerPoint) + 1;
		}
	else
		{
		int iDiff = iSpeed - iStdSpeed;
		return m_iStdArmorMass - (iBonusMassPerPoint * iDiff);
		}
	}

void CArmorLimits::CalcSummary (const CArmorMassDefinitions &Defs, SSummary &Summary) const

//	CalcSummary
//
//	Calculate some summary values about the limits.

	{
	switch (m_iType)
		{
		case typeNone:
			Summary.iMaxArmorMass = 100000;
			Summary.iStdArmorMass = 100000;
			Summary.iMaxSpeedBonus = 0;
			Summary.iMaxSpeedPenalty = 0;
			Summary.rMaxArmorFrequency = 1.0;
			Summary.rStdArmorFrequency = 1.0;
			break;

		case typeAuto:
		case typeTable:
			{
			Summary.iMaxArmorMass = GetMaxArmorMass();
			Summary.iStdArmorMass = GetStdArmorMass();
			Summary.iMaxSpeedBonus = GetMinArmorSpeedBonus();
			Summary.iMaxSpeedPenalty = GetMaxArmorSpeedPenalty();
			Summary.rMaxArmorFrequency = Defs.GetFrequencyMax(m_sMaxArmorClass);
			Summary.rStdArmorFrequency = Defs.GetFrequencyMax(m_sStdArmorClass);
			break;
			}

		case typeCompatible:
			{
			Summary.iMaxArmorMass = GetMaxArmorMass();
			Summary.iStdArmorMass = GetStdArmorMass();
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

				int iMass = pItemType->GetMassKg(CItemCtx());
				iTotalArmor++;

				if (iMass <= Summary.iStdArmorMass)
					iTotalStdArmor++;

				if (iMass <= Summary.iMaxArmorMass)
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
		case typeCompatible:
			{
			if (Item.GetMassKg() > GetMaxArmorMass())
				return resultTooHeavy;

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
					return resultTooHeavy;
				}

			return resultOK;
			}

		default:
			ASSERT(false);
			return resultIncompatible;
		}
	}

bool CArmorLimits::FindArmorLimits (const CItemCtx &ItemCtx, const SArmorLimits **retpLimits, bool *retbClassFound) const

//	FindArmorLimits
//
//	Finds the armor limit descriptor for this armor item.

	{
	if (retbClassFound) *retbClassFound = false;

	const CItem &ArmorItem = ItemCtx.GetItem();
	CArmorClass *pArmor = ItemCtx.GetArmorClass();
	if (pArmor == NULL)
		return false;

	const CString &sMassClass = pArmor->GetMassClass(ItemCtx);

	for (int i = 0; i < m_ArmorLimits.GetCount(); i++)
		{
		const SArmorLimits *pLimits = &m_ArmorLimits[i];

		//	Skip if the wrong mass class

		if (!strEquals(pLimits->sClass, sMassClass))
			continue;

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

void CArmorLimits::InitDefaultArmorLimits (int iMass, int iMaxSpeed, Metric rThrustRatio)

//	InitDefaultArmorLimits
//
//	If no armor limits are specified, we initialize them here based on mass, 
//	speed, and thrust

	{
	//	If we're 1000 tons or more, then no limits

	if (iMass == 0 || iMass >= 1000)
		return;

	//	Compute the heaviest segment of armor we can install.

	const Metric MAX_ARMOR_POWER = 0.7;
	const Metric MAX_ARMOR_FACTOR = 0.6;
	const Metric STD_THRUST_RATIO = 7.0;
	const int MAX_ARMOR_MAX = 50;

	int iMaxArmorTons = Min(MAX_ARMOR_MAX, mathRound(MAX_ARMOR_FACTOR * pow((Metric)iMass, MAX_ARMOR_POWER) * Max(1.0, rThrustRatio / STD_THRUST_RATIO)));
	m_iMaxArmorMass = 1000 * iMaxArmorTons;

	//	Compute the mass of standard armor

	const Metric STD_ARMOR_POWER = 0.8;
	const Metric STD_ARMOR_FACTOR = 0.8;

	int iStdArmorTons = mathRound(STD_ARMOR_FACTOR * pow((Metric)iMaxArmorTons, STD_ARMOR_POWER));
	m_iStdArmorMass = 1000 * iStdArmorTons;

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
	CString sMassClass = pLimits->GetAttribute(MASS_CLASS_ATTRIB);
	if (sMassClass.IsBlank())
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid mass class: %s"), sMassClass);
		return ERR_FAIL;
		}

	//	Criteria

	CString sCriteria = pLimits->GetAttribute(CRITERIA_ATTRIB);

	//	Limits

	int iSpeedAdj = pLimits->GetAttributeInteger(SPEED_ADJ_ATTRIB);

	//	Now that we have all elements, create the entry.

	SArmorLimits &NewLimits = *m_ArmorLimits.Insert();
	NewLimits.sClass = sMassClass;

	if (!sCriteria.IsBlank())
		{
		NewLimits.pCriteria.Set(new CItemCriteria);
		CItem::ParseCriteria(sCriteria, NewLimits.pCriteria);
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

	CString sCriteria;
	if (pDesc->FindAttribute(ARMOR_CRITERIA_ATTRIB, &sCriteria))
		CItem::ParseCriteria(sCriteria, &m_ArmorCriteria);
	else
		CItem::InitCriteriaAll(&m_ArmorCriteria);

	//	We never exclude virtual items because it is common for (e.g., biotech)
	//	classes to have virtual armor.

	m_ArmorCriteria.bExcludeVirtual = false;

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
			m_iMaxArmorMass = strToInt(sValue, -1);
			if (m_iMaxArmorMass < 0)
				m_sMaxArmorClass = sValue;
			}
		else
			m_iMaxArmorMass = 0;

		if (pDesc->FindAttribute(STD_ARMOR_ATTRIB, &sValue))
			{
			m_iStdArmorMass = strToInt(sValue, -1);
			if (m_iStdArmorMass < 0)
				m_sStdArmorClass = sValue;
			}
		else
			m_iStdArmorMass = 0;

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
		else if (m_iMaxArmorMass || !m_sMaxArmorClass.IsBlank())
			{
			if (m_iHullMass == 0)
				{
				Ctx.sError = CONSTLIT("Cannot compute armor limits if hull mass is 0.");
				return ERR_FAIL;
				}

			m_iType = typeAuto;
			}
		else
			m_iType = typeNone;
		}

	return NOERROR;
	}
