//	CLanguageDataBlock.cpp
//
//	CLanguageDataBlock class

#include "PreComp.h"

#define MESSAGE_TAG								CONSTLIT("Message")
#define STRING_TAG								CONSTLIT("String")
#define TEXT_TAG								CONSTLIT("Text")

#define ID_ATTRIB								CONSTLIT("id")
#define TEXT_ATTRIB								CONSTLIT("text")

void CLanguageDataBlock::AddEntry (const CString &sID, const CString &sText)

//	AddEntry
//
//	Adds an entry to the data block

	{
	bool bIsNew;
	SEntry *pEntry = m_Data.SetAt(sID, &bIsNew);

	//	If not new then we need to free the code block

	if (!bIsNew && pEntry->pCode)
		pEntry->pCode->Discard(&g_pUniverse->GetCC());

	//	Init

	pEntry->sText = sText;
	pEntry->pCode = NULL;
	}

ICCItem *CLanguageDataBlock::ComposeCCItem (CCodeChain &CC, ICCItem *pValue, ICCItem *pData) const

//	ComposeCCItem
//
//	Recursively parse all strings for player string escape codes (e.g., %name%). 
//	Callers must discard the result.

	{
	int i;

	if (pValue->IsSymbolTable())
		{
		ICCItem *pResult = CC.CreateSymbolTable();

		CCSymbolTable *pSource = (CCSymbolTable *)pValue;
		CCSymbolTable *pTable = (CCSymbolTable *)pResult;
		for (i = 0; i < pValue->GetCount(); i++)
			{
			ICCItem *pElement = ComposeCCItem(CC, pSource->GetElement(i), pData);
			ICCItem *pKey = CC.CreateString(pSource->GetKey(i));
			pTable->AddEntry(&CC, pKey, pElement);
			pElement->Discard(&CC);
			pKey->Discard(&CC);
			}

		return pResult;
		}

	else if (pValue->IsIdentifier())
		return CC.CreateString(CLanguage::Compose(pValue->GetStringValue(), pData));

	else if (pValue->IsList())
		{
		ICCItem *pResult = CC.CreateLinkedList();

		CCLinkedList *pList = (CCLinkedList *)(pResult);
		for (i = 0; i < pValue->GetCount(); i++)
			{
			ICCItem *pElement = ComposeCCItem(CC, pValue->GetElement(i), pData);
			pList->Append(CC, pElement);
			pElement->Discard(&CC);
			}

		return pResult;
		}

	else
		return pValue->Reference();
	}

CLanguageDataBlock::ETranslateResult CLanguageDataBlock::ComposeResult (ICCItem *pResult, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItem **retpResult) const

//	ComposeResult
//
//	Compose a translation result.

	{
	int i;

	//	Nil

	if (pResult->IsNil())
		return resultNotFound;

	//	String

	else if (pResult->GetValueType() == ICCItem::String)
		{
		if (retsText)
			{
			*retsText = CLanguage::Compose(pResult->GetStringValue(), pData);
			return resultString;
			}
		else
			return resultFound;
		}

	//	Array of strings

	else if (pResult->GetValueType() == ICCItem::List && pResult->GetCount() > 0 && pResult->GetElement(0)->GetValueType() == ICCItem::String)
		{
		if (retText)
			{
			retText->DeleteAll();

			retText->InsertEmpty(pResult->GetCount());
			for (i = 0; i < pResult->GetCount(); i++)
				retText->GetAt(i) = CLanguage::Compose(pResult->GetElement(i)->GetStringValue(), pData);

			return resultArray;
			}
		else
			return resultFound;
		}

	//	Otherwise we return the naked value

	else
		{
		if (retpResult)
			{
			*retpResult = pResult->Reference();
			return resultCCItem;
			}
		else
			return resultFound;
		}
	}

bool CLanguageDataBlock::ComposeTextResult (ETranslateResult iResult, const TArray<CString> &List, CString *retsText) const

