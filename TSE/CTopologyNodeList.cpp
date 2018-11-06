//	CTopologyNodeList.cpp
//
//	CTopologyNodeList class

#include "PreComp.h"

int CTopologyNodeList::CalcDistanceToCriteria (CTopologyNode *pNode, const CTopologyNode::SAttributeCriteria &AttribCriteria) const

//	CalcDistanceToCriteria
//
//	If pNode does not match AttribCriteria, then we return the number of jumps
//	required to reach a node with the given criteria. If a node with the given
//	criteria cannot be reached, we return 100.
//
//	If pNode does match AttribCriteria, we compute the number of jumps required
//	to reach a node that DOES NOT match the criteria and return that as a 
//	negative number. If we cannot reach such a node, we return -100.

	{
	CTopologyNodeList Checked;

	int iDist = CalcDistanceToCriteriaMatch(pNode, AttribCriteria, Checked);
	if (iDist == -1)
		return 100;
	else if (iDist > 0)
		return iDist;
	else
		{
		Checked.DeleteAll();

		iDist = CalcDistanceToCriteriaNoMatch(pNode, AttribCriteria, Checked);
		if (iDist == -1)
			return -100;
		else
			return -iDist;
		}
	}

int CTopologyNodeList::CalcDistanceToCriteriaMatch (CTopologyNode *pNode, const CTopologyNode::SAttributeCriteria &AttribCriteria, CTopologyNodeList &Checked) const

//	CalcDistanceToCriteriaMatch
//
//	Returns the distance from pNode to a neighboring node that matches 
//	AttribCriteria. If we cannot find a matching node, we return -1.

	{
	if (pNode->MatchesAttributeCriteria(AttribCriteria))
		return 0;

	//	Add ourselves to the Checked list and recurse.

	Checked.Insert(pNode);
	int iBestDist = -1;
	for (int i = 0; i < pNode->GetStargateCount(); i++)
		{
		CTopologyNode *pDest = pNode->GetStargateDest(i);

		//	Skip if we've already checked this node

		if (Checked.FindNode(pDest))
			continue;

		int iDist = CalcDistanceToCriteriaMatch(pDest, AttribCriteria, Checked);
		if (iDist != -1 && (iBestDist == -1 || iDist < iBestDist))
			iBestDist = iDist + 1;
		}

	return iBestDist;
	}

int CTopologyNodeList::CalcDistanceToCriteriaNoMatch (CTopologyNode *pNode, const CTopologyNode::SAttributeCriteria &AttribCriteria, CTopologyNodeList &Checked) const

//	CalcDistanceToCriteriaNoMatch
//
//	Returns the distance from pNode to a neighboring node that DOES NOT match
//	AttribCriteria. If we cannot find such a node, we return -1.

	{
	if (!pNode->MatchesAttributeCriteria(AttribCriteria))
		return 0;

	//	Add ourselves to the Checked list and recurse.

	Checked.Insert(pNode);
	int iBestDist = -1;
	for (int i = 0; i < pNode->GetStargateCount(); i++)
		{
		CTopologyNode *pDest = pNode->GetStargateDest(i);

		//	Skip if we've already checked this node

		if (Checked.FindNode(pDest))
			continue;

		int iDist = CalcDistanceToCriteriaNoMatch(pDest, AttribCriteria, Checked);
		if (iDist != -1 && (iBestDist == -1 || iDist < iBestDist))
			iBestDist = iDist + 1;
		}

	return iBestDist;
	}

void CTopologyNodeList::Delete (CTopologyNode *pNode)

//	Delete
//
//	Delete the given node

	{
	int iIndex;
	if (FindNode(pNode, &iIndex))
		m_List.Delete(iIndex);
	}

ALERROR CTopologyNodeList::Filter (CTopologyNode::SCriteriaCtx &Ctx, CXMLElement *pCriteria, CTopologyNodeList *retList, CString *retsError)

//	Filter
//
//	Filters the list based on the criteria and returns a new list

	{
	ALERROR error;

	//	Parse the filter

	CTopologyNode::SCriteria Crit;
	if (error = CTopologyNode::ParseCriteria(pCriteria, &Crit, retsError))
		return error;

	//	Filter

	if (error = Filter(Ctx, Crit, retList))
		{
		*retsError = CONSTLIT("Error filtering nodes.");
		return error;
		}

	return NOERROR;
	}

ALERROR CTopologyNodeList::Filter (CTopologyNode::SCriteriaCtx &Ctx, CTopologyNode::SCriteria &Crit, CTopologyNodeList *ioList)

//	Filter
//
//	Filters the list based on the criteria and returns a new list

	{
	int i;

	//	Loop over all nodes and generate a filtered list

	for (i = 0; i < m_List.GetCount(); i++)
		{
		if (m_List[i]->MatchesCriteria(Ctx, Crit))
			ioList->Insert(m_List[i]);
		}

	return NOERROR;
	}

bool CTopologyNodeList::FindNode (CTopologyNode *pNode, int *retiIndex) const

//	FindNode
//
//	Finds the given node

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		if (pNode == m_List[i])
			{
			if (retiIndex)
				*retiIndex = i;
			return true;
			}

	return false;
	}

bool CTopologyNodeList::FindNode (const CString &sID, int *retiIndex) const

//	FindNode
//
//	Finds the given node

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		if (strEquals(sID, m_List[i]->GetID()))
			{
			if (retiIndex)
				*retiIndex = i;
			return true;
			}

	return false;
	}

bool CTopologyNodeList::IsNodeInRangeOf (CTopologyNode *pNode, int iMin, int iMax, const CTopologyNode::SAttributeCriteria &AttribCriteria, CTopologyNodeList &Checked) const

//	IsNodeInRangeOf
//
//	Returns TRUE if pNode is >= iMin and <= iMax of a node that has AttribsRequired
//	and doesn't have AttribsNotAllowed, and is not in Checked.

	{
	int i;

	//	Add ourselves to the Checked list

	Checked.Insert(pNode);

	//	If iMin is 0, then check the current node

	if (iMin <= 0)
		{
		//	If we match the criteria, then we're OK (since we've satified the distance 
		//	criteria).

		if (pNode->MatchesAttributeCriteria(AttribCriteria))
			return true;
		}

	//	If any adjacent nodes are in range, then we are also in range

	if (iMax > 0)
		{
		for (i = 0; i < pNode->GetStargateCount(); i++)
			{
			CTopologyNode *pDest = pNode->GetStargateDest(i);

			//	Skip if we've already checked this node

			if (Checked.FindNode(pDest))
				continue;

			//	If this node is in range, then we're done

			if (IsNodeInRangeOf(pDest, iMin - 1, iMax - 1, AttribCriteria, Checked))
				return true;
			}
		}

	//	Otherwise, we're not in range

	return false;
	}

void CTopologyNodeList::RestoreMarks (TArray<bool> &Saved)

//	RestoreMarks
//
//	Restore node marks from array

	{
	int i;
	int iCount = Min(Saved.GetCount(), m_List.GetCount());

	for (i = 0; i < iCount; i++)
		m_List[i]->SetMarked(Saved[i]);
	}

void CTopologyNodeList::SaveAndSetMarks (bool bMark, TArray<bool> *retSaved)

//	SaveAndSetMarks
//
//	Saves the marks and sets them all to the new value

	{
	int i;

	retSaved->DeleteAll();
	retSaved->InsertEmpty(m_List.GetCount());

	for (i = 0; i < m_List.GetCount(); i++)
		{
		retSaved->GetAt(i) = m_List[i]->IsMarked();
		m_List[i]->SetMarked(bMark);
		}
	}
