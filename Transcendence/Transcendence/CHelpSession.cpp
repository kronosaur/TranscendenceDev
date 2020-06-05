//	CHelpSession.cpp
//
//	CHelpSession class

#include "PreComp.h"
#include "Transcendence.h"

const int HELP_PAGE_KEYS =									0;
const int HELP_PAGE_DAMAGE_TYPES =							1;
const int HELP_PAGE_COUNT =									1;

const CHelpSession::SHelpLine CHelpSession::m_HelpDesc[] =
	{
		{	CGameKeys::keyAimShip,				CGameKeys::keyNone,					0,	"Maneuver", },
		{	CGameKeys::keyThrustForward,		CGameKeys::keyStop,					0,	"Thrust forward/stop", },
		{	CGameKeys::keyFireWeapon,			CGameKeys::keyNone,					0,	"Fire primary weapon system",					},
		{	CGameKeys::keyFireMissile,			CGameKeys::keyNone,					0,	"Fire missile launcher (if installed)",			},
		{	CGameKeys::keyShipStatus,			CGameKeys::keyNone,					0,	"Ship's status (refuel, view cargo, jettison)",	},
		{	CGameKeys::keyShowMap,				CGameKeys::keyNone,					0,	"Show map of entire system",					},
		{	CGameKeys::keyAutopilot,			CGameKeys::keyNone,					0,	"Accelerate time",								},
		{	CGameKeys::keyInteract,				CGameKeys::keyNone,					0,	"Dock or enter stargate",						},
		{	CGameKeys::keyDock,					CGameKeys::keyNone,					0,	"Dock with nearest station",					},
		{	CGameKeys::keyEnterGate,			CGameKeys::keyNone,					0,	"Enter stargate",								},
		{	CGameKeys::keyUseItem,				CGameKeys::keyNone,					0,	"Use item",										},
		{	CGameKeys::keyNextWeapon,			CGameKeys::keyNone,					0,	"Select primary weapon to use (if multiple)",	},
		{	CGameKeys::keyNextMissile,			CGameKeys::keyNone,					0,	"Select missile to launch (if installed)",		},
		{	CGameKeys::keyCycleTarget,			CGameKeys::keyNone,					0,	"Target next enemy/next friendly",	},
		{	CGameKeys::keyTargetNextEnemy,		CGameKeys::keyTargetNextFriendly,	0,	"Target next enemy/next friendly",	},
		{	CGameKeys::keyClearTarget,			CGameKeys::keyNone,					0,	"Clear target",		},
		{	CGameKeys::keyInvokePower,			CGameKeys::keyNone,					0,	"Invoke special powers",						},
		{	CGameKeys::keyCommunications,		CGameKeys::keySquadronCommands,		0,	"Communications",								},
		{	CGameKeys::keyEnableDevice,			CGameKeys::keyNone,					0,	"Enable/disable devices",						},
		{	CGameKeys::keyShowGalacticMap,		CGameKeys::keyNone,					0,	"Show stargate network (if galactic map installed)",	},
		{	CGameKeys::keyPause,				CGameKeys::keyNone,					0,	"Pauses the game",								},
		{	CGameKeys::keyVolumeUp,				CGameKeys::keyVolumeDown,			0,	"Increase/decrease volume",						},
		{	CGameKeys::keyNone,					CGameKeys::keyNone,					VK_ESCAPE,		"Game menu (save, self-destruct)", }
	};

const int CHelpSession::HELP_LINE_COUNT =			(sizeof(m_HelpDesc) / sizeof(m_HelpDesc[0]));

#define RGB_FRAME							CG32bitPixel(80,80,80)
#define RGB_HELP_TEXT						CG32bitPixel(231,255,227)
#define RGB_BAR_COLOR						CG32bitPixel(0, 2, 10)
const CG32bitPixel RGB_SUBTITLE_TEXT =		CG32bitPixel(128,128,128);
const CG32bitPixel RGB_MORE_HELP_TEXT =		CG32bitPixel(160,160,160);
const CG32bitPixel RGB_IMAGE_BACKGROUND =	CG32bitPixel(35, 41, 37);

const int SECTION_SPACING_Y =				16;

const int KEY_INDENT =						70;
const int KEY_DESC_COLUMN_WIDTH =			200;
const int KEY_COLUMN_SPACING_X =			10;
const int KEY_DESC_SPACING_Y =				5;
const int KEY_DESC_SPACING_X =				12;

