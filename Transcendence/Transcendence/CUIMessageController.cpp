//	CUIMessageController.cpp
//
//	CUIMessageController class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

const DWORD FLAG_IS_HINT =				0x00000001;

struct SUIMessageData
	{
	int iNameLen;
	const char *pszName;
	DWORD dwFlags;
	int iHintRepeat;					//	How many times should player follow hint before disabling hint
	DWORD dwInterval;					//	Min ticks between message showing
	};

static SUIMessageData g_MessageData[uimsgCount] =
	{
		{	CONSTDEF("allMessages"),			0,				0,	0,	},
		{	CONSTDEF("allHints"),				0,				0,	0,	},

		{	CONSTDEF("commsHint"),				FLAG_IS_HINT,	5,	9000,	},
		{	CONSTDEF("dockHint"),				FLAG_IS_HINT,	5,	240,	},
		{	CONSTDEF("mapHint"),				FLAG_IS_HINT,	1,	900,	},
		{	CONSTDEF("autopilotHint"),			FLAG_IS_HINT,	3,	900,	},
		{	CONSTDEF("gateHint"),				FLAG_IS_HINT,	5,	240,	},
		{	CONSTDEF("useItemHint"),			FLAG_IS_HINT,	1,	9000,	},
		{	CONSTDEF("refuelHint"),				FLAG_IS_HINT,	3,	240,	},
		{	CONSTDEF("enableDeviceHint"),		FLAG_IS_HINT,	1,	900,	},
		{	CONSTDEF("switchMissileHint"),		FLAG_IS_HINT,	1,	900,	},
		{	CONSTDEF("fireMissileHint"),		FLAG_IS_HINT,	3,	9000,	},
		{	CONSTDEF("galacticMapHint"),		FLAG_IS_HINT,	1,	900,	},
		{	CONSTDEF("mouseManeuverHint"),		FLAG_IS_HINT,	1,	240,	},
		{	CONSTDEF("keyboardManeuverHint"),	FLAG_IS_HINT,	1,	240,	},
		{	CONSTDEF("stationDamageHint"),		FLAG_IS_HINT,	0,	240,	},
		{	CONSTDEF("miningDamageTypeHint"),	FLAG_IS_HINT,	0,	240,	},
		{	CONSTDEF("fireWeaponHint"),			FLAG_IS_HINT,	1,	240,	},
		{	CONSTDEF("shipStatusHint"),			FLAG_IS_HINT,	1,	240,	},
	};

CUIMessageController::CUIMessageController (void)

//	CUIMessageController constructor

	{
	}

bool CUIMessageController::CanShow (CUniverse &Universe, UIMessageTypes iMsg, const CSpaceObject *pMsgObj) const

//	CanShow
//
//	Returns TRUE if we can show the given hint.

	{
	const SMsgEntry &Msg = m_Messages[iMsg];

	if (!Msg.bEnabled)
		return false;

	if (pMsgObj)
		{
		if (Msg.dwLastObjID == pMsgObj->GetID()
				&& !IsTime(Universe.GetFrameTicks(), Msg.dwLastShown, g_MessageData[iMsg].dwInterval))
			return false;
		}
	else
		{
		if (!IsTime(Universe.GetFrameTicks(), Msg.dwLastShown, g_MessageData[iMsg].dwInterval))
			return false;
		}

	//	Show it

	return true;
	}

UIMessageTypes CUIMessageController::Find (const CString &sMessageName) const

//	Find
//
//	Find the message by name. Returns uimsgUnknown if not found.

	{
	int i;

	for (i = 0; i < uimsgCount; i++)
		if (strEquals(sMessageName, CString(g_MessageData[i].pszName, g_MessageData[i].iNameLen, true)))
			return (UIMessageTypes)i;

	if (strEquals(sMessageName, CONSTLIT("enabledHints")))
		return uimsgEnabledHints;
	else
		return uimsgUnknown;
	}

bool CUIMessageController::IsHint (UIMessageTypes iMsg)

//	IsHint
//
//	Returns TRUE if the message is a hint

	{
	if (iMsg < 0)
		return false;

	return ((g_MessageData[iMsg].dwFlags & FLAG_IS_HINT) ? true : false);
	}

void CUIMessageController::OnHintFollowed (UIMessageTypes iMsg, DWORD dwTick)

