//	TSEPlayer.h
//
//	Player classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#ifndef INCL_TSE_PLAYER
#define INCL_TSE_PLAYER

class IPlayerController
	{
	public:
		IPlayerController (void);
		virtual ~IPlayerController (void) { }

		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		//	IPlayerController interface

		virtual ICCItem *CreateGlobalRef (CCodeChain &CC) { return CC.CreateInteger((int)this); }
		virtual GenomeTypes GetGenome (void) const { return genomeUnknown; }
		virtual CString GetName (void) const { return NULL_STR; }
		virtual bool GetPropertyInteger (const CString &sProperty, int *retiValue) { return false; }
		virtual bool GetPropertyItemList (const CString &sProperty, CItemList *retItemList) { return false; }
		virtual bool GetPropertyString (const CString &sProperty, CString *retsValue) { return false; }
		virtual CSovereign *GetSovereign (void) const;
		virtual void OnMessageFromObj (CSpaceObject *pSender, const CString &sMessage) { }
		virtual bool SetPropertyInteger (const CString &sProperty, int iValue) { return false; }
		virtual bool SetPropertyItemList (const CString &sProperty, const CItemList &ItemList) { return false; }
		virtual bool SetPropertyString (const CString &sProperty, const CString &sValue) { return false; }

	protected:

		//	IPlayerController interface

		void OnReadFromStream (SUniverseLoadCtx &Ctx) { }
		void OnWriteToStream (IWriteStream *pStream) { }

		mutable CSovereign *m_pSovereign;			//	Cached sovereign
	};

#endif

