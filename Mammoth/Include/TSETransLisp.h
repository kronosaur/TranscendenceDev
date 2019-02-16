//	TSETransLisp.h
//
//	Transcendence Lisp Utilities
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	CodeChain context

enum ECodeChainEvents
	{
	eventNone =							0,
	eventOnAIUpdate =					1,
	eventOnUpdate =						2,
	eventGetName =						3,
	eventGetTradePrice =				4,
	eventDoEvent =						5,
	eventObjFireEvent =					6,
	eventOnGlobalTypesInit =			7,
	eventGetGlobalPlayerPriceAdj =		8,
	eventGetDescription =				9,
	eventInitDockScreenList =			10,		//	Code inside <ListOptions> to (e.g.) set list filter
	eventOverlayEvent =					11,
	eventGetReferenceText =				12,		//	Item reference text
	eventOnDestroyCheck =				13,
	eventGetHullPrice =					14,
	};

class CCodeChainCtx
	{
	public:
		CCodeChainCtx (CUniverse &Universe);
		~CCodeChainCtx (void);

		ICCItemPtr Create (ICCItem::ValueTypes iType);
		void DefineContainingType (const CDesignType *pType);
		void DefineContainingType (const CItem &Item);
		void DefineContainingType (const COverlay *pOverlay);
		void DefineContainingType (const CSpaceObject *pObj);
		inline ICCItem *CreateNil (void) { return m_CC.CreateNil(); }
		inline void DefineBool (const CString &sVar, bool bValue) { m_CC.DefineGlobal(sVar, (bValue ? m_CC.CreateTrue() : m_CC.CreateNil())); }
		void DefineDamageCtx (const SDamageCtx &Ctx, int iDamage = -1);
		void DefineDamageEffects (const CString &sVar, SDamageCtx &Ctx);
		inline void DefineInteger (const CString &sVar, int iValue) { m_CC.DefineGlobalInteger(sVar, iValue); }
		void DefineItem (const CItem &Item);
		void DefineItem (const CString &sVar, const CItem &Item);
		void DefineItem (const CString &sVar, CItemCtx &ItemCtx);
		void DefineItemType (const CString &sVar, CItemType *pType);
		inline void DefineNil (const CString &sVar) { m_CC.DefineGlobal(sVar, m_CC.CreateNil()); }
		void DefineOrbit (const CString &sVar, const COrbit &OrbitDesc);
		void DefineSource (CSpaceObject *pSource);
		void DefineSpaceObject (const CString &sVar, CSpaceObject *pObj);
		inline void DefineString (const CString &sVar, const CString &sValue) { m_CC.DefineGlobalString(sVar, sValue); }
		inline void DefineVar (const CString &sVar, ICCItem *pValue) { m_CC.DefineGlobal(sVar, pValue); }
		void DefineVector (const CString &sVar, const CVector &vVector);
		inline void Discard (ICCItem *pItem) { pItem->Discard(); }
		DWORD GetAPIVersion (void) const;
		inline CG32bitImage *GetCanvas (void) const { return m_pCanvas; }
		inline CCodeChain &GetCC (void) { return m_CC; }
		inline CExtension *GetExtension (void) const { return m_pExtension; }
		inline CItemType *GetItemType (void) const { return m_pItemType; }
		inline CDesignType *GetScreensRoot (void) const { return m_pScreensRoot; }
		inline SSystemCreateCtx *GetSystemCreateCtx (void) const { return m_pSysCreateCtx; }
		inline CUniverse &GetUniverse (void) const { return m_Universe; }
		ICCItemPtr LinkCode (const CString &sString, CCodeChain::SLinkOptions &Options = CCodeChain::SLinkOptions());
		void RestoreVars (void);
		ICCItem *Run (ICCItem *pCode);
		ICCItem *Run (const SEventHandlerDesc &Event);
		ICCItemPtr RunCode (ICCItem *pCode);
		ICCItemPtr RunCode (const SEventHandlerDesc &Event);
		bool RunEvalString (const CString &sString, bool bPlain, CString *retsResult);
		ICCItem *RunLambda (ICCItem *pCode);
		ICCItemPtr RunLambdaCode (ICCItem *pCode, ICCItem *pArgs = NULL);
		void SaveAndDefineDataVar (ICCItem *pData);
		void SaveAndDefineItemVar (const CItem &Item);
		void SaveAndDefineItemVar (CItemCtx &ItemCtx);
		void SaveAndDefineOverlayID (DWORD dwID);
		void SaveAndDefineSourceVar (const CSpaceObject *pSource);
		void SaveAndDefineSovereignVar (CSovereign *pSource);
		void SaveAndDefineType (DWORD dwUNID);
		void SaveItemVar (void);
		void SaveSourceVar (void);
		inline void SetDockScreenList (IListData *pListData) { m_pListData = pListData; }
		inline void SetCanvas (CG32bitImage *pCanvas) { m_pCanvas = pCanvas; }
		void SetEvent (ECodeChainEvents iEvent);
		inline void SetExtension (CExtension *pExtension) { m_pExtension = pExtension; }
		void SetGlobalDefineWrapper (CExtension *pExtension);
		inline void SetItemType (CItemType *pType) { m_pItemType = pType; }
		inline void SetScreen (void *pScreen) { m_pScreen = pScreen; }
		inline void SetScreensRoot (CDesignType *pRoot) { m_pScreensRoot = pRoot; }
		inline void SetSystemCreateCtx (SSystemCreateCtx *pCtx) { m_pSysCreateCtx = pCtx; }

		C3DObjectPos As3DObjectPos (CSpaceObject *pObj, ICCItem *pItem, bool bAsoluteRotation = false);
		bool AsArc (ICCItem *pItem, int *retiMinArc, int *retiMaxArc, bool *retbOmnidirectional = NULL);
		CItemType *AsItemType (ICCItem *pItem) const;
		DWORD AsNameFlags (ICCItem *pItem);
		CSpaceObject *AsSpaceObject (ICCItem *pItem);
		CVector AsVector (ICCItem *pItem);

		static bool InEvent (ECodeChainEvents iEvent);

	private:
		struct SInvokeFrame
			{
			ECodeChainEvents iEvent;		//	Event raised

			//	If we're viewing a dock screen list
			IListData *pListData;
			};

		void AddFrame (void);
		void RemoveFrame (void);

		CUniverse &m_Universe;						//	Universe
		CCodeChain &m_CC;							//	CodeChain
		ECodeChainEvents m_iEvent = eventNone;		//	Event raised
		void *m_pScreen = NULL;						//	Cast to CDockScreen by upper-levels (may be NULL)
		CG32bitImage *m_pCanvas = NULL;				//	Used for dock screen canvas (may be NULL)
		CItemType *m_pItemType = NULL;				//	Used for item events (may be NULL)
		CDesignType *m_pScreensRoot = NULL;			//	Used to resolve local screens (may be NULL)
		SSystemCreateCtx *m_pSysCreateCtx = NULL;	//	Used during system create (may be NULL)
		CExtension *m_pExtension = NULL;			//	Extension that defined this code

		IListData *m_pListData = NULL;

		//	Saved variables
		ICCItem *m_pOldData = NULL;
		ICCItem *m_pOldSource = NULL;
		ICCItem *m_pOldItem = NULL;
		ICCItem *m_pOldOverlayID = NULL;
		ICCItem *m_pOldType = NULL;

		bool m_bRestoreGlobalDefineHook = false;
		IItemTransform *m_pOldGlobalDefineHook = NULL;

		static TArray<SInvokeFrame> g_Invocations;
	};

