//	CCLambda.cpp
//
//	Implements CCLambda class

#include "PreComp.h"

static CObjectClass<CCLambda>g_Class(OBJID_CCLAMBDA, NULL);

CCLambda::CCLambda (void) : ICCAtom(&g_Class),
		m_pArgList(NULL),
		m_pCode(NULL),
		m_pLocalSymbols(NULL)

//	CCLambda constructor

	{
	}

ICCItem *CCLambda::Clone (CCodeChain *pCC)

//	Clone
//
//	Clone this item

	{
	ICCItem *pNew;
	CCLambda *pClone;

	pNew = pCC->CreateLambda(NULL, false);
	if (pNew->IsError())
		return pNew;

	pClone = dynamic_cast<CCLambda *>(pNew);

	pClone->CloneItem(this);

	if (m_pArgList)
		pClone->m_pArgList = m_pArgList->Reference();
	else
		pClone->m_pArgList = NULL;

	if (m_pCode)
		pClone->m_pCode = m_pCode->Reference();
	else
		pClone->m_pCode = NULL;

	if (m_pLocalSymbols)
		pClone->m_pLocalSymbols = m_pLocalSymbols->Reference();
	else
		pClone->m_pLocalSymbols = NULL;

	return pClone;
	}

ICCItem *CCLambda::CreateFromList (ICCItem *pList, bool bArgsOnly)

//	CreateFromList
//
//	Initializes from a lambda list. Returns True if successful; error otherwise.
//	The list must have exactly three elements: 
//		the symbol lambda
//		a list of arguments
//		a body of code

	{
	ICCItem *pArgs;
	ICCItem *pBody;

	//	The first element must be the symbol lambda

	if (bArgsOnly)
		{
		pArgs = pList->GetElement(0);
		pBody = pList->GetElement(1);
		}
	else
		{
		pArgs = pList->GetElement(0);
		if (pArgs == NULL || !pArgs->IsLambdaSymbol())
			return CCodeChain::CreateError(LITERAL("Lambda symbol expected"), pArgs);

		pArgs = pList->GetElement(1);
		pBody = pList->GetElement(2);
		}

	//	The next item must be a list of arguments

	if (pArgs == NULL || !pArgs->IsList())
		return CCodeChain::CreateError(LITERAL("Argument list expected"), pArgs);

	m_pArgList = pArgs->Reference();

	//	The next item must exist

	if (pBody == NULL)
		{
		m_pArgList->Discard();
		m_pArgList = NULL;
		return CCodeChain::CreateError(LITERAL("Code expected"), pList);
		}

	m_pCode = pBody->Reference();
	m_pLocalSymbols = NULL;

	//	Done

	return CCodeChain::CreateTrue();
	}

void CCLambda::DestroyItem (void)

//	DestroyItem
//
//	Destroy this item

	{
	//	Delete our references

	if (m_pArgList)
		m_pArgList->Discard();

	if (m_pCode)
		m_pCode->Discard();

	if (m_pLocalSymbols)
		m_pLocalSymbols->Discard();

	//	Done

	CCodeChain::DestroyLambda(this);
	}

ICCItem *CCLambda::Execute (CEvalContext *pCtx, ICCItem *pArgs)

