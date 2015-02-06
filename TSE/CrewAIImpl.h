//	CrewAIImpl.h
//
//	CCrewAI class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCrewAI : public CBaseShipAI
	{
	public:
		CCrewAI (void);

		//	IShipController virtuals

		virtual CString DebugCrashInfo (void);
		virtual CString GetClass (void) { return CONSTLIT("crew"); }

	protected:

		//	CBaseShipAI overrides

		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:

	};

