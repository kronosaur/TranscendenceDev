//	CKeyboardMapSession.cpp
//
//	CKeyboardMapSession class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define ALIGN_CENTER							CONSTLIT("center")

#define CMD_CLOSE_SESSION						CONSTLIT("cmdCloseSession")
#define CMD_OK_SESSION							CONSTLIT("cmdOKSession")

#define CMD_RESET_DEFAULT 						CONSTLIT("cmdResetDefault")
#define CMD_RESET_WASD							CONSTLIT("cmdResetWASD")
#define CMD_CLEAR_BINDING 						CONSTLIT("cmdClearBinding")
#define CMD_SET_COMMAND 						CONSTLIT("cmdSetCommand")

#define CMD_ON_KEYBOARD_MAPPING_CHANGED			CONSTLIT("onKeyboardMappingChanged")

#define ID_CTRL_TITLE							CONSTLIT("ctrlTitle")
#define ID_LAYOUT_LABEL							CONSTLIT("idLayoutLabel")
#define ID_SETTINGS								CONSTLIT("idSettings")

#define EVENT_ON_CLICK							CONSTLIT("onClick")

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FONT								CONSTLIT("font")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")

#ifdef DEBUG
//#define DEBUG_KEYMAP
#endif

const int KEY_SPACING = 4;
const int KEY_CORNER_RADIUS = 3;
const int LABEL_SPACING = 4;
const int LABEL_COLUMN_SPACING = 14;

const int LINE_WIDTH = 2;

const int KEYBOARD_HEIGHT = 220;
const int KEYBOARD_ROWS = 6;

const int MAJOR_PADDING_TOP =					20;
const int SMALL_BUTTON_HEIGHT =					48;
const int SMALL_BUTTON_WIDTH =					48;
const int SMALL_SPACING_HORZ =					8;
const int SMALL_SPACING_VERT =					8;

#include "KeyboardMapData.h"

const CKeyboardMapSession::SDeviceData CKeyboardMapSession::DEVICE_DATA[] =
	{
		{
			deviceKeyboard,			//	Device type
			"Keyboard",				//	Name
			KEYBOARD_DATA,			//	Data
			KEYBOARD_DATA_COUNT,

			37,						//	Cols
			6,						//	Rows
			9,						//	Offset
			0,						//	Offset

			0,						//	x offset
			0,						//	y offset
			},
#if 0
		{
			deviceNumpad,			//	Device type
			"Number Pad",			//	Name
			NUMPAD_DATA,			//	Data
			NUMPAD_DATA_COUNT,

			8,						//	Cols
			5,						//	Rows
			0,						//	Offset
			0,						//	Offset

			0,						//	x offset
			0,						//	y offset
			},
#endif
		{
			deviceMouse,			//	Device type
			"Mouse",				//	Name
			MOUSE_DATA,				//	Data
			MOUSE_DATA_COUNT,

			7,						//	Cols
			5,						//	Rows
			0,						//	Offset
			1,						//	Offset

			0,						//	x offset
			0,						//	y offset
			},
	};

const int CKeyboardMapSession::DEVICE_DATA_COUNT = (sizeof(DEVICE_DATA) / sizeof(DEVICE_DATA[0]));

CKeyboardMapSession::CKeyboardMapSession (CHumanInterface &HI, CCloudService &Service, CGameSettings &Settings) : IHISession(HI),
		m_Service(Service),
		m_Settings(Settings)

//  CKeyboardMapSession constructor

	{
	}

void CKeyboardMapSession::ArrangeCommandLabels (const RECT &rcRect, const RECT &rcKeyboard)

