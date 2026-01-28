//	CItemPainter.cpp
//
//	CItemPainter class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

int CItemPainter::CalcItemEntryHeight (int cxWidth) const

//	CalcItemEntryHeight
//
//	Returns the height of the item, given the width.

	{
	if (m_pItem == NULL || m_pItem->GetType() == NULL)
		return ITEM_DEFAULT_HEIGHT;

	const CG16bitFont &LargeBold = m_VI.GetFont(fontLargeBold);
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);

	//	Get the item

	CItemCtx Ctx(m_pItem, m_pItem->GetSource());
	const CItemType &Type = *m_pItem->GetType();

	//	Compute the height of the row

	int cyHeight = 0;

	//	Account for margin

	cyHeight += (m_Options.bNoPadding ? 0 : ITEM_TEXT_MARGIN_Y);

	//	Item title

	cyHeight += LargeBold.GetHeight();
	if (m_Options.bTitle)
		cyHeight += ITEM_TITLE_EXTRA_MARGIN;

	//	Attributes

	if (!m_AttribBlock.IsEmpty())
		cyHeight += m_AttribBlock.GetHeight() + ATTRIB_SPACING_Y;

	//	Reference

	DWORD dwRefFlags = (m_Options.bDisplayAsKnown ? CItemType::FLAG_ACTUAL_ITEM : 0);
	CString sReference = m_pItem->GetReference(Ctx, CItem(), dwRefFlags);

	//	If this is a weapon, then add room for the weapon damage

	if (m_pItem->GetReferenceDamageType(Ctx, CItem(), dwRefFlags, NULL, NULL))
		cyHeight += Medium.GetHeight();

	//	If this is armor or a shield, then add room for damage resistance

	else if (m_pItem->GetReferenceDamageAdj(m_pItem->GetSource(), dwRefFlags, NULL, NULL))
		cyHeight += Medium.GetHeight();

	//	Launchers

	if (!m_Launchers.IsEmpty())
		cyHeight += m_Launchers.GetHeight();

	//	Measure the reference text

	int iLines;
	if (!sReference.IsBlank())
		{
		iLines = Medium.BreakText(sReference, RectWidth(m_rcDraw), NULL, 0);
		cyHeight += iLines * Medium.GetHeight();
		}

	//	Measure the description

	CString sDesc = m_pItem->GetDesc(m_Options.bDisplayAsKnown);
	iLines = Medium.BreakText(sDesc, RectWidth(m_rcDraw), NULL, 0);
	cyHeight += iLines * Medium.GetHeight();

	//	Margin

	cyHeight += (m_Options.bNoPadding ? 0 : ITEM_TEXT_MARGIN_BOTTOM);

	if (!m_Options.bNoIcon)
		cyHeight = Max(m_cyIcon, cyHeight);

	//	Enhancements

	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();
	if (pEnhancements)
		{
		for (int i = 0; i < pEnhancements->GetCount(); i++)
			{
			cyHeight += Max(ENHANCEMENT_ICON_HEIGHT, 2 * Medium.GetHeight());
			}
		}

	//	Done

	return cyHeight;
	}

void CItemPainter::FormatDisplayAttributes (const CVisualPalette &VI, TArray<SDisplayAttribute> &Attribs, const RECT &rcRect, CCartoucheBlock &retBlock, int *retcyHeight)

