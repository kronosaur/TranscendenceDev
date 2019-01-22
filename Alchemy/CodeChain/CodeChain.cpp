//	CCodeChain.cpp
//
//	Implementation of CCodeChain object

#include "PreComp.h"
#include "Functions.h"
#include "DefPrimitives.h"

#define INTEGER_POOL								0
#define STRING_POOL									1
#define LIST_POOL									2
#define PRIMITIVE_POOL								3
#define SYMBOLTABLE_POOL							4
#define LAMBDA_POOL									5
#define ATOMTABLE_POOL								6
#define VECTOR_POOL									7
#define DOUBLE_POOL									8

#define POOL_COUNT									9

CCodeChain::CCodeChain (void) :
		m_pGlobalSymbols(NULL)

//	CCodeChain constructor

	{
	}

CCodeChain::~CCodeChain (void)

//	CCodeChain destructor

	{
	CleanUp();
	}

ICCItem *CCodeChain::Apply (ICCItem *pFunc, ICCItem *pArgs, LPVOID pExternalCtx)

//	Apply
//
//	Runs the given function with the given arguments

	{
	ICCItem *pResult;
	CEvalContext EvalCtx;

	//	Set up the context

	EvalCtx.pCC = this;
	EvalCtx.pLexicalSymbols = m_pGlobalSymbols;
	EvalCtx.pLocalSymbols = NULL;
	EvalCtx.pExternalCtx = pExternalCtx;

	//	Evalute the actual code

	pArgs->SetQuoted();

	if (pFunc->IsFunction())
		pResult = pFunc->Execute(&EvalCtx, pArgs);
	else
		pResult = pFunc->Reference();

	return pResult;
	}

ALERROR CCodeChain::Boot (void)

//	Boot
//
//	Initializes the object. Clean up is done in the destructor

	{
	ALERROR error;
	int i;
	ICCItem *pItem;

	//	Initialize memory error

	m_sMemoryError.SetError();
	m_sMemoryError.SetValue(LITERAL("Out of memory"));

	//	Initialize Nil

	pItem = new CCNil;
	if (pItem == NULL)
		return ERR_FAIL;

	m_pNil = pItem->Reference();

	//	Initialize True

	pItem = new CCTrue;
	if (pItem == NULL)
		return ERR_FAIL;

	m_pTrue = pItem->Reference();

	//	Initialize global symbol table

	pItem = CreateSymbolTable();
	if (pItem->IsError())
		return ERR_FAIL;

	m_pGlobalSymbols = pItem;

	//	Register the built-in primitives

	for (i = 0; i < DEFPRIMITIVES_COUNT; i++)
		if (error = RegisterPrimitive(&g_DefPrimitives[i]))
			return error;

	return NOERROR;
	}

void CCodeChain::CleanUp (void)

//	CleanUp
//
//	The oppposite of Boot

	{
	if (m_pGlobalSymbols)
		{
		m_pGlobalSymbols->Discard(this);
		m_pGlobalSymbols = NULL;
		}

	//	Free strings, because if CCodeChain is global, its destructor
	//	might get called after strings have terminated.

	m_sMemoryError.SetValue(NULL_STR);
	}

ICCItem *CCodeChain::CreateAtomTable (void)

//	CreateAtomTable
//
//	Creates an item

	{
	ICCItem *pItem;

	pItem = m_AtomTablePool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pItem->Reset();
	return pItem->Reference();
	}

ICCItem *CCodeChain::CreateBool (bool bValue)

//	CreateBool
//
//	Creates either True or Nil

	{
	if (bValue)
		return CreateTrue();
	else
		return CreateNil();
	}

ICCItem *CCodeChain::CreateError (const CString &sError, ICCItem *pData)

//	CreateError
//
//	Creates an item
//
//	sError: Error messages
//	pData: Item that caused error.

	{
	ICCItem *pError;
	CString sArg;
	CString sErrorLine;

	//	Convert the argument to a string

	if (pData)
		{
		sArg = pData->Print(this);
		sErrorLine = strPatternSubst(LITERAL("%s [%s]"), sError, sArg);
		}
	else
		sErrorLine = sError;

	//	Create the error

	pError = CreateString(sErrorLine);
	pError->SetError();
	return pError;
	}

ICCItem *CCodeChain::CreateErrorCode (int iErrorCode)

//	CreateError
//
//	Creates an item
//
//	iErrorCode: Error code (CCRESULT_???)

	{
	ICCItem *pError;

	pError = CreateInteger(iErrorCode);
	pError->SetError();
	return pError;
	}

ICCItem *CCodeChain::CreateDouble (double dValue)

