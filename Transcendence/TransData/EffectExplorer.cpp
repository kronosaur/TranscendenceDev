//	EffectExplorer.cpp
//
//	Generate a poster of effects.
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define EFFECT_ATTRIB						CONSTLIT("effect")
#define X_ATTRIB							CONSTLIT("x")
#define Y_ATTRIB							CONSTLIT("y")

#define EFFECT_ORB							CONSTLIT("Orb")

#define PROPERTY_RADIUS						CONSTLIT("radius")
#define PROPERTY_STYLE						CONSTLIT("style")

#define STYLE_TITLE							CONSTLIT("title")

static constexpr DWORD UNID_ORB_EFFECT =	0x00030100;

bool CalcPropertyRange (const CString &sEffect, const CString &sParam, TArray<CEffectParamDesc> &retRange);

void GenerateEffectExplorer (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	//	Figure out the effect class

	DWORD dwEffectUNID;
	CString sEffect = pCmdLine->GetAttribute(EFFECT_ATTRIB);
	if (sEffect.IsBlank())
		sEffect = EFFECT_ORB;

	if (strEquals(sEffect, EFFECT_ORB))
		dwEffectUNID = UNID_ORB_EFFECT;
	else
		{
		printf("ERROR: Unknown effect type: %s\n", (LPSTR)sEffect);
		return;
		}

	CEffectCreator *pCreator = Universe.FindEffectType(dwEffectUNID);
	if (pCreator == NULL)
		{
		printf("ERROR: Unknown effect type: %08x\n", dwEffectUNID);
		return;
		}

	//	Figure out the property to explore on the x-axis and the y-axis.

	CString sXParam = pCmdLine->GetAttribute(X_ATTRIB);
	if (sXParam.IsBlank())
		sXParam = PROPERTY_STYLE;

	TArray<CEffectParamDesc> XRange;
	if (!CalcPropertyRange(sEffect, sXParam, XRange))
		{
		printf("ERROR: Invalid param: %s\n", (LPSTR)sXParam);
		return;
		}

	CString sYParam = pCmdLine->GetAttribute(Y_ATTRIB);
	if (sYParam.IsBlank())
		sYParam = PROPERTY_RADIUS;

	TArray<CEffectParamDesc> YRange;
	if (!CalcPropertyRange(sEffect, sYParam, YRange))
		{
		printf("ERROR: Invalid param: %s\n", (LPSTR)sXParam);
		return;
		}

	//	Compute dimensions

	int cxCell = 200;
	int cyCell = 200;
	int cyLabels = 100;
	int cxImage = cxCell * XRange.GetCount();
	int cyImage = (cyCell + cyLabels) * YRange.GetCount();

	//	Initialize the output

	COutputChart Output;
	Output.SetContentSize(cxImage, cyImage);
	Output.SetOutputFilespec(pCmdLine->GetAttribute(CONSTLIT("output")));

	int iMargin = pCmdLine->GetAttributeIntegerBounded(CONSTLIT("margin"), 0, -1, 20);
	Output.SetMargin(iMargin, iMargin);

	//	Initialize fonts

	Output.SetStyleFont(STYLE_TITLE, pCmdLine->GetAttribute(CONSTLIT("font")));
	Output.SetStyleColor(STYLE_TITLE, CG32bitPixel(0xFF, 0xFF, 0xFF));

	//	Prepare the universe

	CSystem *pSystem;
	if (Universe.CreateEmptyStarSystem(&pSystem) != NOERROR)
		{
		printf("ERROR: Unable to create empty star system.\n");
		return;
		}

	//	Create the effect

	CSpaceObject *pEffect;
	if (pCreator->CreateEffect(pSystem,
			NULL,
			NullVector,
			NullVector,
			0,
			0,
			NULL,
			&pEffect))
		{
		printf("ERROR: Unable to create effect.\n");
		return;
		}

	//	Set the POV

	Universe.SetPOV(pEffect);
	pSystem->SetPOVLRS(pEffect);

	//	Prepare system

	Universe.UpdateExtended();
	Universe.GarbageCollectLibraryBitmaps();
	Universe.StartGame(true);

	SSystemUpdateCtx Ctx;
	Ctx.bForceEventFiring = true;
	Ctx.bForcePainted = true;
	Universe.Update(Ctx);

	//	Loop over all param combinations.

	CString sError;
	int xOrigin, yOrigin;
	CG32bitImage &Image = Output.GetOutputImage(&xOrigin, &yOrigin);

	int yPos = yOrigin;
	for (int y = 0; y < YRange.GetCount(); y++)
		{
		if (!pEffect->SetProperty(sYParam, YRange[y].AsItem(), &sError))
			{
			printf("ERROR: %s\n", (LPSTR)sError);
			return;
			}

		int xPos = xOrigin;
		for (int x = 0; x < XRange.GetCount(); x++)
			{
			if (!pEffect->SetProperty(sXParam, XRange[x].AsItem(), &sError))
				{
				printf("ERROR: %s\n", (LPSTR)sError);
				return;
				}

			//	Paint

			RECT rcView;
			rcView.left = xPos;
			rcView.top = yPos;
			rcView.right = xPos + cxCell;
			rcView.bottom = yPos + cyCell;

			g_pUniverse->PaintPOV(Image, rcView, CSystem::VWP_NO_STAR_FIELD);

			//	Paint labels

			Image.Fill(xPos, yPos + cyCell, cxCell, cyLabels, CG32bitPixel(40, 40, 40));

			//	Next

			xPos += cxCell;
			}

		yPos += cyCell + cyLabels;
		}

	//	Done

	Output.Output();
	}

bool CalcPropertyRange (const CString &sEffect, const CString &sParam, TArray<CEffectParamDesc> &retRange)
	{
	if (strEquals(sParam, PROPERTY_RADIUS))
		{
		retRange.InsertEmpty(8);
		retRange[0] = 10;
		retRange[1] = 15;
		retRange[2] = 23;
		retRange[3] = 34;
		retRange[4] = 51;
		retRange[5] = 76;
		retRange[6] = 114;
		retRange[7] = 171;
		}
	else if (strEquals(sEffect, EFFECT_ORB))
		{
		if (strEquals(sParam, PROPERTY_STYLE))
			{
			retRange.InsertEmpty(9);
			retRange[0] = CEffectParamDesc(CString("smooth"));
			retRange[1] = CEffectParamDesc(CString("flare"));
			retRange[2] = CEffectParamDesc(CString("cloud"));
			retRange[3] = CEffectParamDesc(CString("fireblast"));
			retRange[4] = CEffectParamDesc(CString("smoke"));
			retRange[5] = CEffectParamDesc(CString("diffraction"));
			retRange[6] = CEffectParamDesc(CString("firecloud"));
			retRange[7] = CEffectParamDesc(CString("blackhole"));
			retRange[8] = CEffectParamDesc(CString("lightning"));
			}
		else
			return false;
		}
	else
		return false;

	return true;
	}