//  ArrangeCommandLabels
//
//  Arrange command labels properly

	{
	bool bDirectLine = false;

	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &LabelFont = VI.GetFont(fontMedium);

	int xCenter = rcRect.left + (RectWidth(rcRect) / 2);
	int yCenter = rcRect.top + (RectHeight(rcRect) / 2);

	int cxLabelSpacing = 2 * LABEL_SPACING;
	int cyLabelSpacing = 2 * LABEL_SPACING;

	//  Generate an array describing the labels for the arranger.

	TArray<CLabelArranger::SLabelDesc> Labels;
	Labels.InsertEmpty(m_Commands.GetCount());
	for (int i = 0; i < m_Commands.GetCount(); i++)
		{
		Labels[i].sLabel = m_Commands[i].sLabel;
		Labels[i].cxWidth = m_Commands[i].cxLabel + cxLabelSpacing;
		Labels[i].cyHeight = LabelFont.GetHeight() + cyLabelSpacing;

		if (m_Commands[i].iKeyBinding != -1)
			{
			const SKeyDesc &Key = m_Keys[m_Commands[i].iKeyBinding];
			Labels[i].xDest = Key.rcRect.left + (RectWidth(Key.rcRect) / 2);
			Labels[i].yDest = Key.rcRect.top + (RectHeight(Key.rcRect) / 2);
			}
		else
			{
			Labels[i].xDest = xCenter;
			Labels[i].yDest = yCenter;
			}
		}

	//  Initialize the arranger and arrange the labels.

	CLabelArranger Arranger;
	Arranger.SetStyle(CLabelArranger::styleSideColumns);
	Arranger.SetBounds(rcRect);
	Arranger.SetRadius(1024);
	Arranger.AddExclusion(rcKeyboard);
	Arranger.Arrange(Labels);

	//  Now initialize the command positions

	for (int i = 0; i < m_Commands.GetCount(); i++)
		{
		m_Commands[i].rcRect = Labels[i].rcRect;
		m_Commands[i].rcRect.bottom -= cyLabelSpacing;
		m_Commands[i].Line.DeleteAll();
		}

	//  Now compute lines to connect commands to keys

	if (bDirectLine)
		{
		for (int i = 0; i < m_Commands.GetCount(); i++)
			{
			SCommandDesc &Command = m_Commands[i];
			if (Command.iKeyBinding != -1)
				{
				const SKeyDesc &Key = m_Keys[m_Commands[i].iKeyBinding];

				//  Are we on the left or right?

				bool bLeftOfKey = (Command.rcRect.right < Key.rcRect.left);

				//  Start in the middle of the command

				POINT *pPt = Command.Line.Insert();
				pPt->x = (bLeftOfKey ? Command.rcRect.right : Command.rcRect.left);
				pPt->y = Command.rcRect.top + (RectHeight(Command.rcRect) / 2);

				//  Move to the key

				pPt = Command.Line.Insert();
				pPt->x = (bLeftOfKey ? Key.rcRect.left : Key.rcRect.right);
				pPt->y = Key.rcRect.top + (RectHeight(Key.rcRect) / 2);
				}
			}
		}
	else
		{
		for (int i = 0; i < m_Commands.GetCount(); i++)
			{
			SCommandDesc &Command = m_Commands[i];
			if (Command.iKeyBinding != -1)
				{
				const SKeyDesc &Key = m_Keys[m_Commands[i].iKeyBinding];
				int yCommand = Command.rcRect.top + (RectHeight(Command.rcRect) / 2);

				//  Are we on the left or right?

				bool bLeftOfKey = (Command.rcRect.right < Key.rcRect.left);
				bool bInsideKey = (yCommand >= Key.rcRect.top + 2 && yCommand < Key.rcRect.bottom - 2);
				bool bAboveKey = (yCommand < Key.rcRect.top + 2);

				//  Start in the middle of the command

				POINT *pPt = Command.Line.Insert();
				pPt->x = (bLeftOfKey ? Command.rcRect.right : Command.rcRect.left);
				pPt->y = yCommand;
			
				//  Move to the key x coord

				int xOffset = (bInsideKey ? 0 : KEY_SPACING);
				pPt = Command.Line.Insert();
				pPt->x = (bLeftOfKey ? Key.rcRect.left + xOffset : Key.rcRect.right - xOffset - 1);
				pPt->y = Command.Line[0].y;

				//  Now move to the key

				if (!bInsideKey)
					{
					pPt = Command.Line.Insert();
					pPt->x = Command.Line[1].x;
					pPt->y = (bAboveKey ? Key.rcRect.top : Key.rcRect.bottom - 1);
					}
				}
			}
		}
	}

bool CKeyboardMapSession::CanBindKey (int iKeyIndex, CGameKeys::Keys iCmd) const

//	CanBindKey
//
//	Returns TRUE if we can bind the key to the given command.

	{
	//	Can't bind reserved keys

	if (m_Keys[iKeyIndex].dwFlags & FLAG_RESERVED)
		return false;

	//	If this command requires an XY position, then we need a virtual key with
	//	XY input (e.g., joystick, mouse).

	if (CGameKeys::IsXYInputCommand(iCmd)
			&& !(CVirtualKeyData::GetKeyFlags(m_Keys[iKeyIndex].dwVirtKey) & CVirtualKeyData::FLAG_XY_INPUT))
		return false;

	//	Binding allowed

	return true;
	}

void CKeyboardMapSession::CloseSession (void)

//	CloseSession
//
//	Closes the session.

	{
	m_HI.HICommand(CMD_ON_KEYBOARD_MAPPING_CHANGED);
	m_HI.ClosePopupSession();
	}

void CKeyboardMapSession::CmdClearBinding (void)

//	CmdClearBinding
//
//	Clear the key binding of the selected command

	{
	if (m_iSelectedCommand != -1 && m_Commands[m_iSelectedCommand].iKeyBinding != -1)
		{
		const SCommandDesc &Command = m_Commands[m_iSelectedCommand];
		m_Settings.GetKeyMap().SetGameKey(m_Keys[Command.iKeyBinding].sKeyID, CGameKeys::keyNone);
		m_iMode = modeNormal;
		m_iSelectedCommand = -1;

		InitBindings();
		UpdateMenu();
		}
	}

void CKeyboardMapSession::CmdResetDefault (CGameKeys::ELayouts iLayout)

//	CmdResetDefault
//
//	Resets the bindings to default

	{
	m_Settings.GetKeyMap().SetLayout(iLayout);
	InitBindings();
	m_iMode = modeNormal;
	m_iSelectedCommand = -1;
	m_iSelectedKey = -1;
	UpdateMenu();
	}

void CKeyboardMapSession::CmdRevert (void)

//	CmdRevert
//
//	Revert to mapping when we started.

	{
	m_Settings.GetKeyMap() = m_SavedKeyMap;
	InitBindings();
	m_iMode = modeNormal;
	m_iSelectedCommand = -1;
	m_iSelectedKey = -1;
	UpdateMenu();
	}

void CKeyboardMapSession::CmdSetCommand (void)

//	CmdSetCommand
//
//	Sets the mode to command set.

	{
	if (m_iMode != modeNormal)
		{
		m_iMode = modeNormal;
		UpdateMenu();
		}
	else if (m_iSelectedKey != -1)
		{
		if (m_iMode != modeSetCommand)
			{
			m_iMode = modeSetCommand;
			UpdateMenu();
			}
		}
	else if (m_iSelectedCommand != -1)
		{
		if (m_iMode != modeSetKey)
			{
			m_iMode = modeSetKey;
			UpdateMenu();
			}
		}
	}

bool CKeyboardMapSession::HitTest (int x, int y, STargetCtx &Ctx)