//	ComposeTextResult
//
//	Composes a result.

	{
	switch (iResult)
		{
		case resultNotFound:
			return false;

		case resultFound:
			//	This only happens if the caller passed in NULL for retsText.
			//	It means that we don't need the result.
			return true;

		case resultArray:
			{
			if (List.GetCount() > 0)
				{
				if (retsText)
					*retsText = List[0];
				return true;
				}
			else
				return false;
			}

		case resultString:
			return true;

		default:
			ASSERT(false);
			return false;
		}
	}

void CLanguageDataBlock::Copy (const CLanguageDataBlock &Src)

//	Copy
//
//	Copy from Src; assumes we are uninitialized.

	{
	int i;

	m_Data = Src.m_Data;

	//	Add a reference to every item

	for (i = 0; i < m_Data.GetCount(); i++)
		{
		if (m_Data[i].pCode)
			m_Data[i].pCode = m_Data[i].pCode->Reference();
		}
	}

void CLanguageDataBlock::DeleteAll (void)

//	DeleteAll
//
//	Deletes all entries

	{
	int i;
	CCodeChain *pCC = &g_pUniverse->GetCC();

	for (i = 0; i < m_Data.GetCount(); i++)
		if (m_Data[i].pCode)
			m_Data[i].pCode->Discard(pCC);

	m_Data.DeleteAll();
	}

CLanguageDataBlock::SEntryDesc CLanguageDataBlock::GetEntry (int iIndex) const

//	GetEntry
//
//	Returns the entry by index.

	{
	SEntryDesc Entry;

	Entry.sID = m_Data.GetKey(iIndex);
	Entry.sText = m_Data[iIndex].sText;
	if (m_Data[iIndex].pCode)
		Entry.pCode = ICCItemPtr(m_Data[iIndex].pCode->Reference());

	return Entry;
	}