//	CreateDouble
//
//	Creates a double

	{
	ICCItem *pItem;
	CCDouble *pDouble;

	pItem = m_DoublePool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pDouble = dynamic_cast<CCDouble *>(pItem);
	pDouble->Reset();
	pDouble->SetValue(dValue);

	return pDouble->Reference();
	}

ICCItem *CCodeChain::CreateInteger (int iValue)

//	CreateInteger
//
//	Creates an item

	{
	ICCItem *pItem;
	CCInteger *pInteger;

	pItem = m_IntegerPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pInteger = dynamic_cast<CCInteger *>(pItem);
	pInteger->Reset();
	pInteger->SetValue(iValue);

	return pInteger->Reference();
	}

ICCItem *CCodeChain::CreateLambda (ICCItem *pList, bool bArgsOnly)

//	CreateLambda
//
//	Creates a lambda expression from a list beginning with
//	a lambda symbol

	{
	ICCItem *pItem;
	CCLambda *pLambda;

	pItem = m_LambdaPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pLambda = dynamic_cast<CCLambda *>(pItem);
	pLambda->Reset();

	//	Initialize from list

	if (pList)
		{
		pItem = pLambda->CreateFromList(this, pList, bArgsOnly);
		if (pItem->IsError())
			{
			DestroyLambda(pLambda);
			return pItem;
			}

		pItem->Discard(this);
		}

	//	Done

	return pLambda->Reference();
	}

ICCItem *CCodeChain::CreateLinkedList (void)

//	CreateLinkedList
//
//	Creates an item

	{
	ICCItem *pItem;

	pItem = m_ListPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pItem->Reset();
	return pItem->Reference();
	}

ICCItem *CCodeChain::CreateNumber (double dValue)

//	CreateNumber
//
//	Creates either an integer or a double.

	{
	int iInt = (int)dValue;
	if (dValue == (double)iInt)
		return CreateInteger(iInt);
	else
		return CreateDouble(dValue);
	}

ICCItem *CCodeChain::CreateDoubleIfPossible (const CString &sString)
	{
	bool bFailed;
	double rValue = strToDouble(sString, 0.0, &bFailed);
	if (bFailed)
		return CreateString(sString);
	else
		return CreateDouble(rValue);
	}

ICCItem *CCodeChain::CreateIntegerIfPossible (const CString &sString)
	{
	bool bFailed;
	int iValue = strToInt(sString, 0, &bFailed);
	if (bFailed)
		return CreateString(sString);
	else
		return CreateInteger(iValue);
	}

ICCItem *CCodeChain::CreateLiteral (const CString &sString)

//	CreateLiteral
//
//	Creates either an integer, double, or string, depending.

	{
	enum EStates
		{
		stateStart,
		stateZero,
		stateNumber,
		stateHex,
		stateDecimal,
		stateExponent,
		stateExponentDigits,
		};

	char *pPos = sString.GetASCIIZPointer();
	char *pPosEnd = pPos + sString.GetLength();

	EStates iState = stateStart;
	while (true)
		{
		switch (iState)
			{
			case stateStart:
				if (pPos >= pPosEnd)
					return CreateString(sString);
				else if (*pPos == '0')
					iState = stateZero;
				else if ((*pPos >= '0' && *pPos <= '9') || *pPos == '-' || *pPos == '+')
					iState = stateNumber;
				else
					return CreateString(sString);
				break;

			case stateZero:
				if (pPos >= pPosEnd)
					return CreateInteger(0);
				else if (*pPos >= '0' && *pPos <= '9')
					iState = stateNumber;
				else if (*pPos == '.')
					iState = stateDecimal;
				else if (*pPos == 'e' || *pPos == 'E')
					iState = stateExponent;
				else if (*pPos == 'x' || *pPos == 'X')
					iState = stateHex;
				else
					return CreateString(sString);
				break;

			case stateNumber:
				if (pPos >= pPosEnd)
					return CreateIntegerIfPossible(sString);
				else if (*pPos >= '0' && *pPos <= '9')
					;
				else if (*pPos == '.')
					iState = stateDecimal;
				else if (*pPos == 'e' || *pPos == 'E')
					iState = stateExponent;
				else
					return CreateString(sString);
				break;

			case stateHex:
				if (pPos >= pPosEnd)
					return CreateIntegerIfPossible(sString);
				else if (*pPos >= '0' && *pPos <= '9')
					;
				else if (*pPos >= 'a' && *pPos <= 'f')
					;
				else if (*pPos >= 'A' && *pPos <= 'F')
					;
				else
					return CreateString(sString);
				break;

			case stateDecimal:
				if (pPos >= pPosEnd)
					return CreateDoubleIfPossible(sString);
				else if (*pPos >= '0' && *pPos <= '9')
					;
				else if (*pPos == 'e' || *pPos == 'E')
					iState = stateExponent;
				else
					return CreateString(sString);
				break;

			case stateExponent:
				if (pPos >= pPosEnd)
					return CreateString(sString);
				else if (*pPos == '+' || *pPos == '-')
					iState = stateExponentDigits;
				else if (*pPos >= '0' && *pPos <= '9')
					iState = stateExponentDigits;
				else
					return CreateString(sString);
				break;

			case stateExponentDigits:
				if (pPos >= pPosEnd)
					return CreateDoubleIfPossible(sString);
				else if (*pPos >= '0' && *pPos <= '9')
					;
				else
					return CreateString(sString);
				break;

			default:
				ASSERT(false);
				return CreateString(sString);
			}

		pPos++;
		}
	}