//  HitTest
//
//  Set if we've clicked on an element.

	{
	POINT pt;
	pt.x = x;
	pt.y = y;

	//  Check all keys

	for (int i = 0; i < m_Keys.GetCount(); i++)
		{
		const SKeyDesc &Key = m_Keys[i];
		if (PtInRect(&Key.rcRect, pt))
			{
			Ctx.bInKey = true;
			Ctx.iKeyIndex = i;
			Ctx.iCmdIndex = Key.iCmdBinding;
			return true;
			}
		}

	//  Check all commands

	for (int i = 0; i < m_Commands.GetCount(); i++)
		{
		const SCommandDesc &Command = m_Commands[i];
		if (PtInRect(&Command.rcRect, pt))
			{
			Ctx.bInKey = false;
			Ctx.iCmdIndex = i;
			Ctx.iKeyIndex = Command.iKeyBinding;
			return true;
			}
		}

	//  Noting hit

	return false;
	}

void CKeyboardMapSession::InitBindings (void)

//	InitBindings
//
//	Initializes bindings between commands and keys. We assume that both InitCommands
//	and InitDevice have been called.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	//  Reset keyboard to remove bindings

	for (int i = 0; i < m_Keys.GetCount(); i++)
		m_Keys[i].iCmdBinding = -1;

	//  Load commands. NOTE: These do not change from session to session, so 
	//	they should match when we initialized.

	TArray<CGameKeys::SCommandKeyDesc> Commands;
	m_Settings.GetKeyMap().GetCommands(Commands, m_Settings.GetBoolean(CGameSettings::debugMode));
	ASSERT(Commands.GetCount() == m_Commands.GetCount());

	for (int i = 0; i < Commands.GetCount(); i++)
		{
		m_Commands[i].sKeyBinding = NULL_STR;
		m_Commands[i].iKeyBinding = -1;

		//	See if we have a binding to one of the keys on this device.

		for (int j = 0; j < Commands[i].Keys.GetCount(); j++)
			{
			const CGameKeys::SBindingDesc &Binding = Commands[i].Keys[j];
			int *pKey = m_KeyIDToIndex.GetAt(Binding.sKeyID);
			if (pKey)
				{
				m_Commands[i].sKeyBinding = Binding.sKeyID;
				m_Commands[i].iKeyBinding = *pKey;
				m_Keys[*pKey].iCmdBinding = i;
				break;
				}
			}

		//	If we have a binding, but not to a key in this device, init the
		//	binding ID so that we know that this command is not completely
		//	unbound.

		if (m_Commands[i].sKeyBinding.IsBlank() && Commands[i].Keys.GetCount() > 0)
			m_Commands[i].sKeyBinding = Commands[i].Keys[0].sKeyID;
		}

	//  Figure out where each command label should go

	RECT rcKeyboard;
	rcKeyboard.left = m_xKeyboard;
	rcKeyboard.top = m_yKeyboard;
	rcKeyboard.right = m_xKeyboard + m_cxKeyboard;
	rcKeyboard.bottom = m_yKeyboard + m_cyKeyboard;

	int cxBoundsSpacing = Max(0, (int)Min(m_rcRect.right - rcKeyboard.right, rcKeyboard.left - m_rcRect.left) - (LABEL_COLUMN_SPACING * m_cxKeyCol));
	RECT rcBounds = m_rcRect;
	rcBounds.top += MAJOR_PADDING_TOP + VI.GetFont(fontHeader).GetHeight();
	rcBounds.left = m_rcRect.left + cxBoundsSpacing;
	rcBounds.right = m_rcRect.right - cxBoundsSpacing;
	rcBounds.bottom -= MAJOR_PADDING_TOP + VI.GetFont(fontHeader).GetHeight();
	ArrangeCommandLabels(rcBounds, rcKeyboard);
	}

void CKeyboardMapSession::InitCommands (void)

//	InitCommands
//
//	Initializes the list of commands.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &LabelFont = VI.GetFont(fontMedium);

	//  Load commands

	TArray<CGameKeys::SCommandKeyDesc> Commands;
	m_Settings.GetKeyMap().GetCommands(Commands, m_Settings.GetBoolean(CGameSettings::debugMode));

	m_Commands.DeleteAll();
	m_Commands.GrowToFit(Commands.GetCount());
	for (int i = 0; i < Commands.GetCount(); i++)
		{
		int iCmdIndex = m_Commands.GetCount();
		SCommandDesc *pNewCmd = m_Commands.Insert();
		pNewCmd->iCmd = Commands[i].iCmd;
		pNewCmd->sLabel = Commands[i].sCmdLabel;
		pNewCmd->cxLabel = LabelFont.MeasureText(pNewCmd->sLabel);
		}
	}

void CKeyboardMapSession::InitDeviceLayout (const SDeviceData &Device)

