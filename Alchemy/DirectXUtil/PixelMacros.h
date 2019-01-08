//	PixelMacros.h
//
//	Drawing implementation classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#define INTERPOLATE_COPY32(pos,lineInc,XYFloat,xInt,yInt,resultAlpha,result)	\
	{															\
	CG32bitPixel a = (*(pos));											\
	CG32bitPixel b = (*((pos) + (lineInc)));							\
	CG32bitPixel c = (*((pos) + 1));									\
	CG32bitPixel d = (*((pos) + (lineInc) + 1));						\
	Metric xf = (XYFloat).GetX() - (Metric)(xInt);				\
	Metric yf = (XYFloat).GetY() - (Metric)(yInt);				\
																\
	Metric ka = (1.0 - xf) * (1.0 - yf);						\
	Metric kb = (1.0 - xf) * yf;								\
	Metric kc = xf * (1.0 - yf);								\
	Metric kd = xf * yf;										\
																\
	int iBackCount = 0;											\
	if (a.GetAlpha() == 0x00)	{ iBackCount++; }	\
	if (b.GetAlpha() == 0x00)	{ iBackCount++; }	\
	if (c.GetAlpha() == 0x00)	{ iBackCount++; }	\
	if (d.GetAlpha() == 0x00)	{ iBackCount++; }	\
																\
	if (iBackCount == 4)										\
		(result) = CG32bitPixel::Null();					\
	else														\
		{														\
		DWORD red = (DWORD)(ka * a.GetRed()		\
				+ kb * b.GetRed()				\
				+ kc * c.GetRed()				\
				+ kd * d.GetRed());				\
																\
		DWORD green = (DWORD)(ka * a.GetGreen()	\
				+ kb * b.GetGreen()				\
				+ kc * c.GetGreen()				\
				+ kd * d.GetGreen());			\
																\
		DWORD blue = (DWORD)(ka * a.GetBlue()	\
				+ kb * b.GetBlue()				\
				+ kc * c.GetBlue()				\
				+ kd * d.GetBlue());				\
																\
		(result) = CG32bitPixel((BYTE)red, (BYTE)green, (BYTE)blue, (BYTE)resultAlpha);		\
		}														\
	}

