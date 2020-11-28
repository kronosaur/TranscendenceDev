//	TSESpaceObjectDefs.h
//
//	CSpaceObject definitions
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum class EObjectPart
	{
	none,

	equipment,										//	A piece of equipment (e.g., LRS)
	interior,										//	The interior of the object (e.g., cargo hold)
	item,											//	An item on the object
	};

struct SObjectPartDesc
	{
	SObjectPartDesc () {}
	SObjectPartDesc (const CItem &ItemArg) :
			iPart(EObjectPart::item),
			Item(ItemArg)
		{ }

	SObjectPartDesc (EObjectPart iPart) :
			iPart(iPart)
		{
		switch (iPart)
			{
			case EObjectPart::item:
			case EObjectPart::equipment:
				throw CException(ERR_FAIL);
				break;
				
			default:
				break;
			}
		}

	EObjectPart iPart = EObjectPart::none;

	CItem Item;										//	If iPart == item
	Abilities iEquipment = ablUnknown;				//	If iPart == equipment
	};