//	FormatDisplayAttributes
//
//	Formats the display attributes into a cartouche block.

	{
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	for (int i = 0; i < Attribs.GetCount(); i++)
		{
		CG32bitPixel rgbBackColor = Attribs[i].rgbColor;
		CG32bitPixel rgbTextColor = Attribs[i].rgbTextColor;

		//	Figure out the colors

		bool bNeedBackColor = !rgbBackColor.GetAlpha();
		bool bNeedTextColor = !rgbTextColor.GetAlpha();

		if (bNeedBackColor || bNeedTextColor)
			{

			switch (Attribs[i].iType)
				{
				case attribPositive:
				case attribEnhancement:
					rgbBackColor = bNeedBackColor ? VI.GetColor(colorAreaAdvantage) : rgbBackColor;
					rgbTextColor = bNeedTextColor ? VI.GetColor(colorTextAdvantage) : rgbTextColor;
					break;

				case attribNegative:
				case attribWeakness:
					rgbBackColor = bNeedBackColor ? VI.GetColor(colorAreaDisadvantage) : rgbBackColor;
					rgbTextColor = bNeedTextColor ? VI.GetColor(colorTextDisadvantage) : rgbTextColor;
					break;

				default:
					rgbBackColor = bNeedBackColor ? RGB_MODIFIER_NORMAL_BACKGROUND : rgbBackColor;
					rgbTextColor = bNeedTextColor ? RGB_MODIFIER_NORMAL_TEXT : rgbTextColor;
					break;
				}

			}

		//	Add to block

		retBlock.AddCartouche(Attribs[i].sText, rgbTextColor, rgbBackColor);
		}

	//	Format

	retBlock.SetFont(&Medium);
	retBlock.Format(RectWidth(rcRect));

	//	Done

	if (retcyHeight)
		*retcyHeight = RectHeight(retBlock.GetBounds());
	}

void CItemPainter::FormatLaunchers (const CVisualPalette &VI, const CMissileItem &MissileItem, const TArray<CItem> &Launchers, const RECT &rcRect, CIconLabelBlock &retLaunchers)

//	FormatLaunchers
//
//	Format the list of launchers for this missile.

	{
	retLaunchers.DeleteAll();

	for (int i = 0; i < Launchers.GetCount(); i++)
		{
		const CItem &Launcher = Launchers[i];

		//	Skip virtual launchers

		if (Launcher.IsVirtual())
			continue;

		//	Generate a launcher icon.

		const CObjectImageArray &Image = Launchers[i].GetImage();

		CIconLabelBlock::SLabelDesc NewEntry;
		NewEntry.pIcon = TSharedPtr<CG32bitImage>(new CG32bitImage);
		NewEntry.pIcon->Create(LAUNCHER_ICON_WIDTH, LAUNCHER_ICON_HEIGHT, CG32bitImage::alpha8, CG32bitPixel::Null());
		Image.PaintScaledImage(*NewEntry.pIcon, 0, 0, 0, 0, LAUNCHER_ICON_WIDTH, LAUNCHER_ICON_HEIGHT, CObjectImageArray::FLAG_UPPER_LEFT);

		//	Add the text

		NewEntry.sText = MissileItem.GetLaunchedByText(Launcher);

		//	Add it

		NewEntry.bNewLine = true;
		retLaunchers.Add(NewEntry);
		}

	retLaunchers.SetFont(VI.GetFont(fontMedium));
	retLaunchers.Format(RectWidth(rcRect));
	}

void CItemPainter::Init (const CItem &Item, int cxWidth, const SOptions &Options)

//	Init
//
//	Initializes the painter with the given item and options.

	{
	if (Item.IsEmpty())
		{
		ASSERT(false);
		return;
		}

	m_pItem = &Item;
	m_Options = Options;
	m_cxWidth = cxWidth;

	InitMetrics(cxWidth);

	//	Initialize attributes

	TArray<SDisplayAttribute> Attribs;
	if (m_pItem->GetDisplayAttributes(&Attribs, NULL, m_Options.bDisplayAsKnown))
		FormatDisplayAttributes(m_VI, Attribs, m_rcDraw, m_AttribBlock);
	else
		m_AttribBlock = CCartoucheBlock();

	//	Init launcher lines

	m_Launchers = CIconLabelBlock();
	if (m_Options.bDisplayAsKnown || Item.IsKnown())
		{
		if (const CMissileItem MissileItem = Item.AsMissileItem())
			FormatLaunchers(m_VI, MissileItem, MissileItem.GetLaunchWeapons(), m_rcDraw, m_Launchers);
		}

	//	Compute height

	m_cyHeight = CalcItemEntryHeight(cxWidth);
	}