const int DAMAGE_TYPE_INDENT =				24;
const int DAMAGE_TYPE_SPACING_Y =			4;

void CHelpSession::OnCleanUp (void)

//	OnCleanUp
//
//	Clean up

	{
	}

ALERROR CHelpSession::OnInit (CString *retsError)

//	OnInit
//
//	Initialize

	{
	ALERROR error;

	//	Load a JPEG of the background image

	HBITMAP hDIB;
	if (error = JPEGLoadFromResource(NULL,
			MAKEINTRESOURCE(IDR_HELP_BACKGROUND),
			JPEG_LFR_DIB, 
			NULL, 
			&hDIB))
		return error;

	bool bSuccess = m_HelpImage.CreateFromBitmap(hDIB);
	::DeleteObject(hDIB);
	if (!bSuccess)
		return ERR_FAIL;

	//	Done

	m_iHelpPage = HELP_PAGE_KEYS;

	return NOERROR;
	}

void CHelpSession::OnKeyDown (int iVirtKey, DWORD dwKeyData)

//	OnKeyDown
//
//	KeyDown

	{
	//	Done

	m_HI.ClosePopupSession();
	}

void CHelpSession::OnLButtonDown (int x, int y, DWORD dwFlags, bool *retbCapture)

//	OnLButtonDown
//
//	LButtonDown

	{
	//	Done

	m_HI.ClosePopupSession();
	}

void CHelpSession::OnPaint (CG32bitImage &Screen, const RECT &rcInvalid)

//	OnPaint
//
//	Paint

	{
	int i;
	const CVisualPalette &VI = m_HI.GetVisuals();

	RECT rcCenter;
	VI.DrawSessionBackground(Screen, m_HelpImage, RGB_IMAGE_BACKGROUND, 0, &rcCenter);

	//	Paint help page

	RECT rcPage;
	rcPage.left = rcCenter.left + (4 * RectWidth(rcCenter) / 9);
	rcPage.top = rcCenter.top + VI.GetFont(fontLargeBold).GetHeight();
	rcPage.right = rcCenter.right - VI.GetFont(fontLargeBold).GetHeight();
	rcPage.bottom = rcCenter.bottom - VI.GetFont(fontLargeBold).GetHeight();

	//	Paint help page

	switch (m_iHelpPage)
		{
		case HELP_PAGE_KEYS:
			{
			int x = rcPage.left;
			int y = rcPage.top;

			//	Paint keys

			Screen.DrawText(x, y, VI.GetFont(fontHeaderBold), RGB_SUBTITLE_TEXT, CONSTLIT("Keys"));
			y += VI.GetFont(fontHeaderBold).GetHeight();

			RECT rcControls = rcPage;
			rcControls.right = rcControls.left + PANE_WIDTH;
			rcControls.top = y;
			rcControls.bottom = rcControls.top + PANE_HEIGHT;

			int cyPane;
			PaintControls(Screen, rcControls, &cyPane);

			//	Paint damage types

			y += cyPane + INNER_PADDING_VERT;

			//	Paint damage types

			int cxColWidth = 20 * VI.GetFont(fontMediumBold).GetAverageWidth();

			Screen.DrawText(x, y, VI.GetFont(fontHeaderBold), RGB_SUBTITLE_TEXT, CONSTLIT("Damage Types"));
			y += VI.GetFont(fontHeaderBold).GetHeight();

			int yCol = y;

			for (i = 0; i < damageCount; i++)
				{
				VI.DrawDamageTypeIcon(Screen, x, y, (DamageTypes)i);
				
				Screen.DrawText(x + DAMAGE_TYPE_INDENT, 
						y, 
						VI.GetFont(fontMedium), 
						RGB_HELP_TEXT, 
						strCapitalize(::GetDamageName((DamageTypes)i)));

				//	Increment

				if (((i + 1) % 4) != 0)
					y += VI.GetFont(fontMediumBold).GetHeight() + DAMAGE_TYPE_SPACING_Y;
				else
					{
					x += cxColWidth;
					y = yCol;
					}
				}

			break;
			}

		case HELP_PAGE_DAMAGE_TYPES:
			{
			int x = rcPage.left;
			int y = rcPage.top;
			int cxColWidth = 20 * VI.GetFont(fontMediumBold).GetAverageWidth();

			Screen.DrawText(x, y, VI.GetFont(fontHeaderBold), RGB_SUBTITLE_TEXT, CONSTLIT("Damage Types"));
			y += VI.GetFont(fontHeaderBold).GetHeight();

			int yCol = y;

			for (i = 0; i < damageCount; i++)
				{
				VI.DrawDamageTypeIcon(Screen, x, y, (DamageTypes)i);
				
				Screen.DrawText(x + DAMAGE_TYPE_INDENT, 
						y, 
						VI.GetFont(fontMediumBold), 
						RGB_HELP_TEXT, 
						strCapitalize(::GetDamageName((DamageTypes)i)));

				//	Increment

				if (((i + 1) % 4) != 0)
					y += VI.GetFont(fontMediumBold).GetHeight() + DAMAGE_TYPE_SPACING_Y;
				else
					{
					x += cxColWidth;
					y = yCol;
					}
				}

			break;
			}
		}
	}

