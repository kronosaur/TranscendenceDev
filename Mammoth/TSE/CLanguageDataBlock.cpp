//	CLanguageDataBlock.cpp
//
//	CLanguageDataBlock class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CODE_TAG								CONSTLIT("Code")
#define MESSAGE_TAG								CONSTLIT("Message")
#define RTF_TAG									CONSTLIT("RTF")
#define SCRIPT_TAG								CONSTLIT("Script")
#define STRING_TAG								CONSTLIT("String")
#define TEXT_TAG								CONSTLIT("Text")

#define ID_ATTRIB								CONSTLIT("id")
#define TEXT_ATTRIB								CONSTLIT("text")

#define SCRIPT_ID_PREFIX						CONSTLIT("[script]")

const CLanguageDataBlock CLanguageDataBlock::m_Null;

void CLanguageDataBlock::AccumulateScript (const CDesignType *pSource, const CString &sScript, TSortMap<CString, CScript::SScriptEntry> &Script) const

//	AccumulateScript
//
//	Accumualtes script elements.

	{
	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		const SEntry &Entry = m_Data[i];
		if (!strStartsWith(m_Data.GetKey(i), SCRIPT_ID_PREFIX))
			continue;

		CString sScriptID = strSubString(m_Data.GetKey(i), SCRIPT_ID_PREFIX.GetLength());

		CString sEntryScript;
		CString sOrder;
		CString sHeader;
		if (!ParseScriptParam(sScriptID, &sEntryScript, &sOrder, &sHeader))
			continue;

		//	Do we match

		if (!sScript.IsBlank() && !strEquals(sScript, sEntryScript))
			continue;

		//	Add it

		CScript::SScriptEntry *pEntry = Script.SetAt(sOrder);
		pEntry->pSource = pSource;
		pEntry->sScript = Entry.sText;
		}
	}

void CLanguageDataBlock::AddEntry (const CString &sID, const CString &sText)

//	AddEntry
//
//	Adds an entry to the data block

	{
	bool bIsNew;
	SEntry *pEntry = m_Data.SetAt(sID, &bIsNew);

	//	If not new then we need to free the code block

	if (!bIsNew && pEntry->pCode)
		pEntry->pCode->Discard();

	//	Init

	pEntry->sText = sText;
	pEntry->pCode = NULL;
	}

ICCItemPtr CLanguageDataBlock::ComposeCCItem (ICCItem *pValue, const ICCItem *pData) const

//	ComposeCCItem
//
//	Recursively parse all strings for player string escape codes (e.g., %name%). 
//	Callers must discard the result.

	{
	if (pValue->IsSymbolTable())
		{
		ICCItemPtr pResult(ICCItem::SymbolTable);

		for (int i = 0; i < pValue->GetCount(); i++)
			{
			ICCItemPtr pElement = ComposeCCItem(pValue->GetElement(i), pData);
			ICCItemPtr pKey = ICCItemPtr(pValue->GetKey(i));
			if (!pElement->IsNil())
				pResult->AddEntry(pKey, pElement);
			}

		return pResult;
		}

	else if (pValue->IsIdentifier())
		return ICCItemPtr(CLanguage::Compose(pValue->GetStringValue(), pData));

	else if (pValue->IsList())
		{
		ICCItemPtr pResult(ICCItem::List);

		for (int i = 0; i < pValue->GetCount(); i++)
			{
			ICCItemPtr pElement = ComposeCCItem(pValue->GetElement(i), pData);
			pResult->Append(pElement);
			}

		return pResult;
		}

	else
		return ICCItemPtr(pValue->Reference());
	}

bool CLanguageDataBlock::ComposeCCResult (ETranslateResult iResult, const ICCItem *pData, const TArray<CString> &List, const CString &sText, ICCItem *pCCResult, ICCItemPtr &retResult) const