//	InitDeviceLayout
//
//	Initializes the given device. NOTE: We assume that m_cxKeyCol and m_cyKeyRow
//	are initialized.

	{
	//	Center the keyboard on the screen

	m_cxKeyboard = (Device.iCols * m_cxKeyCol) - KEY_SPACING;
	m_cyKeyboard = (Device.iRows * m_cyKeyRow) - KEY_SPACING;

	m_xKeyboard = m_rcRect.left + (RectWidth(m_rcRect) - m_cxKeyboard) / 2 + Device.xOffset;
	m_yKeyboard = m_rcRect.top + (RectHeight(m_rcRect) - m_cyKeyboard) / 2 + Device.yOffset;

	//  Initialize all the keys metrics

	m_Keys.DeleteAll();
	m_KeyIDToIndex.DeleteAll();

	m_Keys.InsertEmpty(Device.iKeyCount);
	m_KeyIDToIndex.GrowToFit(Device.iKeyCount);
	for (int i = 0; i < m_Keys.GetCount(); i++)
		{
		if (Device.pKeys[i].pszKeyID)
			{
			m_Keys[i].sKeyID = CString(Device.pKeys[i].pszKeyID);
			m_Keys[i].dwVirtKey = CVirtualKeyData::GetKey(m_Keys[i].sKeyID);
			}
		else
			m_Keys[i].dwVirtKey = CVirtualKeyData::INVALID_VIRT_KEY;

		if (Device.pKeys[i].pszLabel)
			m_Keys[i].sLabel = CString(Device.pKeys[i].pszLabel);

		m_Keys[i].iSymbol = Device.pKeys[i].iSymbol;

		m_Keys[i].rcRect.left = m_xKeyboard + m_cxKeyCol * Device.pKeys[i].xCol;
		m_Keys[i].rcRect.top = m_yKeyboard + m_cyKeyRow * Device.pKeys[i].yRow;
		m_Keys[i].rcRect.right = m_Keys[i].rcRect.left + (m_cxKeyCol * Device.pKeys[i].cxWidth) - KEY_SPACING;
		m_Keys[i].rcRect.bottom = m_Keys[i].rcRect.top + (m_cyKeyRow * Device.pKeys[i].cyHeight) - KEY_SPACING;

		m_Keys[i].iCmdBinding = -1;
		m_Keys[i].dwFlags = Device.pKeys[i].dwFlags;

		//  Add this key ID to the index

		if (!m_Keys[i].sKeyID.IsBlank())
			m_KeyIDToIndex.Insert(m_Keys[i].sKeyID, i);
		}
	}

void CKeyboardMapSession::InitKeys (void)

//	InitKeys
//
//	Initialize m_Keys and m_KeyIDToIndex arrays.

	{
	//	We combine the three devices into a single one. First we compute the 
	//	total size of the combined device.

	SDeviceData CombinedDevice;
	for (int i = 0; i < DEVICE_DATA_COUNT; i++)
		{
		const SDeviceData &Device = DEVICE_DATA[i];

		CombinedDevice.iCols = Max(CombinedDevice.iCols, Device.iColOffset + Device.iCols);
		CombinedDevice.iRows = Max(CombinedDevice.iRows, Device.iRowOffset + Device.iRows);

		CombinedDevice.iKeyCount += Device.iKeyCount;
		}

	TArray<SKeyData> AllKeys;
	AllKeys.GrowToFit(CombinedDevice.iKeyCount);

	for (int i = 0; i < DEVICE_DATA_COUNT; i++)
		{
		const SDeviceData &Device = DEVICE_DATA[i];

		for (int j = 0; j < Device.iKeyCount; j++)
			{
			SKeyData *pNewKey = AllKeys.Insert();
			*pNewKey = Device.pKeys[j];

			pNewKey->xCol += Device.iColOffset;
			pNewKey->yRow += Device.iRowOffset;
			}
		}

	CombinedDevice.pKeys = &AllKeys[0];

	//	Initialize

	InitDeviceLayout(CombinedDevice);
	}

bool CKeyboardMapSession::IsCommandRequired (CGameKeys::Keys iCmd) const

//	IsCommandRequired
//
//	Returns TRUE if this command needs to be bound, given the other command
//	bindings.

	{
	const CGameKeys &KeyMap = m_Settings.GetKeyMap();

	switch (iCmd)
		{
		//	AimShip is required if we don't already have at least one of the
		//	turning keys mapped.

		case CGameKeys::keyAimShip:
			return (!KeyMap.IsKeyBound(CGameKeys::keyRotateLeft) && !KeyMap.IsKeyBound(CGameKeys::keyRotateRight));

		//	Rotate left is required if rotate right is bound (and vice versa).

		case CGameKeys::keyRotateLeft:
			return KeyMap.IsKeyBound(CGameKeys::keyRotateRight);

		case CGameKeys::keyRotateRight:
			return KeyMap.IsKeyBound(CGameKeys::keyRotateLeft);

		//	Separate dock and gate commands not required if we have interact.

		case CGameKeys::keyDock:
		case CGameKeys::keyEnterGate:
			return !KeyMap.IsKeyBound(CGameKeys::keyInteract);

		//	Interact is not required if we have either dock or gate

		case CGameKeys::keyInteract:
			return (!KeyMap.IsKeyBound(CGameKeys::keyDock) && !KeyMap.IsKeyBound(CGameKeys::keyEnterGate));

		//	Squadron command is not required (since we can access via comms)

		case CGameKeys::keySquadronCommands:
			return false;

		//	Clear target is not required if cycle target is bound (and vice 
		//	versa).

		case CGameKeys::keyCycleTarget:
			return !KeyMap.IsKeyBound(CGameKeys::keyClearTarget);

		case CGameKeys::keyClearTarget:
			return !KeyMap.IsKeyBound(CGameKeys::keyCycleTarget);

		//	Most commands need to be bound.

		default:
			return true;
		}
	}

ALERROR CKeyboardMapSession::OnCommand (const CString &sCmd, void *pData)

//  OnCommand
//
//  Handle commands

	{
	if (strEquals(sCmd, CMD_CLOSE_SESSION))
		{
		CmdRevert();
		CloseSession();
		}
	else if (strEquals(sCmd, CMD_OK_SESSION))
		CloseSession();
	else if (strEquals(sCmd, CMD_RESET_DEFAULT))
		CmdResetDefault(CGameKeys::layoutDefault);
	else if (strEquals(sCmd, CMD_RESET_WASD))
		CmdResetDefault(CGameKeys::layoutWASD);
	else if (strEquals(sCmd, CMD_CLEAR_BINDING))
		CmdClearBinding();
	else if (strEquals(sCmd, CMD_SET_COMMAND))
		CmdSetCommand();

	return NOERROR;
	}

