//	IDockScreenDisplay.cpp
//
//	IDockScreenDisplay class

#include "PreComp.h"
#include "Transcendence.h"

#define HERO_IMAGE_TAG				CONSTLIT("HeroImage")
#define LIST_TAG					CONSTLIT("List")
#define LIST_OPTIONS_TAG			CONSTLIT("ListOptions")
#define ON_DISPLAY_INIT_TAG			CONSTLIT("OnDisplayInit")

#define BACKGROUND_ID_ATTRIB		CONSTLIT("backgroundID")
#define CRITERIA_ATTRIB				CONSTLIT("criteria")
#define DATA_FROM_ATTRIB			CONSTLIT("dataFrom")
#define DISPLAY_ACTUAL_ATTRIB		CONSTLIT("displayActual")
#define HEIGHT_ATTRIB				CONSTLIT("height")
#define ICON_HEIGHT_ATTRIB			CONSTLIT("iconHeight")
#define ICON_SCALE_ATTRIB			CONSTLIT("iconScale")
#define ICON_WIDTH_ATTRIB			CONSTLIT("iconWidth")
#define IMAGE_HEIGHT_ATTRIB			CONSTLIT("imageHeight")
#define IMAGE_ID_ATTRIB				CONSTLIT("imageID")
#define IMAGE_WIDTH_ATTRIB			CONSTLIT("imageWidth")
#define IMAGE_X_ATTRIB				CONSTLIT("imageX")
#define IMAGE_Y_ATTRIB				CONSTLIT("imageY")
#define INITIAL_ITEM_ATTRIB			CONSTLIT("initialItem")
#define LIST_ATTRIB					CONSTLIT("list")
#define NO_ARMOR_SPEED_DISPLAY_ATTRIB	CONSTLIT("noArmorSpeedDisplay")
#define NO_EMPTY_SLOTS_ATTRIB		CONSTLIT("noEmptySlots")
#define POS_X_ATTRIB				CONSTLIT("posX")
#define POS_Y_ATTRIB				CONSTLIT("posY")
#define ROW_HEIGHT_ATTRIB			CONSTLIT("rowHeight")
#define SLOT_NAME_ATTRIB			CONSTLIT("slotName")
#define TYPE_ATTRIB					CONSTLIT("type")
#define WIDTH_ATTRIB				CONSTLIT("width")

#define ALIGN_CENTER				CONSTLIT("center")
#define ALIGN_RIGHT					CONSTLIT("right")
#define ALIGN_LEFT					CONSTLIT("left")
#define ALIGN_BOTTOM				CONSTLIT("bottom")
#define ALIGN_TOP					CONSTLIT("top")
#define ALIGN_MIDDLE				CONSTLIT("middle")

#define DATA_FROM_PLAYER			CONSTLIT("player")
#define DATA_FROM_SOURCE			CONSTLIT("source")
#define DATA_FROM_STATION			CONSTLIT("station")

#define FIELD_ALIGN					CONSTLIT("align")
#define FIELD_IMAGE					CONSTLIT("image")
#define FIELD_OBJ					CONSTLIT("obj")
#define FIELD_PADDING_BOTTOM		CONSTLIT("padding-bottom")
#define FIELD_PADDING_LEFT			CONSTLIT("padding-left")
#define FIELD_PADDING_RIGHT			CONSTLIT("padding-right")
#define FIELD_PADDING_TOP			CONSTLIT("padding-top")
#define FIELD_TYPE					CONSTLIT("type")

#define PROPERTY_LIST_SOURCE		CONSTLIT("listSource")

