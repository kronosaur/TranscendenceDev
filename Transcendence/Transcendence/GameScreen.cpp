//	GameScreen.cpp
//
//	Handles CTranscendenceWnd methods related to playing the game

#include "PreComp.h"
#include "Transcendence.h"
#include "XMLUtil.h"

#define BEGIN_EXCEPTION_HANDLER		try
#define END_EXCEPTION_HANDLER		catch (...) { g_pHI->GetScreenMgr().StopDX(); throw; }

#define MENU_DISPLAY_WIDTH					200
#define MENU_DISPLAY_HEIGHT					600

#define PICKER_DISPLAY_WIDTH				1024
#define PICKER_DISPLAY_HEIGHT				160

#define DEVICE_DISPLAY_WIDTH				1024
#define DEVICE_DISPLAY_HEIGHT				160

#define SRSSNOW_IMAGE_WIDTH					256
#define SRSSNOW_IMAGE_HEIGHT				256

#define CMD_PAUSE							100
#define CMD_SAVE							101
#define CMD_SELF_DESTRUCT					103
#define CMD_DELETE							104
#define CMD_REVERT							105

#define CMD_CONFIRM							110
#define CMD_CANCEL							111

#define MAX_COMMS_OBJECTS					24

#define STR_INSTALLED						CONSTLIT(" (installed)")
#define STR_DISABLED						CONSTLIT(" (disabled)")

#define BAR_COLOR							CG32bitPixel(0, 2, 10)

#define SQUADRON_LETTER						'Q'
#define SQUADRON_KEY						CONSTLIT("Q")
#define SQUADRON_LABEL						CONSTLIT("Squadron")

#define SO_ATTACK_IN_FORMATION				CONSTLIT("Attack in formation")
#define SO_BREAK_AND_ATTACK					CONSTLIT("Break & attack")
#define SO_FORM_UP							CONSTLIT("Form up")
#define SO_ATTACK_TARGET					CONSTLIT("Attack target")
#define SO_WAIT								CONSTLIT("Wait")
#define SO_CANCEL_ATTACK					CONSTLIT("Cancel attack")
#define SO_ALPHA_FORMATION					CONSTLIT("Alpha formation")
#define SO_BETA_FORMATION					CONSTLIT("Beta formation")
#define SO_GAMMA_FORMATION					CONSTLIT("Gamma formation")

const int g_MessageDisplayWidth = 400;
const int g_MessageDisplayHeight = 32;

void CTranscendenceWnd::Autopilot (bool bTurnOn)

//	Autopilot
//
//	Turn autopilot on/off

	{
	if (bTurnOn != m_bAutopilot)
		{
		if (bTurnOn)
			{
			if (GetPlayer()->GetShip()->HasAutopilot())
				{
				m_bAutopilot = true;
				}
			else
				DisplayMessage(CONSTLIT("No autopilot installed"));
			}
		else
			{
			m_bAutopilot = false;
			}
		}
	}

void CTranscendenceWnd::DisplayMessage (CString sMessage)

//	DisplayMessage
//
//	Display a message for the player

	{
	if (auto pPlayer = GetPlayer())
		{
		pPlayer->DisplayMessage(sMessage);
		}
	}

void CTranscendenceWnd::DoCommsMenu (int iIndex)

//	DoCommsMenu
//
//	Send message to the object

	{
	if (m_pMenuObj)
		{
		if (GetPlayer())
			m_pMenuObj->CommsMessageFrom(GetPlayer()->GetShip(), iIndex);

		m_pMenuObj->SetHighlightChar(0);
		m_pMenuObj = NULL;
		}
	}

void CTranscendenceWnd::DoCommsSquadronMenu (const CString &sName, MessageTypes iOrder, DWORD dwData2)

