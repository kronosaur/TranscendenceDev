//	CFunctionContextWrapper.cpp
//
//	CFunctionContextWrapper class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CFunctionContextWrapper::CFunctionContextWrapper (ICCItem *pFunction) : ICCAtom(NULL),
		m_pExtension(NULL)

//	CFunctionContextWrapper constructor

	{
	Reference();
	m_pFunction = pFunction->Reference();
	}

ICCItem *CFunctionContextWrapper::Clone (CCodeChain *pCC)

//	Clone
//
//	Clone the item

	{
	ASSERT(false);
	return NULL;
	}

void CFunctionContextWrapper::DestroyItem (void)

//	DestroyItem
//
//	Destroy

	{
	m_pFunction->Discard();
	delete this;
	}

ICCItem *CFunctionContextWrapper::Execute (CEvalContext *pCtx, ICCItem *pArgs)

//	Execute
//
//	Execute the function

	{
	//	Set some context before we execute

	CCodeChainCtx *pTransCtx = (CCodeChainCtx *)pCtx->pExternalCtx;
	CExtension *pOldExtension = pTransCtx->GetExtension();
	pTransCtx->SetExtension(m_pExtension);

	//	Execute

	ICCItem *pResult = m_pFunction->Execute(pCtx, pArgs);

	//	Restore context

	pTransCtx->SetExtension(pOldExtension);

	return pResult;
	}

//	CAddFunctionContextWrapper -------------------------------------------------

ICCItem *CAddFunctionContextWrapper::Transform (ICCItem *pItem)

//	Transform
//
//	Add a wrapper, if necessary

	{
	//	If the item is a lambda expression, then we add the wrapper.

	if (pItem->IsLambdaFunction() 
			&& dynamic_cast<CFunctionContextWrapper *>(pItem) == NULL)
		{
		//	Create the wrapper for the function

		CFunctionContextWrapper *pWrapper = new CFunctionContextWrapper(pItem);
		pWrapper->SetExtension(m_pExtension);

		//	Done

		return pWrapper;
		}
	else
		return pItem->Reference();
	}
