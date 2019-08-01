//	CUIResources.cpp
//
//	CUIResources class

#include "PreComp.h"
#include "Transcendence.h"

void CUIResources::CombineNames (const TArray<CString> &Names, const CString &sSeparator, int cxMaxWidth, const CG16bitFont &Font, TArray<CString> &retLines)

//	CombineNames
//
//	Formats names into an array of lines that fit in max width.

	{
	retLines.DeleteAll();
	if (Names.GetCount() == 0)
		return;

	const int cxSeparator = Font.MeasureText(sSeparator);
	CString sLine;

	sLine = Names[0];
	int cxLine = Font.MeasureText(sLine);

	for (int i = 1; i < Names.GetCount(); i++)
		{
		const int cxName = Font.MeasureText(Names[i]);
		if (cxLine + cxSeparator + cxName <= cxMaxWidth)
			{
			sLine = strPatternSubst(CONSTLIT("%s%s%s"), sLine, sSeparator, Names[i]);
			cxLine += cxSeparator + cxName;
			}
		else
			{
			retLines.Insert(sLine);
			sLine = Names[i];
			cxLine = cxName;
			}
		}

	if (!sLine.IsBlank())
		retLines.Insert(sLine);
	}

void CUIResources::CreateTitleAnimation (int x, int y, int iDuration, IAnimatron **retpAni) const

//	CreateTitleAnimation
//
//	Creates an animation of the main title

	{
	CAniText *pText = new CAniText;
	pText->SetPropertyVector(CONSTLIT("position"), CVector((Metric)x, (Metric)y));
	pText->SetPropertyColor(CONSTLIT("color"), m_pFonts->rgbTitleColor);
	pText->SetPropertyString(CONSTLIT("text"), CONSTLIT("TRANSCENDENCE"));

	pText->SetPropertyFont(CONSTLIT("font"), &m_pFonts->LogoTitle);
	pText->SetFontFlags(CG16bitFont::AlignCenter);

	pText->AnimateLinearFade(iDuration, 30, 30);

	//	Done

	*retpAni = pText;
	}

void CUIResources::CreateLargeCredit (const CString &sCredit, const CString &sName, int x, int y, int iDuration, IAnimatron **retpAni) const

//	CreateLargeCredit
//
//	Creates an animation of a title credit

	{
	CAniText *pCredit = new CAniText;
	pCredit->SetPropertyVector(CONSTLIT("position"), CVector((Metric)x, (Metric)y));
	pCredit->SetPropertyColor(CONSTLIT("color"), m_pFonts->rgbTitleColor);
	pCredit->SetPropertyString(CONSTLIT("text"), sCredit);

	pCredit->SetPropertyFont(CONSTLIT("font"), &m_pFonts->Header);
	pCredit->SetFontFlags(CG16bitFont::AlignCenter);

	pCredit->AnimateLinearFade(iDuration, 30, 30);

	CAniText *pName = new CAniText;
	pName->SetPropertyVector(CONSTLIT("position"), CVector((Metric)x, (Metric)(y + m_pFonts->Header.GetHeight())));
	pName->SetPropertyColor(CONSTLIT("color"), m_pFonts->rgbTitleColor);
	pName->SetPropertyString(CONSTLIT("text"), sName);

	pName->SetPropertyFont(CONSTLIT("font"), &m_pFonts->Title);
	pName->SetFontFlags(CG16bitFont::AlignCenter);

	pName->AnimateLinearFade(iDuration, 30, 30);

	//	Add them both to a sequencer

	CAniSequencer *pSeq = new CAniSequencer;
	pSeq->AddTrack(pCredit, 0);
	pSeq->AddTrack(pName, 0);

	//	Done

	*retpAni = pSeq;
	}

void CUIResources::CreateMediumCredit (const CString &sCredit, const TArray<CString> &Names, int x, int y, int iDuration, IAnimatron **retpAni) const

//	CreateMediumCredit
//
//	Creates a credit animation

	{
	//	Combine names into lines

	TArray<CString> Lines;
	CombineNames(Names, strPatternSubst(CONSTLIT(" %&bull; ")), CREDIT_LINE_MAX_WIDTH, m_pFonts->SubTitle, Lines);

	//	Create sequencer

	CAniSequencer *pSeq = new CAniSequencer;

	//	The credit label lasts for the whole time

	CAniText *pCredit = new CAniText;
	pCredit->SetPropertyVector(CONSTLIT("position"), CVector((Metric)x, (Metric)y));
	pCredit->SetPropertyColor(CONSTLIT("color"), m_pFonts->rgbLightTitleColor);
	pCredit->SetPropertyString(CONSTLIT("text"), sCredit);

	pCredit->SetPropertyFont(CONSTLIT("font"), &m_pFonts->Header);
	pCredit->SetFontFlags(CG16bitFont::AlignCenter);

	pCredit->AnimateLinearFade(iDuration * Lines.GetCount(), 20, 20);

	pSeq->AddTrack(pCredit, 0);

	//	Each line of names comes and goes

	int iTime = 0;
	for (int i = 0; i < Lines.GetCount(); i++)
		{
		CAniText *pLine = new CAniText;
		pLine->SetPropertyVector(CONSTLIT("position"), CVector((Metric)x, (Metric)(y + m_pFonts->SubTitle.GetHeight())));
		pLine->SetPropertyColor(CONSTLIT("color"), m_pFonts->rgbTitleColor);
		pLine->SetPropertyString(CONSTLIT("text"), Lines[i]);

		pLine->SetPropertyFont(CONSTLIT("font"), &m_pFonts->SubTitle);
		pLine->SetFontFlags(CG16bitFont::AlignCenter);

		pLine->AnimateLinearFade(iDuration, 20, 20);

		pSeq->AddTrack(pLine, iTime);
		iTime += iDuration;
		}

	//	Done

	*retpAni = pSeq;
	}

ALERROR CUIResources::Init (SFontTable *pFonts)

//	Init
//
//	Initializes the resources

	{
	//	Fonts

	m_pFonts = pFonts;

	return NOERROR;
	}