ALERROR CKeyboardMapSession::OnInit (CString *retsError)

//  OnInit
//
//  Initialize the session.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	VI.GetWidescreenRect(&m_rcRect);

	//	Remember the original key mapping, in case we need to revert

	m_SavedKeyMap = m_Settings.GetKeyMap();

	//	Load the commands. The set of commands don't change during the session.

	InitCommands();

	//  Keyboard metrics

	m_cyKeyRow = AlignDown((KEYBOARD_HEIGHT + KEY_SPACING) / KEYBOARD_ROWS, 2);
	m_cxKeyCol = m_cyKeyRow / 2;

	//	Initialize device

	InitKeys();

	//	Initialize bindings between keys and commands

	InitBindings();
	m_iSelectedCommand = -1;
	m_iSelectedKey = -1;

	//	Set up a menu

	TArray<CUIHelper::SMenuEntry> Menu;
	CUIHelper::SMenuEntry *pEntry = Menu.Insert();
	pEntry->sCommand = CMD_RESET_DEFAULT;
	pEntry->sLabel = CONSTLIT("Reset to default layout");

	pEntry = Menu.Insert();
	pEntry->sCommand = CMD_RESET_WASD;
	pEntry->sLabel = CONSTLIT("Reset to WASD layout");

	pEntry = Menu.Insert();
	pEntry->sCommand = CMD_SET_COMMAND;
	pEntry->sLabel = CONSTLIT("Bind key to command...");

	pEntry = Menu.Insert();
	pEntry->sCommand = CMD_CLEAR_BINDING;
	pEntry->sLabel = CONSTLIT("Clear key binding");

	//  Create the background and title

	CUIHelper Helper(m_HI);
	IAnimatron *pTitle;
	DWORD dwOptions = CUIHelper::OPTION_SESSION_OK_BUTTON;
	Helper.CreateSessionTitle(this, m_Service, CONSTLIT("Keyboard Settings"), Menu, dwOptions, &pTitle);
	StartPerformance(pTitle, ID_CTRL_TITLE, CReanimator::SPR_FLAG_DELETE_WHEN_DONE);

	UpdateMenu();

	return NOERROR;
	}

void CKeyboardMapSession::OnKeyDown (int iVirtKey, DWORD dwKeyData)

//  OnKeyDown
//
//  Handle keys

	{
	//	Flash the key

#ifdef DEBUG_KEYMAP
	DWORD dwTVirtKey = CGameKeys::TranslateVirtKey(iVirtKey, dwKeyData);
	for (int i = 0; i < m_Keys.GetCount(); i++)
		if (m_Keys[i].dwVirtKey == dwTVirtKey)
			{
			m_iFlashKey = i;
			m_iFlashUntil = m_iTick + 150;
			}
#endif

	//	Command

	switch (iVirtKey)
		{
		case VK_ESCAPE:
			if (m_iMode != modeNormal)
				CmdSetCommand();
			else
				{
				CmdRevert();
				CloseSession();
				}
			break;

		case VK_RETURN:
			CloseSession();
			break;
		}
	}

void CKeyboardMapSession::OnLButtonDown (int x, int y, DWORD dwFlags, bool *retbCapture)

//  OnLButtonDown
//
//  Handle mouse click

	{
	//	Figure out where we clicked.

	STargetCtx HitResult;
	bool bHit = HitTest(x, y, HitResult);

	//	Depends on mode

	switch (m_iMode)
		{
		case modeSetCommand:

			//	If we clicked away, then we cancel setting the command.

			if (!bHit || m_iSelectedKey == -1)
				{
				m_iMode = modeNormal;
				m_iSelectedCommand = -1;
				m_iSelectedKey = -1;
				}

			//	If we clicked on a command then we assign the key to the command.

			else if (!HitResult.bInKey && HitResult.iCmdIndex != -1)
				{
				const SCommandDesc &NewCommand = m_Commands[HitResult.iCmdIndex];

				//  If this is a new binding, set it

				if ((m_iSelectedCommand == -1 || m_Commands[m_iSelectedCommand].iCmd != NewCommand.iCmd)
						&& CanBindKey(m_iSelectedKey, NewCommand.iCmd))
					{
					CGameKeys::Keys iCmd = NewCommand.iCmd;
					m_Settings.GetKeyMap().SetGameKey(m_Keys[m_iSelectedKey].sKeyID, iCmd);

					//  Clear the old key

					if (NewCommand.iKeyBinding != -1)
						m_Settings.GetKeyMap().SetGameKey(m_Keys[NewCommand.iKeyBinding].sKeyID, CGameKeys::keyNone);

					//  Reload mappings

					InitBindings();

					//	Return to normal

					m_iMode = modeNormal;
					m_iSelectedCommand = HitResult.iCmdIndex;
					}
				}

			//	If we clicked on a different key, then we cancel.

			else
				{
				m_iMode = modeNormal;
				m_iSelectedCommand = HitResult.iCmdIndex;
				m_iSelectedKey = HitResult.iKeyIndex;
				}

			break;

		case modeSetKey:

			//	If we clicked away, then we cancel setting a key.

			if (!bHit || m_iSelectedCommand == -1)
				{
				m_iMode = modeNormal;
				m_iSelectedCommand = -1;
				m_iSelectedKey = -1;
				}

			//	If we clicked on a key then we assign the key to the command.

			else if (HitResult.bInKey)
				{
				const SCommandDesc &Command = m_Commands[m_iSelectedCommand];

				//  If this is a new binding, set it

				if (HitResult.iKeyIndex != -1
						&& HitResult.iKeyIndex != Command.iKeyBinding
						&& CanBindKey(HitResult.iKeyIndex, Command.iCmd))
					{
					CGameKeys::Keys iCmd = Command.iCmd;
					m_Settings.GetKeyMap().SetGameKey(m_Keys[HitResult.iKeyIndex].sKeyID, Command.iCmd);

					//  Clear the old key

					if (Command.iKeyBinding != -1)
						m_Settings.GetKeyMap().SetGameKey(m_Keys[Command.iKeyBinding].sKeyID, CGameKeys::keyNone);

					//  Reload mappings

					InitBindings();

					//	Return to normal

					m_iMode = modeNormal;
					m_iSelectedKey = HitResult.iKeyIndex;
					}
				}

			//	If we clicked on a different command, then we cancel.

			else
				{
				m_iMode = modeNormal;
				m_iSelectedCommand = HitResult.iCmdIndex;
				m_iSelectedKey = HitResult.iKeyIndex;
				}

			break;

		default:
			if (bHit)
				{
				m_iSelectedCommand = HitResult.iCmdIndex;
				m_iSelectedKey = HitResult.iKeyIndex;
				}
			else
				{
				m_iSelectedKey = -1;
				m_iSelectedCommand = -1;
				}
			break;
		}

	UpdateMenu();
	}

