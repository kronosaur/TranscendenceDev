//	CCString.cpp
//
//	Implements CCString class

#include "PreComp.h"

static CObjectClass<CCString>g_Class(OBJID_CCSTRING, NULL);

CCString::CCString (void) : ICCString(&g_Class),
		m_dwBinding(0),
		m_pBinding(NULL)

//	CCString constructor

	{
	}

ICCItem *CCString::Clone (CCodeChain *pCC)

//	Clone
//
//	Returns a new item with a single ref-count

	{
	ICCItem *pResult;
	CCString *pClone;
	
	pResult = pCC->CreateString(m_sValue);
	if (pResult->IsError())
		return pResult;

	pClone = (CCString *)pResult;
	pClone->CloneItem(this);
	pClone->m_dwBinding = m_dwBinding;
	if (m_pBinding)
		pClone->m_pBinding = m_pBinding->Reference();
	else
		pClone->m_pBinding = NULL;

	return pClone;
	}

void CCString::DestroyItem (void)

//	DestroyItem
//
//	Destroys the item

	{
	if (m_pBinding)
		{
		m_pBinding->Discard();
		m_pBinding = NULL;
		}

#ifdef DEBUG
	//	Clear out the value so that this string doesn't
	//	appear to be leaked.
	m_sValue = CString();
#endif
	CCodeChain::DestroyString(this);
	}

bool CCString::GetBinding (int *retiFrame, int *retiOffset)

//	GetBinding
//
//	Returns the binding of this identifier

	{
	int iFrame, iOffset;

	iFrame = LOWORD(m_dwBinding);
	iOffset = HIWORD(m_dwBinding);

	if (iFrame == 0)
		return false;
	else
		{
		*retiFrame = iFrame - 1;
		*retiOffset = iOffset;
		return true;
		}
	}

CString CCString::Print (DWORD dwFlags)

//	Print
//
//	Print item

	{
	//	If it is quoted we need to surround the entry with quotes

	if (IsQuoted())
		dwFlags |= PRFLAG_FORCE_QUOTES;

	//	Do it

	return Print(m_sValue, dwFlags);
	}

CString CCString::Print (const CString &sString, DWORD dwFlags)

//	Print
//
//	Converts an arbitrary string into something that could be parsed as a string
//	by CodeChain (in particular, it escapes all weird characters).

	{
	bool bEscapeNeeded = ((dwFlags & PRFLAG_FORCE_QUOTES) ? true : false);

	//	Short-circuit null strings

	if (sString.IsBlank())
		{
		if (!(dwFlags & PRFLAG_NO_QUOTES)
				&& !(dwFlags & PRFLAG_ENCODE_FOR_DISPLAY))
			return CONSTLIT("\"\"");
		else
			return sString;
		}

	//	Parse

	char *pPos = sString.GetASCIIZPointer();
	char *pPosEnd = pPos + sString.GetLength();

	//	First see if we need to do anything at all.

	if (!bEscapeNeeded)
		{
		while (pPos < pPosEnd)
			{
			if ((BYTE)*pPos <= (BYTE)' ' || *pPos == '(' || *pPos == ')' || *pPos == ';' || *pPos == '\'' || *pPos == '\"'
					|| *pPos == '{' || *pPos == '}' || *pPos == ':'
					|| (*pPos >= '0' && *pPos <= '9'))
				{
				bEscapeNeeded = true;
				break;
				}

			pPos++;
			}
		}

	//	If we don't need escape or quotes, then we're done

	if (!bEscapeNeeded)
		return sString;

	//	Otherwise we need to parse

	pPos = sString.GetASCIIZPointer();
	
	char *pBuffer = new char [4 * sString.GetLength() + 32];
	char *pDest = pBuffer;

	if (!(dwFlags & PRFLAG_NO_QUOTES)
			&& !(dwFlags & PRFLAG_ENCODE_FOR_DISPLAY))
		*pDest++ = '\"';

	//	If we're encoding for display, we allow tabs and new-lines

	if (dwFlags & PRFLAG_ENCODE_FOR_DISPLAY)
		{
		while (pPos < pPosEnd)
			{
			//	Ignore zero

			if (*pPos == '\0')
				NULL;

			//	Allow tabs and new-lines

			else if (*pPos == '\t' || *pPos == '\r' || *pPos == '\n')
				*pDest++ = *pPos;

			//	Convert any low-ASCII characters to whitespace

			else if ((BYTE)*pPos < ' ' || (BYTE)(*pPos) == 255)
				*pDest++ = ' ';

			//	Otherwise, take the character unmodified

			else
				*pDest++ = *pPos;

			pPos++;
			}
		}

	//	Otherwise we escape every character in a reversable way
	else
		{
		while (pPos < pPosEnd)
			{
			if (*pPos == '\\')
				{
				*pDest++ = '\\';
				*pDest++ = '\\';
				}
			else if (*pPos == '\0')
				{
				*pDest++ = '\\';
				*pDest++ = '0';
				}
			else if (*pPos == '\"')
				{
				*pDest++ = '\\';
				*pDest++ = '\"';
				}
			else if (*pPos == '\n')
				{
				*pDest++ = '\\';
				*pDest++ = 'n';
				}
			else if ((BYTE)*pPos < ' ' || (BYTE)(*pPos) == 255)
				{
				*pDest++ = '\\';
				*pDest++ = 'x';
				*pDest++ = strGetHexDigit(((BYTE)(*pPos) / 16));
				*pDest++ = strGetHexDigit(((BYTE)(*pPos) % 16));
				}
			else
				*pDest++ = *pPos;

			pPos++;
			}
		}

	if (!(dwFlags & PRFLAG_NO_QUOTES)
			&& !(dwFlags & PRFLAG_ENCODE_FOR_DISPLAY))
		*pDest++ = '\"';

	*pDest++ = '\0';

	//	Done

	CString sResult(pBuffer);
	delete pBuffer;
	return sResult;
	}

void CCString::Reset (void)

//	Reset
//
//	Reset to initial conditions

	{
	ASSERT(m_dwRefCount == 0);
	m_sValue = LITERAL("");
	m_dwBinding = 0;
	m_pBinding = NULL;
	}

void CCString::SetBinding (int iFrame, int iOffset)

//	SetBinding
//
//	Sets the binding

	{
	m_dwBinding = MAKELONG(iFrame + 1, iOffset);
	}

void CCString::SetFunctionBinding (CCodeChain *pCC, ICCItem *pBinding)

//	SetFunctionBinding
//
//	Associates a function with the string

	{
	if (m_pBinding)
		m_pBinding->Discard();

	m_pBinding = pBinding->Reference();
	}

