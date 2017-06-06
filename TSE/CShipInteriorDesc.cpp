//	CShipInteriorDesc.cpp
//
//	CShipInteriorDesc class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define COMPARTMENT_TAG							CONSTLIT("Compartment")
#define SECTION_TAG								CONSTLIT("Section")

#define ATTACH_TO_ATTRIB						CONSTLIT("attachTo")
#define CLASS_ATTRIB							CONSTLIT("class")
#define HIT_POINTS_ATTRIB						CONSTLIT("hitPoints")
#define ID_ATTRIB								CONSTLIT("id")
#define NAME_ATTRIB								CONSTLIT("name")
#define POS_X_ATTRIB							CONSTLIT("posX")
#define POS_Y_ATTRIB							CONSTLIT("posY")
#define SIZE_X_ATTRIB							CONSTLIT("sizeX")
#define SIZE_Y_ATTRIB							CONSTLIT("sizeY")
#define TYPE_ATTRIB								CONSTLIT("type")

#define TYPE_CARGO								CONSTLIT("cargo")
#define TYPE_GENERAL							CONSTLIT("general")
#define TYPE_MAIN_DRIVE							CONSTLIT("mainDrive")

static TStaticStringTable<TStaticStringEntry<ECompartmentTypes>, 3> COMPARTMENT_TYPE_TABLE = {
	"cargo",				deckCargo,
	"general",				deckGeneral,
	"mainDrive",			deckMainDrive,
	};

ALERROR CShipInteriorDesc::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Bind

	{
	ALERROR error;
	int i;

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		if (error = m_Compartments[i].Class.Bind(Ctx))
			return error;
		}

	return NOERROR;
	}

int CShipInteriorDesc::GetHitPoints (void) const

//	GetHitPoints
//
//	Returns the total number of hit points of all compartments.

	{
	int i;

	int iTotalHP = 0;
	for (i = 0; i < m_Compartments.GetCount(); i++)
		iTotalHP += m_Compartments[i].iMaxHP;

	return iTotalHP;
	}

ALERROR CShipInteriorDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;
	int i;

	//	Load all compartments

	m_Compartments.InsertEmpty(pDesc->GetContentElementCount());
	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pComp = pDesc->GetContentElement(i);
		SCompartmentDesc &Comp = m_Compartments[i];

		//	Init data common to all

		Comp.sID = pComp->GetAttribute(ID_ATTRIB);

		if (!pComp->FindAttribute(NAME_ATTRIB, &Comp.sName))
			Comp.sName = CONSTLIT("interior compartment");

		Comp.iType = ParseCompartmentType(pComp->GetAttribute(TYPE_ATTRIB));
		if (Comp.iType == deckUnknown)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown compartment type: %s"), pComp->GetAttribute(TYPE_ATTRIB));
			return ERR_FAIL;
			}

		//	Parse an interior compartment

		if (strEquals(pComp->GetTag(), COMPARTMENT_TAG))
			{
			//	Hit points

			Comp.iMaxHP = pComp->GetAttributeIntegerBounded(HIT_POINTS_ATTRIB, 0, -1, 0);

			//	Position and size

			int xPos = pComp->GetAttributeInteger(POS_X_ATTRIB);
			int yPos = pComp->GetAttributeInteger(POS_Y_ATTRIB);
			int cxWidth = pComp->GetAttributeInteger(SIZE_X_ATTRIB);
			int cyHeight = pComp->GetAttributeInteger(SIZE_Y_ATTRIB);

			//	Input is in Cartessian coordinate; we need to convert to image
			//	coordinates relative to the center.

			Comp.rcPos.left = xPos - (cxWidth / 2);
			Comp.rcPos.right = Comp.rcPos.left + cxWidth;
			Comp.rcPos.top = -yPos - (cyHeight / 2);
			Comp.rcPos.bottom = Comp.rcPos.top + cyHeight;

			//	If no coordinates then this is a default compartment

			if (cxWidth == 0 && cyHeight == 0)
				Comp.fDefault = true;
			}

		//	Parse an attached section

		else if (strEquals(pComp->GetTag(), SECTION_TAG))
			{
			//	We need a class

			if (error = Comp.Class.LoadUNID(Ctx, pComp->GetAttribute(CLASS_ATTRIB)))
				return error;

			if (Comp.Class.GetUNID() == 0)
				{
				Ctx.sError = CONSTLIT("Class required for ship <Section>.");
				return ERR_FAIL;
				}

			//	Attach point

			Comp.sAttachID = pComp->GetAttribute(ATTACH_TO_ATTRIB);

			if (!Comp.AttachPos.InitFromXML(pComp, C3DObjectPos::FLAG_NO_XY))
				{
				Ctx.sError = CONSTLIT("Invalid attach pos.");
				return ERR_FAIL;
				}

			Comp.fIsAttached = true;
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid interior element: %s"), pComp->GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

ECompartmentTypes CShipInteriorDesc::ParseCompartmentType (const CString &sValue)

//	ParseCompartmentType
//
//	Returns a type

	{
	if (sValue.IsBlank())
		return deckGeneral;

	const TStaticStringEntry<ECompartmentTypes> *pEntry = COMPARTMENT_TYPE_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return deckUnknown;

	return pEntry->Value;
	}