//	DoCommsSquadronMenu
//
//	Invokes a squadron menu

	{
	int i;
	if (GetPlayer() == NULL)
		return;

	CSpaceObject *pShip = GetPlayer()->GetShip();
	CSystem *pSystem = pShip->GetSystem();

	//	Iterate over all ships in formation

	DWORD dwFormationPlace = 0;

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->GetEscortPrincipal() == pShip
				&& pObj != pShip)
			{
			//	First try to send as a comms message

			int iIndex;
			if ((iIndex = pObj->FindCommsMessageByName(sName)) != -1
					&& pObj->IsCommsMessageValidFrom(*pShip, iIndex))
				pObj->CommsMessageFrom(pShip, iIndex);

			//	Otherwise, we send the old-style way

			else
				GetPlayer()->Communications(pObj, iOrder, dwData2, &dwFormationPlace);
			}
		}
	}

DWORD CTranscendenceWnd::GetCommsStatus (void)

//	GetCommsStatus
//
//	Returns the messages accepted by the squadron

	{
	CSpaceObject *pShip = GetPlayer()->GetShip();
	if (!pShip)
		return 0;

	return pShip->GetSquadronCommsStatus();
	}

void CTranscendenceWnd::HideCommsMenu (void)

//	HideCommsMenu
//
//	Remove all highlight keys

	{
	if (m_pMenuObj)
		m_pMenuObj->SetHighlightChar(0);
	}

void CTranscendenceWnd::HideCommsTargetMenu (CSpaceObject *pExclude)

//	HideCommsTargetMenu
//
//	Remove all highlight keys

	{
	for (int i = 0; i < m_MenuData.GetCount(); i++)
		{
		CSpaceObject *pObj = (CSpaceObject *)m_MenuData.GetItemData(i);
		if (pObj && pObj != pExclude)
			pObj->SetHighlightChar(0);
		}
	}

ALERROR CTranscendenceWnd::InitDisplays (void)

//	InitDisplays
//
//	Initializes display structures

	{
	RECT rcRect;

	//	Find some bitmaps that we need. NOTE: We lock the images because we
	//	don't dispose of them.
	//
	//	LATER: These should be obtained form the player ship.

	m_pSRSSnow = g_pUniverse->GetLibraryBitmap(UNID_SRS_SNOW_PATTERN, CDesignCollection::FLAG_IMAGE_LOCK);

	//	Initialize some displays (these need to be done after we've
	//	created the universe).

	rcRect.left = m_rcScreen.right - (MENU_DISPLAY_WIDTH + 4);
	rcRect.top = (RectHeight(m_rcScreen) - MENU_DISPLAY_HEIGHT) / 2;
	rcRect.right = rcRect.left + MENU_DISPLAY_WIDTH;
	rcRect.bottom = rcRect.top + MENU_DISPLAY_HEIGHT;
	m_MenuDisplay.SetFontTable(&m_Fonts);
	m_MenuDisplay.Init(&m_MenuData, rcRect);

	rcRect.left = m_rcScreen.left + (RectWidth(m_rcScreen) - DEVICE_DISPLAY_WIDTH) / 2;
	rcRect.right = rcRect.left + DEVICE_DISPLAY_WIDTH;
	rcRect.top = m_rcScreen.bottom - DEVICE_DISPLAY_HEIGHT;
	rcRect.bottom = m_rcScreen.bottom;
	m_DeviceDisplay.SetFontTable(&m_Fonts);
	m_DeviceDisplay.Init(GetPlayer(), rcRect);

	return NOERROR;
	}

void CTranscendenceWnd::PaintMainScreenBorder (CG32bitPixel rgbColor)

