//	CDockScreenType.cpp
//
//	CDockScreenType class

#include "PreComp.h"

#define PANES_TAG					CONSTLIT("Panes")

CDockScreenType::CDockScreenType (void) : 
		m_pDesc(NULL)

//	CDockScreenType constructor

	{
	}

CDockScreenType::~CDockScreenType (void)

//	CDockScreenType destructor

	{
	}

CXMLElement *CDockScreenType::GetPane (const CString &sPane)

//	GetPane
//
//	Returns a dock screen pane

	{
	//	First check to see if we have it.

	CXMLElement *pPanes;
	if (m_pDesc
			&& (pPanes = m_pDesc->GetContentElementByTag(PANES_TAG)))
		{
		CXMLElement *pPane = pPanes->GetContentElementByTag(sPane);
		if (pPane)
			return pPane;
		}

	//	Otherwise, see if a base class has it.

	CDockScreenType *pBaseClass = CDockScreenType::AsType(GetInheritFrom());
	if (pBaseClass)
		return pBaseClass->GetPane(sPane);

	//	Otherwise, not found

	return NULL;
	}

CString CDockScreenType::GetStringUNID (CDesignType *pRoot, const CString &sScreen)

//	GetStringUNID
//
//	Returns a screen UNID of the form:
//
//	uuuuu
//	uuuuu/localName

	{
	if (pRoot == NULL)
		return sScreen;
	else if (pRoot->GetType() == designDockScreen)
		return strPatternSubst("0x%08x", pRoot->GetUNID());
	else
		return strPatternSubst("0x%08x/%s", pRoot->GetUNID(), sScreen);
	}

void CDockScreenType::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Returns flags to determine how we merge from inherited types.

	{
	//	We know how to handle these tags through the inheritance hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(PANES_TAG), CXMLElement::MERGE_OVERRIDE);
	}

ALERROR CDockScreenType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	m_pDesc = pDesc;
	return NOERROR;
	}

//	CDockScreenTypeRef --------------------------------------------------------

ALERROR CDockScreenTypeRef::Bind (CXMLElement *pLocalScreens)
	{
	SDesignLoadCtx Dummy;
	return Bind(Dummy, pLocalScreens);
	}

ALERROR CDockScreenTypeRef::Bind (SDesignLoadCtx &Ctx, CXMLElement *pLocalScreens)
	{
	if (!m_sUNID.IsBlank())
		{
		bool bNotANumber;
		DWORD dwUNID = (DWORD)strToInt(m_sUNID, 0, &bNotANumber);

		//	If the UNID is not a number, then we're looking for a local screen

		if (bNotANumber)
			{
			if (pLocalScreens == NULL)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("No local screen found: %s"), m_sUNID);
				return ERR_FAIL;
				}

			m_pLocal = pLocalScreens->GetContentElementByTag(m_sUNID);
			if (m_pLocal == NULL)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("No local screen found: %s"), m_sUNID);
				return ERR_FAIL;
				}
			}

		//	Otherwise, look for a global screen

		else
			{
			if (ALERROR error = CDesignTypeRef<CDockScreenType>::BindType(Ctx, dwUNID, m_pType))
				return error;
			}
		}

	return NOERROR;
	}

CXMLElement *CDockScreenTypeRef::GetDesc (void) const
	{
	if (m_pLocal)
		return m_pLocal;
	else if (m_pType)
		return m_pType->GetDesc();
	else
		return NULL;
	}

CDesignType *CDockScreenTypeRef::GetDockScreen (CDesignType *pRoot, CString *retsName) const

//	GetDockScreen
//
//	Returns a root type and a screen name.
//
//	If the root type is a dock screen, then retsName is blank
//	Otherwise, retsName is a screen in the local screens of the root type

	{
	if (m_pLocal)
		{
		if (retsName)
			*retsName = m_sUNID;
		return pRoot;
		}
	else if (m_pType)
		{
		if (retsName)
			*retsName = NULL_STR;
		return m_pType;
		}
	else
		return NULL;
	}

CString CDockScreenTypeRef::GetStringUNID (CDesignType *pRoot) const

//	GetStringUNID
//
//	Returns an UNID of the form:
//
//	uuuuu
//	uuuuu/localName

	{
	CString sScreen;
	CDesignType *pScreen = GetDockScreen(pRoot, &sScreen);
	return CDockScreenType::GetStringUNID(pScreen, sScreen);
	}

void CDockScreenTypeRef::LoadUNID (SDesignLoadCtx &Ctx, const CString &sUNID)
	{
	if (Ctx.pExtension)
		{
		const char *pPos = sUNID.GetASCIIZPointer();
		if (*pPos == '@')
			{
			WORD wLow = LOWORD(Ctx.pExtension->GetUNID()) + (WORD)strParseIntOfBase(pPos+1, 16, 0);
			m_sUNID = strFromInt(MAKELONG(wLow, HIWORD(Ctx.pExtension->GetUNID())));
			}
		else
			m_sUNID = sUNID;
		}
	else
		m_sUNID = sUNID;
	}