ICCItem *CCodeChain::CreatePrimitive (PRIMITIVEPROCDEF *pDef, IPrimitiveImpl *pImpl)

//	CreatePrimitive
//
//	Creates a primitive function

	{
	ICCItem *pItem;
	CCPrimitive *pPrimitive;

	pItem = m_PrimitivePool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pPrimitive = dynamic_cast<CCPrimitive *>(pItem);
	pPrimitive->SetProc(pDef, pImpl);
	return pPrimitive->Reference();
	}

ICCItem *CCodeChain::CreateString (const CString &sString)

//	CreateString
//
//	Creates an item

	{
	ICCItem *pItem;
	CCString *pString;

	pItem = m_StringPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pString = dynamic_cast<CCString *>(pItem);
	pString->Reset();
	pString->SetValue(sString);
	return pString->Reference();
	}

ICCItem *CCodeChain::CreateSymbolTable (void)

//	CreateSymbolTable
//
//	Creates an item

	{
	ICCItem *pItem;

	pItem = m_SymbolTablePool.CreateItem(this);
	if (pItem->IsError())
		return pItem;

	pItem->Reset();
	return pItem->Reference();
	}

ICCItem *CCodeChain::CreateSystemError (ALERROR error)

//	CreateSystemError
//
//	Generates an error

	{
	ICCItem *pError;

	pError = CreateString(strPatternSubst(LITERAL("ERROR: %d"), error));
	pError->SetError();
	return pError;
	}

ICCItem *CCodeChain::CreateVariant (const CString &sValue)

//	CreateVariant
//
//	Parses sValue and returns either an integer, double, or string.

	{
	char *pPos = sValue.GetASCIIZPointer();
	char *pPosEnd = pPos + sValue.GetLength();

	//	Skip any leading whitespace

	while (strIsWhitespace(pPos))
		pPos++;

	//	If we start with what could be a number, then we try to parse.

	if (strIsDigit(pPos) || *pPos == '+' || *pPos == '-' || *pPos == '.')
		{
		//	See if this is an integer

		bool bFailed;
		char *pNumberEnd;
		int iValue = strParseInt(pPos, 0, &pNumberEnd, &bFailed);
		if (!bFailed && pNumberEnd == pPosEnd)
			return CreateInteger(iValue);

		//	Otherwise, see if this is a double

		double rValue = strParseDouble(pPos, 0.0, &pNumberEnd, &bFailed);
		if (!bFailed && pNumberEnd == pPosEnd)
			return CreateDouble(rValue);

		//	Otherwise, this is a string.

		return CreateString(sValue);
		}

	//	Otherwise, this is a plain string.

	else
		return CreateString(sValue);
	}

ICCItem *CCodeChain::CreateVectorOld (int iSize)

//	CreateVectorOld
//
//	Creates a vector of the given number of elements

	{
	CCVectorOld *pVector;
	ICCItem *pError;

	pVector = new CCVectorOld(this);
	if (pVector == NULL)
		return CreateMemoryError();

	pError = pVector->SetSize(this, iSize);
	if (pError->IsError())
		{
		delete pVector;
		return pError;
		}

	pError->Discard(this);

	//	Done

	return pVector->Reference();
	}

ICCItem *CCodeChain::CreateFilledVector(double dScalar, TArray<int> vShape)

//	CreateFilledVector
//
//	Creates a vector filled with the given scalar

