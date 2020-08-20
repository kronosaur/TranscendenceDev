//	CItemTable.cpp
//
//	CItemTable object
//
//	NOTES:
//
//	The count attribute is held in the collection objects. For example, in a table of items, the table
//	object keeps an item count for each item entry (instead of the item keeping its own count).
//	At some point, it might be better to switch this around.

#include "PreComp.h"
#include "ItemGeneratorImpl.h"

#ifdef DEBUG_AVERAGE_VALUE
#include <stdio.h>
#endif

#define AVERAGE_VALUE_TAG						CONSTLIT("AverageValue")
#define COMPONENTS_TAG							CONSTLIT("Components")
#define GROUP_TAG								CONSTLIT("Group")
#define ITEM_TAG								CONSTLIT("Item")
#define ITEMS_TAG								CONSTLIT("Items")
#define LEVEL_TABLE_TAG							CONSTLIT("LevelTable")
#define LOCATION_CRITERIA_TABLE_TAG				CONSTLIT("LocationCriteriaTable")
#define LOOKUP_TAG								CONSTLIT("Lookup")
#define NULL_TAG								CONSTLIT("Null")
#define RANDOM_ITEM_TAG							CONSTLIT("RandomItem")
#define TABLE_TAG								CONSTLIT("Table")

#define ATTRIBUTES_ATTRIB						CONSTLIT("attributes")
#define CATEGORIES_ATTRIB						CONSTLIT("categories")
#define CHANCE_ATTRIB							CONSTLIT("chance")
#define COUNT_ATTRIB							CONSTLIT("count")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define DAMAGED_ATTRIB							CONSTLIT("damaged")
#define DEBUG_ONLY_ATTRIB						CONSTLIT("debugOnly")
#define ENHANCED_ATTRIB							CONSTLIT("enhanced")
#define ENHANCEMENT_ATTRIB						CONSTLIT("enhancement")
#define ITEM_ATTRIB								CONSTLIT("item")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define LEVEL_CURVE_ATTRIB						CONSTLIT("levelCurve")
#define LEVEL_FREQUENCY_ATTRIB					CONSTLIT("levelFrequency")
#define LEVEL_VALUE_ATTRIB						CONSTLIT("levelValue")
#define MODIFIERS_ATTRIB						CONSTLIT("modifiers")
#define TABLE_ATTRIB							CONSTLIT("table")
#define UNID_ATTRIB								CONSTLIT("unid")
#define VALUE_ATTRIB							CONSTLIT("value")

#define ATTRIB_NOT_FOR_SALE						CONSTLIT("notForSale")
#define FIELD_TREASURE_VALUE					CONSTLIT("treasureValue")

#define STR_G_ITEM								CONSTLIT("gItem")

static const Metric NOT_FOR_SALE_DISCOUNT =		0.2;

//	IItemGenerator -------------------------------------------------------------

int IItemGenerator::CalcLocationAffinity (SItemAddCtx &Ctx, const CAffinityCriteria &Criteria)

//	CalcLocationAffinity
//
//	Computes the location affinity.

	{
	if (Ctx.pSystem)
		return Ctx.pSystem->CalcLocationAffinity(Criteria, NULL_STR, Ctx.vPos);
	else
		return Criteria.CalcWeight([](const CString &sAttrib) { return false; });
	}

ALERROR IItemGenerator::CreateFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc, IItemGenerator **retpGenerator)

//	CreateFromXML
//
//	Creates a new generator

	{
	ALERROR error;
	IItemGenerator *pGenerator = NULL;

	if (strEquals(pDesc->GetTag(), ITEM_TAG))
		pGenerator = new CSingleItem;
	else if (strEquals(pDesc->GetTag(), TABLE_TAG))
		pGenerator = new CTableOfGenerators;
	else if (strEquals(pDesc->GetTag(), RANDOM_ITEM_TAG))
		pGenerator = new CRandomItems;
	else if (strEquals(pDesc->GetTag(), GROUP_TAG) 
			|| strEquals(pDesc->GetTag(), COMPONENTS_TAG)
			|| strEquals(pDesc->GetTag(), ITEMS_TAG)
			|| strEquals(pDesc->GetTag(), AVERAGE_VALUE_TAG))
		pGenerator = new CGroupOfGenerators;
	else if (strEquals(pDesc->GetTag(), LOOKUP_TAG))
		pGenerator = new CLookup;
	else if (strEquals(pDesc->GetTag(), LEVEL_TABLE_TAG))
		pGenerator = new CLevelTableOfItemGenerators;
	else if (strEquals(pDesc->GetTag(), LOCATION_CRITERIA_TABLE_TAG))
		pGenerator = new CLocationCriteriaTableOfItemGenerators;
	else if (strEquals(pDesc->GetTag(), NULL_TAG))
		pGenerator = new CNullItem;
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown item generator: %s"), pDesc->GetTag());
		return ERR_FAIL;
		}

	if (error = pGenerator->LoadFromXML(Ctx, pDesc))
		{
		if (pGenerator)
			delete pGenerator;
		return error;
		}

	*retpGenerator = pGenerator;

	return NOERROR;
	}