//	PaintMainScreenBorder
//
//	Paints the borders of the main screen in case the display is larger
//	than the main screen (larger than 1024x768)

	{
	CG32bitImage &TheScreen = g_pHI->GetScreen();

	if (m_rcMainScreen.left != m_rcScreen.left)
		{
		int cxMainScreen = RectWidth(m_rcMainScreen);
		int cyMainScreen = RectHeight(m_rcMainScreen);

		TheScreen.Fill(0, 0, g_cxScreen, m_rcMainScreen.top, rgbColor);
		TheScreen.Fill(0, m_rcMainScreen.bottom, g_cxScreen, g_cyScreen - m_rcMainScreen.bottom, rgbColor);
		TheScreen.Fill(0, m_rcMainScreen.top, m_rcMainScreen.left, cyMainScreen, rgbColor);
		TheScreen.Fill(m_rcMainScreen.right, m_rcMainScreen.top, g_cxScreen - m_rcMainScreen.right, cyMainScreen, rgbColor);
		}
	}

void CTranscendenceWnd::PaintSnow (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight)

//	PaintSnow
//
//	Fills the rect with snow

	{
	int y1 = y;
	int ySrc = mathRandom(0, SRSSNOW_IMAGE_HEIGHT-1);
	int cySrc = SRSSNOW_IMAGE_HEIGHT - ySrc;
	while (y1 < cyHeight)
		{
		int x1 = x;
		int xSrc = mathRandom(0, SRSSNOW_IMAGE_WIDTH-1);
		int cxSrc = SRSSNOW_IMAGE_WIDTH - xSrc;
		while (x1 < cxWidth)
			{
			Dest.Blt(xSrc, ySrc, cxSrc, cySrc, *m_pSRSSnow, x1, y1);
			x1 += cxSrc;
			xSrc = 0;
			cxSrc = min(SRSSNOW_IMAGE_WIDTH, (g_cxScreen - x1));
			}

		y1 += cySrc;
		ySrc = 0;
		cySrc = min(SRSSNOW_IMAGE_HEIGHT, (g_cyScreen - y1));
		}
	}

void CTranscendenceWnd::PaintSRSSnow (void)

//	PaintSRSSnow
//
//	Fills the screen with snow

	{
	CG32bitImage &TheScreen = g_pHI->GetScreen();

	PaintSnow(TheScreen, 0, 0, g_cxScreen, g_cyScreen);

	CShip *pShip = GetPlayer()->GetShip();
	if (pShip && pShip->GetSystem())
		g_pUniverse->PaintObject(TheScreen, m_rcMainScreen, pShip);
	}

void CTranscendenceWnd::ShowCommsMenu (CSpaceObject *pObj)

//	ShowCommsMenu
//
//	Shows the comms menu for this object

	{
	int i;

	if (GetPlayer())
		{
		CSpaceObject *pShip = GetPlayer()->GetShip();
		m_pMenuObj = pObj;

		m_MenuData.SetTitle(m_pMenuObj->GetNounPhrase());
		m_MenuData.DeleteAll();

		int iMsgCount = m_pMenuObj->GetCommsMessageCount();
		for (i = 0; i < iMsgCount; i++)
			{
			CString sName;
			CString sKey;

			if (m_pMenuObj->IsCommsMessageValidFrom(*pShip, i, &sName, &sKey))
				m_MenuData.AddMenuItem(NULL_STR,
						sKey,
						sName,
						CMenuData::FLAG_SORT_BY_KEY,
						i);
			}

		m_MenuDisplay.Invalidate();
		}
	}

bool CTranscendenceWnd::ShowCommsSquadronMenu (void)