void CItemPainter::InitMetrics (int cxWidth)

//	InitMetrics
//
//	Initializes rects and metrics

	{
	m_cxIcon = (m_Options.bSmallIcon ? SMALL_ICON_WIDTH : ICON_WIDTH);
	m_cyIcon = (m_Options.bSmallIcon ? SMALL_ICON_HEIGHT : ICON_HEIGHT);

	m_rcDraw.left = 0;
	m_rcDraw.right = cxWidth;
	m_rcDraw.top = 0;
	m_rcDraw.bottom = ITEM_DEFAULT_HEIGHT;
	if (!m_Options.bNoPadding)
		{
		m_rcDraw.left += ITEM_TEXT_MARGIN_X;
		m_rcDraw.right -= ITEM_TEXT_MARGIN_X + ITEM_RIGHT_PADDING;
		}

	if (!m_Options.bNoIcon)
		{
		if (m_Options.bNoPadding)
			m_rcDraw.left += m_cxIcon + ITEM_TEXT_MARGIN_X;
		else
			m_rcDraw.left += ITEM_LEFT_PADDING + m_cxIcon;
		}
	}

void CItemPainter::Paint (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbTextColor, DWORD dwOptions) const

//	Paint
//
//	Paints the item.

	{
	if (m_pItem == NULL || m_pItem->GetType() == NULL)
		return;

	CUniverse &Universe = m_pItem->GetUniverse();

	bool bSelected = ((dwOptions & OPTION_SELECTED) ? true : false);
	bool bDisabled = ((dwOptions & OPTION_DISABLED) ? true : false);

	const CG16bitFont &LargeBold = m_VI.GetFont(fontLargeBold);
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);
    CG32bitPixel rgbColorTitle = (bDisabled ? CG32bitPixel(80, 80, 80) : rgbTextColor);
    CG32bitPixel rgbColorRef = rgbColorTitle;
	CG32bitPixel rgbColorDescSel = (bDisabled ? rgbColorTitle : CG32bitPixel(200,200,200));
	CG32bitPixel rgbColorDesc = (bDisabled ? rgbColorTitle : CG32bitPixel(128,128,128));
	CG32bitPixel rgbDisadvantage = m_VI.GetColor(colorTextDisadvantage);

	//	Icons size

	int cxIcon = (m_Options.bSmallIcon ? SMALL_ICON_WIDTH : ICON_WIDTH);
	int cyIcon = (m_Options.bSmallIcon ? SMALL_ICON_HEIGHT : ICON_HEIGHT);

	//	Item context

	CItemCtx Ctx(m_pItem, m_pItem->GetSource());
	const CItemType &ItemType = *m_pItem->GetType();

	//	Calc the rect where we will draw

	RECT rcDrawRect;
	rcDrawRect.left = x;
	rcDrawRect.top = y;
	rcDrawRect.right = x + m_cxWidth;
	rcDrawRect.bottom = y + m_cyHeight;

	if (!m_Options.bNoPadding)
		{
		rcDrawRect.left += ITEM_TEXT_MARGIN_X;
		rcDrawRect.right -= ITEM_TEXT_MARGIN_X + ITEM_RIGHT_PADDING;
		rcDrawRect.top += ITEM_TEXT_MARGIN_Y;
		}

	//	Paint the image

	if (!m_Options.bNoIcon)
		{
		int xIcon = (m_Options.bNoPadding ? x : x + ITEM_LEFT_PADDING);
		DrawItemTypeIcon(Dest, xIcon, y, &ItemType, cxIcon, cyIcon, bDisabled, m_Options.bDisplayAsKnown);
		rcDrawRect.left = xIcon + cxIcon + ITEM_TEXT_MARGIN_X;
		}

	//	Paint the item name

	DWORD dwNounPhraseFlags = nounNoModifiers;
	if (m_Options.bTitle)
		dwNounPhraseFlags |= nounTitleCapitalize | nounShort;
	else
		dwNounPhraseFlags |= nounCount;
	if (m_Options.bDisplayAsKnown)
		dwNounPhraseFlags |= nounActual;

	int cyHeight;
	RECT rcTitle = rcDrawRect;
	LargeBold.DrawText(Dest,
			rcTitle,
			rgbColorTitle,
			m_pItem->GetNounPhrase(dwNounPhraseFlags),
			0,
			CG16bitFont::SmartQuotes | CG16bitFont::TruncateLine,
			&cyHeight);

	rcDrawRect.top += cyHeight;
	if (m_Options.bTitle)
		rcDrawRect.top += ITEM_TITLE_EXTRA_MARGIN;

	//	Paint the display attributes

	if (!m_AttribBlock.IsEmpty())
		{
		CCartoucheBlock::SPaintOptions Options;
		Options.bDisabled = bDisabled;

		m_AttribBlock.Paint(Dest, rcDrawRect.left, rcDrawRect.top, Options);
		rcDrawRect.top += m_AttribBlock.GetHeight() + ATTRIB_SPACING_Y;
		}

	//	Stats

	CString sStat;
	DWORD dwRefFlags = (m_Options.bDisplayAsKnown ? CItemType::FLAG_ACTUAL_ITEM : 0);

	int iLevel = (m_Options.bDisplayAsKnown ? m_pItem->GetLevel() : m_pItem->GetApparentLevel());
	CString sReference = m_pItem->GetReference(Ctx, CItem(), dwRefFlags);
	DamageTypes iDamageType;
	CString sDamageRef;
	int iDamageAdj[damageCount];
	int iHP;

	if (m_pItem->GetReferenceDamageType(Ctx, CItem(), dwRefFlags, &iDamageType, &sDamageRef))
		{
		//	Paint the damage type reference		

		PaintReferenceDamageType(m_VI,
				Dest,
				rcDrawRect.left,
				rcDrawRect.top,
				iDamageType,
				sDamageRef,
                rgbColorRef,
				dwOptions);

		rcDrawRect.top += Medium.GetHeight();

		//	Paint additional reference in the line below

		if (!sReference.IsBlank())
			{
			Medium.DrawText(Dest, 
					rcDrawRect,
					rgbColorRef,
					sReference,
					0,
					0,
					&cyHeight);

			rcDrawRect.top += cyHeight;
			}
		}
	else if (m_pItem->GetReferenceDamageAdj(m_pItem->GetSource(), dwRefFlags, &iHP, iDamageAdj))
		{
		//	Paint the initial text

		sStat = strPatternSubst("hp: %d ", iHP);
		int cxWidth = Medium.MeasureText(sStat, &cyHeight);
		Medium.DrawText(Dest, 
				rcDrawRect,
				rgbColorRef,
				sStat,
				0,
				0,
				&cyHeight);

		//	Paint the damage type array

		PaintReferenceDamageAdj(m_VI,
				Dest,
				rcDrawRect.left + cxWidth + DAMAGE_ADJ_SPACING_X,
				rcDrawRect.top,
				iLevel,
				iHP,
				iDamageAdj,
				rgbColorRef,
				rgbDisadvantage,
				dwOptions,
				m_pItem->IsArmor() ? Universe.GetArmorDamageAdj(iLevel) : Universe.GetShieldDamageAdj(iLevel));

		rcDrawRect.top += cyHeight;

		//	If we have a speed bonus/penalty for armor, then show it.
		//	Usually we show penalty relative to the player.

		CSpaceObject *pBonusSource = Universe.GetPlayerShip();
		CItemCtx BonusCtx(m_pItem, pBonusSource);

		int iSpeedBonus;
		if (!m_Options.bNoArmorSpeed && m_pItem->GetReferenceSpeedBonus(BonusCtx, dwRefFlags, &iSpeedBonus))
			{
			//	Figure out what kind of bonus we want.

			CG32bitPixel rgbBonus = rgbColorRef;
			CString sBonus;
			if (iSpeedBonus == 0)
				{
				sBonus = CONSTLIT("too heavy");
				rgbBonus = (bDisabled ? rgbColorRef : rgbDisadvantage);
				}
			else if (iSpeedBonus > 0)
				{
				sBonus = strPatternSubst(CONSTLIT("+.%02dc bonus"), iSpeedBonus);
				}
			else
				{
				sBonus = strPatternSubst(CONSTLIT("-.%02dc penalty"), -iSpeedBonus);
				rgbBonus = (bDisabled ? rgbColorRef : rgbDisadvantage);
				}

			//	If we have a different color, then paint with RTF; otherwise,
			//	normal paint.

			if (rgbBonus != rgbColorRef)
				{
				sReference = strPatternSubst(CONSTLIT("{/rtf %s %&mdash; {/c:%d; %s}}"), CTextBlock::Escape(sReference), (COLORREF)rgbBonus, CTextBlock::Escape(sBonus));
				CUIHelper::PaintRTFText(m_VI, Dest, rcDrawRect, sReference, Medium, rgbColorRef, &cyHeight);
				}
			else
				{
				sReference = strPatternSubst(CONSTLIT("%s %&mdash; %s"), sReference, sBonus);
				Medium.DrawText(Dest, rcDrawRect, rgbColorRef, sReference, 0, 0, &cyHeight);
				}

			//	Height

			rcDrawRect.top += cyHeight;
			}

		//	Paint additional reference in the line below

		else if (!sReference.IsBlank())
			{
			Medium.DrawText(Dest, 
					rcDrawRect,
					rgbColorRef,
					sReference,
					0,
					0,
					&cyHeight);

			rcDrawRect.top += cyHeight;
			}
		}
	else
		{
		Medium.DrawText(Dest, 
				rcDrawRect,
				rgbColorRef,
				sReference,
				0,
				0,
				&cyHeight);

		rcDrawRect.top += cyHeight;
		}

	//	Launchers

	if (!m_Launchers.IsEmpty())
		{
		CIconLabelBlock::SPaintOptions PaintOptions;
		PaintOptions.bDisabled = bDisabled;

		m_Launchers.Paint(Dest, rcDrawRect.left, rcDrawRect.top, rgbColorRef, PaintOptions);
		rcDrawRect.top += m_Launchers.GetHeight();
		}

	//	Description

	CString sDesc = m_pItem->GetDesc(m_Options.bDisplayAsKnown);
	Medium.DrawText(Dest,
			rcDrawRect,
			(bSelected ? rgbColorDescSel : rgbColorDesc),
			sDesc,
			0,
			CG16bitFont::SmartQuotes,
			&cyHeight);
	rcDrawRect.top += cyHeight;

	//	Paint Enhancements

	TSharedPtr<CItemEnhancementStack> pEnhancements = m_pItem->GetEnhancementStack();
	if (pEnhancements)
		{
		for (int i = 0; i < pEnhancements->GetCount(); i++)
			{
			int cyHeight;
			PaintItemEnhancement(m_VI, 
					Dest, 
					m_pItem->GetSource(), 
					*m_pItem, 
					pEnhancements->GetEnhancement(i), 
					rcDrawRect, 
					(bSelected ? rgbColorDescSel : rgbColorDesc), 
					dwOptions,
					&cyHeight);

			rcDrawRect.top += cyHeight;
			}
		}
	}