//	Execute
//
//	Executes the function and returns a result

	{
	CCodeChain *pCC = pCtx->pCC;
	ICCItem *pItem;
	ICCItem *pOldSymbols;
	ICCItem *pLocalSymbols;
	ICCItem *pVar;
	ICCItem *pArg;
	ICCItem *pResult;
	int i;
	bool bNoEval;

	//	We must have been initialized

	if (m_pArgList == NULL || m_pCode == NULL)
		return pCC->CreateNil();

	//	If the argument list if quoted, then it means that the arguments
	//	have already been evaluated. This happens if we've been called by
	//	(apply).

	bNoEval = pArgs->IsQuoted();

	//	Set up the symbol table

	pLocalSymbols = pCC->CreateSymbolTable();
	if (pLocalSymbols->IsError())
		return pLocalSymbols;

	//	Loop over each item and associate it

	for (i = 0; i < m_pArgList->GetCount(); i++)
		{
		pVar = m_pArgList->GetElement(i);
		pArg = pArgs->GetElement(i);

		//	If the name of this variable is %args, then the rest of the arguments
		//	should go into a list

		if (strCompareAbsolute(pVar->GetStringValue(), CONSTLIT("%args")) == 0)
			{
			ICCItem *pVarArgs;

			//	If there are arguments left, add them to a list

			if (pArg)
				{
				int j;
				CCLinkedList *pList;

				//	Create a list

				pVarArgs = pCC->CreateLinkedList();
				if (pVarArgs->IsError())
					{
					pLocalSymbols->Discard();
					return pVarArgs;
					}
				pList = (CCLinkedList *)pVarArgs;

				//	Add each argument to the list

				for (j = i; j < pArgs->GetCount(); j++)
					{
					pArg = pArgs->GetElement(j);

					if (bNoEval)
						pResult = pArg->Reference();
					else
						pResult = pCC->Eval(pCtx, pArg);

					pList->Append(pResult);
					pResult->Discard();
					}
				}
			else
				pVarArgs = pCC->CreateNil();

			//	Add to the local symbol table

			pItem = pLocalSymbols->AddEntry(pVar, pVarArgs);
			pVarArgs->Discard();
			}

		//	Bind the variable to the argument

		else if (pArg == NULL)
			pItem = pLocalSymbols->AddEntry(pVar, pCC->CreateNil());
		else
			{
			ICCItem *pResult;

			//	Evaluate the arg and add to the table. If the whole list is quoted
			//	or if the arg is preceded by '%' then we don't evaluate the arge

			if (bNoEval || *(pVar->GetStringValue().GetASCIIZPointer()) == '%')
				pResult = pArg->Reference();
			else
				{
				pResult = pCC->Eval(pCtx, pArg);

				//	HACK: If we get back an error and it is "Function name expected"
				//	then we need to exit. We can't just exit with any error because
				//	we might want to pass an error into a function.

				if (pResult->IsError()
						&& strStartsWith(pResult->GetStringValue(), CONSTLIT("Function name expected")))
					{
					pLocalSymbols->Discard();
					return pResult;
					}
				}

			pItem = pLocalSymbols->AddEntry(pVar, pResult);
			pResult->Discard();
			}

		//	Check for error

		if (pItem->IsError())
			{
			pLocalSymbols->Discard();
			return pItem;
			}

		pItem->Discard();
		}

	//	Setup the context. If the lambda expression has a local symbol scope
	//	then that is our parent; otherwise, our parent is the global scope

	if (m_pLocalSymbols)
		pLocalSymbols->SetParent(m_pLocalSymbols);
	else
		pLocalSymbols->SetParent(pCtx->pLexicalSymbols);

	pOldSymbols = pCtx->pLocalSymbols;
	pCtx->pLocalSymbols = pLocalSymbols;

	//	Evalute the code

	pResult = pCC->Eval(pCtx, m_pCode);

	//	Clean up

	pCtx->pLocalSymbols = pOldSymbols;
	pLocalSymbols->Discard();

	return pResult;
	}

CString CCLambda::Print (DWORD dwFlags)

//	Print
//
//	Prints the lambda expression

	{
	CString sString;

	//	Start with lambda

	if (IsQuoted())
		sString = LITERAL("'(lambda ");
	else
		sString = LITERAL("(lambda ");

	//	Add arguments

	sString.Append(m_pArgList->Print(dwFlags));
	sString.Append(CONSTLIT(" "));

	//	Add code

	sString.Append(m_pCode->Print(dwFlags));
	
	//	Done

	sString.Append(CONSTLIT(")"));
	return sString;
	}

void CCLambda::Reset (void)

//	Reset
//
//	Reset the internal variables

	{
	m_pArgList = NULL;
	m_pCode = NULL;
	m_pLocalSymbols = NULL;
	}

void CCLambda::SetLocalSymbols (CCodeChain *pCC, ICCItem *pSymbols)

//	SetLocalSymbols
//
//	Associates local symbols 

	{
	if (m_pLocalSymbols)
		m_pLocalSymbols->Discard();

#ifdef TRUE_CLOSURES
	//	For closures to work, we need a reference to our parent's local symbols
	//	so we can manipulate them.

	m_pLocalSymbols = pSymbols->Reference();
#else
	//	We clone the symbol table because the values might change
	//	later and we want the values at this point in time.

	m_pLocalSymbols = pSymbols->Clone(pCC);
#endif
	}