//	OnHintFollowed
//
//	Player has used the command for this hint.
//
//	NOTE: We don't store iHintFollowedCount. Across sessions it is OK if we 
//	reset (since the player may have forgotten). But once the player has done
//	the proper number of commands, we disable the hint and store that fact.

	{
	if (iMsg < 0 || !m_Messages[iMsg].bEnabled || !(g_MessageData[iMsg].dwFlags & FLAG_IS_HINT))
		return;

	m_Messages[iMsg].iHintFollowedCount++;

	//	If we have a tick value, set the last shown time (so that we don't show
	//	again for a while).

	if (dwTick)
		m_Messages[iMsg].dwLastShown = dwTick;

	//	If the player has used this command enough, we disable it.

	if (m_Messages[iMsg].iHintFollowedCount >= g_MessageData[iMsg].iHintRepeat)
		SetEnabled(iMsg, false);
	}

void CUIMessageController::OnMessageShown (CUniverse &Universe, UIMessageTypes iMsg, const CSpaceObject *pMsgObj)

//	OnMessageShown
//
//	Message has been shown to player, so we update our timer.

	{
	m_Messages[iMsg].dwLastShown = Universe.GetFrameTicks();
	m_Messages[iMsg].dwLastObjID = (pMsgObj ? pMsgObj->GetID() : 0);
	}

void CUIMessageController::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from a save file

	{
	//	Read the number of bytes stored

	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);

	//	Load the array of bytes
	//	(We assume it is DWORD aligned)

	int iArraySize = AlignUp((int)dwLoad, sizeof(DWORD));
	BYTE *pLoad = new BYTE [iArraySize];
	Ctx.pStream->Read((char *)pLoad, iArraySize);

	//	Each byte is an enabled state
	//	(We don't use SetEnabled because that has special meaning for
	//	certain message).

	for (int i = 0; i < Min((int)uimsgCount, (int)dwLoad); i++)
		m_Messages[i].bEnabled = (pLoad[i] ? true : false);

	//	NOTE: If uimsgCount increases between version, that's OK because
	//	we initialize m_bMsgEnabled properly in the constructor.

	//	Done

	delete [] pLoad;
	}

void CUIMessageController::SetEnabled (UIMessageTypes iMsg, bool bEnabled)

//	SetEnabled
//
//	Enable or disable a message

	{
	int i;

	if (iMsg == uimsgAllMessages)
		{
		for (i = 0; i < uimsgCount; i++)
			m_Messages[i].bEnabled = bEnabled;
		}
	else if (iMsg == uimsgAllHints)
		{
		for (i = 0; i < uimsgCount; i++)
			if (IsHint((UIMessageTypes)i))
				m_Messages[i].bEnabled = bEnabled;

		//	We keep track of whether any hint is enabled

		m_Messages[uimsgAllHints].bEnabled = bEnabled;
		}
	else if (iMsg == uimsgEnabledHints)
		{
		if (bEnabled)
			{
			//	Re-enable hints, but only if we have some hints left.

			for (i = 0; i < uimsgCount; i++)
				if (IsHint((UIMessageTypes)i) && m_Messages[i].bEnabled)
					{
					m_Messages[uimsgAllHints].bEnabled = true;
					break;
					}
			}
		else
			m_Messages[uimsgAllHints].bEnabled = false;
		}
	else
		{
		m_Messages[iMsg].bEnabled = bEnabled;

		//	We keep track of whether any hint is enabled

		if (IsHint(iMsg))
			{
			if (bEnabled)
				m_Messages[uimsgAllHints].bEnabled = true;
			else if (m_Messages[uimsgAllHints].bEnabled)
				{
				m_Messages[uimsgAllHints].bEnabled = false;
				for (i = 0; i < uimsgCount; i++)
					if (IsHint((UIMessageTypes)i) && m_Messages[i].bEnabled)
						{
						m_Messages[uimsgAllHints].bEnabled = true;
						break;
						}
				}
			}
		}
	}

bool CUIMessageController::ShowMessage (CUniverse &Universe, UIMessageTypes iMsg, const CSpaceObject *pMsgObj)

//	ShowMessage
//
//	If the message can be shown, we return TRUE and set the last shown state.
//	Otherwise, we return FALSE.

	{
	if (!CanShow(Universe, iMsg, pMsgObj))
		return false;

	OnMessageShown(Universe, iMsg, pMsgObj);
	return true;
	}

void CUIMessageController::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Write out to the save file

	{
	//	Write the count of messages

	DWORD dwSave = uimsgCount;
	pStream->Write(dwSave);

	//	Create an array with a byte for each message state
	//	(We make sure that it is DWORD aligned)

	int iArraySize = AlignUp(uimsgCount, sizeof(DWORD));
	BYTE *pSave = new BYTE [iArraySize];
	for (int i = 0; i < uimsgCount; i++)
		pSave[i] = (m_Messages[i].bEnabled ? 1 : 0);

	//	Save

	pStream->Write((char *)pSave, iArraySize);
	}