ALERROR IItemGenerator::CreateLookupTable (SDesignLoadCtx &Ctx, DWORD dwUNID, IItemGenerator **retpGenerator)

//	CreateLookupTable
//
//	Creates a new lookup item table

	{
	return CLookup::Create(dwUNID, retpGenerator, &Ctx.sError);
	}

ALERROR IItemGenerator::CreateRandomItemTable (CUniverse &Universe,
											   const CItemCriteria &Crit, 
											   const CString &sLevelFrequency,
											   IItemGenerator **retpGenerator)

//	CreateRandomItemTable
//
//	Creates a new random item table

	{
	return CRandomItems::Create(Universe, Crit, sLevelFrequency, retpGenerator);
	}

CItemTable::CItemTable (void) : 
		m_pGenerator(NULL)

//	CItemTable constructor

	{
	}

CItemTable::~CItemTable (void)

//	CItemTable destructor

	{
	if (m_pGenerator)
		delete m_pGenerator;
	}

bool CItemTable::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	SItemAddCtx Ctx(GetUniverse());

	//	Deal with the meta-data that we know about

	if (strEquals(sField, FIELD_TREASURE_VALUE))
		*retsValue = strFromInt((int)GetAverageValue(Ctx));
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

void CItemTable::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Adds types used by this table

	{
	if (m_pGenerator)
		m_pGenerator->AddTypesUsed(retTypesUsed);
	}

ALERROR CItemTable::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML descriptor

	{
	ALERROR error;

	CXMLElement *pElement = pDesc->GetContentElement(0);
	if (pElement)
		{
		if (error = IItemGenerator::CreateFromXML(Ctx, pElement, &m_pGenerator))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	return NOERROR;
	}

ALERROR CItemTable::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Load design references

	{
	ALERROR error;

	if (m_pGenerator)
		{
		if (error = m_pGenerator->OnDesignLoadComplete(Ctx))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("ItemTable (%x): %s"), GetUNID(), Ctx.sError);
			return error;
			}
		}

	return NOERROR;
	}

//	CGroupOfGenerators --------------------------------------------------------

CGroupOfGenerators::~CGroupOfGenerators (void)

//	CGroupOfGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pItem)
			delete m_Table[i].pItem;
	}

void CGroupOfGenerators::AddItems (SItemAddCtx &Ctx)

//	AddItems
//
//	Add items

	{
	int i;

	//	If we need to adjust counts, then do a separate algorithm

	if (SetsAverageValue())
		{
		int iLoopCount;
		Metric rScale;
		if (GetDesiredValue(Ctx, Ctx.iLevel, &iLoopCount, &rScale).IsEmpty())
			return;

		for (i = 0; i < iLoopCount; i++)
			AddItemsScaled(Ctx, rScale);
		}
	else
		AddItemsInt(Ctx);
	}

void CGroupOfGenerators::AddItemsInt (SItemAddCtx &Ctx) const

//	AddItemsInt
//
//	Adds items to the context.

	{
	int i, j;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (mathRandom(1, 100) <= m_Table[i].iChance)
			{
			int iCount = m_Table[i].Count.Roll();

			for (j = 0; j < iCount; j++)
				m_Table[i].pItem->AddItems(Ctx);
			}
		}
	}

void CGroupOfGenerators::AddItemsScaled (SItemAddCtx &Ctx, Metric rAdj) const

//	AddItemsScaled
//
//	Adds items and then scales by the given adjustment. E.g., if we add one
//	item and rAdj is 0.5, then we add the item 50% of the time.

	{
	int i;

#ifdef DEBUG_AVERAGE_VALUE
	bool bDebug = (Ctx.pDest && Ctx.pDest->GetType()->GetUNID() == 0x080200C0);
	if (bDebug)
		printf("[%d] Adjust: %.2f\n", Ctx.pDest->GetID(), rAdj);
#endif

	//	Add the items to a private list.

	CItemList LocalList;
	CItemListManipulator ItemList(LocalList);
	SItemAddCtx LocalCtx(ItemList);
	LocalCtx.pSystem = Ctx.pSystem;
	LocalCtx.iLevel = Ctx.iLevel;
	AddItemsInt(LocalCtx);

	//	Now loop over the items and adjust the count appropriately.

	for (i = 0; i < LocalList.GetCount(); i++)
		{
		const CItem &Item = LocalList.GetItem(i);
		int iOriginalCount = Item.GetCount();
		int iNewCount = mathRoundStochastic(iOriginalCount * rAdj);

#ifdef DEBUG_AVERAGE_VALUE
		if (bDebug)
			{
			printf("%s: %d -> %.2f = %d\n", (LPSTR)Item.GetNounPhrase(), iOriginalCount, rNewCount, iNewCount);
			}
#endif

		//	Add the item with the new count

		if (iNewCount == iOriginalCount)
			{
			Ctx.ItemList.AddItem(Item);
			}
		else if (iNewCount > 0)
			{
			CItem NewItem(Item);
			NewItem.SetCount(iNewCount);
			Ctx.ItemList.AddItem(NewItem);
			}
		}
	}

void CGroupOfGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this generator

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pItem->AddTypesUsed(retTypesUsed);
	}

CurrencyValue CGroupOfGenerators::GetAverageValue (SItemAddCtx &Ctx)

//	GetAverageValue
//
//	Returns the average value.

	{
	if (SetsAverageValue())
		{
		if (Ctx.iLevel >= 0 && Ctx.iLevel < m_DesiredValue.GetCount())
			return m_DesiredValue[Ctx.iLevel].GetCreditValue();
		else
			return 0;
		}
	else
		{
		//	Make sure we don't recurse

		CRecursingCheck Check(m_bRecursing);
		if (Check.IsRecursing())
			return 0;

		return (CurrencyValue)mathRound(GetExpectedValue(Ctx, Ctx.iLevel));
		}
	}

CCurrencyAndValue CGroupOfGenerators::GetDesiredValue (SItemAddCtx &Ctx, int iLevel, int *retiLoopCount, Metric *retrScale) const

//	GetDesiredValue
//
//	Returns the desired value, if any.

	{
	if (iLevel < 0 || iLevel >= m_DesiredValue.GetCount())
		return CCurrencyAndValue();

	//	If all we care about is one value, then just return that.

	else if (retiLoopCount == NULL && retrScale == NULL)
		return m_DesiredValue[iLevel];

	//	Otherwise, do the full calculations

	else
		{
		//	Figure out how many loops to do. We do as many loops as necessary
		//	to react the desired value, but we must have at least 1 loop and we
		//	truncate to a maximum number of loops.

		static constexpr int MAX_LOOP_COUNT = 15;
		Metric rDesiredValue = (Metric)m_DesiredValue[iLevel].GetCreditValue();
		Metric rExpectedValue = Max(1.0, GetExpectedValue(Ctx, iLevel));

		int iLoopCount = Max(1, Min(MAX_LOOP_COUNT, mathRound(rDesiredValue / rExpectedValue)));
		if (retiLoopCount)
			*retiLoopCount = iLoopCount;

		//	Scale the resulting counts to handle round-off error and to adjust
		//	for the loop count getting truncated above.

		Metric rScale = (rDesiredValue / (iLoopCount * rExpectedValue));
		if (retrScale)
			*retrScale = rScale;

		//	Done

		return m_DesiredValue[iLevel];
		}
	}

Metric CGroupOfGenerators::GetExpectedValue (SItemAddCtx &Ctx, int iLevel) const

//	GetExpectedValue
//
//	Returns the expected, average value generated at the given level.

	{
	if (iLevel < 0 || iLevel >= m_ExpectedValue.GetCount())
		return 0.0;

	if (m_ExpectedValue[iLevel] < 0.0)
		{
		Metric rTotal = 0.0;
		for (int i = 0; i < m_Table.GetCount(); i++)
			{
			if (m_Table[i].iChance < 100)
				rTotal += (m_Table[i].Count.GetAveValueFloat() * (Metric)m_Table[i].pItem->GetAverageValue(Ctx) * (Metric)m_Table[i].iChance / 100.0);
			else
				rTotal += m_Table[i].Count.GetAveValueFloat() * m_Table[i].pItem->GetAverageValue(Ctx);
			}

		m_ExpectedValue[iLevel] = rTotal;
		}

	return m_ExpectedValue[iLevel];
	}

CItemTypeProbabilityTable CGroupOfGenerators::GetProbabilityTable (SItemAddCtx &Ctx) const

//	GetProbabilityTable
//
//	Returns a table with the probability of each item type appearing.

	{
	int i;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return CItemTypeProbabilityTable();

	//	We combine probabilities.

	CItemTypeProbabilityTable Result;
	for (i = 0; i < m_Table.GetCount(); i++)
		Result.Union(m_Table[i].pItem->GetProbabilityTable(Ctx));

	return Result;
	}

bool CGroupOfGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pItem->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