//	ShowCommsSquadronMenu
//
//	Shows the comms menu for the whole squadron. Returns FALSE if the menu could
//	not be shown (e.g., if there is no squadron).

	{
	if (!GetPlayer())
		return false;

	m_MenuData.SetTitle(CONSTLIT("Squadron Orders"));
	m_MenuData.DeleteAll();

	DWORD dwStatus = GetCommsStatus();

	if (dwStatus & resCanAttack)
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("A"), SO_ATTACK_TARGET, 0, msgAttack);

	if (dwStatus & resCanBreakAndAttack)
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("B"), SO_BREAK_AND_ATTACK, 0, msgBreakAndAttack);

	if ((dwStatus & resCanFormUp) || (dwStatus & resCanAbortAttack))
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("F"), SO_FORM_UP, 0, msgFormUp, 0xffffffff);

	if (dwStatus & resCanAttackInFormation)
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("I"), SO_ATTACK_IN_FORMATION, 0, msgAttackInFormation);

	if (dwStatus & resCanWait)
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("W"), SO_WAIT, 0, msgWait);

	if (dwStatus & resCanBeInFormation)
		{
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("1"), SO_ALPHA_FORMATION, 0, msgFormUp, 0);
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("2"), SO_BETA_FORMATION, 0, msgFormUp, 1);
		m_MenuData.AddMenuItem(NULL_STR, CONSTLIT("3"), SO_GAMMA_FORMATION, 0, msgFormUp, 2);
		}

	//	Show Menu

	if (m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No carrier signal"));
		return false;
		}

	m_MenuDisplay.Invalidate();
	return true;
	}

bool CTranscendenceWnd::ShowCommsTargetMenu (void)

//	ShowCommsTargetMenu
//
//	Shows list of objects to communicate with. Returns FALSE if there are no
//	targets.

	{
	if (!GetPlayer())
		return false;

	CShip *pShip = GetPlayer()->GetShip();
	m_MenuData.SetTitle(CONSTLIT("Communications"));
	m_MenuData.DeleteAll();

	//	Keep track of which keys we've used, in case specific objects want
	//	to use their own keys.

	TSortMap<CString, bool> KeyMap;
	int iNextKey = 0;

	//	Add the comms key to the list of keys to exclude

	char chCommsKey = m_pTC->GetKeyMap().GetKeyIfChar(CGameKeys::keyCommunications);
	KeyMap.Insert(CString(&chCommsKey, 1), true);

	//	First add all the objects that are following the player

	CSystem *pSystem = pShip->GetSystem();
	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->CanCommunicateWith(*pShip)
				&& pObj->GetEscortPrincipal() == pShip
				&& pObj != pShip)
			{
			if (m_MenuData.GetCount() < MAX_COMMS_OBJECTS)
				{
				CString sKey = pObj->GetDesiredCommsKey();
				if (sKey.IsBlank() || KeyMap.GetAt(sKey) != NULL)
					sKey = CMenuDisplayOld::GetHotKeyFromOrdinal(&iNextKey, KeyMap);

				if (!sKey.IsBlank())
					{
					m_MenuData.AddMenuItem(NULL_STR,
							sKey,
							pObj->GetNounPhrase(),
							CMenuData::FLAG_SORT_BY_KEY,
							(DWORD)pObj);

					pObj->SetHighlightChar(*sKey.GetASCIIZPointer());
					KeyMap.SetAt(sKey, true);
					}
				}
			}
		}

	//	Next, add all other objects

	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->CanCommunicateWith(*pShip)
				&& pObj->GetEscortPrincipal() != pShip
				&& pObj != pShip)
			{
			if (m_MenuData.GetCount() < MAX_COMMS_OBJECTS)
				{
				CString sKey = pObj->GetDesiredCommsKey();
				if (sKey.IsBlank() || KeyMap.GetAt(sKey) != NULL)
					sKey = CMenuDisplayOld::GetHotKeyFromOrdinal(&iNextKey, KeyMap);

				if (!sKey.IsBlank())
					{
					m_MenuData.AddMenuItem(NULL_STR,
							sKey,
							pObj->GetNounPhrase(),
							CMenuData::FLAG_SORT_BY_KEY,
							(DWORD)pObj);

					pObj->SetHighlightChar(*sKey.GetASCIIZPointer());
					KeyMap.SetAt(sKey, true);
					}
				}
			}
		}

	//	Done

	if (m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No carrier signal"));
		return false;
		}

	m_MenuDisplay.Invalidate();
	return true;
	}



