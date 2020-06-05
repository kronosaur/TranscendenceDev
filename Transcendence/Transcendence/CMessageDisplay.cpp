//	CMessageDisplay.cpp
//
//	CMessageDisplay class

#include "PreComp.h"
#include "Transcendence.h"

#define MAX_AGE_FADE								4
#define AGE_FADE_OFFSET								4
#define AGE_FADE_TOTAL								(MAX_AGE_FADE + AGE_FADE_OFFSET)

void CMessageDisplay::ClearAll (void)

//	ClearAll
//
//	Clear all messages

	{
	m_iFirstMessage = 0;
	m_iNextMessage = 0;
	m_cySmoothScroll = 0;
	}

void CMessageDisplay::AddMessage (DWORD dwVirtKey, const CString &sMessage)

//	AddMessage
//
//	Display a new message in the given color

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Font = VI.GetFont(fontHeader);

	//	Don't accept duplicate messages

	if (m_iNextMessage != m_iFirstMessage)
		{
		int iMsg = Prev(m_iNextMessage);
		if (m_Messages[iMsg].iState != stateClear
				&& strEquals(m_Messages[iMsg].sMessage, sMessage))
			return;
		}

	//	If we're at the end, delete the last message

	if (Next(m_iNextMessage) == m_iFirstMessage)
		m_iFirstMessage = Next(m_iFirstMessage);

	//	If there are already messages here, then smooth scroll

	int iCount = 0;
	int iMsg = Prev(m_iNextMessage);
	while (Next(iMsg) != m_iFirstMessage)
		{
		if (m_Messages[iMsg].iState != stateClear)
			iCount++;
		iMsg = Prev(iMsg);
		}

	if (iCount > 0)
		m_cySmoothScroll = -Font.GetHeight();

	//	Remember the message

	m_Messages[m_iNextMessage].sMessage = sMessage;
	m_Messages[m_iNextMessage].dwVirtKey = dwVirtKey;
	m_Messages[m_iNextMessage].rgbColor = VI.GetColor(colorTextHighlight);
	m_Messages[m_iNextMessage].x = -1;

	//	Start at the appropriate state

	m_Messages[m_iNextMessage].iState = stateBlinking;
	m_Messages[m_iNextMessage].iTick = DEFAULT_BLINK_TIME;

	//	Point to next message

	m_iNextMessage = Next(m_iNextMessage);
	}

void CMessageDisplay::Init (const RECT &rcScreen)

//	Init
//
//	Initialize. NOTE: It is OK to display a message before Init is called. This
//	initialization is only to compute display metrics.

	{
	m_rcRect.left = rcScreen.left + (RectWidth(rcScreen) - DISPLAY_WIDTH) / 2;
	m_rcRect.right = m_rcRect.left + DISPLAY_WIDTH;
	m_rcRect.top = rcScreen.bottom - (RectHeight(rcScreen) / 3);
	m_rcRect.bottom = m_rcRect.top + DISPLAY_HEIGHT;
	}

void CMessageDisplay::Paint (CG32bitImage &Dest)

