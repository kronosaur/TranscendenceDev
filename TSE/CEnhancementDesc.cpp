//	CEnhancementDesc.cpp
//
//	CEnhancementDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ENHANCE_TAG								CONSTLIT("Enhance")

#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define ENHANCEMENT_ATTRIB						CONSTLIT("enhancement")
#define LEVEL_CHECK_ATTRIB						CONSTLIT("levelCheck")
#define TYPE_ATTRIB								CONSTLIT("type")

bool CEnhancementDesc::Accumulate (int iLevel, const CItem &Target, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) const

//	Accumulate
//
//	Adds enhancements to the stack. Returns TRUE if any enhancements were added.

	{
	int i;
	bool bEnhanced = false;

	for (i = 0; i < m_Enhancements.GetCount(); i++)
		{
		//	If this type of enhancement has already been applied, skip it

		if (!m_Enhancements[i].sType.IsBlank()
				&& EnhancementIDs.Find(m_Enhancements[i].sType))
			continue;

		//	If we don't match the criteria, skip it.

		if (!Target.MatchesCriteria(m_Enhancements[i].Criteria))
			continue;

		//	If we're checking level, then make sure our level is at least as 
		//	high as the target.

		if (!m_Enhancements[i].LevelCheck.MatchesCriteria(iLevel, Target))
			continue;

		//	Add the enhancement

		pEnhancements->Insert(m_Enhancements[i].Enhancement);
		bEnhanced = true;

		//	Remember that we added this enhancement class

		if (!m_Enhancements[i].sType.IsBlank())
			EnhancementIDs.Insert(m_Enhancements[i].sType);
		}

	return bEnhanced;
	}

bool CEnhancementDesc::Accumulate (CItemCtx &Ctx, const CItem &Target, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) const

//	Accumulate
//
//	Adds enhancements to the stack. Returns TRUE if any enhancements were added.

	{
	return Accumulate(Ctx.GetItem().GetLevel(), Target, EnhancementIDs, pEnhancements);
	}

ALERROR CEnhancementDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes from XML. We expect either a single <Enhance> element or a list
//	of <Enhance> elements.

	{
	ALERROR error;
	int i;

	m_Enhancements.DeleteAll();

	if (strEquals(pDesc->GetTag(), ENHANCE_TAG))
		{
		m_Enhancements.InsertEmpty(1);
		if (error = InitFromEnhanceXML(Ctx, pDesc, m_Enhancements[0]))
			return error;
		}
	else
		{
		m_Enhancements.InsertEmpty(pDesc->GetContentElementCount());
		for (i = 0; i < pDesc->GetContentElementCount(); i++)
			{
			if (error = InitFromEnhanceXML(Ctx, pDesc->GetContentElement(i), m_Enhancements[i]))
				return error;
			}
		}

	return NOERROR;
	}

ALERROR CEnhancementDesc::InitFromEnhanceXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, SEnhancerDesc &Enhance)

//	InitFromEnhanceXML
//
//	Initializes from an <Enhance> element.

	{
	ALERROR error;

	Enhance.sType = pDesc->GetAttribute(TYPE_ATTRIB);

	//	Load the item criteria

	CString sCriteria;
	if (!pDesc->FindAttribute(CRITERIA_ATTRIB, &sCriteria))
		sCriteria = CONSTLIT("*");

	CItem::ParseCriteria(sCriteria, &Enhance.Criteria);

	//	Level check criteria

	if (error = Enhance.LevelCheck.InitFromXML(Ctx, pDesc->GetAttribute(LEVEL_CHECK_ATTRIB)))
		return error;

	//	Parse the enhancement itself

	if (error = Enhance.Enhancement.InitFromDesc(Ctx, pDesc->GetAttribute(ENHANCEMENT_ATTRIB)))
		return error;

	return NOERROR;
	}

void CEnhancementDesc::InsertHPBonus (int iBonus)

//	InsertHPBonus
//
//	Adds a standard HP bonus enhancement.

	{
	SEnhancerDesc *pEnhance = m_Enhancements.Insert();
	CItem::InitCriteriaAll(&pEnhance->Criteria);
	pEnhance->Enhancement.SetModBonus(iBonus);
	}

void CEnhancementDesc::SetCriteria (int iEntry, const CItemCriteria &Criteria)

//	SetCriteria
//
//	Sets the criteria for the given entry in the list of enhancements.
//	If iEntry is -1 then we set all entries.

	{
	int i;

	if (iEntry == -1)
		{
		for (i = 0; i < m_Enhancements.GetCount(); i++)
			m_Enhancements[i].Criteria = Criteria;
		}
	else if (iEntry >= 0 && iEntry < m_Enhancements.GetCount())
		m_Enhancements[iEntry].Criteria = Criteria;
	}

void CEnhancementDesc::SetLevelCheck (int iEntry, const CItemLevelCriteria &LevelCheck)

//	SetLevelCheck
//
//	Sets the flag for the given entry in the list of enhancements.
//	If iEntry is -1 then we set all entries.

	{
	int i;

	if (iEntry == -1)
		{
		for (i = 0; i < m_Enhancements.GetCount(); i++)
			m_Enhancements[i].LevelCheck = LevelCheck;
		}
	else if (iEntry >= 0 && iEntry < m_Enhancements.GetCount())
		m_Enhancements[iEntry].LevelCheck = LevelCheck;
	}

void CEnhancementDesc::SetType (int iEntry, const CString &sType)

//	SetType
//
//	Sets the type for the given entry in the list of enhancements.
//	If iEntry is -1 we set all entries.

	{
	int i;

	if (iEntry == -1)
		{
		for (i = 0; i < m_Enhancements.GetCount(); i++)
			m_Enhancements[i].sType = sType;
		}
	else if (iEntry >= 0 && iEntry < m_Enhancements.GetCount())
		m_Enhancements[iEntry].sType = sType;
	}

void CEnhancementDesc::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD				Count of SEnhancerDesc
//
//	CString				sType
//	CString				Criteria
//	CItemLevelCriteria	LevelCheck
//	CItemEnhancement	Enhancement

	{
	int i;
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_Enhancements.DeleteAll();
	m_Enhancements.InsertEmpty(dwLoad);

	for (i = 0; i < m_Enhancements.GetCount(); i++)
		{
		SEnhancerDesc &Enhancer = m_Enhancements[i];

		Ctx.pStream->Read(Enhancer.sType);

		CString sCriteria;
		Ctx.pStream->Read(sCriteria);
		CItem::ParseCriteria(sCriteria, &Enhancer.Criteria);
		Enhancer.LevelCheck.ReadFromStream(Ctx);
		Enhancer.Enhancement.ReadFromStream(Ctx);
		}
	}

void CEnhancementDesc::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	DWORD				Count of SEnhancerDesc
//
//	CString				sType
//	CString				Criteria
//	CItemLevelCriteria	LevelCheck
//	CItemEnhancement	Enhancement

	{
	int i;

	DWORD dwSave = m_Enhancements.GetCount();
	Stream.Write(dwSave);

	for (i = 0; i < m_Enhancements.GetCount(); i++)
		{
		const SEnhancerDesc &Enhancer = m_Enhancements[i];

		Stream.Write(Enhancer.sType);
		Stream.Write(CItem::GenerateCriteria(Enhancer.Criteria));
		Enhancer.LevelCheck.WriteToStream(Stream);
		Enhancer.Enhancement.WriteToStream(&Stream);
		}
	}
