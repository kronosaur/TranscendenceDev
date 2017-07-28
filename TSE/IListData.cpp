//	IListData.cpp
//
//	IListData and subclasses
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_DESC							CONSTLIT("desc")
#define FIELD_ICON							CONSTLIT("icon")
#define FIELD_ICON_SCALE					CONSTLIT("iconScale")
#define FIELD_TITLE							CONSTLIT("title")

const int ICON_WIDTH =						96;
const int ICON_HEIGHT =						96;

const CItem g_DummyItem;
CItemList g_DummyItemList;
CItemListManipulator g_DummyItemListManipulator(g_DummyItemList);

//	CItemListWrapper -----------------------------------------------------------

CItemListWrapper::CItemListWrapper (CSpaceObject *pSource) :
		m_pSource(pSource),
		m_ItemList(pSource->GetItemList())

//	CItemListWrapper constructor

	{
	}

CItemListWrapper::CItemListWrapper (CItemList &ItemList) :
		m_pSource(NULL),
		m_ItemList(ItemList)

//	CItemListWrapper constructor

	{
	}

//	CListWrapper ---------------------------------------------------------------
//
//	This class handles arbitrary lists. We expect the data to be a CC list. Each
//	entry can be either a list or a struct.
//
//	If a list, we expect the following members:
//
//	0. Title
//	1. Icon descriptor
//	2. Description
//
//	For structs, we expect the following members:
//
//	title: The title of the entry
//	icon: An icon descriptor
//	desc: A longer description of the entry.

const int TITLE_INDEX =			0;
const int ICON_INDEX =			1;
const int DESC_INDEX =			2;

const int IMAGE_UNID_INDEX =	0;
const int IMAGE_X_INDEX =		1;
const int IMAGE_Y_INDEX =		2;
const int IMAGE_ELEMENTS =		3;

const int IMAGE_WIDTH_INDEX =	3;
const int IMAGE_HEIGHT_INDEX =	4;
const int IMAGE_DESC_ELEMENTS =	5;

CListWrapper::CListWrapper (CCodeChain *pCC, ICCItem *pList) :
		m_pCC(pCC),
		m_pList(pList->Reference()),
		m_iCursor(-1)

//	CListWrapper constructor

	{
	}

CString CListWrapper::GetDescAtCursor (void)

//	GetDescAtCursor
//
//	Returns the description of the list element

	{
	if (IsCursorValid())
		{
		ICCItem *pItem = m_pList->GetElement(m_iCursor);

		if (pItem->IsSymbolTable())
			return pItem->GetStringAt(FIELD_DESC);
		else if (DESC_INDEX < pItem->GetCount())
			{
			ICCItem *pDesc = pItem->GetElement(DESC_INDEX);
			if (pDesc->IsNil())
				return NULL_STR;

			return pDesc->GetStringValue();
			}
		}

	return NULL_STR;
	}

ICCItem *CListWrapper::GetEntryAtCursor (CCodeChain &CC)

//	GetEntryAtCursor
//
//	Returns the entry at the cursor

	{
	if (!IsCursorValid())
		return CC.CreateNil();

	ICCItem *pItem = m_pList->GetElement(m_iCursor);
	return pItem->Reference();
	}

DWORD CListWrapper::GetImageDescAtCursor (RECT *retrcImage, Metric *retrScale) const