ALERROR CGroupOfGenerators::LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	int i;
	ALERROR error;

	//	Load content elements

	m_Table.InsertEmpty(pDesc->GetContentElementCount());
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);
			
		m_Table[i].iChance = pEntry->GetAttributeInteger(CHANCE_ATTRIB);
		if (m_Table[i].iChance == 0)
			m_Table[i].iChance = 100;

		CString sCount = pEntry->GetAttribute(COUNT_ATTRIB);
		if (sCount.IsBlank())
			m_Table[i].Count = DiceRange(0, 0, 1);
		else
			m_Table[i].Count.LoadFromXML(sCount);

		if (error = IItemGenerator::CreateFromXML(Ctx, pEntry, &m_Table[i].pItem))
			return error;
		}

	//	See if we force an average value

	CString sAttrib;
	if (pDesc->FindAttribute(LEVEL_VALUE_ATTRIB, &sAttrib))
		{
		TArray<CString> Values;
		ParseStringList(sAttrib, 0, &Values);

		m_DesiredValue.InsertEmpty(MAX_ITEM_LEVEL + 1);
		for (i = 0; i < Values.GetCount(); i++)
			if (error = m_DesiredValue[i + 1].InitFromXML(Ctx, Values[i], i + 1))
				return error;
		}
	else if (pDesc->FindAttribute(VALUE_ATTRIB, &sAttrib))
		{
		m_DesiredValue.InsertEmpty(MAX_ITEM_LEVEL + 1);
		for (i = 1; i <= MAX_ITEM_LEVEL; i++)
			if (error = m_DesiredValue[i].InitFromXML(Ctx, sAttrib, i))
				return error;
		}

	return NOERROR;
	}

ALERROR CGroupOfGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	int i;
	ALERROR error;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pItem->OnDesignLoadComplete(Ctx))
			return error;
		}

	//	Bind average values

	for (i = 0; i < m_DesiredValue.GetCount(); i++)
		if (error = m_DesiredValue[i].Bind(Ctx))
			return error;

	//	Initialize array to cache expected value.

	m_ExpectedValue.InsertEmpty(Max(0, MAX_SYSTEM_LEVEL - m_ExpectedValue.GetCount()));
	for (i = 0; i < m_ExpectedValue.GetCount(); i++)
		m_ExpectedValue[i] = -1.0;

	return NOERROR;
	}

//	CSingleItem ---------------------------------------------------------------

void CSingleItem::AddItems (SItemAddCtx &Ctx)

//	AddItems
//
//	Add items

	{
	//	Ignore if not debugging

	if (m_bDebugOnly && !Ctx.GetUniverse().InDebugMode())
		return;

	//	Create item

	CItem NewItem(m_pItemType, 1);
	if (mathRandom(1, 100) <= m_iDamaged)
		NewItem.SetDamaged();
	else
		m_Enhanced.EnhanceItem(NewItem);

	Ctx.ItemList.AddItem(NewItem);
	}

void CSingleItem::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this generator

	{
	retTypesUsed->SetAt(m_pItemType.GetUNID(), true);
	}

CurrencyValue CSingleItem::CalcItemValue (CItemType *pType)

//	CalcItemValue
//
//	Computes the value (to the player) of this item.

	{
	CItemCtx ItemCtx;
	CurrencyValue ItemValue = CEconomyType::ExchangeToCredits(pType->GetCurrencyType(), pType->GetValue(ItemCtx, true));

	if (pType->HasAttribute(ATTRIB_NOT_FOR_SALE))
		ItemValue = Max((CurrencyValue)1, (CurrencyValue)(ItemValue * NOT_FOR_SALE_DISCOUNT));

	return ItemValue;
	}

CurrencyValue CSingleItem::GetAverageValue (SItemAddCtx &Ctx)

//	GetAverageValue
//
//	Returns the average value (in credits)

	{
	return CalcItemValue(m_pItemType);
	}

CItemTypeProbabilityTable CSingleItem::GetProbabilityTable (SItemAddCtx &Ctx) const

//	GetProbabilityTable
//
//	Returns a table with the probability of each item type appearing.

	{
	CItemTypeProbabilityTable Result;

	if (m_pItemType)
		Result.Union(m_pItemType, 1.0);

	return Result;
	}

bool CSingleItem::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute

	{
	if (m_pItemType && m_pItemType->HasAttribute(sAttrib))
		return true;

	return false;
	}

ALERROR CSingleItem::LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	ALERROR error;

	if (error = m_pItemType.LoadUNID(Ctx, pDesc->GetAttribute(ITEM_ATTRIB)))
		return error;

	m_iDamaged = pDesc->GetAttributeInteger(DAMAGED_ATTRIB);
	m_bDebugOnly = pDesc->GetAttributeBool(DEBUG_ONLY_ATTRIB);

	if (m_pItemType.GetUNID() == 0)
		{
		CString sUNID = pDesc->GetAttribute(ITEM_ATTRIB);
		if (sUNID.IsBlank())
			Ctx.sError = strPatternSubst(CONSTLIT("<Item> element missing item attribute."));
		else
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid item UNID: %s"), sUNID);
		return ERR_FAIL;
		}

	if (error = m_Enhanced.InitFromXML(Ctx, pDesc))
		return error;

	return NOERROR;
	}

ALERROR CSingleItem::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	ALERROR error;

	//	Ignore if not debugging. We don't bind because sometimes we have a
	//	reference to an item type that only exists in debug mode.

	if (m_bDebugOnly && !Ctx.GetUniverse().InDebugMode())
		return NOERROR;

	//	Bind

	if (error = m_pItemType.Bind(Ctx))
		return error;

	return NOERROR;
	}

