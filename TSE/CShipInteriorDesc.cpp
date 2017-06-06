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

void CShipInteriorDesc::CalcCompartmentPositions (int iScale, TArray<CVector> &Result) const

//	CalcCompartmentPositions
//
//	Creates an array of all compartments and stores the position relative to
//	the center of the root object. Positions are Cartessian and in pixels.

	{
	int i;

	//	Initialize

	Result.DeleteAll();
	Result.InsertEmpty(m_Compartments.GetCount());

	//	Positions may be relative to other compartments, so we first need to
	//	generate a map of positions by ID

	TSortMap<CString, CVector> PosByID;

	//	Add rectangles for each compartment.

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		const SCompartmentDesc &CompDesc = m_Compartments[i];
		if (!CompDesc.fIsAttached)
			continue;

		//	Compute the position of what we're attached to.

		CVector vOrigin;
		if (CompDesc.sAttachID.IsBlank() || !PosByID.Find(CompDesc.sAttachID, &vOrigin))
			vOrigin = CVector();

		//	Compute the position

		CVector vPos;
		m_Compartments[i].AttachPos.CalcCoord(iScale, &vPos);
		Result[i] = vOrigin + (vPos / g_KlicksPerPixel);

		//	Set this position in the map

		if (CompDesc.sID)
			PosByID.SetAt(CompDesc.sID, Result[i]);
		}
	}

int CShipInteriorDesc::CalcImageSize (CShipClass *pClass, CVector *retvOrigin) const

//	CalcImageSize
//
//	Computes the size of the resulting image if we paint all the attached 
//	components together.

	{
	int i;

	int iScale = pClass->GetImage().GetImageViewportSize();

	//	Compute the position of all compartments

	TArray<CVector> Pos;
	CalcCompartmentPositions(iScale, Pos);

	//	We start with a rectangle for the main (root) image (centered at 0,0).

	int cxWidth = pClass->GetImage().GetImageWidth();
	int cxHalfWidth = cxWidth / 2;
	RECT rcImage;
	rcImage.left = -cxHalfWidth;
	rcImage.right = rcImage.left + cxWidth;
	rcImage.top = -cxHalfWidth;
	rcImage.bottom = rcImage.top + cxWidth;

	//	Add rectangles for each compartment.

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		const SCompartmentDesc &CompDesc = m_Compartments[i];
		CShipClass *pClass = CompDesc.Class;
		if (pClass == NULL)
			continue;

		cxWidth = pClass->GetImage().GetImageWidth();
		cxHalfWidth = cxWidth / 2;

		//	Offset from center of rect

		int x = (int)mathRound(Pos[i].GetX());
		int y = -(int)mathRound(Pos[i].GetY());

		//	Create a rectangle.

		RECT rcComp;
		rcComp.left = x - cxHalfWidth;
		rcComp.right = rcComp.left + cxWidth;
		rcComp.top = y - cxHalfWidth;
		rcComp.bottom = rcComp.top + cxWidth;

		//	Add to rect

		::UnionRect(&rcImage, &rcImage, &rcComp);
		}

	//	Make sure the resuting rect is square

	int cxImage = RectWidth(rcImage);
	int cyImage = RectHeight(rcImage);
	int cxSize = Max(cxImage, cyImage);

	if (cxSize > cxImage)
		{
		int cxExtra = cxSize - cxImage;
		int cxHalfExtra = cxExtra / 2;

		rcImage.left -= cxHalfExtra;
		rcImage.right += cxExtra - cxHalfExtra;
		}

	else if (cxSize > cyImage)
		{
		int cyExtra = cxSize - cyImage;
		int cyHalfExtra = cyExtra / 2;

		rcImage.top -= cyHalfExtra;
		rcImage.bottom += cyExtra - cyHalfExtra;
		}

	//	If necessary, compute the origin of the rect (where the main object is 
	//	centered) relative to the center of the rect.

	if (retvOrigin)
		*retvOrigin = CVector(-(cxSize / 2) - rcImage.left, (cxSize / 2) + rcImage.top);

	//	Return the size

	return cxSize;
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

	m_fHasAttached = false;

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
			m_fHasAttached = true;
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
