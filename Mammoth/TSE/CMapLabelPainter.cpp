//	CMapLabelPainter.cpp
//
//	CMapLabelPainter class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CMapLabelPainter::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read non-cached data.

	{
	DWORD dwLoad;

	if (Ctx.dwVersion >= 162)
		{
		Ctx.pStream->Read(dwLoad);
		SetPos(LoadPosition(dwLoad));
		}
	else
		{
		int xMapLabel, yMapLabel;
		Ctx.pStream->Read(xMapLabel);
		Ctx.pStream->Read(yMapLabel);

		//	For backwards compatibility we reverse engineer label coordinates
		//	to a label position.

		if (xMapLabel < 0 && yMapLabel > 0)
			SetPos(posBottom);
		else if (xMapLabel < 0)
			SetPos(posLeft);
		else
			SetPos(posRight);
		}
	}

void CMapLabelPainter::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	Writes out non-cached data.

	{
	Stream.Write(SavePosition(GetPos()));
	}
