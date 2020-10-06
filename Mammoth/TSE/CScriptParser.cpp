//	CScriptParser.cpp
//
//	CScriptParser class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "ScriptParserImpl.h"

CScriptParser::CScriptParser (const CDesignCollection &Design, const CDesignType *pSource, const char *pBuffer, int iLength) :
		m_Design(Design),
		m_pSource(pSource),
		m_pBuffer(pBuffer),
		m_pBufferEnd(pBuffer + iLength),
		m_pPos(pBuffer),
		m_pStart(pBuffer),
		m_Buffer(MAX_PARAGRAPH_SIZE)

//	CScriptParser constructor

	{
	if (m_Buffer.Create() != NOERROR)
		throw CException(ERR_MEMORY);
	}

void CScriptParser::CreateParagraphFromBuffer (CScript::ParagraphStyle iStyle, CScript::SParagraph *retParagraph)

//	CreateParagraphFromBuffer
//
//	Initializes a paragraph from the current buffer and cleans up the buffer.

	{
	if (retParagraph)
		{
		retParagraph->iStyle = iStyle;
		retParagraph->sText = CString(m_Buffer.GetPointer(), m_Buffer.GetLength());
		}

	m_Buffer.Seek(0);
	}

void CScriptParser::FlushToBuffer (void)

//	FlushToBuffer
//
//	Flush to the buffer.

	{
	if (m_pPos > m_pStart)
		{
		m_Buffer.Write(m_pStart, m_pPos - m_pStart);
		m_pStart = m_pPos;
		}
	}

bool CScriptParser::ParseNextParagraph (CScript::SParagraph *retParagraph)