void CKeyboardMapSession::OnLButtonUp (int x, int y, DWORD dwFlags)

//  OnLButtonUp
//
//  Handle mouse click

	{
	}

void CKeyboardMapSession::OnMouseMove (int x, int y, DWORD dwFlags)

//  OnMouseMove
//
//  Handle mouse move

	{
	STargetCtx HitResult;
	bool bHit = HitTest(x, y, HitResult);

	switch (m_iMode)
		{
		case modeSetCommand:
			if (bHit 
					&& HitResult.iCmdIndex != -1
					&& CanBindKey(m_iSelectedKey, m_Commands[HitResult.iCmdIndex].iCmd))
				{
				m_iHoverCommand = HitResult.iCmdIndex;
				m_iHoverKey = -1;
				}
			else
				{
				m_iHoverCommand = -1;
				m_iHoverKey = -1;
				}
			break;

		case modeSetKey:
			if (bHit 
					&& HitResult.iKeyIndex != -1
					&& CanBindKey(HitResult.iKeyIndex, m_Commands[m_iSelectedCommand].iCmd))
				{
				m_iHoverCommand = HitResult.iCmdIndex;
				m_iHoverKey = (m_iHoverCommand == -1 ? HitResult.iKeyIndex : -1);
				}
			else
				{
				m_iHoverCommand = -1;
				m_iHoverKey = -1;
				}
			break;

		default:
			if (bHit)
				{
				m_iHoverCommand = HitResult.iCmdIndex;
				m_iHoverKey = (m_iHoverCommand == -1 ? HitResult.iKeyIndex : -1);
				}
			else
				{
				m_iHoverCommand = -1;
				m_iHoverKey = -1;
				}
			break;
		}
	}

void CKeyboardMapSession::OnPaint (CG32bitImage &Screen, const RECT &rcInvalid)

