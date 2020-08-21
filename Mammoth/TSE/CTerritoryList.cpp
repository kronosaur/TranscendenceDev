//	CTerritoryList.cpp
//
//	CTerritoryList class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

CTerritoryList::~CTerritoryList (void)

//	CTerritoryList destructor

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		delete m_List[i];
	}

void CTerritoryList::DebugPaint (CG32bitImage &Dest, CMapViewportCtx &PaintCtx, const CG16bitFont &LabelFont) const

//	DebugPaint
//
//	Paint each zone.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		m_List[i]->DebugPaint(Dest, PaintCtx, LabelFont);
	}

CString CTerritoryList::GetAttribsAtPos (const CVector &vPos) const

//	GetAttribsAtPos
//
//	Returns the attributes at the given position

	{
	int i;

	CString sAttribs;
	for (i = 0; i < m_List.GetCount(); i++)
		{
		//	If this territory matches the criteria then we include it.

		if (m_List[i]->PointInTerritory(vPos)
				&& MatchesCriteria(m_List[i], vPos))
			sAttribs = ::AppendModifiers(sAttribs, m_List[i]->GetAttributes());
		}

	return sAttribs;
	}

bool CTerritoryList::HasAttribute (const CVector &vPos, const CString &sAttrib) const

//	HasAttribute
//
//	Returns TRUE if the given point has the given attribute

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		//	Skipped marked items (because we are in the middle
		//	of evaluating them)

		if (m_List[i]->IsMarked())
			continue;

		//	Test

		if (m_List[i]->HasAttribute(vPos, sAttrib)
				&& MatchesCriteria(m_List[i], vPos))
			return true;
		}

	return false;
	}

bool CTerritoryList::MatchesCriteria (CTerritoryDef *pTerritory, const CVector &vPos) const

//	MatchesCriteria
//
//	See if the territory's criteria matches.

	{
	const CString &sCriteria = pTerritory->GetCriteria();
	if (sCriteria.IsBlank())
		return true;

	//	Mark this territory so we don't try to evaluate it

	pTerritory->SetMarked(true);

	//	Parse the criteria

	CString sError;
	CAffinityCriteria Criteria;
	if (Criteria.Parse(sCriteria, &sError) != NOERROR)
		kernelDebugLogPattern("Error parsing territory: %s", sError);

	//	See if we match

	bool bMatches = Criteria.Matches(
		[this, vPos](const CString &sAttrib) { return HasAttribute(vPos, sAttrib); }
		);

	//	Restore

	pTerritory->SetMarked(false);

	//	Done

	return bMatches;
	}

void CTerritoryList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads the structure from a stream
//
//	DWORD			Count
//	CTerritoryDef	Entries

	{
	int i;
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_List.InsertEmpty(dwLoad);

	for (i = 0; i < (int)dwLoad; i++)
		{
		CTerritoryDef *pDef = new CTerritoryDef;
		pDef->ReadFromStream(Ctx);

		m_List[i] = pDef;
		}
	}

void CTerritoryList::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes the structure to a stream
//
//	DWORD			Count
//	CTerritoryDef	Entries

	{
	int i;
	DWORD dwSave;

	dwSave = m_List.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_List.GetCount(); i++)
		m_List[i]->WriteToStream(pStream);
	}