#define SCREEN_TYPE_ARMOR_SELECTOR		CONSTLIT("armorSelector")
#define SCREEN_TYPE_CANVAS				CONSTLIT("canvas")
#define SCREEN_TYPE_CAROUSEL_SELECTOR	CONSTLIT("carouselSelector")
#define SCREEN_TYPE_CUSTOM_PICKER		CONSTLIT("customPicker")
#define SCREEN_TYPE_CUSTOM_ITEM_PICKER	CONSTLIT("customItemPicker")
#define SCREEN_TYPE_DETAILS_PANE		CONSTLIT("detailsPane")
#define SCREEN_TYPE_DEVICE_SELECTOR		CONSTLIT("deviceSelector")
#define SCREEN_TYPE_ICON_LIST			CONSTLIT("iconList")
#define SCREEN_TYPE_ITEM_PICKER			CONSTLIT("itemPicker")
#define SCREEN_TYPE_MISC_SELECTOR		CONSTLIT("miscSelector")
#define SCREEN_TYPE_SUBJUGATE_MINIGAME	CONSTLIT("subjugateMinigame")
#define SCREEN_TYPE_WEAPONS_SELECTOR	CONSTLIT("weaponsSelector")

#define TYPE_HERO					CONSTLIT("hero")
#define TYPE_IMAGE					CONSTLIT("image")
#define TYPE_NONE					CONSTLIT("none")
#define TYPE_OBJECT					CONSTLIT("object")
#define TYPE_SCHEMATIC				CONSTLIT("schematic")

const int ICON_WIDTH =				96;
const int ICON_HEIGHT =				96;

IDockScreenDisplay *IDockScreenDisplay::Create (CDockScreen &DockScreen, const CString &sType, CString *retsError)

//	Create
//
//	Creates a display of the given type (or return NULL if there was an error).
//	NOTE: Callers must still call Init on the display after this.

	{
	if (sType.IsBlank() || strEquals(sType, SCREEN_TYPE_CANVAS))
		return new CDockScreenNullDisplay(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_ITEM_PICKER))
		return new CDockScreenItemList(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_CAROUSEL_SELECTOR))
		return new CDockScreenCarousel(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_CUSTOM_PICKER))
		return new CDockScreenCustomList(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_CUSTOM_ITEM_PICKER))
		return new CDockScreenCustomItemList(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_DETAILS_PANE))
		return new CDockScreenDetailsPane(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_ICON_LIST))
		return new CDockScreenIconList(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_ARMOR_SELECTOR))
		return new CDockScreenSelector(DockScreen, CGSelectorArea::configArmor);

	else if (strEquals(sType, SCREEN_TYPE_DEVICE_SELECTOR))
		return new CDockScreenSelector(DockScreen, CGSelectorArea::configDevices);

	else if (strEquals(sType, SCREEN_TYPE_MISC_SELECTOR))
		return new CDockScreenSelector(DockScreen, CGSelectorArea::configMiscDevices);

	else if (strEquals(sType, SCREEN_TYPE_SUBJUGATE_MINIGAME))
		return new CDockScreenSubjugate(DockScreen);

	else if (strEquals(sType, SCREEN_TYPE_WEAPONS_SELECTOR))
		return new CDockScreenSelector(DockScreen, CGSelectorArea::configWeapons);
	
	else
		{
		if (retsError) *retsError = strPatternSubst(CONSTLIT("ERROR: Invalid display type: %s."), sType);
		return NULL;
		}
	}

bool IDockScreenDisplay::GetDisplayOptions (SInitCtx &Ctx, SDisplayOptions *retOptions, CString *retsError)