ALERROR CLanguageDataBlock::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes from an XML block

	{
	int i;

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);
		CString sID = pItem->GetAttribute(ID_ATTRIB);
		if (sID.IsBlank())
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid id in <Language> block"));
			return ERR_FAIL;
			}

		//	Add an entry

		bool bIsNew;
		SEntry *pEntry = m_Data.SetAt(sID, &bIsNew);
		if (!bIsNew)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Duplicate <Language> element: %s"), sID);
			return ERR_FAIL;
			}

		//	Read the text

		if (strEquals(pItem->GetTag(), TEXT_TAG))
			{
			//	If this is code, then link it.

			if (IsCode(pItem->GetContentText(0)))
				{
				//	Link the code

				CCodeChainCtx CCCtx;
				ICCItemPtr pCode = CCCtx.LinkCode(pItem->GetContentText(0));

				//	Nil means blank

				if (pCode->IsNil())
					{
					pEntry->pCode = NULL;
					pEntry->sText = NULL_STR;
					}

				//	Report errors

				else if (pCode->IsError())
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Language id: %s : %s"), sID, pCode->GetStringValue());
					return ERR_FAIL;
					}

				//	If pCode is a string and not an identifier, then we can just
				//	store it directly.

				else if (pCode->IsIdentifier() && pCode->IsQuoted())
					{
					pEntry->pCode = NULL;
					pEntry->sText = pCode->GetStringValue();
					}

				//	Otherwise we store the code

				else
					pEntry->pCode = pCode->Reference();
				}

			//	Otherwise, we parse it into a string.

			else
				{
				pEntry->pCode = NULL;
				pEntry->sText = ParseTextBlock(pItem->GetContentText(0));
				}
			}
		else if (strEquals(pItem->GetTag(), STRING_TAG))
			{
			pEntry->pCode = NULL;
			pEntry->sText = ParseTextBlock(pItem->GetContentText(0));
			}
		else if (strEquals(pItem->GetTag(), MESSAGE_TAG))
			{
			pEntry->pCode = NULL;
			pEntry->sText = pItem->GetAttribute(TEXT_ATTRIB);
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid element in <Language> block: <%s>"), pItem->GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

bool CLanguageDataBlock::IsCode (const CString &sText) const

//	IsCode
//
//	Returns TRUE if the given string is a piece of code that should be executed,
//	as opposed to plain text (without surrounding quotes).

	{
	char *pPos = sText.GetASCIIZPointer();
	char *pPosEnd = pPos + sText.GetLength();

	//	Skip any leading whitespace

	while (pPos < pPosEnd && strIsWhitespace(pPos))
		pPos++;

	if (pPos == pPosEnd)
		return false;

	//	If this is a paren or a brace or a comment start, then we automatically 
	//	assume it is code.

	if (*pPos == '(' || *pPos == '{' || *pPos == ';')
		return true;

	//	If this is a quote, then we need to do a bit more work.

	else if (*pPos == '\"')
		{
		//	If we have any embedded CRLFs then we assume that this is a
		//	paragraph of text with quotes instead of a quoted string (the latter
		//	needs to be treated as code).

		while (pPos < pPosEnd && *pPos != '\n' && *pPos != '\r')
			pPos++;

		//	Skip all whitespace.

		while (pPos < pPosEnd && strIsWhitespace(pPos))
			pPos++;

		//	If we've hit the end of the text, then we assume this is a one-line
		//	literal string (which is treated as code)

		if (pPos == pPosEnd)
			return true;

		//	Otherwise, we have embedded CRLFs, which means this is a plain text
		//	block (and not code).

		return false;
		}

	//	Otherwise, this is not code

	else
		return false;
	}

void CLanguageDataBlock::MergeFrom (const CLanguageDataBlock &Source)

//	MergeFrom
//
//	Merges entries from Source that we do not already have.

	{
	DEBUG_TRY

	int i;

	for (i = 0; i < Source.m_Data.GetCount(); i++)
		{
		if (m_Data.GetAt(Source.m_Data.GetKey(i)) == NULL)
			{
			SEntry *pEntry = m_Data.Insert(Source.m_Data.GetKey(i));

			if (Source.m_Data[i].pCode)
				pEntry->pCode = Source.m_Data[i].pCode->Reference();
			else
				{
				pEntry->pCode = NULL;
				pEntry->sText = Source.m_Data[i].sText;
				}
			}
		}

	DEBUG_CATCH
	}

CString CLanguageDataBlock::ParseTextBlock (const CString &sText) const

//	ParseTextBlock
//
//	Parses a block of text and trims and whitespace separating different lines.

	{
	enum EStates
		{
		stateStart,
		stateText,
		stateSpace,
		stateLine,
		stateLineAfterSpace,
		stateDoubleLine,
		stateEscape,
		};

	char *pPos = sText.GetASCIIZPointer();
	char *pPosEnd = pPos + sText.GetLength();

	//	Trim out all whitespace at the end of the block.

	while (pPosEnd > pPos && strIsWhitespace(pPosEnd - 1))
		pPosEnd--;

	//	We assume the output is no larger than the input, since we only remove
	//	characters.

	CString sOutput;
	char *pDest = sOutput.GetWritePointer(sText.GetLength());
	char *pDestStart = pDest;

	//	Loop

	EStates iState = stateStart;
	while (pPos < pPosEnd)
		{
		switch (iState)
			{
			case stateStart:
				if (*pPos == '\\')
					iState = stateEscape;
				else if (!strIsWhitespace(pPos))
					{
					*pDest++ = *pPos;
					iState = stateText;
					}
				break;

			case stateText:
				if (*pPos == ' ' || *pPos == '\t')
					{
					*pDest++ = ' ';
					iState = stateSpace;
					}
				else if (*pPos == '\r')
					{ }
				else if (*pPos == '\n')
					iState = stateLine;
				else if (*pPos == '\\')
					iState = stateEscape;
				else
					*pDest++ = *pPos;
				break;

			case stateSpace:
				if (*pPos == ' ' || *pPos == '\t')
					{ }
				else if (*pPos == '\r')
					{ }
				else if (*pPos == '\n')
					iState = stateLineAfterSpace;
				else if (*pPos == '\\')
					iState = stateEscape;
				else
					{
					*pDest++ = *pPos;
					iState = stateText;
					}
				break;

			case stateLine:
				if (*pPos == ' ' || *pPos == '\t')
					{ }
				else if (*pPos == '\r')
					{ }
				else if (*pPos == '\n')
					{
					*pDest++ = '\n';
					*pDest++ = '\n';
					iState = stateDoubleLine;
					}
				else if (*pPos == '\\')
					{
					*pDest++ = ' ';
					iState = stateEscape;
					}
				else
					{
					*pDest++ = ' ';
					*pDest++ = *pPos;
					iState = stateText;
					}
				break;

			case stateLineAfterSpace:
				if (*pPos == ' ' || *pPos == '\t')
					{ }
				else if (*pPos == '\r')
					{ }
				else if (*pPos == '\n')
					{
					*pDest++ = '\n';
					*pDest++ = '\n';
					iState = stateDoubleLine;
					}
				else if (*pPos == '\\')
					iState = stateEscape;
				else
					{
					*pDest++ = *pPos;
					iState = stateText;
					}
				break;

			case stateDoubleLine:
				if (*pPos == ' ' || *pPos == '\t')
					iState = stateSpace;
				else if (*pPos == '\r' || *pPos == '\n')
					{ }
				else if (*pPos == '\\')
					iState = stateEscape;
				else
					{
					*pDest++ = *pPos;
					iState = stateText;
					}
				break;

			case stateEscape:
				if (*pPos == '\\')
					{
					*pDest++ = '\\';
					iState = stateText;
					}
				else if (*pPos == 'n')
					{
					*pDest++ = '\n';
					iState = stateDoubleLine;
					}
				else if (*pPos == '%')
					{
					*pDest++ = '%';
					*pDest++ = '%';
					iState = stateText;
					}
				else
					{
					*pDest++ = *pPos;
					iState = stateText;
					}
				break;
			}

		pPos++;
		}

	//	Done

	sOutput.Truncate((int)(pDest - pDestStart));
	return sOutput;
	}

CLanguageDataBlock::ETranslateResult CLanguageDataBlock::Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItem **retpResult) const

