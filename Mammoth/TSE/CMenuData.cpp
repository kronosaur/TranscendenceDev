//	CMenuData.cpp
//
//	CMenuData class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int CMenuData::AddMenuItem (const CString &sID,
							 const CString &sKey,
							 const CString &sLabel,
							 const CObjectImageArray *pImage,
							 int iCount,
							 const CString &sExtra,
							 const CString &sHelp,
							 DWORD dwFlags,
							 DWORD dwData,
							 DWORD dwData2)

//	AddMenuItem
//
//	Add an item

	{
	int iListCount = m_List.GetCount();
	m_List.InsertEmpty();

	//	See if the label has an accelerator

	CString sCleanLabel;
	CString sAccelerator;
	int iAcceleratorPos;
	CLanguage::ParseLabelDesc(sLabel, &sCleanLabel, &sAccelerator, &iAcceleratorPos);

	//	If we have a key, sort by key. Otherwise, we
	//	add it at the end.

	int iPos;
	if (sKey.IsBlank() || !(dwFlags & FLAG_SORT_BY_KEY))
		iPos = iListCount;
	else
		{
		iPos = 0;
		while (iPos < iListCount
				&& !m_List[iPos].sKey.IsBlank()
				&& strCompareAbsolute(sKey, m_List[iPos].sKey) > 0)
			iPos++;

		//	Move other items up

		for (int i = iListCount - 1; i >= iPos; i--)
			m_List[i+1] = m_List[i];
		}

	//	Add item

	m_List[iPos].sID = sID;
	m_List[iPos].sKey = sKey;
	m_List[iPos].sLabel = sCleanLabel;
	m_List[iPos].sAccelerator = sAccelerator;
	m_List[iPos].iAcceleratorPos = iAcceleratorPos;
	m_List[iPos].dwData = dwData;
	m_List[iPos].dwData2 = dwData2;
	m_List[iPos].iCount = iCount;
	m_List[iPos].sExtra = sExtra;
	m_List[iPos].sHelp = sHelp;
	m_List[iPos].dwFlags = dwFlags;
	m_List[iPos].pImage = pImage;

	return iPos;
	}

int CMenuData::FindItemByKey (const CString &sKey)

//	FindItemByKey
//
//	Returns the index of the menu item with the given key

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		if (strEquals(sKey, m_List[i].sKey)
				|| strEquals(sKey, m_List[i].sAccelerator))
			return i;

	return -1;
	}

bool CMenuData::FindItemData (const CString &sKey, DWORD *retdwData, DWORD *retdwData2)

//	FindItemData
//
//	Returns the data associated with the menu item that
//	has the given key

	{
	int iIndex = FindItemByKey(sKey);
	if (iIndex != -1)
		{
		if (retdwData)
			*retdwData = m_List[iIndex].dwData;

		if (retdwData2)
			*retdwData2 = m_List[iIndex].dwData2;

		return true;
		}

	return false;
	}

int CMenuData::GetItemCooldown (int iIndex, DWORD dwNow) const

//	GetItemCooldown
//
//	Returns the cooldown from 0 (just started) to 100 (done). If we're not in
//	cooldown, then we return -1.

	{
	if (m_List[iIndex].dwCooldownEndsOn <= m_List[iIndex].dwCooldownStartedOn)
		return -1;

	else if (dwNow < m_List[iIndex].dwCooldownStartedOn)
		return 0;

	else if (dwNow >= m_List[iIndex].dwCooldownEndsOn)
		return -1;

	int iRange = m_List[iIndex].dwCooldownEndsOn - m_List[iIndex].dwCooldownStartedOn;
	int iPos = dwNow - m_List[iIndex].dwCooldownStartedOn;

	return 100 * iPos / iRange;
	}

bool CMenuData::IsItemEnabled (int iIndex, DWORD dwNow) const

//	IsItemEnabled
//
//	Returns whether the item is enabled (not in cooldown).

	{
	if (m_List[iIndex].dwFlags & FLAG_SHOW_COOLDOWN)
		{
		if (GetItemCooldown(iIndex, dwNow) != -1)
			return false;
		}
		
	return true;
	}

void CMenuData::SetItemCooldown (int iIndex, DWORD dwStartedOn, DWORD dwEndsOn)

//	SetItemCooldown
//
//	Sets the cooldown.

	{
	m_List[iIndex].dwCooldownStartedOn = dwStartedOn;
	m_List[iIndex].dwCooldownEndsOn = dwEndsOn;
	}