{
	int i;
	int iSize = 0;
	CCVector *pVector;
	ICCItem *pError;
	ICCItem *pItem;

	for (i = 0; i < vShape.GetCount(); i++)
	{
		if (i == 0)
			iSize = vShape[0];
		else
			iSize = iSize*vShape[i];
	};

	pItem = m_VectorPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;
	pItem->Reset();
	pVector = dynamic_cast<CCVector *>(pItem);

	pVector->SetContext(this);
	pVector->SetShape(this, vShape);

	pError = pVector->SetDataArraySize(this, iSize);
	if (pError->IsError())
	{
		delete pVector;
		return pError;
	}

	TArray<double> vContentArray = pVector->GetDataArray();
	for (i = 0; i < iSize; i++)
	{
		vContentArray[i] = dScalar;
	};
	pVector->SetArrayData(this, vContentArray);

	
	pError->Discard(this);
	//	Done
	return pVector->Reference();
}

ICCItem *CCodeChain::CreateVectorGivenContent(TArray<int> vShape, CCLinkedList *pContentList)

//	CreateVectorGivenContent (new)
//
//	Creates a vector with given shape and content

{
	int i;
	CCVector *pVector;
	ICCItem *pItem;

	pItem = m_VectorPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;
	pItem->Reset();

	pVector = dynamic_cast<CCVector *>(pItem);
	pVector->SetContext(this);

	pVector->SetShape(this, vShape);

	pItem = pContentList->GetFlattened(this, NULL);
	if (pItem->IsError())
	{ 
		pVector->Discard(this);
		return pItem;
	};
	CCLinkedList *pFlattenedContentList = dynamic_cast<CCLinkedList *>(pItem);

	TArray<double> vDataArray;
	for (i = 0; i < pFlattenedContentList->GetCount(); i++)
	{
		vDataArray.Insert(pFlattenedContentList->GetElement(i)->GetDoubleValue());
	};
	pVector->SetArrayData(this, vDataArray);

	//	Done
	pFlattenedContentList->Discard(this);
	return pVector->Reference();
}

ICCItem *CCodeChain::CreateVectorGivenContent(TArray<int> vShape, TArray<double> vContentArray)

//	CreateVectorGivenContent (new)
//
//	Creates a vector with given shape and content

{
	int i;
	int iSize = 0;
	CCVector *pVector;
	ICCItem *pError;
	ICCItem *pItem;

	for (i = 0; i < vShape.GetCount(); i++)
	{
		iSize = iSize * vShape[i];
	};

	pItem = m_VectorPool.CreateItem(this);
	if (pItem->IsError())
		return pItem;
	pItem->Reset();
	pVector = dynamic_cast<CCVector *>(pItem);
	pVector->SetContext(this);

	pError = pVector->SetDataArraySize(this, iSize);
	if (pError->IsError())
	{
		delete pVector;
		return pError;
	}

	pVector->SetShape(this, vShape);

	pVector->SetArrayData(this, vContentArray);

	//	Done
	pError->Discard(this);
	return pVector->Reference();
}

ALERROR CCodeChain::DefineGlobal (const CString &sVar, ICCItem *pValue)

//	DefineGlobal
//
//	Defines a global variable programmatically

	{
	ALERROR error;

	//	Create a string item

	ICCItem *pVar = CreateString(sVar);

	//	Add the symbol

	ICCItem *pError;
	pError = m_pGlobalSymbols->AddEntry(this, pVar, pValue);
	pVar->Discard(this);

	//	Check for error

	if (pError->IsError())
		error = ERR_FAIL;
	else
		error = NOERROR;

	pError->Discard(this);

	return error;
	}

ALERROR CCodeChain::DefineGlobalInteger (const CString &sVar, int iValue)
	{
	ALERROR error;
	ICCItem *pValue = CreateInteger(iValue);
	error = DefineGlobal(sVar, pValue);
	pValue->Discard(this);
	return error;
	}

ALERROR CCodeChain::DefineGlobalString (const CString &sVar, const CString &sValue)
	{
	ALERROR error;
	ICCItem *pValue = CreateString(sValue);
	error = DefineGlobal(sVar, pValue);
	pValue->Discard(this);
	return error;
	}

void CCodeChain::DiscardAllGlobals (void)

//	DiscardAllGlobals
//
//	Discard all globals.

	{
	if (m_pGlobalSymbols)
		m_pGlobalSymbols->DeleteAll(this, true);
	}

ICCItem *CCodeChain::Eval (CEvalContext *pEvalCtx, ICCItem *pItem)