//	Translate
//
//	Translates an entry to either a string or an array of strings.

	{
	//	If we can't find this ID then we can't translate

	const SEntry *pEntry = m_Data.GetAt(sID);
	if (pEntry == NULL)
		return resultNotFound;

	//	If we don't want the text back then all we need to do is return that we
	//	have the text.

	if (retText == NULL && retsText == NULL)
		return resultFound;

	//	If we don't need to run code then we just return the string.

	if (pEntry->pCode == NULL)
		{
		if (retsText)
			{
			*retsText = CLanguage::Compose(pEntry->sText, pData);
			return resultString;
			}
		else
			return resultFound;
		}

	//	Otherwise we have to run some code

	CCodeChainCtx Ctx;
	Ctx.SaveAndDefineSourceVar(pObj);
	Ctx.DefineString(CONSTLIT("aTextID"), sID);
	Ctx.SaveAndDefineDataVar(pData);
	
	ICCItem *pResult = Ctx.Run(pEntry->pCode);	//	LATER:Event
	ETranslateResult iResult = ComposeResult(pResult, pData, retText, retsText, retpResult);
	Ctx.Discard(pResult);

	return iResult;
	}

CLanguageDataBlock::ETranslateResult CLanguageDataBlock::Translate (const CItem &Item, const CString &sID, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItem **retpResult) const