//	CLevelTableOfItemGenerators -----------------------------------------------

CLevelTableOfItemGenerators::~CLevelTableOfItemGenerators (void)

//	CLevelTableOfItemGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pEntry)
			delete m_Table[i].pEntry;
	}

void CLevelTableOfItemGenerators::AddItems (SItemAddCtx &Ctx)

//	AddItems
//
//	Adds items

	{
	int i, j;

	//	Compute probabilities, if necessary

	if (m_iComputedLevel != Ctx.iLevel)
		{
		//	Create a table of probabilities

		m_iTotalChance = 0;
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			m_Table[i].iChance = GetFrequencyByLevel(m_Table[i].sLevelFrequency, Ctx.iLevel);
			m_iTotalChance += m_Table[i].iChance;
			}

		m_iComputedLevel = Ctx.iLevel;
		}

	//	Generate

	if (m_iTotalChance)
		{
		int iRoll = mathRandom(1, m_iTotalChance);
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			iRoll -= m_Table[i].iChance;

			if (iRoll <= 0)
				{
				int iCount = m_Table[i].Count.Roll();

				for (j = 0; j < iCount; j++)
					m_Table[i].pEntry->AddItems(Ctx);

				break;
				}
			}
		}
	}

void CLevelTableOfItemGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this generator

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pEntry->AddTypesUsed(retTypesUsed);
	}

CurrencyValue CLevelTableOfItemGenerators::GetAverageValue (SItemAddCtx &Ctx)

//	GetAverageValue
//
//	Returns the average value.

	{
	int i;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return 0;

	//	Compute the table for this level.

	Metric rTotal = 0.0;
	int iTotalChance = 0;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iChance = GetFrequencyByLevel(m_Table[i].sLevelFrequency, Ctx.iLevel);
		iTotalChance += iChance;
		}

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iChance = GetFrequencyByLevel(m_Table[i].sLevelFrequency, Ctx.iLevel);
		if (iChance > 0)
			rTotal += (m_Table[i].Count.GetAveValueFloat() * (Metric)m_Table[i].pEntry->GetAverageValue(Ctx) * (Metric)iChance / (Metric)iTotalChance);
		}

	return (CurrencyValue)(rTotal + 0.5);
	}

CItemTypeProbabilityTable CLevelTableOfItemGenerators::GetProbabilityTable (SItemAddCtx &Ctx) const

//	GetProbabilityTable
//
//	Returns a table with the probability of each item type appearing.

	{
	int i;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return CItemTypeProbabilityTable();

	//	Compute probabilities, if necessary

	if (m_iComputedLevel != Ctx.iLevel)
		{
		//	Create a table of probabilities

		m_iTotalChance = 0;
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			m_Table[i].iChance = GetFrequencyByLevel(m_Table[i].sLevelFrequency, Ctx.iLevel);
			m_iTotalChance += m_Table[i].iChance;
			}

		m_iComputedLevel = Ctx.iLevel;
		}

	//	Short-circuit

	if (m_iTotalChance == 0)
		return CItemTypeProbabilityTable();

	//	Scale chances

	CItemTypeProbabilityTable Result;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		CItemTypeProbabilityTable Table = m_Table[i].pEntry->GetProbabilityTable(Ctx);
		Table.Scale((Metric)m_Table[i].iChance / (Metric)m_iTotalChance);
		Result.Add(Table);
		}

	return Result;
	}

bool CLevelTableOfItemGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pEntry->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

ALERROR CLevelTableOfItemGenerators::LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	LoadFromXML
//
//	Load table from XML

	{
	int i;
	ALERROR error;

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);
		SEntry *pNewEntry = m_Table.Insert();

		pNewEntry->sLevelFrequency = pEntry->GetAttribute(LEVEL_FREQUENCY_ATTRIB);

		pNewEntry->Count.LoadFromXML(pEntry->GetAttribute(COUNT_ATTRIB));
		if (pNewEntry->Count.IsEmpty())
			pNewEntry->Count.SetConstant(1);

		if (error = IItemGenerator::CreateFromXML(Ctx, pEntry, &pNewEntry->pEntry))
			{
			m_Table.Delete(m_Table.GetCount() - 1);
			return error;
			}
		}

	m_iComputedLevel = -1;

	return NOERROR;
	}

ALERROR CLevelTableOfItemGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;

	m_iComputedLevel = -1;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pEntry->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CLocationCriteriaTableOfItemGenerators -------------------------------------

CLocationCriteriaTableOfItemGenerators::~CLocationCriteriaTableOfItemGenerators (void)

//	CLevelTableOfItemGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pEntry)
			delete m_Table[i].pEntry;
	}

void CLocationCriteriaTableOfItemGenerators::AddItems (SItemAddCtx &Ctx)