void CHelpSession::OnReportHardCrash (CString *retsMessage)

//	OnReportHardCrash
//
//	Describe current state

	{
	*retsMessage = CONSTLIT("session: CHelpSession\r\n");
	}

void CHelpSession::OnUpdate (bool bTopMost)

//	OnUpdate
//
//	Update

	{
	}

void CHelpSession::PaintControls (CG32bitImage &Dest, const RECT &rcRect, int *retcyHeight) const

//	PaintControls
//
//	Paints the currently mapped controls.

	{
	int x = rcRect.left;
	int y = rcRect.top;

	//	Split into two columns

	int cxColumn = RectWidth(rcRect) / 2;
	int yMax = rcRect.bottom - 32;

	//	Paint each standard command

	for (int i = 0; i < HELP_LINE_COUNT; i++)
		{
		int cyEntry;
		PaintEntry(Dest, x, y, cxColumn, m_HelpDesc[i], &cyEntry);
		if (cyEntry == 0)
			continue;

		y += cyEntry + INNER_PADDING_VERT;
		if (y >= yMax)
			{
			x += cxColumn;
			y = rcRect.top;
			}
		}

	if (retcyHeight)
		*retcyHeight = RectHeight(rcRect);
	}

void CHelpSession::PaintEntry (CG32bitImage &Dest, int x, int y, int cxWidth, const SHelpLine &Entry, int *retcyHeight) const

