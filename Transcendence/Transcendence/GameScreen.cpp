//	GameScreen.cpp
//
//	Handles CTranscendenceWnd methods related to playing the game

#include "PreComp.h"
#include "Transcendence.h"
#include "XMLUtil.h"

#define BEGIN_EXCEPTION_HANDLER		try
#define END_EXCEPTION_HANDLER		catch (...) { g_pHI->GetScreenMgr().StopDX(); throw; }

#define INVOKE_DISPLAY_WIDTH				200
#define INVOKE_DISPLAY_HEIGHT				300

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
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				pObj->CommsMessageFrom(pShip, iIndex);

			//	Otherwise, we send the old-style way

			else
				GetPlayer()->Communications(pObj, iOrder, dwData2, &dwFormationPlace);
			}
		}
	}

void CTranscendenceWnd::DoEnableDisableItemCommand (DWORD dwData)

//	DoEnableDisableItemCommand
//
//	Enable/disable an item

	{
	CShip *pShip = GetPlayer()->GetShip();
	CItemList &ItemList = pShip->GetItemList();
	CItem &Item = ItemList.GetItem(dwData);

	if (Item.IsInstalled() && Item.GetType()->IsDevice())
		{
		int iDev = Item.GetInstalled();
		CInstalledDevice *pDevice = pShip->GetDevice(iDev);
		pShip->EnableDevice(iDev, !pDevice->IsEnabled());
		}
	}

void CTranscendenceWnd::DoInvocation (CPower *pPower)

//	DoInvocation
//
//	Invoke power

	{
	if (GetPlayer())
		{
		CString sError;
		pPower->InvokeByPlayer(GetPlayer()->GetShip(), GetPlayer()->GetTarget(), &sError);
		if (!sError.IsBlank())
			{
			DisplayMessage(sError);
			kernelDebugLogString(sError);
			}
		}
	}

void CTranscendenceWnd::DoUseItemCommand (DWORD dwData)

//	DoUseItemCommand
//
//	Use an item

	{
	//	Get the item

	CShip *pShip = GetPlayer()->GetShip();
	CItemList &ItemList = pShip->GetItemList();
	CItem &Item = ItemList.GetItem(dwData);

	//	Use it

	GetModel().UseItem(Item);
	}

DWORD CTranscendenceWnd::GetCommsStatus (void)

