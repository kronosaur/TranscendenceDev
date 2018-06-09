//	TSETransLisp.h
//
//	Transcendence Lisp Utilities
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	SmartPtr

class ICCItemPtr
	{
	public:
		constexpr ICCItemPtr (void) : m_pPtr(NULL) { }
		constexpr ICCItemPtr (std::nullptr_t) : m_pPtr(NULL) { }

		explicit ICCItemPtr (ICCItem *pPtr) : m_pPtr(pPtr) { }

		ICCItemPtr (const ICCItemPtr &Src);

		ICCItemPtr (ICCItemPtr &&Src) : m_pPtr(Src.m_pPtr)
			{
			Src.m_pPtr = NULL;
			}

		~ICCItemPtr (void);

		ICCItemPtr &operator= (const ICCItemPtr &Src);
		ICCItemPtr &operator= (ICCItem *pSrc);
		operator ICCItem *() const { return m_pPtr; }
		ICCItem * operator->() const { return m_pPtr; }

		explicit operator bool() const { return (m_pPtr != NULL); }

		void Delete (void);

		void TakeHandoff (ICCItem *pPtr);
		void TakeHandoff (ICCItemPtr &Src);

		void Set (const ICCItemPtr &Src)
			{
			*this = Src;
			}

	private:
		ICCItem *m_pPtr;
	};

//	CCodeChainConvert ----------------------------------------------------------
//
//	Helper class to convert from ICCItem to various types.

class CTLispConvert
	{
	public:
		enum ETypes 
			{
			typeNil,

			typeShipClass,
			typeSpaceObject,
			};

		static ETypes ArgType (ICCItem *pItem, ETypes iDefaultType, ICCItem **retpValue = NULL);
		static DWORD AsImageDesc (ICCItem *pItem, RECT *retrcRect);
		static bool AsScreen (ICCItem *pItem, CString *retsScreen = NULL, ICCItemPtr *retpData = NULL, int *retiPriority = NULL);
		static ICCItemPtr CreateCurrencyValue (CCodeChain &CC, CurrencyValue Value);
	};

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
		CCodeChainCtx (void);
		~CCodeChainCtx (void);

		void DefineContainingType (CDesignType *pType);
		void DefineContainingType (const CItem &Item);
		void DefineContainingType (const COverlay *pOverlay);
		void DefineContainingType (CSpaceObject *pObj);
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
		inline void Discard (ICCItem *pItem) { pItem->Discard(&m_CC); }
		DWORD GetAPIVersion (void) const;
		inline CG32bitImage *GetCanvas (void) const { return m_pCanvas; }
		inline CExtension *GetExtension (void) const { return m_pExtension; }
		inline CItemType *GetItemType (void) const { return m_pItemType; }
		inline CDesignType *GetScreensRoot (void) const { return m_pScreensRoot; }
		inline SSystemCreateCtx *GetSystemCreateCtx (void) const { return m_pSysCreateCtx; }
		inline CUniverse &GetUniverse (void) { return *g_pUniverse; }
		inline ICCItem *Link (const CString &sString, int iOffset, int *retiLinked) { return m_CC.Link(sString, iOffset, retiLinked); }
		void RestoreVars (void);
		ICCItem *Run (ICCItem *pCode);
		ICCItem *Run (const SEventHandlerDesc &Event);
		ICCItemPtr RunCode (const SEventHandlerDesc &Event);
		bool RunEvalString (const CString &sString, bool bPlain, CString *retsResult);
		ICCItem *RunLambda (ICCItem *pCode);
		void SaveAndDefineDataVar (ICCItem *pData);
		void SaveAndDefineItemVar (const CItem &Item);
		void SaveAndDefineItemVar (CItemCtx &ItemCtx);
		void SaveAndDefineOverlayID (DWORD dwID);
		void SaveAndDefineSourceVar (CSpaceObject *pSource);
		void SaveAndDefineSovereignVar (CSovereign *pSource);
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

		CCodeChain &m_CC;					//	CodeChain
		ECodeChainEvents m_iEvent;			//	Event raised
		void *m_pScreen;					//	Cast to CDockScreen by upper-levels (may be NULL)
		CG32bitImage *m_pCanvas;			//	Used for dock screen canvas (may be NULL)
		CItemType *m_pItemType;				//	Used for item events (may be NULL)
		CDesignType *m_pScreensRoot;		//	Used to resolve local screens (may be NULL)
		SSystemCreateCtx *m_pSysCreateCtx;	//	Used during system create (may be NULL)
		CExtension *m_pExtension;			//	Extension that defined this code

		IListData *m_pListData;

		//	Saved variables
		ICCItem *m_pOldData;
		ICCItem *m_pOldSource;
		ICCItem *m_pOldItem;
		ICCItem *m_pOldOverlayID;

		bool m_bRestoreGlobalDefineHook;
		IItemTransform *m_pOldGlobalDefineHook;

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
		virtual CString Print (CCodeChain *pCC, DWORD dwFlags = 0) override { return m_pFunction->Print(pCC, dwFlags); }
		virtual void Reset (void) override { }

	protected:
		//	ICCItem virtuals
		virtual void DestroyItem (CCodeChain *pCC) override;
		virtual ICCItem *StreamItem (CCodeChain *pCC, IWriteStream *pStream) override;
		virtual ICCItem *UnstreamItem (CCodeChain *pCC, IReadStream *pStream) override;

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
		virtual ICCItem *Transform (CCodeChain &CC, ICCItem *pItem);

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
		virtual CString Print (CCodeChain *pCC, DWORD dwFlags = 0) override { return CCString::Print(GetStringValue(), dwFlags); }
		virtual void Reset (void) override { }

	protected:
		virtual void DestroyItem (CCodeChain *pCC) override;
		virtual ICCItem *StreamItem (CCodeChain *pCC, IWriteStream *pStream) override;
		virtual ICCItem *UnstreamItem (CCodeChain *pCC, IReadStream *pStream) override;

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