//	PaintEntry
//
//	Paints an entry for the given command.

	{
	//	Compute some metrics

	const CVisualPalette &VI = GetHI().GetVisuals();
	const CG16bitFont &KeyFont1 = VI.GetFont(fontMedium);
	const CG16bitFont &KeyFont2 = VI.GetFont(fontSmall);
	const CG16bitFont &TextFont = VI.GetFont(fontMedium);
	CG32bitPixel rgbText = VI.GetColor(colorTextAltHighlight);
	CG32bitPixel rgbBack = VI.GetColor(colorAreaDialog);

	int cxKeyCol = 3 * KEY_SIZE + 2 * KEY_PADDING_HORZ;
	int cyKey;

	//	Handle some special cases

	if (Entry.iCmd == CGameKeys::keyAimShip 
			&& !m_Settings.GetKeyMap().IsKeyBound(Entry.iCmd))
		{
		DWORD dwThrust = m_Settings.GetKeyMap().GetKey(CGameKeys::keyThrustForward);
		DWORD dwStop = m_Settings.GetKeyMap().GetKey(CGameKeys::keyStop);
		DWORD dwLeft = m_Settings.GetKeyMap().GetKey(CGameKeys::keyRotateLeft);
		DWORD dwRight = m_Settings.GetKeyMap().GetKey(CGameKeys::keyRotateRight);

		CInputKeyPainter KeyPainter;
		KeyPainter.SetBackColor(rgbText);
		KeyPainter.SetTextColor(rgbBack);

		KeyPainter.Init(dwThrust, KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter.Paint(Dest, x + KEY_SIZE + KEY_PADDING_HORZ, y);

		KeyPainter.Init(dwLeft, KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter.Paint(Dest, x, y + KEY_SIZE + INNER_PADDING_VERT);

		KeyPainter.Init(dwStop, KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter.Paint(Dest, x + KEY_SIZE + KEY_PADDING_HORZ, y + KEY_SIZE + INNER_PADDING_VERT);

		KeyPainter.Init(dwRight, KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter.Paint(Dest, x + 2 * (KEY_SIZE + KEY_PADDING_HORZ), y + KEY_SIZE + INNER_PADDING_VERT);

		cyKey = 2 * KEY_SIZE + INNER_PADDING_VERT;
		}

	//	Double keys

	else if (Entry.iCmd2 != CGameKeys::keyNone
			&& m_Settings.GetKeyMap().IsKeyBound(Entry.iCmd)
			&& m_Settings.GetKeyMap().IsKeyBound(Entry.iCmd2))
		{
		//	Skip under certain cases

		switch (Entry.iCmd)
			{
			case CGameKeys::keyThrustForward:
				if (!m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyAimShip))
					{
					if (retcyHeight) *retcyHeight = 0;
					return;
					}
				break;

			case CGameKeys::keyTargetNextEnemy:
				if (!m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyTargetNextEnemy)
						|| !m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyTargetNextFriendly)
						|| !m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyClearTarget))
					{
					if (retcyHeight) *retcyHeight = 0;
					return;
					}
				break;
			}

		//	Paint both keys

		CInputKeyPainter KeyPainter1;
		KeyPainter1.Init(m_Settings.GetKeyMap().GetKey(Entry.iCmd), KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter1.SetBackColor(rgbText);
		KeyPainter1.SetTextColor(rgbBack);

		CInputKeyPainter KeyPainter2;
		KeyPainter2.Init(m_Settings.GetKeyMap().GetKey(Entry.iCmd2), KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter2.SetBackColor(rgbText);
		KeyPainter2.SetTextColor(rgbBack);

		int cxTotal = KeyPainter1.GetWidth() + KEY_PADDING_HORZ + KeyPainter2.GetWidth();
		int xKey = x + (cxKeyCol - cxTotal) / 2;

		KeyPainter1.Paint(Dest, xKey, y);
		KeyPainter2.Paint(Dest, xKey + KeyPainter1.GetWidth() + KEY_PADDING_HORZ, y);

		cyKey = KEY_SIZE;
		}

	//	Normal key

	else
		{
		DWORD dwVirtKey = (Entry.iCmd != CGameKeys::keyNone ? m_Settings.GetKeyMap().GetKey(Entry.iCmd) : Entry.dwVirtKey);
		if (dwVirtKey == CVirtualKeyData::INVALID_VIRT_KEY)
			{
			if (retcyHeight) *retcyHeight = 0;
			return;
			}

		//	Skip if duplicated by another key.

		switch (Entry.iCmd)
			{
			case CGameKeys::keyDock:
			case CGameKeys::keyEnterGate:
				if (m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyInteract))
					{
					if (retcyHeight) *retcyHeight = 0;
					return;
					}
				break;

			case CGameKeys::keyCycleTarget:
				if (m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyTargetNextEnemy)
						&& m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyTargetNextFriendly)
						&& m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyClearTarget))
					{
					if (retcyHeight) *retcyHeight = 0;
					return;
					}
				break;

			case CGameKeys::keyTargetNextEnemy:
			case CGameKeys::keyTargetNextFriendly:
			case CGameKeys::keyClearTarget:
				if (!m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyTargetNextEnemy)
						|| !m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyTargetNextFriendly)
						|| !m_Settings.GetKeyMap().IsKeyBound(CGameKeys::keyClearTarget))
					{
					if (retcyHeight) *retcyHeight = 0;
					return;
					}
				break;
			}

		//	Paint the key

		CInputKeyPainter KeyPainter;
		KeyPainter.Init(dwVirtKey, KEY_SIZE, KeyFont1, KeyFont2);
		KeyPainter.SetBackColor(rgbText);
		KeyPainter.SetTextColor(rgbBack);

		KeyPainter.Paint(Dest, x + (cxKeyCol - KeyPainter.GetWidth()) / 2, y);

		cyKey = KEY_SIZE;
		}

	//	Paint the text

	RECT rcText;
	rcText.left = x + cxKeyCol + INNER_PADDING_HORZ;
	rcText.right = x + cxWidth;
	rcText.top = y;
	rcText.bottom = y + 1000;

	TArray<CString> Lines;
	TextFont.BreakText(Entry.szDesc, cxWidth - (cxKeyCol + INNER_PADDING_HORZ), &Lines);

	int xText = x + cxKeyCol + INNER_PADDING_HORZ;
	int yText = y + Max(0, (cyKey - Lines.GetCount() * TextFont.GetHeight()) / 2);

	for (int i = 0; i < Lines.GetCount(); i++)
		{
		TextFont.DrawText(Dest, xText, yText, rgbText, Lines[i], 0, 0);
		yText += TextFont.GetHeight();
		}

	//	Done

	if (retcyHeight)
		*retcyHeight = Max(cyKey, Lines.GetCount() * TextFont.GetHeight());
	}
