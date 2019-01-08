//	IListData.cpp
//
//	IListData and subclasses
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_DESC							CONSTLIT("desc")
#define FIELD_ICON							CONSTLIT("icon")
#define FIELD_ICON_SCALE					CONSTLIT("iconScale")
#define FIELD_TITLE							CONSTLIT("title")

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

	return CTLispConvert::AsImageDesc(pIcon, retrcImage);
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

	CPaintHelper::PaintScaledImage(Dest, x, y, cxWidth, cyHeight, *pImage, rcImage, rScale);
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
