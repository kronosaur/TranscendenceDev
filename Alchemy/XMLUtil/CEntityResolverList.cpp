//	CEntityResolverList.cpp
//
//	CEntityResolverList class
//	Copyright (c) 2019 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CString CEntityResolverList::ResolveExternalEntity (const CString &sName, bool *retbFound)

//	ResolveExternalEntity
//
//	Resolve entity

	{
	int i;

	for (i = 0; i < m_Resolvers.GetCount(); i++)
		{
		bool bFound;
		CString sResult = m_Resolvers[i]->ResolveExternalEntity(sName, &bFound);
		if (bFound)
			{
			if (retbFound)
				*retbFound = true;
			return sResult;
			}
		}

	//	Not found

	if (retbFound)
		*retbFound = false;

	return NULL_STR;
	}