class CFunctionContextWrapper : public ICCAtom
	{
	public:
		CFunctionContextWrapper (ICCItem *pFunction);

		inline void SetExtension (CExtension *pExtension) { m_pExtension = pExtension; }

		//	ICCItem virtuals
		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *Execute (CEvalContext *pCtx, ICCItem *pArgs) override;
		virtual CString GetHelp (void) override { return NULL_STR; }
		virtual CString GetStringValue (void) override { return m_pFunction->GetStringValue(); }
		virtual ValueTypes GetValueType (void) override { return Function; }
		virtual bool IsIdentifier (void) override { return false; }
		virtual bool IsFunction (void) override { return true; }
		virtual bool IsLambdaFunction (void) override { return true; }
		virtual bool IsPrimitive (void) override { return false; }
		virtual CString Print (DWORD dwFlags = 0) override { return m_pFunction->Print(dwFlags); }
		virtual void Reset (void) override { }

	protected:
		//	ICCItem virtuals
		virtual void DestroyItem (void) override;

	private:
		ICCItem *m_pFunction;
		CExtension *m_pExtension;
	};

class CAddFunctionContextWrapper : public IItemTransform
	{
	public:
		CAddFunctionContextWrapper (void) : m_pExtension(NULL) { }

		inline void SetExtension (CExtension *pExtension) { m_pExtension = pExtension; }

		//	IItemTransform
		virtual ICCItem *Transform (ICCItem *pItem) override;

	private:
		CExtension *m_pExtension;
	};

