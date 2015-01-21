//	TSEPlayer.h
//
//	Player classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#ifndef INCL_TSE_PLAYER
#define INCL_TSE_PLAYER

class IPlayerController
	{
	public:
		virtual ~IPlayerController (void) { }

		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		//	IPlayerController interface

		virtual ICCItem *CreateGlobalRef (CCodeChain &CC) { return CC.CreateInteger((int)this); }
		virtual GenomeTypes GetGenome (void) const { return genomeUnknown; }
		virtual CString GetName (void) const { return NULL_STR; }

	protected:

		//	IPlayerController interface

		void OnReadFromStream (SUniverseLoadCtx &Ctx) { }
		void OnWriteToStream (IWriteStream *pStream) { }
	};

#endif

