//	TSESpaceObjectDefs.h
//
//	CSpaceObject definitions
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum class EObjectPart
	{
	none,

	equipment,										//	A piece of equipment (e.g., LRS)
	interior,										//	The interior of the object (e.g., cargo hold)
	item,											//	An item on the object
	};

struct SObjectPartDesc
	{
	SObjectPartDesc () {}
	SObjectPartDesc (const CItem &ItemArg) :
			iPart(EObjectPart::item),
			Item(ItemArg)
		{ }

	SObjectPartDesc (EObjectPart iPart) :
			iPart(iPart)
		{
		switch (iPart)
			{
			case EObjectPart::item:
			case EObjectPart::equipment:
				throw CException(ERR_FAIL);
				break;
				
			default:
				break;
			}
		}

	EObjectPart iPart = EObjectPart::none;

	CItem Item;										//	If iPart == item
	Abilities iEquipment = ablUnknown;				//	If iPart == equipment
	};

//	CMenuData ------------------------------------------------------------------

class CMenuData
	{
	public:
		static constexpr DWORD FLAG_GRAYED =				0x00000001;
		static constexpr DWORD FLAG_SORT_BY_KEY =			0x00000002;
		static constexpr DWORD FLAG_SHOW_COOLDOWN =			0x00000004;

		CMenuData (void) { }

		int AddMenuItem (const CString &sID,
						  const CString &sKey,
						  const CString &sLabel,
						  DWORD dwFlags = 0,
						  DWORD dwData = 0,
						  DWORD dwData2 = 0) { return AddMenuItem(sID, sKey, sLabel, NULL, 0, NULL_STR, NULL_STR, dwFlags, dwData, dwData2); }
		int AddMenuItem (const CString &sID,
						  const CString &sKey,
						  const CString &sLabel,
						  const CObjectImageArray *pImage,
						  int iCount,
						  const CString &sExtra,
						  const CString &sHelp,
						  DWORD dwFlags,
						  DWORD dwData,
						  DWORD dwData2 = 0);
		void SetTitle (const CString &sTitle) { m_sTitle = sTitle; }

		void DeleteAll (void) { m_List.DeleteAll(); }
		int FindItemByKey (const CString &sKey);
		bool FindItemData (const CString &sKey, DWORD *retdwData = NULL, DWORD *retdwData2 = NULL);
		int GetCount (void) const { return m_List.GetCount(); }
		int GetItemAcceleratorPos (int iIndex) const { return m_List[iIndex].iAcceleratorPos; }
		DWORD GetItemData (int iIndex) const { return m_List[iIndex].dwData; }
		DWORD GetItemData2 (int iIndex) const { return m_List[iIndex].dwData2; }
		int GetItemCooldown (int iIndex, DWORD dwNow) const;
		int GetItemCount (int iIndex) const { return m_List[iIndex].iCount; }
		const CString &GetItemExtra (int iIndex) const { return m_List[iIndex].sExtra; }
		const CString &GetItemHelpText (int iIndex) const { return m_List[iIndex].sHelp; }
		const CString &GetItemID (int iIndex) const { return m_List[iIndex].sID; }
		const CObjectImageArray *GetItemImage (int iIndex) const { return m_List[iIndex].pImage; }
		DWORD GetItemFlags (int iIndex) const { return m_List[iIndex].dwFlags; }
		const CString &GetItemKey (int iIndex) const { return m_List[iIndex].sKey; }
		const CString &GetItemLabel (int iIndex) const { return m_List[iIndex].sLabel; }
		const CString &GetTitle (void) const { return m_sTitle; }
		bool IsEmpty (void) const { return m_List.GetCount() == 0; }
		bool IsItemEnabled (int iIndex, DWORD dwNow) const;
		void SetItemCooldown (int iIndex, DWORD dwStartedOn, DWORD dwEndsOn);

	private:
		struct Entry
			{
			CString sID;
			CString sKey;
			CString sLabel;
			CString sAccelerator;
			int iAcceleratorPos = -1;
			const CObjectImageArray *pImage = NULL;
			int iCount = 0;
			DWORD dwCooldownStartedOn = 0;
			DWORD dwCooldownEndsOn = 0;
			CString sExtra;
			CString sHelp;
			DWORD dwFlags = 0;

			DWORD dwData = 0;
			DWORD dwData2 = 0;
			};

		CString m_sTitle;
		TArray<Entry> m_List;
	};