//	Eval
//
//	Evaluates the given item and returns a result

	{
	//	Errors always evaluate to themselves

	if (pItem->IsError())
		return pItem->Reference();

	//	If this item is quoted, then return an unquoted item

	if (pItem->IsQuoted())
		{
		//	If this is a literal symbol table then we need to evaluate its 
		//	values.

		if (pItem->IsSymbolTable())
			return EvalLiteralStruct(pEvalCtx, pItem);

		//	HACK: We clone the item so that when we try to modify a literal list we
		//	mody a copy instead of the original.

		else
			{
			ICCItem *pResult = pItem->Clone(this);
			pResult->ClearQuoted();
			return pResult;
			}
		}

	//	Evaluate differently depending on whether or not
	//	this is an atom or a list. If it is an atom, either return
	//	the value or look up the atom in a symbol table. If the item
	//	is a list, try to evaluate as a function

	else if (pItem->IsIdentifier())
		return Lookup(pEvalCtx, pItem);

	//	If this is an expression (a list with multiple terms) then we
	//	try to evaluate it.

	else if (pItem->IsExpression())
		{
		ICCItem *pFunctionName;
		ICCItem *pFunction;
		ICCItem *pArgs;
		ICCItem *pResult;

		//	The first element of the list is the function

		pFunctionName = pItem->Head(this);

		//	We must have a first element since this is a list (but not Nil)

		ASSERT(pFunctionName);

		//	If this is an identifier, then look up the function
		//	in the global symbols

		if (pFunctionName->IsIdentifier())
			pFunction = LookupFunction(pEvalCtx, pFunctionName);

		//	Otherwise, evaluate it

		else
			pFunction = Eval(pEvalCtx, pFunctionName);

		//	If we get an error, return it

		if (pFunction->IsError())
			return pFunction;

		//	Make sure this is a function

		if (!pFunction->IsFunction())
			{
			pFunction->Discard(this);
			return CreateError(LITERAL("Function name expected"), pFunctionName);
			}

		//	Get the arguments

		pArgs = pItem->Tail(this);

		//	Do it

		pResult = pFunction->Execute(pEvalCtx, pArgs);

		//	Handle error by appending the function call that failed

		if (pResult->IsError())
			{
			CCString *pError = dynamic_cast<CCString *>(pResult);
			if (pError)
				{
				CString sError = pError->GetValue();
				if (!sError.IsBlank())
					{
					char *pPos = sError.GetASCIIZPointer() + sError.GetLength() - 1;
					if (*pPos != '#')
						{
						sError.Append(strPatternSubst(CONSTLIT(" ### %s ###"), pItem->Print(this)));
						pError->SetValue(sError);
						}
					}
				}
			}

		//	Done

		pFunction->Discard(this);
		pArgs->Discard(this);
		return pResult;
		}
	
	//	Anything else is a literal so we return it.

	else
		return pItem->Reference();
	}

ICCItem *CCodeChain::EvalLiteralStruct (CEvalContext *pCtx, ICCItem *pItem)

//	EvalLiteralStruct
//
//	Evalues the values in a structure and returns a structure
//	(or an error).

	{
	int i;

	CCSymbolTable *pTable = dynamic_cast<CCSymbolTable *>(pItem);
	if (pTable == NULL)
		return CreateError(CONSTLIT("Not a structure"), pItem);

	ICCItem *pNew = CreateSymbolTable();
	if (pNew->IsError())
		return pNew;

	CCSymbolTable *pNewTable = dynamic_cast<CCSymbolTable *>(pNew);

	//	Loop over all key/value pairs

	for (i = 0; i < pTable->GetCount(); i++)
		{
		CString sKey = pTable->GetKey(i);
		ICCItem *pValue = pTable->GetElement(i);

		ICCItem *pNewKey = CreateString(sKey);
		ICCItem *pNewValue = (pValue ? Eval(pCtx, pValue) : CreateNil());

		ICCItem *pResult = pNewTable->AddEntry(this, pNewKey, pNewValue);
		pNewKey->Discard(this);
		pNewValue->Discard(this);

		if (pResult->IsError())
			{
			pNewTable->Discard(this);
			return pResult;
			}

		pResult->Discard(this);
		}

	//	Done

	return pNewTable;
	}

ICCItem *CCodeChain::EvaluateArgs (CEvalContext *pCtx, ICCItem *pArgs, const CString &sArgValidation)

