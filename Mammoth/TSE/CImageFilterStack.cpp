//	CImageFilterStack.cpp
//
//	CImageFilterStack class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CImageFilterStack::operator== (const CImageFilterStack &Val) const

//	CImageFilterStack operator ==

	{
	int i;

	if (m_Stack.GetCount() != Val.m_Stack.GetCount())
		return false;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i] != Val.m_Stack[i])
			return false;

	return true;
	}

void CImageFilterStack::ApplyTo (CG32bitImage &Image) const

//	ApplyTo
//
//	Applies all filters to the given image.

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		m_Stack[i].ApplyTo(Image);
	}

ALERROR CImageFilterStack::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Load from XML.

	{
	ALERROR error;
	int i;

	m_Stack.DeleteAll();
	m_Stack.InsertEmpty(Desc.GetContentElementCount());

	for (i = 0; i < Desc.GetContentElementCount(); i++)
		{
		if (error = m_Stack[i].InitFromXML(Ctx, *Desc.GetContentElement(i)))
			return error;
		}

	return NOERROR;
	}
