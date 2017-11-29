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

CShipInteriorDesc::CShipInteriorDesc (void) :
		m_fHasAttached(false),
		m_fIsMultiHull(false)

//	CShipInteriorDesc constructor

	{
	}

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

int CShipInteriorDesc::CalcPaintOrder (int iIndex, const TSortMap<CString, int> &ByID, TArray<int> &PaintOrder) const

//	CalcPaintOrder
//
//	Recursively calculates paint order based on what we're attached to. PaintOrder
//	is an array of all compartments. Higher numbers paint before lower numbers.

	{
	const SCompartmentDesc &Desc = m_Compartments[iIndex];

	//	If we're not attached to another section, then it means that we're 
	//	attached to the main body, so our paint order is 0.

	if (Desc.sAttachID.IsBlank())
		return 0;

	//	Otherwise, lookup the index of what we're attached to.

	int iAttachedTo;
	if (!ByID.Find(Desc.sAttachID, &iAttachedTo) || iAttachedTo == iIndex)
		return 0;

	//	If we don't have a paint order we need to recursively figure it out.

	if (PaintOrder[iAttachedTo] == -1)
		{
		//	We initialize ourselves to some value in case there is a circular
		//	loop.

		PaintOrder[iIndex] = 0;

		//	Recurse.

		PaintOrder[iAttachedTo] = CalcPaintOrder(iAttachedTo, ByID, PaintOrder);
		}

	//	If we have a paint order for that, then we're +1 that (that is, we
	//	paint before what we're attached to).

	return PaintOrder[iAttachedTo] + 1;
	}

void CShipInteriorDesc::DebugPaint (CG32bitImage &Dest, int x, int y, int iRotation, int iScale) const

//	DebugPaint
//
//	Paints the outline of the compartments.

	{
	int i;
	CG32bitPixel rgbColor = CG32bitPixel(0, 255, 255);

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		const SCompartmentDesc &Desc = m_Compartments[i];

		//	Skip attached compartments

		if (Desc.fIsAttached)
			continue;

		//	Generate points for the rectangle.

		CVector vUL(Desc.rcPos.left, -Desc.rcPos.top);
		vUL = vUL.Rotate(iRotation);

		CVector vUR(Desc.rcPos.right, -Desc.rcPos.top);
		vUR = vUR.Rotate(iRotation);

		CVector vLR(Desc.rcPos.right, -Desc.rcPos.bottom);
		vLR = vLR.Rotate(iRotation);

		CVector vLL(Desc.rcPos.left, -Desc.rcPos.bottom);
		vLL = vLL.Rotate(iRotation);

		int xUL = x + (int)vUL.GetX();
		int yUL = y - (int)vUL.GetY();
		int xUR = x + (int)vUR.GetX();
		int yUR = y - (int)vUR.GetY();
		int xLR = x + (int)vLR.GetX();
		int yLR = y - (int)vLR.GetY();
		int xLL = x + (int)vLL.GetX();
		int yLL = y - (int)vLL.GetY();

		//	Paint outline

		CGDraw::Line(Dest, xUL, yUL, xUR, yUR, 1, rgbColor);
		CGDraw::Line(Dest, xUR, yUR, xLR, yLR, 1, rgbColor);
		CGDraw::Line(Dest, xLR, yLR, xLL, yLL, 1, rgbColor);
		CGDraw::Line(Dest, xLL, yLL, xUL, yUL, 1, rgbColor);
		}
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
	m_fIsMultiHull = false;

	//	Keep a temporary map of IDs to section

	TSortMap<CString, int> ByID;

	//	Load all compartments

	m_Compartments.InsertEmpty(pDesc->GetContentElementCount());
	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pComp = pDesc->GetContentElement(i);
		SCompartmentDesc &Comp = m_Compartments[i];

		//	Init data common to all

		Comp.sID = pComp->GetAttribute(ID_ATTRIB);
		if (!Comp.sID.IsBlank())
			ByID.SetAt(Comp.sID, i);

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

			m_fIsMultiHull = true;
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

	//	Initialize the paint order based on which section attached to which. In general, 
	//	whatever we're attached to paints on top.
	//
	//	We generate a map indexed for each section and compute its paint order 
	//	recursively.

	if (m_fHasAttached)
		{
		TArray<int> PaintOrder;
		PaintOrder.InsertEmpty(m_Compartments.GetCount());
		for (i = 0; i < PaintOrder.GetCount(); i++)
			PaintOrder[i] = -1;

		for (i = 0; i < PaintOrder.GetCount(); i++)
			{
			if (PaintOrder[i] != -1)
				continue;

			PaintOrder[i] = CalcPaintOrder(i, ByID, PaintOrder);
			}

		//	Sort the paint order by highest to lowest

		TSortMap<int, int> SortedOrder(DescendingSort);
		for (i = 0; i < PaintOrder.GetCount(); i++)
			SortedOrder.Insert(PaintOrder[i], i);

		//	Now we've got a valid paint order.

		m_PaintOrder.InsertEmpty(SortedOrder.GetCount());
		for (i = 0; i < SortedOrder.GetCount(); i++)
			m_PaintOrder[i] = SortedOrder[i];
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