//  OnPaint
//
//  Paint session

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &LabelFont = VI.GetFont(fontMedium);
	const CG16bitFont &HeaderFont = VI.GetFont(fontHeader);
	bool bEditable = true;

	CG32bitPixel rgbBoundKey = CG32bitPixel::Darken(VI.GetColor(colorTextHighlight), 220);
	CG32bitPixel rgbUnboundKey = CG32bitPixel::Darken(VI.GetColor(colorTextNormal), 128);
	CG32bitPixel rgbText = VI.GetColor(colorAreaDialog);

	CG32bitPixel rgbHoverColor;
	CG32bitPixel rgbSelected;
	switch (m_iMode)
		{
		case modeSetCommand:
		case modeSetKey:
			rgbHoverColor = VI.GetColor(colorTextQuote);
			rgbSelected = VI.GetColor(colorAreaDialogHighlight);
			break;

		default:
			rgbHoverColor = VI.GetColor(colorTextQuote);
			rgbSelected = VI.GetColor(colorTextAltHighlight);
			break;
		}

	RECT rcCenter;
	VI.DrawSessionBackground(Screen, CG32bitImage(), CG32bitPixel(), CVisualPalette::OPTION_SESSION_DLG_BACKGROUND, &rcCenter);

	//  Paint all keys

	for (int i = 0; i < m_Keys.GetCount(); i++)
		{
		const SKeyDesc &Key = m_Keys[i];

		//  Skip if this key is the hover key (we paint it last)

		if ((m_iHoverCommand != -1 && m_iHoverCommand == Key.iCmdBinding)
				|| (m_iSelectedCommand != -1 && m_iSelectedCommand == Key.iCmdBinding)
				|| (m_iHoverKey == i)
				|| (m_iSelectedKey == i))
			continue;

		//  Draw the key

		PaintKeyBackground(Screen, Key, (Key.iCmdBinding != -1 ? rgbBoundKey : rgbUnboundKey), i == m_iFlashKey);
		}

	//  Paint all the commands

	for (int i = 0; i < m_Commands.GetCount(); i++)
		{
		const SCommandDesc &Command = m_Commands[i];

		//  Set up colors

		CG32bitPixel rgbBack;
		if (i == m_iSelectedCommand)
			rgbBack = rgbSelected;
		else if (i == m_iHoverCommand)
			rgbBack = rgbHoverColor;
		else if (bEditable && Command.sKeyBinding.IsBlank() && IsCommandRequired(Command.iCmd))
			rgbBack = VI.GetColor(colorTextDockWarning);
		else
			rgbBack = rgbBoundKey;

		CG32bitPixel rgbLine = (bEditable ? VI.GetColor(colorTextHighlight) : VI.GetColor(colorTextNormal));
		CG32bitPixel rgbLabel = VI.GetColor(colorAreaDialog);

		//  Draw command box and label

		CGDraw::RoundedRect(Screen, Command.rcRect.left, Command.rcRect.top, RectWidth(Command.rcRect), RectHeight(Command.rcRect), KEY_CORNER_RADIUS, rgbBack);
		LabelFont.DrawText(Screen, Command.rcRect, rgbLabel, Command.sLabel, 0, CG16bitFont::AlignCenter | CG16bitFont::AlignMiddle);

		//  Paint lines connecting the command

		if (m_iHoverCommand != i)
			{
			for (int j = 0; j < Command.Line.GetCount() - 1; j++)
				Screen.DrawLine(Command.Line[j].x, Command.Line[j].y, Command.Line[j + 1].x, Command.Line[j + 1].y, LINE_WIDTH, rgbLine);
			}
		}

	//  Paint the hover line last

	if (m_iSelectedCommand != -1)
		{
		const SCommandDesc &Command = m_Commands[m_iSelectedCommand];

		if (Command.iKeyBinding != -1)
			PaintKey(Screen, m_Keys[Command.iKeyBinding], rgbSelected, rgbText, Command.iKeyBinding == m_iFlashKey);

		for (int j = 0; j < Command.Line.GetCount() - 1; j++)
			Screen.DrawLine(Command.Line[j].x, Command.Line[j].y, Command.Line[j + 1].x, Command.Line[j + 1].y, LINE_WIDTH, rgbSelected);
		}

	if (m_iSelectedKey != -1)
		{
		const SKeyDesc &Key = m_Keys[m_iSelectedKey];
		PaintKey(Screen, m_Keys[m_iSelectedKey], rgbSelected, rgbText, m_iSelectedKey == m_iFlashKey);
		}

	if (m_iHoverKey != -1 && m_iSelectedKey != m_iHoverKey)
		{
		const SKeyDesc &Key = m_Keys[m_iHoverKey];
		PaintKey(Screen, m_Keys[m_iHoverKey], rgbHoverColor, rgbText, m_iHoverKey == m_iFlashKey);
		}

	if (m_iHoverCommand != -1 && m_iSelectedCommand != m_iHoverCommand)
		{
		const SCommandDesc &Command = m_Commands[m_iHoverCommand];

		if (Command.iKeyBinding != -1)
			PaintKey(Screen, m_Keys[Command.iKeyBinding], rgbHoverColor, rgbText, Command.iKeyBinding == m_iFlashKey);

		for (int j = 0; j < Command.Line.GetCount() - 1; j++)
			Screen.DrawLine(Command.Line[j].x, Command.Line[j].y, Command.Line[j + 1].x, Command.Line[j + 1].y, LINE_WIDTH, rgbHoverColor);
		}

	//	Paint key labels so that they go on top of any lines.

	for (int i = 0; i < m_Keys.GetCount(); i++)
		{
		const SKeyDesc &Key = m_Keys[i];

		//  Skip if this key is the hover key (we paint it last)

		if ((m_iHoverCommand != -1 && m_iHoverCommand == Key.iCmdBinding)
				|| (m_iSelectedCommand != -1 && m_iSelectedCommand == Key.iCmdBinding)
				|| (m_iHoverKey != -1 && m_iHoverKey == i))
			continue;

		//  Draw the key

		PaintKeyLabel(Screen, Key, rgbText);
		}

	//	Paint the layout name

	RECT rcLayoutName = m_rcRect;
	rcLayoutName.top += MAJOR_PADDING_TOP;
	rcLayoutName.left = m_rcRect.left + (RectWidth(m_rcRect) - MODE_HELP_WIDTH) / 2;
	rcLayoutName.right = rcLayoutName.left + MODE_HELP_WIDTH;

	HeaderFont.DrawText(Screen, rcLayoutName, VI.GetColor(colorTextNormal), CGameKeys::GetLayoutName(m_Settings.GetKeyMap().GetLayout()), 0, CG16bitFont::AlignCenter);

	//	Paint mode help

	RECT rcModeHelp = m_rcRect;
	rcModeHelp.top = m_rcRect.bottom - (MAJOR_PADDING_TOP + HeaderFont.GetHeight());
	rcModeHelp.left = m_rcRect.left + (RectWidth(m_rcRect) - MODE_HELP_WIDTH) / 2;
	rcModeHelp.right = rcModeHelp.left + MODE_HELP_WIDTH;

	switch (m_iMode)
		{
		case modeSetCommand:
			HeaderFont.DrawText(Screen, rcModeHelp, rgbSelected, CONSTLIT("Click on a command to bind it to the key"), 0, CG16bitFont::AlignCenter);
			break;

		case modeSetKey:
			HeaderFont.DrawText(Screen, rcModeHelp, rgbSelected, CONSTLIT("Click on a key to bind it to the command"), 0, CG16bitFont::AlignCenter);
			break;
		}
	}

void CKeyboardMapSession::OnReportHardCrash (CString *retsMessage)

//  OnReportHardCrash
//
//  Report a crash

	{
	*retsMessage = CONSTLIT("session: CKeyboardMapSession\r\n");
	}

void CKeyboardMapSession::OnUpdate (bool bTopMost)

//	OnUpdate
//
//	Update once per frame

	{
	m_iTick++;
	}

void CKeyboardMapSession::PaintKey (CG32bitImage &Screen, const SKeyDesc &Key, CG32bitPixel rgbBack, CG32bitPixel rgbText, bool bFlash) const