//	AddItems
//
//	Adds items

	{
	int i, j;

	//	Cache probabilities, if necessary

	CString sAttribsAtPos = (Ctx.pSystem ? Ctx.pSystem->GetAttribsAtPos(Ctx.vPos) : NULL_STR);
	if (m_pComputedSystem != Ctx.pSystem 
			|| !strEquals(m_sComputedAttribs, sAttribsAtPos))
		{
		//	Create a table of probabilities

		m_iTotalChance = 0;
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			m_Table[i].iChance = CalcLocationAffinity(Ctx, m_Table[i].Criteria);
			m_iTotalChance += m_Table[i].iChance;
			}

		m_pComputedSystem = Ctx.pSystem;
		m_sComputedAttribs = sAttribsAtPos;
		}

	//	Generate

	if (m_iTotalChance)
		{
		int iRoll = mathRandom(1, m_iTotalChance);
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			iRoll -= m_Table[i].iChance;

			if (iRoll <= 0)
				{
				int iCount = m_Table[i].Count.Roll();

				for (j = 0; j < iCount; j++)
					m_Table[i].pEntry->AddItems(Ctx);

				break;
				}
			}
		}
	}

void CLocationCriteriaTableOfItemGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this generator

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pEntry->AddTypesUsed(retTypesUsed);
	}

CurrencyValue CLocationCriteriaTableOfItemGenerators::GetAverageValue (SItemAddCtx &Ctx)

//	GetAverageValue
//
//	Returns the average value.

	{
	int i;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return 0;

	//	Equal probability for each entry

	Metric rTotal = 0.0;
	int iTotalChance = 0;
	for (i = 0; i < m_Table.GetCount(); i++)
		iTotalChance += 1;

	for (i = 0; i < m_Table.GetCount(); i++)
		rTotal += (m_Table[i].Count.GetAveValueFloat() * (Metric)m_Table[i].pEntry->GetAverageValue(Ctx) / (Metric)iTotalChance);

	return (CurrencyValue)(rTotal + 0.5);
	}

CItemTypeProbabilityTable CLocationCriteriaTableOfItemGenerators::GetProbabilityTable (SItemAddCtx &Ctx) const

//	GetProbabilityTable
//
//	Returns a table with the probability of each item type appearing.

	{
	int i;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return CItemTypeProbabilityTable();

	//	Cache probabilities, if necessary

	CString sAttribsAtPos = (Ctx.pSystem ? Ctx.pSystem->GetAttribsAtPos(Ctx.vPos) : NULL_STR);
	if (m_pComputedSystem != Ctx.pSystem 
			|| !strEquals(m_sComputedAttribs, sAttribsAtPos))
		{
		//	Create a table of probabilities

		m_iTotalChance = 0;
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			m_Table[i].iChance = CalcLocationAffinity(Ctx, m_Table[i].Criteria);
			m_iTotalChance += m_Table[i].iChance;
			}

		m_pComputedSystem = Ctx.pSystem;
		m_sComputedAttribs = sAttribsAtPos;
		}

	//	Short-circuit

	if (m_iTotalChance == 0)
		return CItemTypeProbabilityTable();

	//	Scale chances

	CItemTypeProbabilityTable Result;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		CItemTypeProbabilityTable Table = m_Table[i].pEntry->GetProbabilityTable(Ctx);
		Table.Scale((Metric)m_Table[i].iChance / (Metric)m_iTotalChance);
		Result.Add(Table);
		}

	return Result;
	}

bool CLocationCriteriaTableOfItemGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pEntry->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

ALERROR CLocationCriteriaTableOfItemGenerators::LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	LoadFromXML
//
//	Load table from XML

	{
	int i;
	ALERROR error;

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);
		SEntry *pNewEntry = m_Table.Insert();

		CString sCriteria = pEntry->GetAttribute(CRITERIA_ATTRIB);
		if (error = pNewEntry->Criteria.Parse(sCriteria, &Ctx.sError))
			return error;

		pNewEntry->Count.LoadFromXML(pEntry->GetAttribute(COUNT_ATTRIB));
		if (pNewEntry->Count.IsEmpty())
			pNewEntry->Count.SetConstant(1);

		if (error = IItemGenerator::CreateFromXML(Ctx, pEntry, &pNewEntry->pEntry))
			return error;
		}

	m_pComputedSystem = NULL;
	m_sComputedAttribs = NULL_STR;

	return NOERROR;
	}

ALERROR CLocationCriteriaTableOfItemGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;

	m_pComputedSystem = NULL;
	m_sComputedAttribs = NULL_STR;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pEntry->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CLookup -------------------------------------------------------------------

void CLookup::AddItems (SItemAddCtx &Ctx)

//	AddItems
//
//	Add items

	{
	m_pTable->AddItems(Ctx);
	}

void CLookup::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this generator

	{
	m_pTable->AddTypesUsed(retTypesUsed);
	}

ALERROR CLookup::Create (DWORD dwUNID, IItemGenerator **retpGenerator, CString *retsError)

