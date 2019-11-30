//	CMapLabelPainter.cpp
//
//	CMapLabelPainter class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CMapLabelPainter::CalcLabelPos (int cxLabel, int cyLabel, EPositions iPos, int &xMapLabel, int &yMapLabel)

//	CalcLabelPos
//
//	Computes the label position.

	{
	switch (iPos)
		{
		case CMapLabelPainter::posLeft:
			{
			xMapLabel = -(LABEL_SPACING_X + cxLabel);
			yMapLabel = -(cyLabel / 2);
			break;
			}

		case CMapLabelPainter::posBottom:
			{
			xMapLabel = -(cxLabel / 2);
			yMapLabel = LABEL_SPACING_Y;
			break;
			}

		//	Defaults to posRight

		default:
			xMapLabel = LABEL_SPACING_X;
			yMapLabel = -(cyLabel / 2);
			break;
		}
	}

void CMapLabelPainter::CalcLabelPos (const CString &sLabel, const CG16bitFont &Font, EPositions iPos, int &xMapLabel, int &yMapLabel)

//	CalcLabelPos
//
//	Computes the label position.

	{
	int cxLabel = Font.MeasureText(sLabel);
	int cyLabel = Font.GetHeight();
	CalcLabelPos(cxLabel, cyLabel, iPos, xMapLabel, yMapLabel);
	}

void CMapLabelPainter::CalcLabelRect (int x, int y, int cxLabel, int cyLabel, EPositions iPos, RECT &retrcRect)

//	CalcLabelRect
//
//	Computes the label rectangle.

	{
	int xLabel, yLabel;

	CalcLabelPos(cxLabel, cyLabel, iPos, xLabel, yLabel);
	retrcRect.left = x + xLabel;
	retrcRect.top = y + yLabel;
	retrcRect.right = retrcRect.left + cxLabel;
	retrcRect.bottom = retrcRect.top + cyLabel;
	}

void CMapLabelPainter::MeasureLabel (int *retcxLabel, int *retcyLabel) const

//	MeasureLabel
//
//	Returns the size of the label.

	{
	if (m_pFont)
		{
		if (retcxLabel) *retcxLabel = m_pFont->MeasureText(m_sLabel);
		if (retcyLabel) *retcyLabel = m_pFont->GetHeight();
		}
	else
		{
		if (retcxLabel) *retcxLabel = 0;
		if (retcyLabel) *retcyLabel = 0;
		}
	}

void CMapLabelPainter::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read non-cached data.

	{
	DWORD dwLoad;

	if (Ctx.dwVersion >= 162)
		{
		Ctx.pStream->Read(dwLoad);
		SetPos(LoadPosition(dwLoad));
		}
	else
		{
		int xMapLabel, yMapLabel;
		Ctx.pStream->Read(xMapLabel);
		Ctx.pStream->Read(yMapLabel);

		//	For backwards compatibility we reverse engineer label coordinates
		//	to a label position.

		if (xMapLabel < 0 && yMapLabel > 0)
			SetPos(posBottom);
		else if (xMapLabel < 0)
			SetPos(posLeft);
		else
			SetPos(posRight);
		}
	}

void CMapLabelPainter::RealizePos (void)

//	RealizePos
//
//	Initializes m_xLabel and m_yLabel.

	{
	if (IsEmpty())
		return;

	CalcLabelPos(GetLabel(), *m_pFont, GetPos(), m_xLabel, m_yLabel);
	}

void CMapLabelPainter::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	Writes out non-cached data.

	{
	Stream.Write(SavePosition(GetPos()));
	}