//	GetDisplayOptions
//
//	Initializes the display options structure, which is used by list and 
//	selector displays.

	{
	DEBUG_TRY

	//	Initialize background image options

	CString sBackgroundID;
	if (const CXMLElement *pImageDesc = Ctx.pDesc->GetContentElementByTag(HERO_IMAGE_TAG))
		{
		retOptions->BackgroundDesc.iType = EDockScreenBackground::heroImage;
		retOptions->BackgroundDesc.dwImageID = pImageDesc->GetAttributeInteger(IMAGE_ID_ATTRIB);
		retOptions->BackgroundDesc.rcImage.left = pImageDesc->GetAttributeInteger(IMAGE_X_ATTRIB);
		retOptions->BackgroundDesc.rcImage.top = pImageDesc->GetAttributeInteger(IMAGE_Y_ATTRIB);
		retOptions->BackgroundDesc.rcImage.right = retOptions->BackgroundDesc.rcImage.left + pImageDesc->GetAttributeInteger(IMAGE_WIDTH_ATTRIB);
		retOptions->BackgroundDesc.rcImage.bottom = retOptions->BackgroundDesc.rcImage.top + pImageDesc->GetAttributeInteger(IMAGE_HEIGHT_ATTRIB);
		}
	else if (Ctx.pDesc->FindAttribute(BACKGROUND_ID_ATTRIB, &sBackgroundID))
		{
		//	If the attribute exists, but is empty (or equals "none") then
		//	we don't have a background

		if (sBackgroundID.IsBlank() || strEquals(sBackgroundID, CONSTLIT("none")))
			retOptions->BackgroundDesc.iType = EDockScreenBackground::none;

		else if (strEquals(sBackgroundID, TYPE_HERO))
			{
			retOptions->BackgroundDesc.iType = EDockScreenBackground::objHeroImage;
			retOptions->BackgroundDesc.pObj = Ctx.pLocation;
			}

		//	If the ID is "object" then we should ask the object

		else if (strEquals(sBackgroundID, TYPE_OBJECT))
			{
			retOptions->BackgroundDesc.pObj = Ctx.pLocation;
            if (Ctx.pLocation->IsPlayer())
			    retOptions->BackgroundDesc.iType = EDockScreenBackground::objSchematicImage;
            else
			    retOptions->BackgroundDesc.iType = EDockScreenBackground::objHeroImage;
			}

		else if (strEquals(sBackgroundID, TYPE_SCHEMATIC))
			{
			retOptions->BackgroundDesc.iType = EDockScreenBackground::objSchematicImage;
			retOptions->BackgroundDesc.pObj = Ctx.pLocation;
			}

		//	If the ID is "player" then we should ask the player ship

		else if (strEquals(sBackgroundID, DATA_FROM_PLAYER))
			{
			CSpaceObject *pPlayer = g_pUniverse->GetPlayerShip();
			if (pPlayer)
				{
				retOptions->BackgroundDesc.iType = EDockScreenBackground::objSchematicImage;
				retOptions->BackgroundDesc.pObj = pPlayer;
				}
			}

		//	Otherwise, we expect an integer

		else
			{
			retOptions->BackgroundDesc.iType = EDockScreenBackground::image;
			retOptions->BackgroundDesc.dwImageID = strToInt(sBackgroundID, 0);
			}
		}

	retOptions->rcDisplay.left = 0;
	retOptions->rcDisplay.top = 23;
	retOptions->rcDisplay.right = RectWidth(Ctx.rcRect);
	retOptions->rcDisplay.bottom = RectHeight(Ctx.rcRect) - 47;

	//	Initialize control rect. If we have a background, then initialize to
	//	backwards compatible position. Otherwise, we take up the full range.
	//
	//	NOTE: This RECT is relative to Ctx.rcRect.

	if (retOptions->BackgroundDesc.iType != EDockScreenBackground::defaultBackground)
		{
		retOptions->rcControl.left = 4;
		retOptions->rcControl.top = 12;
		retOptions->rcControl.right = 548;
		retOptions->rcControl.bottom = 396;
		}
	else
		retOptions->rcControl = retOptions->rcDisplay;

	//	Get the type

	if (Ctx.pDisplayDesc
			&& Ctx.pDisplayDesc->FindAttribute(TYPE_ATTRIB, &retOptions->sType))
		NULL;
	else
		retOptions->sType = Ctx.pDesc->GetAttribute(TYPE_ATTRIB);

	//	There are a couple of different ways to get options (for backwards
	//	compatibility).

	CXMLElement *pOptions;
	if ((pOptions = Ctx.pDesc->GetContentElementByTag(LIST_OPTIONS_TAG)) == NULL
			&& (pOptions = Ctx.pDesc->GetContentElementByTag(LIST_TAG)) == NULL
			&& (pOptions = Ctx.pDisplayDesc) == NULL)
		return true;

	retOptions->pOptions = pOptions;

	//	Read from the element

	retOptions->sDataFrom = pOptions->GetAttribute(DATA_FROM_ATTRIB);
	if (!pOptions->FindAttribute(CRITERIA_ATTRIB, &retOptions->sItemCriteria))
		retOptions->sItemCriteria = pOptions->GetAttribute(LIST_ATTRIB);
	retOptions->sInitialItemCode = pOptions->GetAttribute(INITIAL_ITEM_ATTRIB);
	retOptions->sRowHeightCode = pOptions->GetAttribute(ROW_HEIGHT_ATTRIB);

	//	Init code

	CXMLElement *pInitCode = pOptions->GetContentElementByTag(ON_DISPLAY_INIT_TAG);
	if (pInitCode == NULL)
		pInitCode = pOptions;

	retOptions->sCode = pInitCode->GetContentText(0);

	//	List options

	retOptions->bNoArmorSpeedDisplay = pOptions->GetAttributeBool(NO_ARMOR_SPEED_DISPLAY_ATTRIB);
	retOptions->bActualItems = pOptions->GetAttributeBool(DISPLAY_ACTUAL_ATTRIB);
	retOptions->cxIcon = pOptions->GetAttributeIntegerBounded(ICON_WIDTH_ATTRIB, 0, -1, ICON_WIDTH);
	retOptions->cyIcon = pOptions->GetAttributeIntegerBounded(ICON_HEIGHT_ATTRIB, 0, -1, ICON_HEIGHT);
	retOptions->rIconScale = pOptions->GetAttributeDoubleBounded(ICON_SCALE_ATTRIB, 0.0, -1.0, 1.0);

    //  Selector options

    retOptions->bNoEmptySlots = pOptions->GetAttributeBool(NO_EMPTY_SLOTS_ATTRIB);
    retOptions->sSlotNameCode = pOptions->GetAttribute(SLOT_NAME_ATTRIB);

	//	See if we have control position

	if (pOptions->FindAttributeInteger(POS_X_ATTRIB, (int *)&retOptions->rcControl.left))
		{
		retOptions->rcControl.top = pOptions->GetAttributeIntegerBounded(POS_Y_ATTRIB, 0, -1);
		retOptions->rcControl.right = retOptions->rcControl.left + pOptions->GetAttributeIntegerBounded(WIDTH_ATTRIB, 0, -1);
		retOptions->rcControl.bottom = retOptions->rcControl.top + pOptions->GetAttributeIntegerBounded(HEIGHT_ATTRIB, 0, -1);
		}

	return true;

	DEBUG_CATCH
	}

