//	CRunDiagnostics.cpp
//
//	CRunDiagnostics Class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Diagnostics.h"

const CSystem &CRunDiagnostics::GetSystem (int iIndex) const

//	GetSystem
//
//	Returns the given system.

	{
	CSystem *pSystem;
	if (!m_AllSystems.Find(m_NodeOrder[iIndex], &pSystem))
		throw CException(ERR_FAIL);

	return *pSystem;
	}

void CRunDiagnostics::RunSystem (int iIndex, int iUpdates)

//	RunSystem
//
//	Runs diagnostics for the given system.

	{
	CSystem *pSystem;
	if (!m_AllSystems.Find(m_NodeOrder[iIndex], &pSystem))
		return;

	m_Universe.SetNewSystem(*pSystem);

	if (!m_bStartDiagnosticsCalled)
		{
		m_Universe.GetDesignCollection().FireOnGlobalStartDiagnostics(m_DiagnosticsCtx);
		m_Universe.GetDesignCollection().FireOnGlobalRunDiagnostics(m_DiagnosticsCtx);
		m_bStartDiagnosticsCalled = true;
		}

	//	Update for a while

	SSystemUpdateCtx UpdateCtx;
	UpdateCtx.bForceEventFiring = true;
	UpdateCtx.bForcePainted = true;

	for (int i = 0; i < iUpdates; i++)
		{
		//Ctx.dwUpdateCount++;
		DWORD dwStart = ::GetTickCount();

		m_Universe.Update(UpdateCtx);

		//Ctx.dwUpdateTime += ::sysGetTicksElapsed(dwStart);
		}

	//	Diagnostics

	m_Universe.GetDesignCollection().FireOnGlobalSystemDiagnostics(m_DiagnosticsCtx);
	}

bool CRunDiagnostics::Start (ILog *pProgress, CString *retsError)

//	Start
//
//	Starts a new diagnostics run. Creates a new universe.

	{
	//	Reset

	m_AllSystems.DeleteAll();
	m_NodeOrder.DeleteAll();
	m_bStartDiagnosticsCalled = false;

	//	If we've got a valid universe, then we need to re-initialize

	if (m_bUniverseCreated)
		{
		m_Universe.Reinit();

		if (m_Universe.InitGame(0, retsError) != NOERROR)
			return false;
		}
	else
		m_bUniverseCreated = true;

	//	Create

	for (int i = 0; i < m_Universe.GetTopologyNodeCount(); i++)
		{
		CTopologyNode *pNode = m_Universe.GetTopologyNode(i);

		//	Do not try to create end game nodes

		if (pNode->IsEndGame())
			continue;

		//	Timing

		DWORD dwStart = ::GetTickCount();

		//	Create this system

		CSystem *pNewSystem;
		CString sError;
		if (ALERROR error = m_Universe.CreateStarSystem(*pNode, &pNewSystem, &sError))
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Unable to create system %s: %s"), pNode->GetID(), sError);
			return false;
			}

		//	Create done

		m_SystemCreatePerf.dwTotalSystemCreateTime += ::sysGetTicksElapsed(dwStart);
		m_SystemCreatePerf.iSystemsCreated++;

		m_AllSystems.SetAt(pNode->GetID(), pNewSystem);

		if (pProgress)
			pProgress->Progress(strPatternSubst(CONSTLIT("Created: [%s] %s"), pNode->GetID(), pNewSystem->GetName()));
		}

	m_Universe.StartGame(true);

	//	Start diagnostics are always in the starting system (if available)

	CString sStartingNode = m_Universe.GetCurrentAdventureDesc().GetStartingNodeID();
	if (!m_AllSystems.Find(sStartingNode))
		{
		if (retsError) *retsError = CONSTLIT("Unable to find starting system.");
		return false;
		}

	//	Calculate the traversal order.

	TArray<CString> Extra;
	m_Universe.GetTopology().CalcTraversal(sStartingNode, m_NodeOrder, Extra);
	m_NodeOrder.Insert(Extra);

	//	Success

	return true;
	}

void CRunDiagnostics::Stop (void)

//	Stop
//
//	Ends a diagnostics run.

	{
	//	Fire OnEndDiagnostics

	m_Universe.GetDesignCollection().FireOnGlobalEndDiagnostics(m_DiagnosticsCtx);
	}
