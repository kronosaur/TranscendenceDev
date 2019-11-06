//	CLabelPainter.cpp
//
//	CLabelPainter class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CLabelPainter::AddLabel (const CString &sText, const CG16bitFont &Font, CG32bitPixel rgbColor, int x, int y)

//	AddLabel
//
//	Adds a label to paint.

	{
	SLabelDesc *pDesc = m_Labels.Insert();
	pDesc->sText = sText;
	pDesc->pFont = &Font;
	pDesc->rgbColor = rgbColor;
	pDesc->x = x;
	pDesc->y = y;
	}

void CLabelPainter::Paint (CG32bitImage &Dest) const

//	Paint
//
//	Paint all labels.

	{
	for (int i = 0; i < m_Labels.GetCount(); i++)
		{
		const SLabelDesc &Label = m_Labels[i];
		if (Label.pFont == NULL)
			continue;

		Label.pFont->DrawText(Dest, Label.x, Label.y, Label.rgbColor, Label.sText);
		}
	}
