//	CWordParser.cpp
//
//	CWordParser Class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

Kernel::CString Kernel::CWordParser::ParseNextWord ()

//	ParseNextWord
//
//	Returns the next word. If there are no more words, we return NULL_STR.

	{
	enum class State
		{
		start,

		word,
		checkPossessive,
		checkPossessiveS,
		percentField,
		};

	State iState = State::start;
	CString sWord;
	const char *pStart = NULL;

	while (m_pPos <= m_pEnd)
		{
		switch (iState)
			{
			case State::start:
				if (m_pPos == m_pEnd)
					{
					return NULL_STR;
					}
				else if (strIsWhitespace(m_pPos))
					{
					}
				else if (*m_pPos == '\"')
					{
					m_bInDoubleQuote = !m_bInDoubleQuote;
					}
				else if (*m_pPos == '\'')
					{
					m_bInSingleQuote = !m_bInSingleQuote;
					}
				else if (*m_pPos == '%' && m_bExcludePercentFields)
					{
					iState = State::percentField;
					}
				else if (*m_pPos == '[' && m_bHandleAccelerators)
					{
					}
				else if (IsPunctuation(m_pPos))
					{
					}
				else
					{
					pStart = m_pPos;
					iState = State::word;
					}

				break;

			case State::word:
				if (m_pPos == m_pEnd 
						|| strIsWhitespace(m_pPos))
					{
					sWord.Append(CString(pStart, (m_pPos - pStart)));
					return sWord;
					}
				else if (*m_pPos == '\'')
					{
					sWord.Append(CString(pStart, (m_pPos - pStart)));
					pStart = m_pPos;
					iState = State::checkPossessive;
					}
				else if (*m_pPos == '[' && m_bHandleAccelerators)
					{
					sWord.Append(CString(pStart, (m_pPos - pStart)));
					pStart = m_pPos + 1;
					}
				else if (*m_pPos == ']' && m_bHandleAccelerators)
					{
					sWord.Append(CString(pStart, (m_pPos - pStart)));
					pStart = m_pPos + 1;
					}
				else if (IsPunctuation(m_pPos))
					{
					sWord.Append(CString(pStart, (m_pPos - pStart)));
					return sWord;
					}

				break;

			case State::checkPossessive:
				if (m_pPos == m_pEnd)
					{
					return sWord;
					}
				else if (strIsWhitespace(m_pPos))
					{
					if (m_bInSingleQuote)
						m_bInSingleQuote = false;

					return sWord;
					}
				else if (IsPunctuation(m_pPos))
					{
					if (m_bInSingleQuote)
						m_bInSingleQuote = false;

					return sWord;
					}
				else if (*m_pPos == 's')
					{
					iState = State::checkPossessiveS;
					}

				//	Otherwise, we assume this is an embedded single-quote.

				else
					{
					iState = State::word;
					}
				
				break;

			case State::checkPossessiveS:
				if (m_pPos == m_pEnd)
					{
					return sWord;
					}
				else if (strIsWhitespace(m_pPos))
					{
					return sWord;
					}
				else if (IsPunctuation(m_pPos))
					{
					return sWord;
					}

				//	Otherwise, we assume this is an embedded single-quote.

				else
					{
					iState = State::word;
					}
				
				break;

			case State::percentField:
				if (m_pPos == m_pEnd)
					return NULL_STR;
				else if (*m_pPos == '%')
					{
					iState = State::start;
					}

				break;
			}

		m_pPos++;
		}

	//	If we get this far, then we're at the end

	return NULL_STR;
	}
