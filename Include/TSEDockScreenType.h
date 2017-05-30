//	TSEDockScreenType.h
//
//	Classes and functions for dock screen types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CDockScreenType : public CDesignType
	{
	public:
		CDockScreenType (void);
		virtual ~CDockScreenType (void);

		static CString GetStringUNID (CDesignType *pRoot, const CString &sScreen);
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		CXMLElement *GetPane (const CString &sPane);
		static CDesignType *ResolveScreen (CDesignType *pLocalScreen, const CString &sScreen, CString *retsScreenActual = NULL, bool *retbIsLocal = NULL);

		//	CDesignType overrides
		static CDockScreenType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designDockScreen) ? (CDockScreenType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designDockScreen; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		CXMLElement *m_pDesc;
	};