//	GetCommsStatus
//
//	Returns the messages accepted by the squadron

	{
	int i;
	CSpaceObject *pShip = GetPlayer()->GetShip();
	CSystem *pSystem = pShip->GetSystem();
	DWORD dwStatus = 0;

	//	First add all the messages accepted by ships using the
	//	old-style "fleet" controller

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj != pShip)
			dwStatus |= pShip->Communicate(pObj, msgQueryCommunications);
		}

	//	Next add in messages accepted through the communications struct

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->CanCommunicateWith(pShip)
				&& pObj != pShip)
			{
			int iIndex;
			if ((iIndex = pObj->FindCommsMessageByName(SO_ATTACK_IN_FORMATION)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanAttackInFormation;

			if ((iIndex = pObj->FindCommsMessageByName(SO_BREAK_AND_ATTACK)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanBreakAndAttack;

			if ((iIndex = pObj->FindCommsMessageByName(SO_FORM_UP)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanFormUp;

			if ((iIndex = pObj->FindCommsMessageByName(SO_ATTACK_TARGET)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanAttack;

			if ((iIndex = pObj->FindCommsMessageByName(SO_WAIT)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanWait;

			if ((iIndex = pObj->FindCommsMessageByName(SO_CANCEL_ATTACK)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanAbortAttack;

			if ((iIndex = pObj->FindCommsMessageByName(SO_ALPHA_FORMATION)) != -1
					&& pObj->IsCommsMessageValidFrom(pShip, iIndex))
				dwStatus |= resCanBeInFormation;
			}
		}

	return dwStatus;
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

	rcRect.left = m_rcScreen.left + (RectWidth(m_rcScreen) - PICKER_DISPLAY_WIDTH) / 2;
	rcRect.right = rcRect.left + PICKER_DISPLAY_WIDTH;
	rcRect.top = m_rcScreen.bottom - 200 - PICKER_DISPLAY_HEIGHT;
//  LATER: Once we move the picker display to m_HUD, this will be fixed
//	rcRect.top = m_ArmorDisplay.GetRect().top - PICKER_DISPLAY_HEIGHT;
	rcRect.bottom = rcRect.top + PICKER_DISPLAY_HEIGHT;
	m_PickerDisplay.SetFontTable(&m_Fonts);
	m_PickerDisplay.Init(&m_MenuData, rcRect);

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

			if (m_pMenuObj->IsCommsMessageValidFrom(pShip, i, &sName, &sKey))
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
				&& pObj->CanCommunicateWith(pShip)
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
				&& pObj->CanCommunicateWith(pShip)
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

	//	Add the squadron option, if necessary

	if ((m_MenuData.GetCount() > 1 && GetCommsStatus() != 0) 
			|| GetPlayer()->HasFleet())
		m_MenuData.AddMenuItem(NULL_STR, SQUADRON_KEY, SQUADRON_LABEL, 0, 0);

	//	Done

	if (m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No carrier signal"));
		return false;
		}

	m_MenuDisplay.Invalidate();
	return true;
	}

bool CTranscendenceWnd::ShowInvokeMenu (void)

//	ShowInvokeMenu
//
//	Shows menu of powers to invoke. Returns FALSE if no powers to invoke.

	{
	if (!GetPlayer())
		return false;

	m_MenuData.SetTitle(CONSTLIT("Invoke Powers"));
	m_MenuData.DeleteAll();

	bool bUseLetters = m_pTC->GetOptionBoolean(CGameSettings::allowInvokeLetterHotKeys);

	TSortMap<CString, bool> KeyMap;
	char chInvokeKey = m_pTC->GetKeyMap().GetKeyIfChar(CGameKeys::keyInvokePower);
	KeyMap.Insert(CString(&chInvokeKey, 1), true);

	//	Add the powers

	for (int i = 0; i < g_pUniverse->GetPowerCount(); i++)
		{
		CPower *pPower = g_pUniverse->GetPower(i);

		CString sError;
		if (pPower->OnShow(GetPlayer()->GetShip(), NULL, &sError))
			{
			CString sKey = pPower->GetInvokeKey();
			if (sKey.IsBlank())
				continue;

			//	If we're the default letter keys, then make sure we don't
			//	conflict.

			if (bUseLetters)
				{
				//	Make sure key is one character long (we use a double-
				//	letter syntax below).

				sKey.Truncate(1);

				//	If the key conflicts, then pick another key (the next 
				//	key in the sequence).

				while (!sKey.IsBlank() && KeyMap.GetAt(sKey) != NULL)
					{
					char chChar = (*sKey.GetASCIIZPointer()) + 1;
					if (chChar == ':')
						chChar = 'A';

					if (chChar <= 'Z')
						sKey = CString(&chChar, 1);
					else
						sKey = NULL_STR;
					}
				}

			//	If we're not using letters, then convert to a number

			else
				{
				char chLetter = *sKey.GetASCIIZPointer();
				int iOrdinal = chLetter - 'A';

				//	A double letter means that we really want a letter, so we
				//	offset to increment past the numbers.

				if (sKey.GetLength() == 2)
					iOrdinal += 9;

				sKey = CMenuDisplayOld::GetHotKeyFromOrdinal(&iOrdinal, KeyMap);
				}

			//	Add the menu. (We check again to see if the key is valid
			//	because we might have collided and failed to find a substitute.)

			if (!sKey.IsBlank())
				{
				m_MenuData.AddMenuItem(NULL_STR,
						sKey,
						pPower->GetName(),
						CMenuData::FLAG_SORT_BY_KEY,
						(DWORD)pPower);

				KeyMap.Insert(sKey, true);
				}
			}

		if (!sError.IsBlank())
			{
			DisplayMessage(sError);
			return false;
			}
		}

	//	If no powers are available, say so

	if (m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No Powers available"));
		return false;
		}

	//	Show menu

	m_MenuDisplay.Invalidate();
	return true;
	}

bool CTranscendenceWnd::ShowEnableDisablePicker (void)

//	ShowEnableDisablePicker
//
//	Show the picker to select devices to enable/disable. Returns FALSE if there
//	are no devices to enable/disable.

	{
	if (!GetPlayer())
		return false;

	CShip *pShip = GetPlayer()->GetShip();

	//	Fill the menu with all usable items

	m_MenuData.DeleteAll();

	CItemList &List = pShip->GetItemList();
	List.SortItems();

	char chKey = '1';
	for (int i = 0; i < List.GetCount(); i++)
		{
		CItem &Item = List.GetItem(i);
		CItemType *pType = Item.GetType();
		CInstalledDevice *pDevice = pShip->FindDevice(Item);
		CItemCtx ItemCtx(&Item, pShip, pDevice);

		if (pDevice && pDevice->CanBeDisabled(ItemCtx))
			{
			//	Name of item

			CString sName;
			if (pDevice->IsEnabled())
				sName = strPatternSubst(CONSTLIT("Disable %s"), Item.GetNounPhrase());
			else
				sName = strPatternSubst(CONSTLIT("Enable %s"), Item.GetNounPhrase());

			//	Extra

			CString sExtra;
			if (!pDevice->IsEnabled())
				sExtra = CONSTLIT("Disabled");

			//	Help

			CString sHelp;
			if (pDevice->IsEnabled())
				sHelp = CONSTLIT("[Enter] to disable; [Arrows] to select");
			else
				sHelp = CONSTLIT("[Enter] to enable; [Arrows] to select");

			//	Key

			CString sKey(&chKey, 1);

			//	Add the item

			m_MenuData.AddMenuItem(NULL_STR,
					sKey,
					sName,
					&pType->GetImage(),
					0,
					sExtra,
					sHelp,
					(pDevice->IsEnabled() ? 0 : CMenuData::FLAG_GRAYED),
					i);

			//	Next key

			if (chKey == '9')
				chKey = 'A';
			else
				chKey++;
			}
		}

	//	If we've got items, then show the picker...

	if (m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No installed devices"));
		return false;
		}

	m_PickerDisplay.ResetSelection();
	m_PickerDisplay.Invalidate();
	m_PickerDisplay.SetHelpText(NULL_STR);
	return true;
	}

bool CTranscendenceWnd::ShowUsePicker (void)

//	ShowUsePicker
//
//	Show the picker to select an item to use. Returns FALSE if there are no 
//	items to use.

	{
	if (!GetPlayer())
		return false;

	CShip *pShip = GetPlayer()->GetShip();
	char chUseKey = m_pTC->GetKeyMap().GetKeyIfChar(CGameKeys::keyUseItem);

	//	Fill the menu with all usable items

	m_MenuData.DeleteAll();

	CItemList &List = pShip->GetItemList();
	List.SortItems();

	//	Generate a sorted list of items

	TSortMap<CString, int> SortedList;
	for (int i = 0; i < List.GetCount(); i++)
		{
		CItem &Item = List.GetItem(i);
		CItemType *pType = Item.GetType();
		CItemCtx ItemCtx(&Item, pShip);

		//	See if we can use this item, and get the use key

		CString sUseKey;
		if (!Item.CanBeUsed(&sUseKey))
			continue;

		//	Add to the list

		bool bHasUseKey = (Item.IsKnown() && !sUseKey.IsBlank() && (*sUseKey.GetASCIIZPointer() != chUseKey));

		//	Any items without use keys sort first (so that they are easier
		//	to access).
		//
		//	Then we sort by level (higher-level first)
		//
		//	Then we sort by natural order
		//
		//	For items that use charges, we expand if there are multiple

		if (pType->ShowChargesInUseMenu() && Item.IsKnown())
			{
			for (int j = 0; j < Item.GetCount(); j++)
				{
				SortedList.Insert(strPatternSubst(CONSTLIT("%d%s%04d%04d"),
							(bHasUseKey ? 1 : 0),
							(bHasUseKey ? strPatternSubst(CONSTLIT("%s0"), sUseKey) : strPatternSubst(CONSTLIT("%02d"), MAX_ITEM_LEVEL - Item.GetLevel())),
							i, j),
						i);
				}
			}
		else
			SortedList.Insert(strPatternSubst(CONSTLIT("%d%s%04d%04d"),
						(bHasUseKey ? 1 : 0),
						(bHasUseKey ? strPatternSubst(CONSTLIT("%s0"), sUseKey) : strPatternSubst(CONSTLIT("%02d"), MAX_ITEM_LEVEL - Item.GetLevel())),
						i, 0),
					i);
		}

	//	Now add all the items to the menu

	for (int i = 0; i < SortedList.GetCount(); i++)
		{
		CString sSort = SortedList.GetKey(i);
		CItem &Item = List.GetItem(SortedList.GetValue(i));
		CItemType *pType = Item.GetType();

		CItemType::SUseDesc UseDesc;
		if (!pType->GetUseDesc(&UseDesc))
			continue;

		int iCount;
		if (pType->ShowChargesInUseMenu() && Item.IsKnown())
			iCount = Item.GetCharges();
		else if (Item.GetCount() > 1)
			iCount = Item.GetCount();
		else
			iCount = 0;

		//	Installed

		CString sExtra;
		if (Item.IsInstalled())
			sExtra = CONSTLIT("Installed");

		//	Show the key only if the item is identified

		CString sKey;
		if (Item.IsKnown() && (*UseDesc.sUseKey.GetASCIIZPointer() != chUseKey))
			sKey = UseDesc.sUseKey;

		//	Name of item

		CString sName = Item.GetNounPhrase();
		sName = strPatternSubst(CONSTLIT("Use %s"), sName);

		//	Add the item

		m_MenuData.AddMenuItem(NULL_STR,
				sKey,
				sName,
				&pType->GetImage(),
				iCount,
				sExtra,
				NULL_STR,
				0,
				SortedList.GetValue(i));
		}

	//	If no items, then no menu

	if (m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No usable items"));
		return false;
		}

	//	Otherwise, show picker

	GetPlayer()->SetUIMessageFollowed(uimsgUseItemHint);
	m_PickerDisplay.ResetSelection();
	m_PickerDisplay.SetHelpText(CONSTLIT("[Enter] to use; [Arrows] to select"));
	m_PickerDisplay.Invalidate();
	return true;
	}
