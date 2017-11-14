//	IElementGenerator.cpp
//
//	IElementGenerator class
//	Copyright 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define GROUP_TAG					CONSTLIT("Group")
#define NULL_TAG					CONSTLIT("Null")
#define TABLE_TAG					CONSTLIT("Table")
#define RANDOM_ITEM_TAG				CONSTLIT("RandomItem")

#define CHANCE_ATTRIB				CONSTLIT("chance")
#define COUNT_ATTRIB				CONSTLIT("count")

class CElementGroup : public IElementGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);

		virtual void Generate (SCtx &Ctx, TArray<SResult> &retResults) const override;

	private:
		TArray<TUniquePtr<IElementGenerator>> m_Group;
	};

class CElementNull : public IElementGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);

		virtual void Generate (SCtx &Ctx, TArray<SResult> &retResults) const override { }
	};

class CElementSingle : public IElementGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);

		virtual void Generate (SCtx &Ctx, TArray<SResult> &retResults) const override;

	private:
		CXMLElement *m_pElement;			//	NOTE: Callers own this pointer
	};

class CElementTable : public IElementGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);

		virtual void Generate (SCtx &Ctx, TArray<SResult> &retResults) const override;

	private:
		struct SEntry
			{
			int iChance = 0;
			TUniquePtr<IElementGenerator> Item;
			};

		TArray<SEntry> m_Table;
		int m_iTotalChance;
	};

//	IElementGenerator ----------------------------------------------------------

ALERROR IElementGenerator::InitFromXMLInternal (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXMLInternal
//
//	Initializes our member variables

	{
	//	Load the count

	if (m_Count.LoadFromXML(pDesc->GetAttribute(COUNT_ATTRIB), 1) != NOERROR)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid count: %s"), pDesc->GetAttribute(COUNT_ATTRIB));
		return ERR_FAIL;
		}

	//	Success

	return NOERROR;
	}

ALERROR IElementGenerator::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXML
//
//	Creates from an XML element.

	{
	ALERROR error;

	CString sTag = pDesc->GetTag();
	if (strEquals(sTag, GROUP_TAG))
		error = CElementGroup::CreateFromXML(Ctx, pDesc, retGenerator);
	else if (strEquals(sTag, TABLE_TAG))
		error = CElementTable::CreateFromXML(Ctx, pDesc, retGenerator);
	else if (strEquals(sTag, NULL_TAG))
		error = CElementNull::CreateFromXML(Ctx, pDesc, retGenerator);
	else
		error = CElementSingle::CreateFromXML(Ctx, pDesc, retGenerator);

	//	If error, then we didn't create anything.

	if (error != NOERROR)
		return error;

	//	Initialize the rest

	return retGenerator->InitFromXMLInternal(Ctx, pDesc);
	}

ALERROR IElementGenerator::CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXMLAsGroup
//
//	Create from an XML element, assuming that the root element is a gorup of 
//	generators.

	{
	ALERROR error;

	if (error = CElementGroup::CreateFromXML(Ctx, pDesc, retGenerator))
		return error;

	//	Initialize the rest

	return retGenerator->InitFromXMLInternal(Ctx, pDesc);
	}

