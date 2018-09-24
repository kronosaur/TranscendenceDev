//	TSEDesignDefs.h
//
//	Transcendence design definitions (used by TSEDesign.h classes).
//	Copyright 2018 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Dock Screen System ---------------------------------------------------------

class CDockScreenSys
	{
	public:
		struct SSelector
			{
			CString sScreen;						//	Screen to show
			ICCItemPtr pData;						//	Data for screen

			int iPriority = 0;						//	Priority
			bool bOverrideOnly = false;				//	Valid only if we have a screen already
			};
	};