class CCXMLWrapper : public ICCAtom
	{
	public:
		CCXMLWrapper (CXMLElement *pXML, ICCItem *pRef = NULL);

		inline CXMLElement *GetXMLElement (void) { return m_pXML; }

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual CString GetStringValue (void) override { return m_pXML->ConvertToString(); }
		virtual CString GetTypeOf (void) override { return CONSTLIT("xmlElement"); }
		virtual ValueTypes GetValueType (void) override { return Complex; }
		virtual bool IsIdentifier (void) override { return false; }
		virtual bool IsFunction (void) override { return false; }
		virtual bool IsPrimitive (void) override { return false; }
		virtual CString Print (DWORD dwFlags = 0) override { return CCString::Print(GetStringValue(), dwFlags); }
		virtual void Reset (void) override { }

	protected:
		virtual void DestroyItem (void) override;

	private:
		CXMLElement *m_pXML;
		ICCItem *m_pRef;
	};

class CAttributeDataBlock
	{
	public:
        enum ETransferOptions
            {
            transCopy,
            transIgnore,
            };

        struct STransferDesc
            {
            STransferDesc (void) :
                    iOption(transCopy)
                { }

            ETransferOptions iOption;
            };

		CAttributeDataBlock (void);
		CAttributeDataBlock (const CAttributeDataBlock &Src);
		CAttributeDataBlock &operator= (const CAttributeDataBlock &Src);
		~CAttributeDataBlock (void);

		void Copy (const CAttributeDataBlock &Src, const TSortMap<CString, STransferDesc> &Options);
		inline void DeleteAll (void) { CleanUp(); }
//		bool FindData (const CString &sAttrib, const CString **retpData = NULL) const;
		bool FindDataAsItem (const CString &sAttrib, ICCItemPtr &pResult) const;
		bool FindObjRefData (CSpaceObject *pObj, CString *retsAttrib = NULL) const;
		ICCItemPtr GetData (int iIndex) const;
		ICCItemPtr GetDataAsItem (const CString &sAttrib) const;
		inline const CString &GetDataAttrib (int iIndex) const { return m_Data.GetKey(iIndex); }
		inline int GetDataCount (void) const { return m_Data.GetCount(); }
		CSpaceObject *GetObjRefData (const CString &sAttrib) const;
        ICCItemPtr IncData (const CString &sAttrib, ICCItem *pValue = NULL);
		bool IsDataNil (const CString &sAttrib) const;
		inline bool IsEmpty (void) const { return (m_Data.GetCount() == 0 && m_pObjRefData == NULL); }
		bool IsEqual (const CAttributeDataBlock &Src);
		void LoadObjReferences (CSystem *pSystem);
		void MergeFrom (const CAttributeDataBlock &Src);
		void OnObjDestroyed (CSpaceObject *pObj);
		void OnSystemChanged (CSystem *pSystem);
		void ReadFromStream (SLoadCtx &Ctx);
		void ReadFromStream (IReadStream *pStream);
		void SetData (const CString &sAttrib, ICCItem *pItem);
		void SetFromXML (CXMLElement *pData);
		void SetObjRefData (const CString &sAttrib, CSpaceObject *pObj);
		void WriteToStream (IWriteStream *pStream, CSystem *pSystem = NULL);

		static const CAttributeDataBlock Null;

	private:
        struct SDataEntry
            {
            ICCItemPtr pData;
            };

		struct SObjRefEntry
			{
			CString sName;
			CSpaceObject *pObj;
			DWORD dwObjID;

			SObjRefEntry *pNext;
			};

		void CleanUp (void);
        void CleanUpObjRefs (void);
		void Copy (const CAttributeDataBlock &Copy);
        void CopyObjRefs (SObjRefEntry *pSrc);
		bool IsXMLText (const CString &sData) const;
        void ReadDataEntries (IReadStream *pStream);

        TSortMap<CString, SDataEntry> m_Data;
		SObjRefEntry *m_pObjRefData;			//	Custom pointers to CSpaceObject *
	};
