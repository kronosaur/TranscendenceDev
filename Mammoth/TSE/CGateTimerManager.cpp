//	CGateTimerManager
//
//	CGateTimerManger class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int CGateTimerManager::GetTick (DWORD dwGateID, DWORD dwNow)

//	GetTick
//
//	Returns the tick delay for a ship exiting the given gate/obj.

	{
	if (m_dwTick != dwNow)
		{
		m_dwTick = dwNow;
		m_Timers.DeleteAll();
		}

	bool bNew;
	int &iTimer = *m_Timers.SetAt(dwGateID, &bNew);
	if (bNew)
		iTimer = 0;

	int iResult = iTimer;
	iTimer += mathRandom(11, 22);

	return iResult;
	}