//	PaintKey
//
//	Paint key and label.

	{
	PaintKeyBackground(Screen, Key, rgbBack, bFlash);
	PaintKeyLabel(Screen, Key, rgbText);
	}

void CKeyboardMapSession::PaintKeyBackground (CG32bitImage &Screen, const SKeyDesc &Key, CG32bitPixel rgbBack, bool bFlash) const

//  PaintKeyBackground
//
//  Paint a key

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	//	Flash?

	if (bFlash && m_iTick < m_iFlashUntil && (m_iTick % 2) == 0)
		rgbBack = CG32bitPixel(0, 0, 0);

	//  Draw the key

	CGDraw::RoundedRect(Screen, Key.rcRect.left, Key.rcRect.top, RectWidth(Key.rcRect), RectHeight(Key.rcRect), KEY_CORNER_RADIUS, rgbBack);
	}

void CKeyboardMapSession::PaintKeyLabel (CG32bitImage &Screen, const SKeyDesc &Key, CG32bitPixel rgbText) const

//	PaintKeyLabel
//
//	Paints the label for the given key.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &KeyFont = VI.GetFont(fontMediumHeavyBold);
	const CG16bitFont &SmallKeyFont = VI.GetFont(fontSmall);

	//  Draw the key label

	if (Key.iSymbol != symbolNone)
		{
		const int INSET = 8;

		int yTop = Key.rcRect.top + INSET;
		int xLeft = Key.rcRect.left + INSET;
		int xRight = Key.rcRect.right - INSET;
		int yBottom = Key.rcRect.bottom - INSET;
		int cxWidth = xRight - xLeft;
		int cyHeight = yBottom - yTop;
		int cxHalfWidth = cxWidth / 2;
		int cyHalfHeight = cyHeight / 2;
		int cxQuarterWidth = cxHalfWidth / 2;
		int cyQuarterHeight = cyHalfHeight / 2;

		switch (Key.iSymbol)
			{
			case symbolArrowUp:
				CGDraw::TriangleCorner(Screen, xLeft + cxHalfWidth, yTop, 2, cyHalfHeight, rgbText);
				Screen.Fill(xLeft + cxQuarterWidth + 1, yTop + cyHalfHeight, cxHalfWidth - 1, cyHalfHeight, rgbText);
				break;

			case symbolArrowDown:
				CGDraw::TriangleCorner(Screen, xLeft + cxHalfWidth, yBottom - 1, 6, cyHalfHeight, rgbText);
				Screen.Fill(xLeft + cxQuarterWidth + 1, yTop, cxHalfWidth - 1, cyHalfHeight, rgbText);
				break;

			case symbolArrowLeft:
				CGDraw::TriangleCorner(Screen, xLeft, yTop + cyHalfHeight, 4, cxHalfWidth, rgbText);
				Screen.Fill(xLeft + cxHalfWidth, yTop + cyQuarterHeight + 1, cxHalfWidth, cyHalfHeight - 1, rgbText);
				break;

			case symbolArrowRight:
				CGDraw::TriangleCorner(Screen, xRight - 1, yTop + cyHalfHeight, 0, cxHalfWidth, rgbText);
				Screen.Fill(xLeft, yTop + cyQuarterHeight + 1, cxHalfWidth, cyHalfHeight - 1, rgbText);
				break;
			}
		}
	else if (Key.sLabel.GetLength() == 1)
		KeyFont.DrawText(Screen, Key.rcRect, rgbText, Key.sLabel, 0, CG16bitFont::AlignCenter | CG16bitFont::AlignMiddle);
	else
		SmallKeyFont.DrawText(Screen, Key.rcRect, rgbText, Key.sLabel, 0, CG16bitFont::AlignCenter | CG16bitFont::AlignMiddle);
	}

void CKeyboardMapSession::UpdateMenu (void)

//	UpdateMenu
//
//	Enables/disables menu items.

	{
	IAnimatron *pItem = GetElement(CMD_RESET_DEFAULT);
	if (pItem)
		pItem->SetPropertyBool(PROP_ENABLED, m_Settings.GetKeyMap().GetLayout() != CGameKeys::layoutDefault);

	pItem = GetElement(CMD_RESET_WASD);
	if (pItem)
		pItem->SetPropertyBool(PROP_ENABLED, m_Settings.GetKeyMap().GetLayout() != CGameKeys::layoutWASD);

	pItem = GetElement(CMD_SET_COMMAND);
	if (pItem)
		{
		if (m_iMode != modeNormal)
			{
			pItem->SetPropertyString(PROP_TEXT, CONSTLIT("Cancel"));
			pItem->SetPropertyBool(PROP_ENABLED, true);
			}
		else if (m_iSelectedKey != -1)
			{
			pItem->SetPropertyString(PROP_TEXT, CONSTLIT("Bind key to..."));
			pItem->SetPropertyBool(PROP_ENABLED, m_iMode != modeSetCommand);
			}
		else if (m_iSelectedCommand != -1)
			{
			pItem->SetPropertyString(PROP_TEXT, CONSTLIT("Bind command to..."));
			pItem->SetPropertyBool(PROP_ENABLED, m_iMode != modeSetKey);
			}
		else
			{
			pItem->SetPropertyString(PROP_TEXT, CONSTLIT("Bind key"));
			pItem->SetPropertyBool(PROP_ENABLED, false);
			}
		}

	pItem = GetElement(CMD_CLEAR_BINDING);
	if (pItem)
		pItem->SetPropertyBool(PROP_ENABLED, (m_iSelectedCommand != -1 && m_Commands[m_iSelectedCommand].iKeyBinding != -1));
	}