//	Translate
//
//	Translates an entry to either a string or an array of strings.

	{
	//	If we can't find this ID then we can't translate

	const SEntry *pEntry = m_Data.GetAt(sID);
	if (pEntry == NULL)
		return resultNotFound;

	//	If we don't want the text back then all we need to do is return that we
	//	have the text.

	if (retText == NULL && retsText == NULL)
		return resultFound;

	//	If we don't need to run code then we just return the string.

	if (pEntry->pCode == NULL)
		{
		if (retsText)
			{
			*retsText = CLanguage::Compose(pEntry->sText, pData);
			return resultString;
			}
		else
			return resultFound;
		}

	//	Otherwise we have to run some code

	CCodeChainCtx Ctx;
	Ctx.SaveAndDefineSourceVar(NULL);
	Ctx.SaveAndDefineItemVar(Item);
	Ctx.SetItemType(Item.GetType());
	Ctx.DefineString(CONSTLIT("aTextID"), sID);
	Ctx.SaveAndDefineDataVar(pData);
	
	ICCItem *pResult = Ctx.Run(pEntry->pCode);	//	LATER:Event
	ETranslateResult iResult = ComposeResult(pResult, pData, retText, retsText, retpResult);
	Ctx.Discard(pResult);

	return iResult;
	}

bool CLanguageDataBlock::Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, ICCItem **retpResult) const

//	Translate
//
//	Translates to an item. The caller is responsible for discarding.

	{
	int i;

	TArray<CString> List;
	CString sText;
	ICCItem *pResult;
	ETranslateResult iResult = Translate(pObj, sID, pData, &List, &sText, &pResult);

	switch (iResult)
		{
		case resultNotFound:
			return false;

		case resultFound:
			//	This only happens if the caller passed in NULL for retText.
			//	It means that we don't need the result.
			return true;

		case resultArray:
			{
			*retpResult = g_pUniverse->GetCC().CreateLinkedList();

			CCLinkedList *pList = (CCLinkedList *)(*retpResult);
			for (i = 0; i < List.GetCount(); i++)
				pList->AppendString(g_pUniverse->GetCC(), List[i]);

			return true;
			}

		case resultCCItem:
			{
			CString sPlayerName = g_pUniverse->GetPlayerName();
			GenomeTypes iPlayerGenome = g_pUniverse->GetPlayerGenome();

			*retpResult = ComposeCCItem(g_pUniverse->GetCC(), pResult, pData);
			pResult->Discard(&g_pUniverse->GetCC());
			return true;
			}

		case resultString:
			{
			*retpResult = g_pUniverse->GetCC().CreateString(sText);
			return true;
			}

		default:
			ASSERT(false);
			return false;
		}
	}

bool CLanguageDataBlock::Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, TArray<CString> *retText) const

//	Translate
//
//	Translates to an array of text. If not found, return FALSE.

	{
	CString sText;

	ETranslateResult iResult = Translate(pObj, sID, pData, retText, &sText);

	switch (iResult)
		{
		case resultNotFound:
			return false;

		case resultFound:
			//	This only happens if the caller passed in NULL for retText.
			//	It means that we don't need the result.
			return true;

		case resultArray:
			return true;

		case resultString:
			{
			if (retText)
				{
				retText->DeleteAll();
				retText->Insert(sText);
				}
			return true;
			}

		default:
			ASSERT(false);
			return false;
		}
	}

bool CLanguageDataBlock::Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, CString *retsText) const

//	Translate
//
//	Translates to text. If not found, returns FALSE.

	{
	TArray<CString> List;

	ETranslateResult iResult = Translate(pObj, sID, pData, &List, retsText);

	return ComposeTextResult(iResult, List, retsText);
	}

bool CLanguageDataBlock::Translate (const CItem &Item, const CString &sID, ICCItem *pData, CString *retsText) const

//	Translate
//
//	Translates to text. If not found, returns FALSE.

	{
	TArray<CString> List;

	ETranslateResult iResult = Translate(Item, sID, pData, &List, retsText);

	return ComposeTextResult(iResult, List, retsText);
	}