void CItemPainter::PaintItemEnhancement (const CVisualPalette &VI, CG32bitImage &Dest, CSpaceObject *pSource, const CItem &Item, const CItemEnhancement &Enhancement, const RECT &rcRect, CG32bitPixel rgbText, DWORD dwOptions, int *retcyHeight)

//	PaintItemEnhancement
//
//	Paints an item enhancement line.

	{
	bool bDisabled = ((dwOptions & OPTION_DISABLED) ? true : false);

	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	//	Paint the enhancement icon

	const CItemType *pEnhancer = Enhancement.GetEnhancementType();
	if (pEnhancer == NULL)
		{
		pEnhancer = Item.GetUniverse().FindItemType(UNID_GENERIC_ENHANCEMENT);
		if (pEnhancer == NULL)
			return;
		}

	DrawItemTypeIcon(Dest, rcRect.left, rcRect.top, pEnhancer, ENHANCEMENT_ICON_WIDTH, ENHANCEMENT_ICON_HEIGHT, bDisabled);

	//	Figure out the description and attributes

	RECT rcDesc = rcRect;
	rcDesc.left += ENHANCEMENT_ICON_WIDTH + ITEM_TEXT_MARGIN_X;

	CString sDesc = pEnhancer->GetNounPhrase(nounShort | nounTitleCapitalize);

	TArray<SDisplayAttribute> Attribs;
	Enhancement.AccumulateAttributes(Item, &Attribs, CItemEnhancement::FLAG_INCLUDE_HP_BONUS | CItemEnhancement::FLAG_INCLUDE_EXPIRATION);

	int cyAttribHeight;
	CCartoucheBlock AttribBlock;
	FormatDisplayAttributes(VI, Attribs, rcDesc, AttribBlock, &cyAttribHeight);

	//	Vertically center the description and attribs.

	int cyTotalHeight = Medium.GetHeight() + cyAttribHeight;
	rcDesc.top += Max(0, (ENHANCEMENT_ICON_HEIGHT - cyTotalHeight) / 2);
	rcDesc.bottom = rcDesc.top + cyTotalHeight;

	//	Paint the description

	Medium.DrawText(Dest,
			rcDesc,
			rgbText,
			sDesc,
			0,
			CG16bitFont::SmartQuotes);

	//	Enhancement

	CCartoucheBlock::SPaintOptions AttribBlockOptions;
	AttribBlockOptions.bDisabled = bDisabled;
	AttribBlock.Paint(Dest, rcDesc.left, rcDesc.top + Medium.GetHeight(), AttribBlockOptions);

	if (retcyHeight)
		*retcyHeight = Max(ENHANCEMENT_ICON_HEIGHT, cyTotalHeight);
	}