//	Create
//
//	Creates a table for the given item table

	{
	CLookup *pGenerator = new CLookup;
	pGenerator->m_pTable.SetUNID(dwUNID);

	*retpGenerator = pGenerator;

	return NOERROR;
	}

CItemTypeProbabilityTable CLookup::GetProbabilityTable (SItemAddCtx &Ctx) const

//	GetProbabilityTable
//
//	Returns a table with the probability of each item type appearing.

	{
	if (m_pTable == NULL)
		return CItemTypeProbabilityTable();

	return m_pTable->GetProbabilityTable(Ctx);
	}

bool CLookup::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute

	{
	return m_pTable->HasItemAttribute(sAttrib);
	}

ALERROR CLookup::LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	ALERROR error;

	if (error = m_pTable.LoadUNID(Ctx, pDesc->GetAttribute(TABLE_ATTRIB)))
		return error;

	if (m_pTable.GetUNID() == 0)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("<Lookup> element missing table attribute."));
		return ERR_FAIL;
		}

	return NOERROR;
	}

ALERROR CLookup::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	ALERROR error;

	if (error = m_pTable.Bind(Ctx))
		return error;

	return NOERROR;
	}

//	CTableOfGenerators --------------------------------------------------------

CTableOfGenerators::~CTableOfGenerators (void)

//	CTableOfGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pItem)
			delete m_Table[i].pItem;
	}

void CTableOfGenerators::AddItems (SItemAddCtx &Ctx)

//	AddItems
//
//	Add items

	{
	int i, j;
	int iRoll = mathRandom(1, m_iTotalChance);

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		iRoll -= m_Table[i].iChance;

		if (iRoll <= 0)
			{
			int iCount = m_Table[i].Count.Roll();

			for (j = 0; j < iCount; j++)
				m_Table[i].pItem->AddItems(Ctx);
			break;
			}
		}
	}

void CTableOfGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this generator

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pItem->AddTypesUsed(retTypesUsed);
	}

CurrencyValue CTableOfGenerators::GetAverageValue (SItemAddCtx &Ctx)

//	GetAverageValue
//
//	Returns the average value.

	{
	int i;

	if (m_iTotalChance == 0)
		return 0;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return 0;

#ifdef DEBUG_AVERAGE_VALUE
	TArray<Metric> AverageValues;
#endif

	//	Average value is proportional to chances.

	Metric rTotal = 0.0;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		Metric rAverageValue = (m_Table[i].Count.GetAveValueFloat() * (Metric)m_Table[i].pItem->GetAverageValue(Ctx) * (Metric)m_Table[i].iChance / (Metric)m_iTotalChance);
		rTotal += rAverageValue;

#ifdef DEBUG_AVERAGE_VALUE
		AverageValues.Insert(rAverageValue);
#endif
		}

#ifdef DEBUG_AVERAGE_VALUE
	for (i = 0; i < AverageValues.GetCount(); i++)
		printf("%.2f ", AverageValues[i]);

	printf("= %.2f\n", rTotal);
#endif

	return (CurrencyValue)(rTotal + 0.5);
	}

CItemTypeProbabilityTable CTableOfGenerators::GetProbabilityTable (SItemAddCtx &Ctx) const

//	GetProbabilityTable
//
//	Returns a table with the probability of each item type appearing.

	{
	int i;

	//	Make sure we don't recurse

	CRecursingCheck Check(m_bRecursing);
	if (Check.IsRecursing())
		return CItemTypeProbabilityTable();

	//	Short-circuit

	if (m_iTotalChance == 0)
		return CItemTypeProbabilityTable();

	//	Scale chances

	CItemTypeProbabilityTable Result;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		CItemTypeProbabilityTable Table = m_Table[i].pItem->GetProbabilityTable(Ctx);
		Table.Scale((Metric)m_Table[i].iChance / (Metric)m_iTotalChance);
		Result.Add(Table);
		}

	return Result;
	}

bool CTableOfGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pItem->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

ALERROR CTableOfGenerators::LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	int i;
	ALERROR error;

	m_iTotalChance = 0;
	int iCount = pDesc->GetContentElementCount();
	if (iCount > 0)
		{
		m_Table.InsertEmpty(iCount);

		//	Pre-initialize to NULL in case we exit with an error

		for (i = 0; i < iCount; i++)
			m_Table[i].pItem = NULL;

		//	Load

		for (i = 0; i < iCount; i++)
			{
			CXMLElement *pEntry = pDesc->GetContentElement(i);
			
			m_Table[i].iChance = pEntry->GetAttributeInteger(CHANCE_ATTRIB);
			m_iTotalChance += m_Table[i].iChance;

			CString sCount = pEntry->GetAttribute(COUNT_ATTRIB);
			if (sCount.IsBlank())
				m_Table[i].Count = DiceRange(0, 0, 1);
			else
				m_Table[i].Count.LoadFromXML(sCount);

			if (error = IItemGenerator::CreateFromXML(Ctx, pEntry, &m_Table[i].pItem))
				return error;
			}
		}

	return NOERROR;
	}