ICCItemPtr IDockScreenDisplay::GetProperty (const CString &sProperty) const

//	GetProperty
//
//	Returns a property

	{
	if (strEquals(sProperty, PROPERTY_LIST_SOURCE))
		{
		CSpaceObject *pObj = GetSource();
		if (pObj == NULL)
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr((int)pObj);
		}
	else
		return OnGetProperty(sProperty);
	}

bool IDockScreenDisplay::SetProperty (const CString &sProperty, const ICCItem &Value)

//	SetProperty
//
//	Sets a property

	{
	return OnSetProperty(sProperty, Value);
	}

bool IDockScreenDisplay::EvalBool (const CString &sCode, bool *retbResult, CString *retsError)

//	EvalBool
//
//	Evaluates the given string

	{
	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(m_DockScreen.GetRoot());
	Ctx.SaveAndDefineSourceVar(m_pLocation);
	Ctx.SaveAndDefineDataVar(m_pData);

	CCodeChain::SLinkOptions Options;
	Options.iOffset = 1;

	ICCItemPtr pExp = Ctx.LinkCode(sCode, Options);
	ICCItemPtr pResult = Ctx.RunCode(pExp);	//	LATER:Event

	if (pResult->IsError())
		{
		*retsError = pResult->GetStringValue();
		return false;
		}

	*retbResult = !pResult->IsNil();
	return true;
	}