//	GetImageDescAtCursor
//
//	Returns the image descriptor

	{
	if (!IsCursorValid())
		return 0;

	//	Get the item at cursor

	ICCItem *pItem = m_pList->GetElement(m_iCursor);

	//	Get the icon element

	ICCItem *pIcon;
	Metric rScale = 1.0;
	if (pItem->IsSymbolTable())
		{
		pIcon = pItem->GetElement(FIELD_ICON);
		if (pIcon == NULL)
			return 0;

		//	See if we specify an icon scale

		rScale = pItem->GetDoubleAt(FIELD_ICON_SCALE, 1.0);
		}
	else
		{
		if (pItem->GetCount() <= ICON_INDEX)
			return 0;

		pIcon = pItem->GetElement(ICON_INDEX);
		}

	if (retrScale)
		*retrScale = rScale;

	//	Get the image descriptor

	if (pIcon->GetCount() < IMAGE_ELEMENTS)
		return 0;

	retrcImage->left = pIcon->GetElement(IMAGE_X_INDEX)->GetIntegerValue();
	retrcImage->top = pIcon->GetElement(IMAGE_Y_INDEX)->GetIntegerValue();

	//	See if we have a width/height

	if (pIcon->GetCount() < IMAGE_DESC_ELEMENTS)
		{
		retrcImage->right = retrcImage->left + ICON_WIDTH;
		retrcImage->bottom = retrcImage->top + ICON_HEIGHT;
		}
	else
		{
		retrcImage->right = retrcImage->left + pIcon->GetElement(IMAGE_WIDTH_INDEX)->GetIntegerValue();
		retrcImage->bottom = retrcImage->top + pIcon->GetElement(IMAGE_HEIGHT_INDEX)->GetIntegerValue();
		}

	return pIcon->GetElement(IMAGE_UNID_INDEX)->GetIntegerValue();
	}

CString CListWrapper::GetTitleAtCursor (void)

//	GetTitleAtCursor
//
//	Returns the title of the list element

	{
	if (IsCursorValid())
		{
		ICCItem *pItem = m_pList->GetElement(m_iCursor);

		if (pItem->IsSymbolTable())
			return pItem->GetStringAt(FIELD_TITLE);
		else if (TITLE_INDEX < pItem->GetCount())
			{
			ICCItem *pTitle = pItem->GetElement(TITLE_INDEX);
			if (pTitle->IsNil())
				return NULL_STR;

			return pTitle->GetStringValue();
			}
		}

	return NULL_STR;
	}

bool CListWrapper::MoveCursorBack (void)

//	MoveCursorBack
//
//	Move cursor back

	{
	if (m_iCursor <= 0)
		return false;
	else
		{
		m_iCursor--;
		return true;
		}
	}

bool CListWrapper::MoveCursorForward (void)

//	MoveCursorForward
//
//	Moves the cursor forward

	{
	if (m_iCursor + 1 == GetCount())
		return false;
	else
		{
		m_iCursor++;
		return true;
		}
	}

void CListWrapper::PaintImageAtCursor (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, Metric rScale)

//	PaintImageAtCursor
//
//	Paints the image for the current element

	{
	RECT rcImage;
	Metric rIconScale;
	DWORD dwUNID = GetImageDescAtCursor(&rcImage, &rIconScale);
	if (dwUNID == 0)
		return;

	if (rIconScale != 1.0)
		rScale = rIconScale;

	CG32bitImage *pImage = g_pUniverse->GetLibraryBitmap(dwUNID);
	if (pImage == NULL)
		return;

	int cxImage = RectWidth(rcImage);
	int cyImage = RectHeight(rcImage);

	//	If we have a specific scale, then use it.

	if (rScale != 1.0)
		{
		Metric rX = x + (cxWidth / 2);
		Metric rY = y + (cyHeight / 2);

		CGDraw::BltTransformed(Dest, rX, rY, rScale, rScale, 0.0, *pImage, rcImage.left, rcImage.top, cxImage, cyImage);
		}

	//	If the image fits, paint it at the normal size.

	else if (cxImage <= cxWidth && cyImage <= cyHeight)
		{
		Dest.Blt(rcImage.left,
			rcImage.top,
			cxImage,
			cyImage,
			255,
			*pImage,
			x + (cxWidth - cxImage) / 2,
			y + (cyHeight - cyImage) / 2);
		}

	//	Otherwise we need to resize it.

	else
		{
		Metric rScale = Min((Metric)cxWidth / (Metric)cxImage, (Metric)cyHeight / (Metric)cyImage);
		Metric rX = x + (cxWidth / 2);
		Metric rY = y + (cyHeight / 2);

		CGDraw::BltTransformed(Dest, rX, rY, rScale, rScale, 0.0, *pImage, rcImage.left, rcImage.top, cxImage, cyImage);
		}
	}

void CListWrapper::SyncCursor (void)

//	SyncCursor
//
//	Make sure the cursor is inbounds

	{
	if (m_iCursor != -1
			&& m_iCursor >= m_pList->GetCount())
		m_iCursor = m_pList->GetCount() - 1;
	}
