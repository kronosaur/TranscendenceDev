//	RepairerClassProperties.cpp
//
//	CRepairerClass
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CDeviceItem> CRepairerClass::m_PropertyTable = std::array<TPropertyHandler<CDeviceItem>::SPropertyDef, 2> {{
		{
		"regen",						"armor regen",
		[](const CDeviceItem &RepairerItem, const CString &sProperty) 
			{
			const CRepairerClass &RepairerClass = RepairerItem.GetDeviceClassRepairer();
			const SRepairerDesc &Desc = RepairerClass.GetDesc(RepairerItem);
			if (Desc.ArmorRepair.GetCount() == 0)
				return ICCItemPtr::Nil();

			else if (Desc.ArmorRepair.GetCount() == 1)
				{
				if (!Desc.ArmorRepair[0].IsEmpty())
					return ICCItemPtr::DoubleOrInt(Desc.ArmorRepair[0].GetHPPer180(REPAIR_CYCLE_TIME));
				else
					return ICCItemPtr::Nil();
				}
			else
				{
				ICCItemPtr pResult(ICCItem::SymbolTable);
				for (int i = 0; i < Desc.ArmorRepair.GetCount(); i++)
					{
					if (!Desc.ArmorRepair[i].IsEmpty())
						{
						pResult->SetAt(strFromInt(i + 1), ICCItemPtr::DoubleOrInt(Desc.ArmorRepair[i].GetHPPer180(REPAIR_CYCLE_TIME)));
						}
					}

				return pResult;
				}
			},
		NULL,
		},

		{
		"compartmentRegen",				"compartment regen",
		[](const CDeviceItem &RepairerItem, const CString &sProperty) 
			{
			const CRepairerClass &RepairerClass = RepairerItem.GetDeviceClassRepairer();
			const SRepairerDesc &Desc = RepairerClass.GetDesc(RepairerItem);
			if (Desc.CompartmentRepair.IsEmpty())
				return ICCItemPtr::Nil();
			else
				return ICCItemPtr::DoubleOrInt(Desc.CompartmentRepair.GetHPPer180(REPAIR_CYCLE_TIME));
			},
		NULL,
		}
	}};

ICCItem *CRepairerClass::FindItemProperty (CItemCtx &Ctx, const CString &sProperty)

//	FindItemProperty
//
//	Returns a property

	{
	CCodeChain &CC = GetUniverse().GetCC();
	auto DeviceItem = Ctx.GetDeviceItem();

	//	Get the property

	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(DeviceItem, sProperty, pValue))
		return pValue->Reference();

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::FindItemProperty(Ctx, sProperty);
	}