CSpaceObject *IDockScreenDisplay::EvalListSource (const CString &sString, CString *retsError)

//	EvalListSource
//
//	Returns the object from which we should display items

	{
	char *pPos = sString.GetPointer();

	//	See if we need to evaluate

	if (*pPos == '=')
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(m_DockScreen.GetRoot());
		Ctx.SaveAndDefineSourceVar(m_pLocation);
		Ctx.SaveAndDefineDataVar(m_pData);

		CCodeChain::SLinkOptions Options;
		Options.iOffset = 1;

		ICCItemPtr pExp = Ctx.LinkCode(sString, Options);
		ICCItemPtr pResult = Ctx.RunCode(pExp);	//	LATER:Event

		if (pResult->IsError())
			{
			*retsError = pResult->GetStringValue();
			return NULL;
			}

		//	Convert to an object pointer

		CSpaceObject *pSource;
		if (strEquals(pResult->GetStringValue(), DATA_FROM_PLAYER))
			pSource = m_pPlayer->GetShip();
		else if (strEquals(pResult->GetStringValue(), DATA_FROM_STATION)
				|| strEquals(pResult->GetStringValue(), DATA_FROM_SOURCE))
			pSource = m_pLocation;
		else
			pSource = Ctx.AsSpaceObject(pResult);

		return pSource;
		}

	//	Otherwise, compare to constants

	else if (strEquals(sString, DATA_FROM_PLAYER))
		return m_pPlayer->GetShip();
	else
		return m_pLocation;
	}

bool IDockScreenDisplay::EvalString (const CString &sString, bool bPlain, ECodeChainEvents iEvent, CString *retsResult)

//	EvalString
//
//	Evaluates the given string.

	{
	CCodeChainCtx Ctx(GetUniverse());
	Ctx.SetEvent(iEvent);
	Ctx.DefineContainingType(m_DockScreen.GetRoot());
	Ctx.SaveAndDefineSourceVar(m_pLocation);
	Ctx.SaveAndDefineDataVar(m_pData);

	return Ctx.RunEvalString(sString, bPlain, retsResult);
	}

bool IDockScreenDisplay::GetDefaultBackground (SDockScreenBackgroundDesc *retDesc)

//	GetDefaultBackground
//
//	Sets up any default backgrounds from the display.
	
	{
	return OnGetDefaultBackground(retDesc);
	}

CDockScreenStack &IDockScreenDisplay::GetScreenStack (void) const

//	GetScreenStack
//
//	Returns the screen stack.

	{
	return g_pTrans->GetModel().GetScreenStack();
	}

ALERROR IDockScreenDisplay::Init (SInitCtx &Ctx, const SDisplayOptions &Options, CString *retsError)

//	Init
//
//	Initialize

	{
	m_pPlayer = Ctx.pPlayer;
	m_pLocation = Ctx.pLocation;
	m_pData = Ctx.pData;

	return OnInit(Ctx, Options, retsError); 
	}

ICCItemPtr IDockScreenDisplay::OnGetListAsCCItem (void) const

//	OnGetListAsCCItem
//
//	REturns the entire list as an item.

	{
	IListData *pList = GetListData();
	if (!pList)
		return ICCItemPtr::Nil();

	return pList->GetAsCCItem();
	}

ICCItemPtr IDockScreenDisplay::OnGetProperty (const CString &sProperty) const

//	OnGetProperty
//
//	Default has no properties

	{
	return NULL;
	}

void IDockScreenDisplay::OnModifyItemBegin (IDockScreenUI::SModifyItemCtx &Ctx, const CSpaceObject &Source, const CItem &Item) const