//	Paint
//
//	Paint the message

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Font = VI.GetFont(fontHeader);

	bool bSkip = false;
	if (m_cySmoothScroll != 0)
		bSkip = true;

	//	Loop over all messages in the queue. We go backwards because
	//	the messages are shown in reverse chronological order

	int y = m_rcRect.top;
	int iMsg = Prev(m_iNextMessage);
	int iAge = MAX_AGE_FADE;
	while (Next(iMsg) != m_iFirstMessage)
		{
		if (m_Messages[iMsg].iState != stateClear)
			{
			//	Figure out what color to use

			CG32bitPixel rgbColor;
			switch (m_Messages[iMsg].iState)
				{
				case stateNormal:
				case stateSteady:
					rgbColor = CG32bitPixel(
							(iAge + AGE_FADE_OFFSET) * m_Messages[iMsg].rgbColor.GetRed() / AGE_FADE_TOTAL,
							(iAge + AGE_FADE_OFFSET) * m_Messages[iMsg].rgbColor.GetGreen() / AGE_FADE_TOTAL,
							(iAge + AGE_FADE_OFFSET) * m_Messages[iMsg].rgbColor.GetBlue() / AGE_FADE_TOTAL);
					break;

				case stateBlinking:
					{
					if ((m_Messages[iMsg].iTick % 2) == 0)
						rgbColor = CG32bitPixel(255,255,255);
					else
						rgbColor = CG32bitPixel(0,0,0);
					break;
					}

				case stateFading:
					{
					int iFade = (iAge + AGE_FADE_OFFSET) * 1000 * m_Messages[iMsg].iTick / (DEFAULT_FADE_TIME * AGE_FADE_TOTAL);
					rgbColor = CG32bitPixel(
							iFade * m_Messages[iMsg].rgbColor.GetRed() / 1000,
							iFade * m_Messages[iMsg].rgbColor.GetGreen() / 1000,
							iFade * m_Messages[iMsg].rgbColor.GetBlue() / 1000);
					break;
					}
				}

			y += Font.GetHeight();
			iAge = Max(0, iAge - 1);

			//	Draw the message centered

			if (!bSkip)
				{
				PaintMessage(Dest, m_Messages[iMsg], y + m_cySmoothScroll, rgbColor);

				if (m_cySmoothScroll != 0)
					m_cySmoothScroll = Min(0, m_cySmoothScroll + 4);
				}
			else
				bSkip = false;
			}

		iMsg = Prev(iMsg);
		}
	}

void CMessageDisplay::PaintMessage (CG32bitImage &Dest, const SMessage &Msg, int y, CG32bitPixel rgbColor) const

//	PaintMessage
//
//	Paints a single message line.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Font = VI.GetFont(fontHeader);

	//	If we have a key to paint, we go through a different path

	if (Msg.dwVirtKey != CVirtualKeyData::INVALID_VIRT_KEY)
		{
		CInputKeyPainter KeyPainter;
		KeyPainter.Init(Msg.dwVirtKey, Font.GetHeight(), Font, VI.GetFont(fontLarge));
		KeyPainter.SetBackColor(Msg.rgbColor);
		KeyPainter.SetTextColor(VI.GetColor(colorAreaDeep));

		if (Msg.x == -1)
			{
			int cxWidth = KeyPainter.GetWidth() + INNER_PADDING_HORZ + Font.MeasureText(Msg.sMessage, NULL);
			Msg.x = m_rcRect.left + (RectWidth(m_rcRect) - cxWidth) / 2;
			}

		//	Paint

		KeyPainter.Paint(Dest, Msg.x, y + m_cySmoothScroll);
		Dest.DrawText(Msg.x + KeyPainter.GetWidth() + INNER_PADDING_HORZ,
				y + m_cySmoothScroll, 
				Font, 
				rgbColor, 
				Msg.sMessage);
		}

	//	Otherwise, basic message.

	else
		{
		//	If we haven't yet figured out the position of the message
		//	calculate it now

		if (Msg.x == -1)
			{
			int cxWidth = Font.MeasureText(Msg.sMessage, NULL);
			Msg.x = m_rcRect.left + (RectWidth(m_rcRect) - cxWidth) / 2;
			}

		//	Paint

		Dest.DrawText(Msg.x,
				y + m_cySmoothScroll, 
				Font, 
				rgbColor, 
				Msg.sMessage);
		}
	}

void CMessageDisplay::Update (void)

//	Update
//
//	Update the display

	{
	DEBUG_TRY

	int iMsg = Prev(m_iNextMessage);
	while (Next(iMsg) != m_iFirstMessage)
		{
		if (m_Messages[iMsg].iTick > 0)
			{
			if (--(m_Messages[iMsg].iTick) == 0)
				{
				switch (m_Messages[iMsg].iState)
					{
					case stateBlinking:
						m_Messages[iMsg].iState = stateSteady;
						m_Messages[iMsg].iTick = DEFAULT_STEADY_TIME;
						break;

					case stateSteady:
						m_Messages[iMsg].iState = stateFading;
						m_Messages[iMsg].iTick = DEFAULT_FADE_TIME;
						break;

					case stateFading:
						m_Messages[iMsg].iState = stateClear;
						break;
					}
				}
			}

		iMsg = Prev(iMsg);
		}

	DEBUG_CATCH
	}