//	ComposeCCResult
//
//	Takes the result from TranslateEval and returns an appropriate CC result.

	{
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
			retResult = ICCItemPtr(ICCItem::List);

			for (int i = 0; i < List.GetCount(); i++)
				retResult->AppendString(List[i]);

			return true;
			}

		case resultCCItem:
			{
			retResult = ComposeCCItem(pCCResult, pData);
			return true;
			}

		case resultString:
			{
			retResult = ICCItemPtr(sText);
			return true;
			}

		default:
			ASSERT(false);
			return false;
		}
	}

CLanguageDataBlock::ETranslateResult CLanguageDataBlock::ComposeResult (ICCItem *pResult, const ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItemPtr *retpResult) const

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
			//	LATER: Why not just call ComposeCCItem here?
			//	Then we wouldn't have to do it in ComposeCCResult.

			*retpResult = ICCItemPtr(pResult->Reference());
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
	for (int i = 0; i < m_Data.GetCount(); i++)
		if (m_Data[i].pCode)
			m_Data[i].pCode->Discard();

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
	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);
		CString sID = pItem->GetAttribute(ID_ATTRIB);
		if (sID.IsBlank())
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid id in <Language> block"));
			return ERR_FAIL;
			}

		//	Script blocks get a special ID

		bool bIsScriptBlock = strEquals(pItem->GetTag(), SCRIPT_TAG);
		if (bIsScriptBlock)
			sID = strPatternSubst(CONSTLIT("%s%s"), SCRIPT_ID_PREFIX, sID);

		//	Add an entry

		bool bIsNew;
		SEntry *pEntry = m_Data.SetAt(sID, &bIsNew);
		if (!bIsNew)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Duplicate <Language> element: %s"), sID);
			return ERR_FAIL;
			}

		//	Read the text

		bool bForceCode = false;
		if (strEquals(pItem->GetTag(), TEXT_TAG) 
				|| (bForceCode = strEquals(pItem->GetTag(), CODE_TAG)))
			{
			//	If this is code, then link it.

			if (bForceCode || IsCode(pItem->GetContentText(0)))
				{
				//	Link the code

				CCodeChainCtx CCCtx(Ctx.GetUniverse());
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
		else if (strEquals(pItem->GetTag(), RTF_TAG))
			{
			pEntry->pCode = NULL;
			pEntry->sText = ParseTextBlock(pItem->GetContentText(0));
			if (!CTextBlock::IsRTFText(pEntry->sText))
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Language id: %s : Expected RTF text"), sID);
				return ERR_FAIL;
				}
			}
		else if (bIsScriptBlock || strEquals(pItem->GetTag(), STRING_TAG))
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

	int iLeadingLNs = 0;
	while (pPos < pPosEnd && strIsWhitespace(pPos))
		{
		if (*pPos == '\n')
			iLeadingLNs++;

		pPos++;
		}

	if (pPos == pPosEnd)
		return false;

	//	If this is a paren or a brace or a comment start, then we automatically 
	//	assume it is code.

	if (*pPos == '(' || *pPos == '{' || *pPos == ';')
		return true;

	//	If this is a quote, then we need to do a bit more work.

	else if (*pPos == '\"')
		{
		//	If we have at least two lines of whitespace, then we assume this
		//	is a block of text.

		if (iLeadingLNs >= 2)
			return false;

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

bool CLanguageDataBlock::ParseScriptParam (const CString &sValue, CString *retsScript, CString *retsOrder, CString *retsHeader)

//	ParseScriptParam
//
//	Parses a script parameter of the form:
//
//	{script}:{order} [{header}]
//
//	Returns TRUE if we have a valid parameter.

	{
	const char *pPos = sValue.GetASCIIZPointer();

	//	Script

	const char *pStart = pPos;
	while (*pPos && *pPos != ':')
		pPos++;

	if (*pPos != ':')
		return false;

	if (retsScript)
		*retsScript = CString(pStart, pPos - pStart);

	//	Order

	pPos++;
	const char *pOrderStart = pPos;
	while (*pPos && *pPos != ' ')
		pPos++;

	if (retsOrder)
		*retsOrder = CString(pOrderStart, pPos - pOrderStart);

	//	Header

	while (strIsWhitespace(pPos))
		pPos++;

	if (*pPos == '[')
		{
		pPos++;
		const char *pHeaderStart = pPos;
		while (*pPos && *pPos != ']')
			pPos++;

		if (retsHeader)
			*retsHeader = CString(pHeaderStart, pPos - pHeaderStart);

		if (*pPos == ']')
			pPos++;
		}

	return true;
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

bool CLanguageDataBlock::Translate (const CDesignType &Type, const CString &sID, const SParams &Params, ICCItemPtr &retResult) const

//	Translate
//
//	Translates to an item.

	{
	TArray<CString> List;
	CString sText;
	ICCItemPtr pResult;

	ETranslateResult iResult = TranslateEval(Type, sID, Params, &List, &sText, &pResult);

	return ComposeCCResult(iResult, Params.pData, List, sText, pResult, retResult);
	}

CLanguageDataBlock::ETranslateResult CLanguageDataBlock::TranslateEval (const CDesignType &Type, const CString &sID, const SParams &Params, TArray<CString> *retText, CString *retsText, ICCItemPtr *retpResult) const

//	TranslateEval
//
//	Translates an entry, evaluating code, if necessary.

	{
	//	If we can resolve this without running code, then do it.

	ETranslateResult iResult;
	const SEntry *pEntry = TranslateTry(sID, Params.pData, iResult, retText, retsText);
	if (pEntry == NULL)
		return iResult;

	//	Otherwise we have to run some code

	CCodeChainCtx Ctx(Type.GetUniverse());
	Ctx.DefineContainingType(&Type);
	Ctx.SaveAndDefineSourceVar(Params.pSource);
	Ctx.SaveAndDefineDataVar(Params.pData);
	if (Params.pItem)
		{
		Ctx.SaveAndDefineItemVar(*Params.pItem);
		Ctx.SetItemType(Params.pItem->GetType());
		}

	Ctx.DefineString(CONSTLIT("aTextID"), sID);
	
	ICCItemPtr pResult = Ctx.RunCode(pEntry->pCode);	//	LATER:Event
	return ComposeResult(pResult, Params.pData, retText, retsText, retpResult);
	}

bool CLanguageDataBlock::TranslateText (const CDesignType &Type, const CString &sID, const SParams &Params, CString *retsText) const

//	TranslateText
//
//	Translates to text. If not found, returns FALSE.

	{
	TArray<CString> List;

	ETranslateResult iResult = TranslateEval(Type, sID, Params, &List, retsText);

	return ComposeTextResult(iResult, List, retsText);
	}

const CLanguageDataBlock::SEntry *CLanguageDataBlock::TranslateTry (const CString &sID, const ICCItem *pData, ETranslateResult &retiResult, TArray<CString> *retText, CString *retsText) const

//	TranslateTry
//
//	This is a helper function that looks up the ID and tries to translate to a
//	simple string. We return a valid entry, then it means that we need to run
//	code to result.
//
//	If we return NULL, it means that we have completed the request and 
//	retiResult is valid.

	{
	//	If we can't find this ID then we can't translate

	const SEntry *pEntry = m_Data.GetAt(sID);
	if (pEntry == NULL)
		{
		retiResult = resultNotFound;
		return NULL;
		}

	//	If we don't want the text back then all we need to do is return that we
	//	have the text.

	else if (retText == NULL && retsText == NULL)
		{
		retiResult = resultFound;
		return NULL;
		}

	//	If we have code, then our caller will need to handle it.

	else if (pEntry->pCode)
		return pEntry;

	//	Otherwise, we can just return the string.

	else if (retsText)
		{
		*retsText = CLanguage::Compose(pEntry->sText, pData);
		retiResult = resultString;
		return NULL;
		}

	//	Or we just say we have a result

	else
		{
		retiResult = resultFound;
		return NULL;
		}
	}
