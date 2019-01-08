//	CGFrameArea.cpp
//
//	Implementation of CGFrameArea class

#include <windows.h>
#include "Alchemy.h"
#include "DirectXUtil.h"

CGFrameArea::CGFrameArea (void) : m_pMouseCapture(NULL),
		m_pMouseOver(NULL)

//	CGFrameArea constructor

	{
	}

CGFrameArea::~CGFrameArea (void)

//	CGFrameArea destructor

	{
	int i;

	for (i = 0; i < m_Areas.GetCount(); i++)
		delete m_Areas[i];
	}

ALERROR CGFrameArea::AddArea (AGArea *pArea, const RECT &rcRect, DWORD dwTag)

//	AddArea
//
//	Add an area to the frame. The rect for this area is relative
//	to this screen.

	{
	ALERROR error;

	//	Initialize the area

	if (error = pArea->Init(GetScreen(), this, rcRect, dwTag))
		return error;

	//	Add the area

	m_Areas.Insert(pArea);
	GetScreen()->OnAreaAdded(pArea);

	return NOERROR;
	}

AGArea *CGFrameArea::FindArea (DWORD dwTag)

//	FindArea
//
//	Finds area by tag. Returns NULL if do not find the area

	{
	for (int i = 0; i < GetAreaCount(); i++)
		{
		AGArea *pArea = GetArea(i);
		if (pArea->GetTag() == dwTag)
			return pArea;
		}

	return NULL;
	}

RECT CGFrameArea::GetPaintRect (const RECT &rcArea) const

//	GetPaintRect
//
//	Returns the RECT of the area to be relative to screen image.

	{
	return GetParent()->GetPaintRect(rcArea);
	}

bool CGFrameArea::LButtonDoubleClick (int x, int y)

//	LButtonDoubleClick
//
//	Handle left button double click

	{
	int i;

	POINT pt = { x, y };

	//	Give it to the area under the pointer

	for (i = 0; i < GetAreaCount(); i++)
		{
		AGArea *pArea = GetArea(i);
		RECT rcArea = pArea->GetRect();

		if (pArea->IsVisible()
				&& pArea->WantsMouseOver() 
				&& ::PtInRect(&rcArea, pt))
			{
			m_pMouseCapture = pArea;

			pArea->LButtonDoubleClick(x, y);
			return true;
			}
		}

	return false;
	}

bool CGFrameArea::LButtonDown (int x, int y)

//	LButtonDown
//
//	Handle left button down

	{
	int i;

	POINT pt = { x, y };

	//	Give it to the area under the pointer

	for (i = 0; i < GetAreaCount(); i++)
		{
		AGArea *pArea = GetArea(i);
		RECT rcArea = pArea->GetRect();

		if (pArea->IsVisible()
				&& pArea->WantsMouseOver() 
				&& ::PtInRect(&rcArea, pt))
			{
			m_pMouseCapture = pArea;

			pArea->LButtonDown(x, y);
			return true;
			}
		}

	return false;
	}

void CGFrameArea::LButtonUp (int x, int y)

//	LButtonUp
//
//	Handle left button up

	{
	if (m_pMouseCapture)
		{
		//	Remember these values before we call LButtonUp because
		//	we may not have a valid screen after that.

		AGArea *pMouseCapture = m_pMouseCapture;
		m_pMouseCapture = NULL;

		pMouseCapture->LButtonUp(x, y);
		}
	}

void CGFrameArea::MouseLeave (void)

//	MouseLeave
//
//	Mouse leaves area

	{
	if (m_pMouseOver)
		m_pMouseOver->MouseLeave();

	m_pMouseOver = NULL;
	}

void CGFrameArea::MouseMove (int x, int y)

//	MouseMove
//
//	Handle mouse move

	{
	int i;

	POINT pt = { x, y };

	if (m_pMouseCapture)
		{
		m_pMouseCapture->MouseMove(x, y);

		//	Check to see if we've entered the area

		if (m_pMouseOver == NULL)
			{
			if (::PtInRect(&m_pMouseCapture->GetRect(), pt))
				{
				m_pMouseCapture->MouseEnter();
				m_pMouseOver = m_pMouseCapture;
				}
			}
		else
			{
			ASSERT(m_pMouseOver == m_pMouseCapture);

			if (!::PtInRect(&m_pMouseCapture->GetRect(), pt))
				{
				m_pMouseCapture->MouseLeave();
				m_pMouseOver = NULL;
				}
			}
		}
	else
		{
		//	Are we still over the same area?

		if (m_pMouseOver && ::PtInRect(&m_pMouseOver->GetRect(), pt))
			{
			m_pMouseOver->MouseMove(x, y);
			}

		//	If not, find out what area we're over

		else
			{
			//	Tell the old area that we're done

			if (m_pMouseOver)
				m_pMouseOver->MouseLeave();

			//	Figure out which area has the mouse now

			AGArea *pNewMouseOver = NULL;
			for (i = 0; i < GetAreaCount(); i++)
				{
				AGArea *pArea = GetArea(i);
				RECT rcArea = pArea->GetRect();

				if (pArea->IsVisible()
						&& pArea->WantsMouseOver() 
						&& ::PtInRect(&rcArea, pt))
					{
					pNewMouseOver = pArea;
					break;
					}
				}

			//	Tell the new area that it has the mouse

			m_pMouseOver = pNewMouseOver;
			if (m_pMouseOver)
				{
				m_pMouseOver->MouseMove(pt.x, pt.y);
				m_pMouseOver->MouseEnter();
				}
			}
		}
	}

void CGFrameArea::MouseWheel (int iDelta, int x, int y, DWORD dwFlags)

//	MouseWheel
//
//	Handle mouse scrolling

	{
	int i;

	POINT pt = { x, y };

	//	Give it to the area under the pointer

	for (i = 0; i < GetAreaCount(); i++)
		{
		AGArea *pArea = GetArea(i);
		RECT rcArea = pArea->GetRect();

		if (pArea->IsVisible()
				&& pArea->WantsMouseOver() 
				&& ::PtInRect(&rcArea, pt))
			{
			pArea->MouseWheel(iDelta, x, y, dwFlags);
			return;
			}
		}
	}

void CGFrameArea::OnAreaSetRect (void)

//	OnAreaSetRect
//
//	One of our areas has moved

	{
	}

void CGFrameArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the whole area.
//
//	rcRect is the rect to which this area should be painted.

	{
	DEBUG_TRY

	for (int i = 0; i < GetAreaCount(); i++)
		{
		AGArea *pArea = GetArea(i);

		RECT rcAreaRelativeToDest =	GetPaintRect(pArea->GetRect());

		RECT rcIntersect;
		if (pArea->IsVisible()
				&& ::IntersectRect(&rcIntersect, &rcRect, &rcAreaRelativeToDest))
			{
			pArea->Paint(Dest, rcAreaRelativeToDest);
			}
		}

#ifdef DEBUG_MOUSE_OVER
	RECT rcClip = Dest.GetClipRect();
	Dest.ResetClipRect();
	CG16bitFont::GetDefault().DrawText(Dest, rcRect.left, rcRect.top, CG32bitPixel(255, 255, 255), strPatternSubst(CONSTLIT("m_pMouseOver: %x"), (DWORD)m_pMouseOver));
	Dest.SetClipRect(rcClip);
#endif

	DEBUG_CATCH
	}

void CGFrameArea::Update (void)

//	Update
//
//	Update the area

	{
	DEBUG_TRY

	for (int i = 0; i < GetAreaCount(); i++)
		{
		AGArea *pArea = GetArea(i);
		pArea->Update();
		}

	DEBUG_CATCH
	}
