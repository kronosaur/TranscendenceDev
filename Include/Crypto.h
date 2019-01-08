//	Crypto.h
//
//	Basic cryptography classes
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include "Euclid.h"

class CDigest : public CIntegerIP
	{
	public:
		CDigest (void) { }
		CDigest (BYTE *pBytes);
		CDigest (IReadBlock &Data);

	private:
		enum Constants
			{
			digestLength =			20,
			digestLengthDWORD =		5,
			};
	};

void cryptoCreateDigest (IReadBlock &Data, CIntegerIP *retDigest);
void cryptoCreateMAC (IReadBlock &Data, const CIntegerIP &Key, CIntegerIP *retMAC);
void cryptoRandom (int iCount, CIntegerIP *retx);
ALERROR fileCreateDigest (const CString &sFilespec, CIntegerIP *retDigest);