//	EvaluateArgs
//
//	Evaluate arguments and validate their types

	{
	ICCItem *pArg;
	ICCItem *pNew;
	ICCItem *pError;
	CCLinkedList *pEvalList;
	char *pValidation;
	int i;
	bool bNoEval;

	//	If the argument list if quoted, then it means that the arguments
	//	have already been evaluated. This happens if we've been called by
	//	(apply).

	bNoEval = pArgs->IsQuoted();

	//	Create a list to hold the results

	pNew = CreateLinkedList();
	if (pNew->IsError())
		return pNew;

	pEvalList = dynamic_cast<CCLinkedList *>(pNew);

	//	Start parsing at the beginning

	pValidation = sArgValidation.GetPointer();

	//	If there is a '*' in the validation, figure out
	//	how many arguments it represents.
	//
	//	NOTE: We can't have more than a single '*' in a validation string.

	int iVarArgs = pArgs->GetCount() - (sArgValidation.GetLength() - 1);
	if (iVarArgs < 0)
		{
		pEvalList->Discard(this);
		return CreateError(LITERAL("Insufficient arguments"), NULL);
		}

	//	Loop over each argument

	for (i = 0; i < pArgs->GetCount(); i++)
		{
		ICCItem *pResult;

		pArg = pArgs->GetElement(i);

		//	If we're processing variable args, see if we're done

		if (*pValidation == '*')
			{
			if (iVarArgs == 0)
				pValidation++;
			else
				iVarArgs--;
			}

		//	Evaluate the item. If the arg is 'q' or 'u' then we 
		//	don't evaluate it.

		if (bNoEval || *pValidation == 'q' || *pValidation == 'u')
			pResult = pArg->Reference();

		//	If the arg is 'c' then we don't evaluate unless it is
		//	a lambda expression (or an identifier)

		else if (*pValidation == 'c' && !pArg->IsLambdaExpression() && !pArg->IsIdentifier())
			pResult = pArg->Reference();

		//	Evaluate

		else
			{
			pResult = Eval(pCtx, pArg);

			//	We don't want to return on error because some functions
			//	might want to pass errors around

			if (*pValidation != 'v' && *pValidation != '*')
				{
				if (pResult->IsError())
					{
					pEvalList->Discard(this);
					return pResult;
					}
				}
			}

		//	Check to see if the item is valid

		switch (*pValidation)
			{
			//	We expect a function...

			case 'f':
				{
				if (!pResult->IsNil() && !pResult->IsFunction())
					{
					pError = CreateError(LITERAL("Function expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//  We expect a numeral...
			//
			//	NOTE: We treat integer the same a numeral because it's not always
			//	clear to the user when they've created a double or an integer.
			//	It is up to the actual function to use the integer or double 
			//	value appropriately.

			case 'i':
			case 'n':
				{
				if (pResult->IsIdentifier())
					{
					//	If a string was passed in and we expect a number, then 
					//	convert it!

					if (*pValidation == 'i')
						{
						bool bFailed;
						int iValue = strToInt(pResult->GetStringValue(), 0, &bFailed);
						if (bFailed)
							{
							pError = CreateError(LITERAL("Numeral expected"), pResult);
							pResult->Discard(this);
							pEvalList->Discard(this);
							return pError;
							}

						pResult->Discard(this);
						pResult = CreateInteger(iValue);
						}
					else
						{
						bool bFailed;
						double rValue = strToDouble(pResult->GetStringValue(), 0.0, &bFailed);
						if (bFailed)
							{
							pError = CreateError(LITERAL("Numeral expected"), pResult);
							pResult->Discard(this);
							pEvalList->Discard(this);
							return pError;
							}

						pResult->Discard(this);
						pResult = CreateDouble(rValue);
						}
					}
				else if (!pResult->IsNil() && !pResult->IsNumber())
					{
					pError = CreateError(LITERAL("Numeral expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//  We expect a double...

			case 'd':
				{
				if (pResult->IsIdentifier())
					{
					bool bFailed;
					double rValue = strToDouble(pResult->GetStringValue(), 0.0, &bFailed);
					if (bFailed)
						{
						pError = CreateError(LITERAL("Numeral expected"), pResult);
						pResult->Discard(this);
						pEvalList->Discard(this);
						return pError;
						}

					pResult->Discard(this);
					pResult = CreateDouble(rValue);
					}
				else if (!pResult->IsNil() && !pResult->IsDouble())
					{
					pError = CreateError(LITERAL("Double expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//  We expect a vEctor...

			case 'e':
				{
				if (!(pResult->GetValueType() == ICCItem::Vector))
					{
					pError = CreateError(LITERAL("Vector expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//	We expect a linked list

			case 'k':
				{
				if (pResult->GetClass()->GetObjID() != OBJID_CCLINKEDLIST)
					{
					pError = CreateError(LITERAL("Linked-list expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//	We expect a list

			case 'l':
				{
				if (!pResult->IsList())
					{
					pError = CreateError(LITERAL("List expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//	We expect an identifier

			case 's':
				{
				if (!pResult->IsNil() && !pResult->IsIdentifier())
					{
					pError = CreateError(LITERAL("Identifier expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			case 'q':
				{
				if (!pResult->IsIdentifier())
					{
					pError = CreateError(LITERAL("Identifier expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//	We expect an atom table

			case 'x':
				{
				if (!pResult->IsAtomTable())
					{
					pError = CreateError(LITERAL("Atom table expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//	We expect a symbol table

			case 'y':
				{
				if (!pResult->IsNil() && !pResult->IsSymbolTable())
					{
					pError = CreateError(LITERAL("Symbol table expected"), pResult);
					pResult->Discard(this);
					pEvalList->Discard(this);
					return pError;
					}
				break;
				}

			//	We expect anything

			case 'c':
			case 'u':
			case 'v':
				break;

			//	We expect any number of anythings...

			case '*':
				break;

			//	Too many arguments

			case '\0':
				{
				pError = CreateError(LITERAL("Too many arguments"), NULL);
				pResult->Discard(this);
				pEvalList->Discard(this);
				return pError;
				}

			default:
				ASSERT(false);
			}

		//	Add the result to the list

		pEvalList->Append(*this, pResult);
		pResult->Discard(this);

		//	Next validation sequence (note that *pValidation can never
		//	be '\0' because we return above if we find it)

		if (*pValidation != '*')
			pValidation++;
		}

	//	Make sure we have enough arguments

	if (*pValidation != '\0' && *pValidation != '*')
		{
		pError = CreateError(LITERAL("Insufficient arguments"), NULL);
		pEvalList->Discard(this);
		return pError;
		}

	//	Return the evaluation list

	return pEvalList;
	}

bool CCodeChain::HasIdentifier (ICCItem *pCode, const CString &sIdentifier)
	{
	if (!pCode->IsExpression())
		{
		if (pCode->IsIdentifier())
			return strEquals(pCode->GetStringValue(), sIdentifier);
		else
			return false;
		}
	else
		{
		for (int i = 0; i < pCode->GetCount(); i++)
			{
			ICCItem *pElement = pCode->GetElement(i);
			if (HasIdentifier(pElement, sIdentifier))
				return true;
			}

		return false;
		}
	}

ICCItem *CCodeChain::ListGlobals (void)

//	ListGlobals
//
//	Returns a list of all the symbols in the global symbol table

	{
	return m_pGlobalSymbols->ListSymbols(this);
	}

ICCItem *CCodeChain::Lookup (CEvalContext *pCtx, ICCItem *pItem)

//	Lookup
//
//	Returns the binding for this item

	{
	ICCItem *pBinding = NULL;
	bool bFound;
	ICCItem *pStart;
	int iFrame, iOffset;

	//	Start somewhere

	if (pCtx->pLocalSymbols)
		pStart = pCtx->pLocalSymbols;
	else
		pStart = pCtx->pLexicalSymbols;

	//	If this item already has been resolved, do a quick lookup

	if (pItem->GetBinding(&iFrame, &iOffset))
		{
		while (iFrame > 0)
			{
			pStart = pStart->GetParent();
			iFrame--;
			}

		pBinding = pStart->LookupByOffset(this, iOffset);
		bFound = true;
		}

	//	Otherwise, do a lookup

	else
		{
		bFound = false;
		iFrame = 0;

		while (!bFound && pStart)
			{
			pBinding = pStart->SimpleLookup(this, pItem, &bFound, &iOffset);
			if (!bFound)
				{
				pBinding->Discard(this);
				pBinding = NULL;
				pStart = pStart->GetParent();
				iFrame++;
				}
			}

		//	If we found it and this is a local frame,
		//	set the resolution info

		if (bFound && pStart->IsLocalFrame())
			pItem->SetBinding(iFrame, iOffset);
		}

	//	If there is no binding, return an error

	if (!bFound)
		{
		ASSERT(pBinding == NULL);
		//pBinding->Discard(this);
		pBinding = CreateError(LITERAL("No binding for symbol"), pItem);
		}

	return pBinding;
	}

ICCItem *CCodeChain::LookupFunction (CEvalContext *pCtx, ICCItem *pName)

//	LookupFunction
//
//	Returns the binding for a function

	{
	ICCItem *pBinding;

	//	See if the identifier is already bound

	pBinding = pName->GetFunctionBinding();
	if (pBinding)
		return pBinding;

	//	If not bound, check global scope

	pBinding = m_pGlobalSymbols->Lookup(this, pName);
	if (!pBinding->IsError() || pBinding->GetIntegerValue() != CCRESULT_NOTFOUND)
		{
		pName->SetFunctionBinding(this, pBinding);
		return pBinding;
		}

	//	If not found, check local scope

	if (pCtx)
		{
		pBinding->Discard(this);
		pBinding = Lookup(pCtx, pName);
		if (!pBinding->IsError())
			//	We don't set a function binding because local variables often change
			//	(as when we pass a function as a parameter)
			return pBinding;
		}

	//	If we get this far then we could not find it

	pBinding->Discard(this);
	pBinding = CreateError(LITERAL("Unknown function"), pName);
	return pBinding;
	}

ICCItem *CCodeChain::LookupGlobal (const CString &sGlobal, LPVOID pExternalCtx)

//	LookupGlobal
//
//	Returns the binding for this item

	{
	CEvalContext EvalCtx;
	ICCItem *pItem;
	ICCItem *pResult;

	//	Set up the context

	EvalCtx.pCC = this;
	EvalCtx.pLexicalSymbols = m_pGlobalSymbols;
	EvalCtx.pLocalSymbols = NULL;
	EvalCtx.pExternalCtx = pExternalCtx;

	//	Create a variable

	pItem = CreateString(sGlobal);
	if (pItem->IsError())
		return pItem;

	pResult = Lookup(&EvalCtx, pItem);
	pItem->Discard(this);
	return pResult;
	}

ICCItem *CCodeChain::PoolUsage (void)

//	PoolUsage
//
//	Returns a count of each pool

	{
	int iPoolCount[POOL_COUNT];
	int i;
	ICCItem *pResult;
	CCLinkedList *pList;

	//	Get the counts now so we don't affect the results

	iPoolCount[INTEGER_POOL] = m_IntegerPool.GetCount();
	iPoolCount[STRING_POOL] = m_StringPool.GetCount();
	iPoolCount[LIST_POOL] = m_ListPool.GetCount();
	iPoolCount[PRIMITIVE_POOL] = m_PrimitivePool.GetCount();
	iPoolCount[SYMBOLTABLE_POOL] = m_SymbolTablePool.GetCount();
	iPoolCount[LAMBDA_POOL] = m_LambdaPool.GetCount();
	iPoolCount[ATOMTABLE_POOL] = m_AtomTablePool.GetCount();
	iPoolCount[VECTOR_POOL] = m_VectorPool.GetCount();
	iPoolCount[DOUBLE_POOL] = m_DoublePool.GetCount();

	//	Create

	pResult = CreateLinkedList();
	if (pResult->IsError())
		return pResult;

	pList = (CCLinkedList *)pResult;

	for (i = 0; i < POOL_COUNT; i++)
		{
		ICCItem *pItem;

		//	Make an item for the count

		pItem = CreateInteger(iPoolCount[i]);

		//	Add the item to the list

		pList->Append(*this, pItem);
		pItem->Discard(this);
		}

	return pList;
	}

ICCItem *CCodeChain::TopLevel (ICCItem *pItem, LPVOID pExternalCtx)

//	TopLevel
//
//	Top level command processor

	{
	CEvalContext EvalCtx;

	//	Set up the context

	EvalCtx.pCC = this;
	EvalCtx.pLexicalSymbols = m_pGlobalSymbols;
	EvalCtx.pLocalSymbols = NULL;
	EvalCtx.pExternalCtx = pExternalCtx;

	//	Evalute the actual code

	return Eval(&EvalCtx, pItem);
	}

ALERROR CCodeChain::RegisterPrimitive (PRIMITIVEPROCDEF *pDef, IPrimitiveImpl *pImpl)

//	RegisterPrimitive
//
//	Registers a primitive function implemented in C

	{
	ICCItem *pError;
	ICCItem *pDefinition;

	//	Create a primitive definition

	pDefinition = CreatePrimitive(pDef, pImpl);
	if (pDefinition->IsError())
		{
		pDefinition->Discard(this);
		return ERR_FAIL;
		}

	//	Add to global symbol table

	pError = m_pGlobalSymbols->AddEntry(this, pDefinition, pDefinition);
	if (pError->IsError())
		{
		pError->Discard(this);
		return ERR_FAIL;
		}

	//	Don't need these anymore; AddEntry keeps a reference

	pError->Discard(this);
	pDefinition->Discard(this);

	return NOERROR;
	}

ALERROR CCodeChain::RegisterPrimitives (const SPrimitiveDefTable &Table)

//	RegisterPrimitives
//
//	Registers all primitives in the table

	{
	ALERROR error;
	int i;

	for (i = 0; i < Table.iCount; i++)
		{
		if (error = RegisterPrimitive(&Table.pTable[i]))
			return error;
		}

	return NOERROR;
	}