//	OnModifyItemBeing
//
//	An item on Source is about to be modified. This is used to set state in Ctx
//	so that we know how to update the UI after the modification happens.
//
//	NOTE: We do not guarantee that OnModifyItemComplete will ever be called 
//	(we could fail due to error), so we should not alter any state (which is
//	why this method is const).

	{
	//	If we're not displaying items from this source, then we don't need to
	//	do anything.

	if (GetSource() != Source)
		{ }

	//	If this is a selector area, then just reset the list

	else if (GetUIFlags() & FLAG_UI_ITEM_SELECTOR)
		Ctx.iSelChange = IDockScreenUI::selReset;

	//	If this is NOT an item list, then we don't do anything

	else if (!(GetUIFlags() & FLAG_UI_ITEM_LIST))
		{ }

	//	If no item, then it means that we want to keep the current selection.

	else if (Item.IsEmpty())
		{
		Ctx.iSelChange = IDockScreenUI::selOriginal;
		Ctx.OriginalItem = GetCurrentItem();
		}

	//	If the item that we're going to modify is the currently selected item, 
	//	then we select the modified one.

	else if (GetCurrentItem().IsEqual(Item))
		{
		Ctx.iSelChange = IDockScreenUI::selModified;
		Ctx.iOriginalCursor = GetListCursor();
		}

	//	Otherwise, we remember the current selection

	else
		{
		Ctx.iSelChange = IDockScreenUI::selOriginal;
		Ctx.OriginalItem = GetCurrentItem();
		}
	}

IDockScreenDisplay::EResults IDockScreenDisplay::OnModifyItemComplete (IDockScreenUI::SModifyItemCtx &Ctx, const CSpaceObject &Source, const CItem &Result)

//	OnModifyItemComplete
//
//	An item on pSource was modified.

	{
	switch (Ctx.iSelChange)
		{
		case IDockScreenUI::selOriginal:
			ResetList(GetSource());
			if (!Ctx.OriginalItem.IsEmpty())
				SelectItem(Ctx.OriginalItem);
			return resultShowPane;

		case IDockScreenUI::selModified:
			ResetList(GetSource());

			//	If the result is empty, then it means that the item was deleted,
			//	so we use the last cursor position.

			if (Result.IsEmpty())
				{
				if (Ctx.iOriginalCursor != -1)
					SetListCursor(Ctx.iOriginalCursor);
				}
			else
				SelectItem(Result);

			return resultShowPane;

		case IDockScreenUI::selReset:
			ResetList(GetSource());
			return resultShowPane;

		default:
			//	Nothing to do

			return resultNone;
		}
	}

void IDockScreenDisplay::OnShowPane (bool bNoListNavigation) 

//	OnShowPane
//
//	Pane has been shown. This is normally overridden by subclasses, but by
//	default we clear the armor selection.
	
	{
	SelectArmor(-1); 
	}

bool IDockScreenDisplay::ParseAlign (const ICCItem &Align, DWORD *retdwAlign)

//	ParseAlign
//
//	Parse alignment.

	{
	DWORD dwAlign = 0;

	for (int i = 0; i < Align.GetCount(); i++)
		{
		const ICCItem *pEntry = Align.GetElement(i);
		CString sValue = pEntry->GetStringValue();

		if (strEquals(sValue, ALIGN_LEFT))
			dwAlign |= alignLeft;
		else if (strEquals(sValue, ALIGN_RIGHT))
			dwAlign |= alignRight;
		else if (strEquals(sValue, ALIGN_CENTER))
			dwAlign |= alignCenter;
		else if (strEquals(sValue, ALIGN_TOP))
			dwAlign |= alignTop;
		else if (strEquals(sValue, ALIGN_BOTTOM))
			dwAlign |= alignBottom;
		else if (strEquals(sValue, ALIGN_MIDDLE))
			dwAlign |= alignMiddle;
		else
			return false;
		}

	if (retdwAlign)
		*retdwAlign = dwAlign;

	return true;
	}

bool IDockScreenDisplay::ParseBackgrounDesc (ICCItem *pDesc, SDockScreenBackgroundDesc *retDesc)

