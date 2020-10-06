//	ItemGeneratorImpl.h
//
//	Implementations of IItemGenerator
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CRecursingCheck
	{
	public:
		CRecursingCheck (bool &bRecurse) :
				m_bRecurse(bRecurse),
				m_bWorking(false)
			{ }

		~CRecursingCheck ()
			{
			if (m_bWorking)
				m_bRecurse = false;
			}

		bool IsRecursing (void)
			{
			if (m_bRecurse)
				return true;

			m_bWorking = true;
			m_bRecurse = true;
			return false;
			}

	private:
		bool &m_bRecurse;
		bool m_bWorking;
	};

class CGroupOfGenerators : public IItemGenerator
	{
	public:
		CGroupOfGenerators (void) : m_bRecursing(false)
			{ }

		virtual ~CGroupOfGenerators (void) override;
		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override;
		virtual CCurrencyAndValue GetDesiredValue (SItemAddCtx &Ctx, int iLevel, int *retiLoopCount = NULL, Metric *retrScale = NULL) const override;
		virtual IItemGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pItem; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IItemGenerator *pItem;
			int iChance;
			DiceRange Count;
			};

		void AddItemsInt (SItemAddCtx &Ctx) const;
		void AddItemsScaled (SItemAddCtx &Ctx, Metric rAdj) const;
		Metric GetExpectedValue (SItemAddCtx &Ctx, int iLevel) const;
		bool SetsAverageValue (void) const { return m_DesiredValue.GetCount() > 0; }

		TArray<SEntry> m_Table;
		TArray<CCurrencyAndValue> m_DesiredValue;

		mutable TArray<Metric> m_ExpectedValue;			//	Expected average value by level
		mutable bool m_bRecursing;
	};

class CLevelTableOfItemGenerators : public IItemGenerator
	{
	public:
		CLevelTableOfItemGenerators (void) : m_bRecursing(false)
			{ }

		virtual ~CLevelTableOfItemGenerators (void) override;
		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override;
		virtual IItemGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pEntry; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IItemGenerator *pEntry;
			CString sLevelFrequency;
			DiceRange Count;

			mutable int iChance;
			};

		TArray<SEntry> m_Table;
		mutable int m_iTotalChance;
		mutable int m_iComputedLevel;

		mutable bool m_bRecursing;
	};

class CLocationCriteriaTableOfItemGenerators : public IItemGenerator
	{
	public:
		CLocationCriteriaTableOfItemGenerators (void) : m_bRecursing(false)
			{ }

		virtual ~CLocationCriteriaTableOfItemGenerators (void) override;
		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override;
		virtual IItemGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pEntry; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IItemGenerator *pEntry;
			CAffinityCriteria Criteria;
			DiceRange Count;

			mutable int iChance;
			};

		TArray<SEntry> m_Table;
		mutable int m_iTotalChance;
		mutable CSystem *m_pComputedSystem;
		mutable CString m_sComputedAttribs;

		mutable bool m_bRecursing;
	};

class CLookup : public IItemGenerator
	{
	public:
		static ALERROR Create (DWORD dwUNID, IItemGenerator **retpGenerator, CString *retsError = NULL);

		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override { return m_pTable->GetAverageValue(Ctx); }
		virtual IItemGenerator *GetGenerator (int iIndex) override { return m_pTable->GetGenerator(); }
		virtual int GetGeneratorCount (void) override { return ((m_pTable && m_pTable->GetGenerator()) ? 1 : 0); }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		CItemTableRef m_pTable;
	};

class CNullItem : public IItemGenerator
	{
	};

class CRandomItems : public IItemGenerator
	{
	public:
		static ALERROR Create (CUniverse &Universe,
							   const CItemCriteria &Crit, 
							   const CString &sLevelFrequency,
							   IItemGenerator **retpGenerator);

		virtual ~CRandomItems (void) override;
		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override;
		virtual CItemType *GetItemType (int iIndex) override { return m_Table[iIndex].pType; }
		virtual int GetItemTypeCount (void) override { return m_Table.GetCount(); }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			CItemType *pType;
			int iProbability;
			};

		const CTopologyNode &CalcCurrentNode (SItemAddCtx &Ctx) const;
		void InitTable (SItemAddCtx &Ctx) const;

		mutable TArray<SEntry> m_Table;
		mutable CString m_sNodeID;					//	NoteID that we initialized to

		CItemCriteria m_Criteria;
		CString m_sLevelFrequency;
		int m_iLevel = 0;
		int m_iLevelCurve = 0;
		int m_iDamaged = 0;
		CRandomEnhancementGenerator m_Enhanced;
		bool m_bDynamicLevelFrequency = false;		//	If TRUE, level frequency depends on system level
	};

class CSingleItem : public IItemGenerator
	{
	public:
		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override;
		virtual CItemType *GetItemType (int iIndex) override { return m_pItemType; }
		virtual int GetItemTypeCount (void) override { return 1; }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

		static CurrencyValue CalcItemValue (CItemType *pType);

	private:
		CItemTypeRef m_pItemType;
		int m_iDamaged;
		CRandomEnhancementGenerator m_Enhanced;
		bool m_bDebugOnly;
	};

class CTableOfGenerators : public IItemGenerator
	{
	public:
		CTableOfGenerators (void) : m_bRecursing(false)
			{ }

		virtual ~CTableOfGenerators (void) override;
		virtual void AddItems (SItemAddCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) override;
		virtual IItemGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pItem; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IItemGenerator *pItem;
			int iChance;
			DiceRange Count;
			};

		TArray<SEntry> m_Table;
		int m_iTotalChance;

		mutable bool m_bRecursing;
	};