ALERROR CTableOfGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	ALERROR error;
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pItem->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

ALERROR CRandomEnhancementGenerator::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc)

//	InitFromXML
//
//	Loads the structure from XML

	{
	m_iChance = pDesc->GetAttributeInteger(ENHANCED_ATTRIB);
	CString sEnhancement = pDesc->GetAttribute(ENHANCEMENT_ATTRIB);

	//	If we have no enhancement desc, then we come up with a random
	//	one using our own internal algorithm.

	char *pPos = sEnhancement.GetASCIIZPointer();
	if (sEnhancement.IsBlank())
		{
		m_Mods = CItemEnhancement();
		m_pCode = NULL;
		}

	//	If the enhancement desc is a script, then load it now

	else if (*pPos == '=')
		{
		m_Mods = CItemEnhancement();

		CCodeChain::SLinkOptions Options;
		Options.iOffset = 1;

		m_pCode = CCodeChain::LinkCode(pPos, Options);
		if (m_pCode->IsError())
			{
			Ctx.sError = m_pCode->GetStringValue();
			return ERR_FAIL;
			}

		if (m_iChance == 0)
			m_iChance = 100;
		}

	//	Otherwise, see if this is a valid mod number

	else
		{
		if (m_Mods.InitFromDesc(Ctx, sEnhancement) != NOERROR)
			return ERR_FAIL;

		m_pCode = NULL;

		if (!m_Mods.IsEmpty() && m_iChance == 0)
			m_iChance = 100;
		}

	return NOERROR;
	}

bool CRandomEnhancementGenerator::IsVariant (void) const

//	IsVariant
//
//	Returns TRUE if the enhancement varies (randomly).

	{
	if (m_pCode)
		return true;
	else if (m_Mods.IsEmpty())
		return false;
	else if (m_iChance != 100 && m_iChance != 0)
		return true;
	else
		return false;
	}

void CRandomEnhancementGenerator::EnhanceItem (CItem &Item) const

//	EnhanceItem
//
//	Enhances the given item

	{
	//	See if this item is enhanced

	if (mathRandom(1, 100) > m_iChance)
		return;

	//	If we have code, execute the code to figure out the mod

	if (m_pCode)
		{
		CCodeChainCtx Ctx(Item.GetUniverse());

		//	Save the previous value of gItem

		Ctx.SaveAndDefineItemVar(Item);

		//	Execute the code

		ICCItemPtr pResult = Ctx.RunCode(m_pCode);	//	LATER:Event

		//	If we have an error, report it

		CItemEnhancement Mods;
		if (pResult->IsError())
			{
			CString sError = strPatternSubst(CONSTLIT("Generate Enhancement: %s"), pResult->GetStringValue());
			::kernelDebugLogString(sError);
			}

		//	Otherwise, the result code is the mods

		else
			{
			CString sError;
			if (Mods.InitFromDesc(Item.GetUniverse(), *pResult, &sError) != NOERROR)
				{
				::kernelDebugLogPattern("Generate Enhancement: %s", sError);
				Mods = CItemEnhancement();
				}
			}

		//	Enhance the item

		if (!Mods.IsEmpty())
			Item.AddEnhancement(Mods);
		}

	//	Otherwise, if we have a constant mod, apply that

	else if (!m_Mods.IsEmpty())
		{
		Item.AddEnhancement(m_Mods);
		}

	//	Otherwise, we need to generate a random mod appropriate to the
	//	particular item

	else
		{
		DWORD dwMods;
		CItemType *pType = Item.GetType();
		int iRoll = mathRandom(1, 100);

		switch (pType->GetCategory())
			{
			case itemcatArmor:
			case itemcatShields:
				if (iRoll < 40)
					//	HP + 10%
					dwMods = 0x0101;
				else if (iRoll < 65)
					//	HP + 20%
					dwMods = 0x0102;
				else if (iRoll < 80)
					//	HP + 30%
					dwMods = 0x0103;
				else if (iRoll < 90)
					//	Damage Adj at 50% (and 75%)
					dwMods = 0x0A05 + (mathRandom(0, 3) << 4);
				else if (iRoll < 95)
					//	Immunity to special ion attacks
					dwMods = 0x0C00;
				else
					//	Regeneration
					dwMods = 0x0200;
				break;

			case itemcatWeapon:
				if (iRoll < 25)
					//	Damage + 10%
					dwMods = 0x0101;
				else if (iRoll < 75)
					//	Damage + 20%
					dwMods = 0x0102;
				else if (iRoll < 95)
					//	Damage + 30%
					dwMods = 0x0103;
				else
					//	Delay time at 80% of original
					dwMods = 0x1002;
				break;

			default:
				dwMods = 0;
			}

		Item.AddEnhancement(dwMods);
		}
	}

