//	CDifferentiatedItem.cpp
//
//	CDifferentiatedItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TSharedPtr<CItemEnhancementStack> CDifferentiatedItem::m_pNullEnhancements(new CItemEnhancementStack);

void CDifferentiatedItem::ReportEventError (const CSpaceObject *pSource, const CString &sEvent, const ICCItem &ErrorItem) const

//	ReportEventError
//
//	Reports an event error.

	{
	if (pSource)
		pSource->ReportEventError(sEvent, &ErrorItem);
	else
		GetType().ReportEventError(sEvent, &ErrorItem);
	}