ALERROR IElementGenerator::CreateFromXMLAsTable (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXMLAsTable
//
//	Create from an XML element, assuming that the root element is a gorup of 
//	generators.

	{
	ALERROR error;

	if (error = CElementTable::CreateFromXML(Ctx, pDesc, retGenerator))
		return error;

	//	Initialize the rest

	return retGenerator->InitFromXMLInternal(Ctx, pDesc);
	}

void IElementGenerator::Generate (SCtx &Ctx, TArray<CXMLElement *> &retResults) const

//	Generate
//
//	Helper to generate into a simple array.

	{
	int i;

	TArray<SResult> Results;
	Generate(Ctx, Results);

	retResults.DeleteAll();
	retResults.InsertEmpty(Results.GetCount());
	for (i = 0; i < Results.GetCount(); i++)
		retResults[i] = Results[i].pElement;
	}

bool IElementGenerator::Generate (SCtx &Ctx, CXMLElement *pDesc, TArray<SResult> &retResults, CString *retsError)

//	Generate
//
//	Generate a list of single elements.

	{
	retResults.DeleteAll();

	//	Load the table

	SDesignLoadCtx LoadCtx;
	TUniquePtr<IElementGenerator> pGenerator;
	if (CreateFromXML(LoadCtx, pDesc, pGenerator) != NOERROR)
		{
		if (retsError) *retsError = LoadCtx.sError;
		return false;
		}

	//	Generate

	pGenerator->Generate(Ctx, retResults);

	//	Success

	return true;
	}

bool IElementGenerator::Generate (SCtx &Ctx, CXMLElement *pDesc, TArray<CXMLElement *> &retResults, CString *retsError)

//	GenerateAsGroup
//
//	Generate a list of single elements.

	{
	int i;

	TArray<SResult> Results;
	if (!Generate(Ctx, pDesc, Results, retsError))
		return false;

	//	Convert to an array of elements.

	retResults.DeleteAll();
	retResults.InsertEmpty(Results.GetCount());
	for (i = 0; i < Results.GetCount(); i++)
		retResults[i] = Results[i].pElement;

	//	Success

	return true;
	}

bool IElementGenerator::GenerateAsGroup (SCtx &Ctx, CXMLElement *pDesc, TArray<SResult> &retResults, CString *retsError)

//	GenerateAsGroup
//
//	Generate a list of single elements.

	{
	retResults.DeleteAll();

	//	Load the table

	SDesignLoadCtx LoadCtx;
	TUniquePtr<IElementGenerator> pGenerator;
	if (CreateFromXMLAsGroup(LoadCtx, pDesc, pGenerator) != NOERROR)
		{
		if (retsError) *retsError = LoadCtx.sError;
		return false;
		}

	//	Generate

	pGenerator->Generate(Ctx, retResults);

	//	Success

	return true;
	}

bool IElementGenerator::GenerateAsGroup (SCtx &Ctx, CXMLElement *pDesc, TArray<CXMLElement *> &retResults, CString *retsError)

//	GenerateAsGroup
//
//	Generate a list of single elements.

	{
	int i;

	TArray<SResult> Results;
	if (!GenerateAsGroup(Ctx, pDesc, Results, retsError))
		return false;

	//	Convert to an array of elements.

	retResults.DeleteAll();
	retResults.InsertEmpty(Results.GetCount());
	for (i = 0; i < Results.GetCount(); i++)
		retResults[i] = Results[i].pElement;

	//	Success

	return true;
	}

bool IElementGenerator::GenerateAsTable (SCtx &Ctx, CXMLElement *pDesc, TArray<SResult> &retResults, CString *retsError)

//	GenerateAsTable
//
//	Generate a list of single elements.

	{
	retResults.DeleteAll();

	//	Load the table

	SDesignLoadCtx LoadCtx;
	TUniquePtr<IElementGenerator> pGenerator;
	if (CreateFromXMLAsTable(LoadCtx, pDesc, pGenerator) != NOERROR)
		{
		if (retsError) *retsError = LoadCtx.sError;
		return false;
		}

	//	Generate

	pGenerator->Generate(Ctx, retResults);

	//	Success

	return true;
	}

bool IElementGenerator::GenerateAsTable (SCtx &Ctx, CXMLElement *pDesc, TArray<CXMLElement *> &retResults, CString *retsError)

//	GenerateAsTable
//
//	Generate a list of single elements.

	{
	int i;

	TArray<SResult> Results;
	if (!GenerateAsTable(Ctx, pDesc, Results, retsError))
		return false;

	//	Convert to an array of elements.

	retResults.DeleteAll();
	retResults.InsertEmpty(Results.GetCount());
	for (i = 0; i < Results.GetCount(); i++)
		retResults[i] = Results[i].pElement;

	//	Success

	return true;
	}

//	CElementGroup --------------------------------------------------------------

ALERROR CElementGroup::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXML
//
//	Creates a group generator

	{
	int i;
	ALERROR error;

	CElementGroup *pEntry = new CElementGroup;

	//	Load each of the entries in the group

	pEntry->m_Group.InsertEmpty(pDesc->GetContentElementCount());
	for (i = 0; i < pEntry->m_Group.GetCount(); i++)
		{
		if (error = IElementGenerator::CreateFromXML(Ctx, pDesc->GetContentElement(i), pEntry->m_Group[i]))
			{
			delete pEntry;
			return error;
			}
		}

	//	Done

	retGenerator.Set(pEntry);
	return NOERROR;
	}

void CElementGroup::Generate (SCtx &Ctx, TArray<SResult> &retResults) const

//	Generate
//
//	Generate the entries

	{
	int i, j;

	int iCount = m_Count.Roll();
	for (i = 0; i < iCount; i++)
		{
		for (j = 0; j < m_Group.GetCount(); j++)
			m_Group[j]->Generate(Ctx, retResults);
		}
	}

//	CElementNull ---------------------------------------------------------------

ALERROR CElementNull::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXML
//
//	Creates a null generator

	{
	retGenerator.Set(new CElementNull);
	return NOERROR;
	}

//	CElementSingle -------------------------------------------------------------

ALERROR CElementSingle::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXML
//
//	Creates a single element generator

	{
	CElementSingle *pEntry = new CElementSingle;
	pEntry->m_pElement = pDesc;
	retGenerator.Set(pEntry);
	return NOERROR;
	}

void CElementSingle::Generate (SCtx &Ctx, TArray<SResult> &retResults) const

//	Generate
//
//	Generate the entries

	{
	SResult *pResult = retResults.Insert();
	pResult->iCount = m_Count.Roll();
	pResult->pElement = m_pElement;
	}

//	CElementTable --------------------------------------------------------------

ALERROR CElementTable::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator)

//	CreateFromXML
//
//	Creates a table generator

	{
	int i;
	ALERROR error;

	CElementTable *pEntry = new CElementTable;

	//	Load each of the entries in the group

	pEntry->m_iTotalChance = 0;
	pEntry->m_Table.InsertEmpty(pDesc->GetContentElementCount());
	for (i = 0; i < pEntry->m_Table.GetCount(); i++)
		{
		CXMLElement *pTableDesc = pDesc->GetContentElement(i);
		if (error = IElementGenerator::CreateFromXML(Ctx, pTableDesc, pEntry->m_Table[i].Item))
			{
			delete pEntry;
			return error;
			}

		pEntry->m_Table[i].iChance = pTableDesc->GetAttributeIntegerBounded(CHANCE_ATTRIB, 0, -1, 1);
		pEntry->m_iTotalChance += pEntry->m_Table[i].iChance;
		}

	//	Done

	retGenerator.Set(pEntry);
	return NOERROR;
	}

void CElementTable::Generate (SCtx &Ctx, TArray<SResult> &retResults) const

//	Generate
//
//	Generate the entries

	{
	int i, j;

	if (m_iTotalChance <= 0)
		return;

	int iCount = m_Count.Roll();
	for (i = 0; i < iCount; i++)
		{
		int iRoll = mathRandom(1, m_iTotalChance);
		for (j = 0; j < m_Table.GetCount(); j++)
			{
			const SEntry &Entry = m_Table[j];
			if (iRoll <= Entry.iChance)
				{
				Entry.Item->Generate(Ctx, retResults);
				break;
				}
			else
				{
				iRoll -= Entry.iChance;
				}
			}
		}
	}
