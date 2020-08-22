//	TSEListImpl.h
//
//	Transcendence Space Engine
//	Copyright 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	IListData implementation ---------------------------------------------------

class CItemListWrapper : public IListData
	{
	public:
		CItemListWrapper (CSpaceObject *pSource);
		CItemListWrapper (CItemList &ItemList);

		virtual void DeleteAtCursor (int iCount) override { m_ItemList.DeleteAtCursor(iCount); if (m_pSource) m_pSource->InvalidateItemListAddRemove(); }
		virtual bool FindItem (const CItem &Item, int *retiCursor = NULL) override { return m_ItemList.FindItem(Item, 0, retiCursor); }
		virtual int GetCount (void) const override { return m_ItemList.GetCount(); }
		virtual int GetCursor (void) const override { return m_ItemList.GetCursor(); }
		virtual const CItem &GetItemAtCursor (void) const override { return m_ItemList.GetItemAtCursor(); }
		virtual CItemListManipulator &GetItemListManipulator (void) override { return m_ItemList; }
		virtual CSpaceObject *GetSource (void) const override { return m_pSource; }
		virtual bool IsCursorValid (void) const override { return m_ItemList.IsCursorValid(); }
		virtual bool MoveCursorBack (void) override { return m_ItemList.MoveCursorBack(); }
		virtual bool MoveCursorForward (void) override { return m_ItemList.MoveCursorForward(); }
		virtual void ResetCursor (void) override { m_ItemList.Refresh(CItem(), CItemListManipulator::FLAG_SORT_ITEMS); }
		virtual void ResetCursor (const CItemCriteria &EnabledItems) override { if (EnabledItems.IsEmpty()) ResetCursor(); else m_ItemList.RefreshAndSortEnabled(CItem(), EnabledItems); }
		virtual void SetCursor (int iCursor) override { m_ItemList.SetCursor(iCursor); }
		virtual void SetFilter (const CItemCriteria &Filter) override { m_ItemList.SetFilter(Filter); }
		virtual void SyncCursor (void) override { m_ItemList.SyncCursor(); }

	private:
		CSpaceObject *m_pSource;
		CItemListManipulator m_ItemList;
	};

class CListWrapper : public IListData
	{
	public:
		CListWrapper (ICCItem *pList);
		virtual ~CListWrapper (void) { m_pList->Discard(); }

		virtual ICCItemPtr GetAsCCItem (void) const override { return ICCItemPtr(m_pList->Reference()); }
		virtual int GetCount (void) const override { return m_pList->GetCount(); }
		virtual int GetCursor (void) const override { return m_iCursor; }
		virtual CString GetDescAtCursor (void) const override;
		virtual ICCItem *GetEntryAtCursor (void) const override;
		virtual CTileData GetEntryDescAtCursor (void) const override;
		virtual CString GetTitleAtCursor (void) const override;
		virtual bool IsCursorValid (void) const override { return (m_iCursor != -1); }
		virtual bool MoveCursorBack (void) override;
		virtual bool MoveCursorForward (void) override;
		virtual void PaintImageAtCursor (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, Metric rScale) override;
		virtual void ResetCursor (void) override { m_iCursor = -1; }
		virtual void SetCursor (int iCursor) override { m_iCursor = Min(Max(-1, iCursor), GetCount() - 1); }
		virtual void SyncCursor (void) override;

	private:
		DWORD GetImageDescAtCursor (RECT *retrcImage, Metric *retrScale) const;

		ICCItem *m_pList;

		int m_iCursor;
	};