//	ParseNextParagraph
//
//	Parses the next paragraph from the buffer. Returns FALSE if there are no 
//	more paragraphs.

	{
	while (true)
		{
		switch (m_iState)
			{
			case State::start:
				{
				if (m_pPos >= m_pBufferEnd)
					{
					m_iState = State::done;
					return false;
					}

				//	%xyz...
				//  ^

				else if (*m_pPos == '%')
					{
					m_pPos++;
					m_pStart = m_pPos;
					m_iPrevState = State::normalText;
					m_iState = State::variable;
					}

				//	|   xyz
				//	^

				else if (*m_pPos == '|')
					{
					m_pPos++;
					m_iState = State::alignMarker;
					}

				//	Xyz
				//	^

				else
					{
					m_pPos++;
					m_iState = State::normalText;
					}

				break;
				}

			case State::normalText:
				{
				//	Xyz
				//	   ^

				if (m_pPos >= m_pBufferEnd)
					{
					FlushToBuffer();
					CreateParagraphFromBuffer(CScript::ParagraphStyle::normal, retParagraph);

					m_iState = State::done;
					return true;
					}

				//	Xyz\n
				//	    ^

				else if (*m_pPos == '\n')
					{
					FlushToBuffer();

					m_pPos++;
					m_iState = State::normalTextEoL;
					}

				//	Xyz %abc...
				//	    ^

				else if (*m_pPos == '%')
					{
					m_pPos++;
					m_pStart = m_pPos;
					m_iPrevState = State::normalText;
					m_iState = State::variable;
					}

				//	Xyz | abc
				//	    ^

				else if (*m_pPos == '|')
					{
					FlushToBuffer();
					CreateParagraphFromBuffer(CScript::ParagraphStyle::normalLine, retParagraph);

					m_iState = State::start;
					return true;
					}

				//	Xyz
				//	 ^

				else
					{
					m_pPos++;
					}

				break;
				}

			case State::normalTextEoL:
				{
				//	Xyz\n
				//		 ^

				if (m_pPos >= m_pBufferEnd)
					{
					CreateParagraphFromBuffer(CScript::ParagraphStyle::normal, retParagraph);

					m_iState = State::done;
					return true;
					}

				//	Xyz\n\n
				//		  ^

				else if (*m_pPos == '\n')
					{
					CreateParagraphFromBuffer(CScript::ParagraphStyle::normal, retParagraph);

					m_pPos++;
					m_pStart = m_pPos;
					m_iState = State::start;
					return true;
					}

				//	Xyz/nAbc
				//	     ^

				else
					{
					CreateParagraphFromBuffer(CScript::ParagraphStyle::normalLine, retParagraph);

					m_pStart = m_pPos;
					m_iState = State::start;
					return true;
					}

				break;
				}

			case State::alignMarker:
				{
				//	|
				//	 ^

				if (m_pPos >= m_pBufferEnd)
					{
					m_iState = State::done;
					return false;
					}

				//	|  Abc
				//	 ^

				else
					{
					m_iState = State::centeredMarker;
					}
				break;
				}

			case State::centeredMarker:
				{
				//	|
				//	  ^

				if (m_pPos >= m_pBufferEnd)
					{
					m_iState = State::done;
					return false;
					}

				//	|   Abc
				//	 ^

				else if (strIsWhitespace(m_pPos))
					{
					m_pPos++;
					}

				//	|   Abc
				//	    ^

				else
					{
					m_pStart = m_pPos;
					m_iState = State::centeredText;
					}
				break;
				}

			case State::centeredText:
				{
				//	|   Abc
				//	       ^

				if (m_pPos >= m_pBufferEnd)
					{
					FlushToBuffer();
					CreateParagraphFromBuffer(CScript::ParagraphStyle::centered, retParagraph);

					m_iState = State::done;
					return true;
					}

				//	|   Abc\n
				//	        ^

				else if (*m_pPos == '\n')
					{
					FlushToBuffer();

					m_pPos++;
					m_iState = State::centeredTextEoL;
					}

				//	|   Abc %Xyz...
				//	        ^

				else if (*m_pPos == '%')
					{
					m_pPos++;
					m_pStart = m_pPos;
					m_iPrevState = State::centeredText;
					m_iState = State::variable;
					}

				//	|   Abc |
				//	        ^

				else if (*m_pPos == '|')
					{
					FlushToBuffer();
					CreateParagraphFromBuffer(CScript::ParagraphStyle::centeredLine, retParagraph);

					m_iState = State::start;
					return true;
					}

				//	|=   Abc
				//	      ^

				else
					{
					m_pPos++;
					}

				break;
				}

			case State::centeredTextEoL:
				{
				//	|   Abc\n
				//	         ^

				if (m_pPos >= m_pBufferEnd)
					{
					CreateParagraphFromBuffer(CScript::ParagraphStyle::centered, retParagraph);

					m_iState = State::done;
					return true;
					}

				//	|   Abc\n\n
				//	          ^

				else if (*m_pPos == '\n')
					{
					CreateParagraphFromBuffer(CScript::ParagraphStyle::centered, retParagraph);

					m_pPos++;
					m_pStart = m_pPos;
					m_iState = State::start;
					return true;
					}

				//	|   Abc\nXyz
				//	         ^

				else
					{
					CreateParagraphFromBuffer(CScript::ParagraphStyle::centeredLine, retParagraph);

					m_pStart = m_pPos;
					m_iState = State::start;
					return true;
					}

				break;
				}

			case State::variable:
				{
				//	... %
				//	     ^

				if (m_pPos >= m_pBufferEnd)
					{
					m_iState = m_iPrevState;
					}

				//	... %id=
				//	       ^
				
				else if (*m_pPos == '=')
					{
					CString sCmd(m_pStart, m_pPos - m_pStart);
					if (strEquals(sCmd, CONSTLIT("id")))
						{
						m_pPos++;
						m_pStart = m_pPos;
						m_iState = State::variableParam;
						}
					else
						{
						CString sError = strPatternSubst(CONSTLIT("Unknown variable: %s"), sCmd);
						m_Buffer.Write(sError);
						m_iState = State::variableError;
						}
					}
				else
					{
					m_pPos++;
					}

				break;
				}

			case State::variableParam:
				{
				//	... %id=Abc
				//	           ^

				if (m_pPos >= m_pBufferEnd)
					{
					CString sError = CONSTLIT("End of variable expected");
					m_Buffer.Write(sError);
					m_iState = State::variableError;
					}

				//	... %id=Abc%
				//	           ^

				else if (*m_pPos == '%')
					{
					CString sID(m_pStart, m_pPos - m_pStart);
					WriteLanguageEntryToBuffer(sID);

					m_pPos++;
					m_pStart = m_pPos;
					m_iState = m_iPrevState;
					}

				//	... %id=Abc%
				//	         ^

				else
					{
					m_pPos++;
					}

				break;
				}

			case State::variableError:
				{
				if (m_pPos >= m_pBufferEnd)
					{
					m_pStart = m_pPos;
					m_iState = m_iPrevState;
					}
				else if (*m_pPos == '%')
					{
					m_pPos++;
					m_pStart = m_pPos;
					m_iState = m_iPrevState;
					}
				else
					{
					m_pPos++;
					}

				break;
				}

			case State::done:
				return false;

			default:
				throw CException(ERR_FAIL);
			}
		}
	}

void CScriptParser::WriteLanguageEntryToBuffer (const CString &sID)

//	WriteLanguageEntryToBuffer
//
//	Looks up the given entry and write it out to the buffer.

	{
	//	See if we have a type ID.

	const char *pPos = sID.GetASCIIZPointer();
	const char *pStart = pPos;
	while (*pPos != '\0' && *pPos != '/')
		pPos++;

	const CDesignType *pType;
	CString sLanguageID;

	if (*pPos == '/')
		{
		CString sTypeUNID(pStart, pPos - pStart);
		sLanguageID = CString(pPos + 1);

		pType = m_Design.FindEntry(strToInt(sTypeUNID, 0));
		if (!pType)
			{
			CString sError = strPatternSubst(CONSTLIT("Unknown type: %s"), sTypeUNID);
			m_Buffer.Write(sError);
			return;
			}
		}
	else
		{
		pType = m_pSource;
		if (!pType)
			throw CException(ERR_FAIL);

		sLanguageID = sID;
		}

	CString sText;
	if (!pType->TranslateText(sLanguageID, NULL, &sText))
		{
		CString sError = strPatternSubst(CONSTLIT("Unknown language ID: %s"), sLanguageID);
		m_Buffer.Write(sError);
		return;
		}

	m_Buffer.Write(sText);
	}