//	ParseBackroundDesc
//
//	Parses a descriptor. Returns TRUE if successful.

	{
	//	Nil means no default value

	if (pDesc->IsNil())
		retDesc->iType = EDockScreenBackground::defaultBackground;

	//	If we have a struct, we expect a certain format

	else if (pDesc->IsSymbolTable())
		{
		CString sType = pDesc->GetStringAt(FIELD_TYPE);
		if (sType.IsBlank() || strEquals(sType, TYPE_NONE))
			retDesc->iType = EDockScreenBackground::none;

		else if (strEquals(sType, TYPE_HERO))
			{
			if (const ICCItem* pObj = pDesc->GetElement(FIELD_OBJ))
				{
				retDesc->iType = EDockScreenBackground::objHeroImage;
				retDesc->pObj = CreateObjFromItem(pObj);
				if (retDesc->pObj == NULL)
					return false;
				}
			else if (const ICCItem* pImageDesc = pDesc->GetElement(FIELD_IMAGE))
				{
				retDesc->iType = EDockScreenBackground::heroImage;
				retDesc->dwImageID = CTLispConvert::AsImageDesc(pImageDesc, &retDesc->rcImage);
				}
			else
				return false;
			}
		else if (strEquals(sType, TYPE_IMAGE))
			{
			retDesc->iType = EDockScreenBackground::image;

			const ICCItem *pImage = pDesc->GetElement(FIELD_IMAGE);
			if (pImage == NULL)
				return false;

			else if (pImage->IsList())
				retDesc->dwImageID = CTLispConvert::AsImageDesc(pImage, &retDesc->rcImage);

			else if (pImage->IsInteger())
				retDesc->dwImageID = pImage->GetIntegerValue();

			else
				return false;
			}
		else if (strEquals(sType, TYPE_OBJECT))
			{
			retDesc->pObj = CreateObjFromItem(pDesc->GetElement(FIELD_OBJ));
			if (retDesc->pObj == NULL)
				return false;

            if (retDesc->pObj->IsPlayer())
    			retDesc->iType = EDockScreenBackground::objSchematicImage;
            else
    			retDesc->iType = EDockScreenBackground::objHeroImage;
			}
		else if (strEquals(sType, TYPE_SCHEMATIC))
			{
			retDesc->iType = EDockScreenBackground::objSchematicImage;
			retDesc->pObj = CreateObjFromItem(pDesc->GetElement(FIELD_OBJ));
			if (retDesc->pObj == NULL)
				return false;
			}
		else
			return false;

		//	Optional alignment

		if (const ICCItem *pAlign = pDesc->GetElement(FIELD_ALIGN))
			{
			if (!ParseAlign(*pAlign, &retDesc->dwImageAlign))
				return false;
			}

		//	Optional padding

		if (const ICCItem *pPadding = pDesc->GetElement(FIELD_PADDING_BOTTOM))
			retDesc->rcImagePadding.bottom = pPadding->GetIntegerValue();

		if (const ICCItem *pPadding = pDesc->GetElement(FIELD_PADDING_LEFT))
			retDesc->rcImagePadding.left = pPadding->GetIntegerValue();

		if (const ICCItem *pPadding = pDesc->GetElement(FIELD_PADDING_RIGHT))
			retDesc->rcImagePadding.right = pPadding->GetIntegerValue();

		if (const ICCItem *pPadding = pDesc->GetElement(FIELD_PADDING_TOP))
			retDesc->rcImagePadding.top = pPadding->GetIntegerValue();
		}

	//	Otherwise, we can't parse.
	//
	//	LATER: We should eventually handle a list-based image descriptor, but we
	//	would need to enhance SDockScreenBackgroundDesc for that.

	else
		return false;

	//	Success
	
	return true;
	}

void IDockScreenDisplay::SelectArmor (int iSelection)

//  SelectArmor
//
//  Selects the given armor segment in the HUD.

    {
    m_DockScreen.GetGameSession().OnArmorSelected(iSelection);
    }