void CItemPainter::PaintReferenceDamageAdj (const CVisualPalette &VI, CG32bitImage &Dest, int x, int y, int iLevel, int iHP, const int *iDamageAdj, CG32bitPixel rgbAdvantage, CG32bitPixel rgbDisadvantage, DWORD dwOptions, const CDamageAdjDesc *pDamageAdjCurve)

//	PaintReferenceDamageAdj
//
//	Paints a line showing damage adjustment.

	{
	bool bDisabled = ((dwOptions & OPTION_DISABLED) ? true : false);

	struct SEntry
		{
		int iDamageType;
		int iDamageAdj;
		int iDisplayAdj;
		bool bAdvantage;
		};

	//	Eventually these should become adventure options
	bool bSortByDamageType = false;
	bool bOptionShowDamageAdjAsHP = false;
	bool bPrettyPercent = true;					//	round to nearest 5%
	bool bCompressIdentical = true;
	bool bUseDamageLevelsAsFallback = true;
	bool bMarkDisadvantageWithCurves = false;	//	Enabling this causes things that seem like buffs to be marked as disadvantages if they are below the adj curve

	const CG16bitFont &Small = VI.GetFont(fontSmall);
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	//	Must have positive HP

	if (iHP == 0)
		return;

	//	Sort damage types from highest to lowest

	TSortMap<CString, SEntry> Sorted;
	int iLengthEstimate = 0;
	int iImmuneCount = 0;
	for (int i = 0; i < damageCount; i++)
		{
		//	Skip if this damage type is irrelevant
		//	Check against supplied pDamageAdjCurve first

		DamageTypes dmgType = (DamageTypes)i;

		//	ignore anything that by default does < 10% damage at this level, or > 100% damage at this level

		if (pDamageAdjCurve && (pDamageAdjCurve->GetAdj(dmgType) < 10 || pDamageAdjCurve->GetAdj(dmgType) > 100))
			continue;

		//	otherwise we fall back to checking level, if that option is set

		else if (bUseDamageLevelsAsFallback && pDamageAdjCurve == NULL)
			{
			int iDamageLevel = GetDamageTypeLevel((DamageTypes)i);
			if (iDamageLevel < iLevel - 5 || iDamageLevel > iLevel + 3)
				continue;
			}

		//	Skip if the damage adj is 100%

		if (iDamageAdj[i] == iHP)
			continue;

		//	Figure out the sort order

		CString sKey;
		if (bSortByDamageType)
			sKey = strPatternSubst(CONSTLIT("%02d"), i);
		else
			{
			DWORD dwHighToLow = (iDamageAdj[i] == -1 ? 0 : 1000000 - iDamageAdj[i]);
			sKey = strPatternSubst(CONSTLIT("%08x %02d"), dwHighToLow, i);
			}

		//	Add to list

		SEntry *pEntry = Sorted.SetAt(sKey);

		//	Fill basic entry data

		pEntry->iDamageType = i;
		pEntry->iDamageAdj = iDamageAdj[i];

		//	Fill display data, so we can collapse entries as needed

		if (bOptionShowDamageAdjAsHP)
			pEntry->iDisplayAdj = iDamageAdj[i];
		else
			{
			int iPercentAdj = (100 * (iDamageAdj[i] - iHP) / iHP);
			if (bPrettyPercent)
				pEntry->iDisplayAdj = 5 * ((iPercentAdj + 2 * Sign(iPercentAdj)) / 5);
			else
				pEntry->iDamageAdj = iPercentAdj;
			}

		//	Indicate if this is an advantage or disadvantage

		if (bMarkDisadvantageWithCurves && pDamageAdjCurve && pEntry->iDamageAdj >= 0)
			{
			//	We need to compute what the expected displayAdj is and compare
			
			int iRawAdj = pDamageAdjCurve->GetAdj(dmgType);
			int iComputedAdj = (iDamageAdj[i] / iHP);
			pEntry->bAdvantage = iComputedAdj >= iRawAdj;
			}
		else
			pEntry->bAdvantage = (bOptionShowDamageAdjAsHP ? pEntry->iDisplayAdj >= iHP : pEntry->iDisplayAdj >= 0 ) || pEntry->iDamageAdj < 0;

		//	Estimate how many entries we will have (so we can decide the font size)
		//	We assume that immune entries get collapsed.

		if (iDamageAdj[i] != -1)
			iLengthEstimate++;
		else
			iImmuneCount++;
		}

	//	If we have six or more icons, then we need to paint smaller

	iLengthEstimate += Min(2, iImmuneCount);
	const CG16bitFont &TheFont = ((iLengthEstimate >= 6 || iImmuneCount >= 8) ? Small : Medium);
	int cyOffset = (Medium.GetHeight() - TheFont.GetHeight()) / 2;
	int iDamageIconSpacingX = (iLengthEstimate < 8 && iImmuneCount < 10) ? DAMAGE_ADJ_ICON_SPACING_X : 0;
	int iDamageAdjSpacingX = (iLengthEstimate < 8 && iImmuneCount < 10) ? DAMAGE_ADJ_SPACING_X : 2;
	
	//	Paint the icons

	for (int i = 0; i < Sorted.GetCount(); i++)
		{
		const SEntry &Entry = Sorted[i];
		int iDamageType = Entry.iDamageType;
		int iDamageAdj = Entry.iDamageAdj;
		int iDisplayPercent = Entry.iDisplayAdj;

		//	Skip if display number is 0

		if (iDisplayPercent == 0)
			continue;

		//	Do we need to compress other entries with identical stats?
		
		if (bCompressIdentical)
			{

			bool bSkip = false;

			for (int k = 0; k < Sorted.GetCount(); k++)
				{

				//	If we encounter another with identical adjustment before us, assume it has already included us so we skip

				if (Sorted[k].iDisplayAdj == iDisplayPercent && k < i)
					{
					bSkip = true;
					break;
					}

				//	We draw our own icon, we only reach this if no duplicates showed up before us

				else if (k == i)
					{
					VI.DrawDamageTypeIcon(Dest, x, y, (DamageTypes)iDamageType, bDisabled);
					x += DAMAGE_TYPE_ICON_WIDTH + iDamageIconSpacingX;
					}

				//	We draw the icons for other types with the same adj as us

				else if (Sorted[k].iDisplayAdj == iDisplayPercent)
					{
					VI.DrawDamageTypeIcon(Dest, x, y, (DamageTypes)Sorted[k].iDamageType, bDisabled);
					x += DAMAGE_TYPE_ICON_WIDTH + iDamageIconSpacingX;
					}

				}

			if (bSkip)
				continue;
			}
		else
			{

			//	Draw icon

			VI.DrawDamageTypeIcon(Dest, x, y, (DamageTypes)iDamageType, bDisabled);
			x += DAMAGE_TYPE_ICON_WIDTH + iDamageIconSpacingX;

			//	If we have a bunch of entries with "immune", we always compress them

			if (i < (Sorted.GetCount() - 1)
				&& iDamageAdj == -1
				&& (iDamageAdj == Sorted[i + 1].iDamageAdj))
				continue;
			}

		//	Figure out how to display damage adj

		CString sStat;
		if (iDamageAdj == -1)
			sStat = CONSTLIT("immune");
		else if (bOptionShowDamageAdjAsHP)
			sStat = strFromInt(iDamageAdj);
		else
			sStat = strPatternSubst(CONSTLIT("%s%d%%"), (iDisplayPercent > 0 ? CONSTLIT("+") : NULL_STR), iDisplayPercent);
		
		//	Draw

		Dest.DrawText(x,
				y + cyOffset,
				TheFont,
				Entry.bAdvantage ? rgbAdvantage : rgbDisadvantage,
				sStat,
				0,
				&x);

		x += iDamageAdjSpacingX;
		}
	}

void CItemPainter::PaintReferenceDamageType (const CVisualPalette &VI, CG32bitImage &Dest, int x, int y, int iDamageType, const CString &sRef, CG32bitPixel rgbText, DWORD dwOptions)

//	PaintReferenceDamageType
//
//	Paints a line showing a damageType reference.

	{
	bool bDisabled = ((dwOptions & OPTION_DISABLED) ? true : false);

	const CG16bitFont &Small = VI.GetFont(fontSmall);
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	//	Paint the icon first

	if (iDamageType >= damageMinListed)
		{
		//x += DAMAGE_ADJ_SPACING_X;
		VI.DrawDamageTypeIcon(Dest, x, y, (DamageTypes)iDamageType, bDisabled);
		x += DAMAGE_TYPE_ICON_WIDTH + DAMAGE_ADJ_ICON_SPACING_X;
		}

	//	Figure out what font to use
	const CG16bitFont& Font = (sRef.GetLength() >= 64) ? Small : Medium;

	//	Paint the reference text

	Dest.DrawText(x,
			y,
			Font,
			rgbText,
			sRef,
			0);
	}
