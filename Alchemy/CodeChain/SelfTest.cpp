//	SelfTest.cpp
//
//	Implementation of CCodeChain object
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

ICCItemPtr CCodeChain::SystemSelfTest (void)

//	SystemSelfTest
//
//	Conducts unit test of internals down to Kernel. Returns True on success or
//	an error on failure.

	{
	//	Sort unit test

	TArray<int> Test001;
	Test001.Insert(17);
	Test001.Insert(-10);
	Test001.Insert(4);
	Test001.Insert(0);
	Test001.Sort();
	if (Test001[0] != -10 || Test001[1] != 0 || Test001[2] != 4 || Test001[3] != 17)
		return ICCItemPtr(CreateError(CONSTLIT("TArray::Sort failed.")));

	Test001.Sort([](const int &Left, const int &Right){
		if (Left > Right)
			return -1;
		else if (Left < Right)
			return 1;
		else
			return 0;
		});

	if (Test001[3] != -10 || Test001[2] != 0 || Test001[1] != 4 || Test001[0] != 17)
		return ICCItemPtr(CreateError(CONSTLIT("TArray::Sort failed.")));

	//	Success

	return ICCItemPtr(ICCItem::True);
	}
