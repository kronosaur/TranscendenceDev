//	CCPrimitive.cpp
//
//	Implements CCPrimitive class

#include "PreComp.h"

static CObjectClass<CCPrimitive>g_Class(OBJID_CCPRIMITIVE, NULL);

CCPrimitive::CCPrimitive (void) : ICCAtom(&g_Class)

//	CCInteger constructor

	{
	}

ICCItem *CCPrimitive::Clone (CCodeChain *pCC)

//	Clone
//
//	Clone this item

	{
	//	Better not happen
	ASSERT(false);
	return NULL;
	}

void CCPrimitive::DestroyItem (CCodeChain *pCC)

//	DestroyItem
//
//	Destroy this item

	{
	pCC->DestroyPrimitive(this);
	}

ICCItem *CCPrimitive::Execute (CEvalContext *pCtx, ICCItem *pArgs)

//	Execute
//
//	Executes the function and returns a result

	{
	bool bCustomArgEval = ((m_dwFlags & PPFLAG_CUSTOM_ARG_EVAL) ? true : false);

	//	Evaluate args, if necessary

	ICCItem *pEvalArgs;
	if (!bCustomArgEval)
		{
		pEvalArgs = pCtx->pCC->EvaluateArgs(pCtx, pArgs, m_sArgPattern);
		if (pEvalArgs->IsError())
			return pEvalArgs;
		}
	else
		pEvalArgs = pArgs;

	//	Invoke the function

	ICCItem *pResult;
	bool bReportError = false;
	try
		{
		if (m_dwFlags & PPFLAG_METHOD_INVOKE)
			pResult = ((IPrimitiveImpl *)m_pfFunction)->InvokeCCPrimitive(pCtx, pEvalArgs, m_dwData);
		else
			pResult = ((PRIMITIVEPROC)m_pfFunction)(pCtx, pEvalArgs, m_dwData);
		}
	catch (...)
		{
		bReportError = true;
		}

	//	Report error

	if (bReportError)
		{
		CString sArgs;
		try
			{
			sArgs = pEvalArgs->Print(pCtx->pCC);
			}
		catch (...)
			{
			sArgs = CONSTLIT("[invalid arg item]");
			}

		CString sError = strPatternSubst(CONSTLIT("Exception in %s; arg = %s"), m_sName, sArgs);
		pResult = pCtx->pCC->CreateError(sError, pEvalArgs);
		kernelDebugLogString(sError);
		}

	//	Done

	if (!bCustomArgEval)
		pEvalArgs->Discard(pCtx->pCC);

	return pResult;
	}

CString CCPrimitive::Print (CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Render as text

	{
	return strPatternSubst(LITERAL("[primitive: %s]"), m_sName);
	}

void CCPrimitive::Reset (void)

//	Reset
//
//	Reset the internal variables

	{
	//	Better not happen
	ASSERT(false);
	}

void CCPrimitive::SetProc (PRIMITIVEPROCDEF *pDef, IPrimitiveImpl *pImpl)

//	SetProc
//
//	Initializes primitive

	{
	m_sName = LITERAL(pDef->pszName);
	m_dwData = pDef->dwData;
	m_dwFlags = pDef->dwFlags;

	//	Description

	if (pDef->pszDescription && *(pDef->pszDescription))
		m_sDesc = LITERAL(pDef->pszDescription);

	//	Arg pattern

	if (pDef->pszArguments)
		m_sArgPattern = LITERAL(pDef->pszArguments);
	else
		m_dwFlags |= PPFLAG_CUSTOM_ARG_EVAL;

	//	Function

	if (pImpl)
		{
		m_pfFunction = pImpl;
		m_dwFlags |= PPFLAG_METHOD_INVOKE;
		}
	else
		m_pfFunction = pDef->pfFunction;
	}

