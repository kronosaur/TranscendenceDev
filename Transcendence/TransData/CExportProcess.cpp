//	CExportProcess.cpp
//
//	CExportProcess class
//	Copyright (c) 2023 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ATTRIB_INSTALLATION						CONSTLIT("installation")
#define ATTRIB_OUTPUT							CONSTLIT("output")

#define FILENAME_SHIP_CLASSES_CSV				CONSTLIT("ShipClasses.csv")

#define INSTALLATION_CORE						CONSTLIT("core")

#define PROPERTY_BALANCE_TYPE					CONSTLIT("balance.type")
#define PROPERTY_FUEL_EFFICIENCY				CONSTLIT("fuelEfficiency")
#define PROPERTY_MANEUVER						CONSTLIT("maneuver")
#define PROPERTY_MASS							CONSTLIT("mass")
#define PROPERTY_MAX_SPEED						CONSTLIT("maxSpeed")
#define PROPERTY_POWER							CONSTLIT("power")
#define PROPERTY_THRUST							CONSTLIT("thrust")

bool CExportProcess::ParseOptions (const CXMLElement& CmdLine, SOptions& retOptions)

//	ParseOptions
//
//	Parses options; returns FALSE if there is an error.

	{
	retOptions = SOptions();
	retOptions.sInstallation = CmdLine.GetAttribute(ATTRIB_INSTALLATION);
	if (retOptions.sInstallation.IsBlank())
		retOptions.sInstallation = INSTALLATION_CORE;

	retOptions.sOutputFolder = CmdLine.GetAttribute(ATTRIB_OUTPUT);
	if (retOptions.sOutputFolder.IsBlank())
		retOptions.sOutputFolder = CONSTLIT("Export");

	return true;
	}

ALERROR CExportProcess::Run ()

//	Run
//
//	Runs the process.

	{
	CFileWriteStream ShipClassesFile(ComposeOutputFilespec(FILENAME_SHIP_CLASSES_CSV));
	if (ShipClassesFile.Create() != NOERROR)
		{
		printf("ERROR: Unable to create output file: %s\n", FILENAME_SHIP_CLASSES_CSV.GetASCIIZPointer());
		return ERR_FAIL;
		}

	WriteShipClasses(ShipClassesFile);

	return NOERROR;
	}

void CExportProcess::WriteShipClasses (IWriteStream& Output)

//	ShipClasses.csv
//
//		Installation		String				"core" or some extension group
//		UNID				Integer				Unique ID in Installation
//		Manufacturer		String				E.g., "Pacific Defense Systems"
//		ClassName			String				E.g., "Centurion"
//		TypeName			String				E.g., "gunship"
//		Image				String				"Image.png|128x128|12x10x1|0,0"
//		HeroImage			String				"Image.png|320x320|1x1x1|0,0"
//		Tags				String				Comma-separated list of tags
//		Sovereign			Integer				Default sovereign UNID
//		Level				Integer				Level of ship
// 
//		LevelType			String				"minion", "standard", "boss", etc.
//		CombatStrength		Float				Relative strength
//		Score				Integer				Score of ship
//		Length				Integer				Length in meters
//		MassT				Integer				Total mass in metric tons
//		Maneuverability		Float				Maneuverability (degrees per tick)
//		Thrust				Integer				Thrust in tons
//		MaxSpeed			Integer				Max speed in % of c
//		ReactorPower		Integer				Reactor power in 1/10th MW
//		MaxFuel				Float				Max fuel units
// 
//		FuelConsumption		Float				Fuel units consumed per tick per 1/10th MW.
//		CargoSpaceT			Integer				Cargo space in CBM (cubic meters)
//		HullMassT			Integer				Hull mass in metric tons
//		HullValue			String				Price in some currency (e.g., "1000 credit")
//		HullCargoSpaceT		Integer				Cargo space in CBM
//		MaxCargoSpaceT		Integer				Max cargo space in CBM
//		StdArmorMassT		Integer				Standard armor size in CBM (no penalty)
//		MaxArmorMassT		Integer				Max armor size in CBM
//		MaxArmorSpeedAdj	Integer				Speed penalty (1/100th of c) at max armor
//		MinArmorSpeedAdj	Integer				Speed bonus (1/100th of c) at 1/2 std armor
// 
//		MaxDevices			Integer				Max number of devices
//		MaxWeapons			Integer				Max number of weapons
//		MaxNonWeapons		Integer				Max number of non-weapons
//		MaxReactorPower		Integer				Max reactor power (in 1/10th MW)
//		TotalInteriorHP		Integer				Total interior HP
//		ArmorSegments		String				Comma-separated list of armor segment angles
//		ArmorUNIDs			String				Comma-separated list of armor UNIDs
//		DeviceSlots			String				Comma-separated list of device slot descriptors
//		DeviceUNIDs			String				Comma-separated list of device UNIDs
//		AmmoUNIDs			String				Comma-separated list of ammo UNIDs and counts

	{
	//	Write the header first.

	CString sHeaders("Installation,UNID,Manufacturer,ClassName,TypeName,Image,HeroImage,Tags,Sovereign,Level,"
			"LevelType,CombatStrength,Score,Length,MassT,Maneuverability,Thrust,MaxSpeed,ReactorPower,MaxFuel,"
			"FuelConsumption,CargoSpaceT,HullMassT,HullValue,HullCargoSpaceT,MaxCargoSpaceT,StdArmorMassT,MaxArmorMassT,MaxArmorSpeedAdj,MinArmorSpeedAdj,"
			"MaxDevices,MaxWeapons,MaxNonWeapons,MaxReactorPower,TotalInteriorHP,ArmorSegments,ArmorUNIDs,DeviceSlots,DeviceUNIDs,AmmoUNIDs\r\n");

	Output.Write(sHeaders.GetASCIIZPointer(), sHeaders.GetLength());

	//	Now write all the record.

	for (int i = 0; i < m_Universe.GetShipClassCount(); i++)
		{
		const CShipClass& Class = *m_Universe.GetShipClass(i);

		//	Only include generic classes 

		if (!Class.HasLiteralAttribute(CONSTLIT("genericClass")))
			continue;

		//	Generate the record

		CString sRecord = strPatternSubst(CONSTLIT("%s,0x%08x,%s,%s,%s,%s,%s,%s,0x%08x,%d,"
				"%s,%s,%d,%d,%d,%d,%d,%d,%d,%S,"
				"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
				"%d,%d,%d,%d,%d,%d,%s,%s,%s,%s,%s\r\n"),
				m_Options.sInstallation,
				Class.GetUNID(),
				Class.GetManufacturerName(),
				Class.GetNounPhrase(nounGeneric),
				Class.GetNounPhrase(nounShort | nounNoModifiers),
				Class.GetImage().GetFilename(),					//	LATER
				Class.GetHeroImage().GetFilename(),				//	LATER
				CString(),										//	LATER
				Class.GetDefaultSovereign()->GetUNID(),
				Class.GetLevel(),

				Class.GetPropertyString(PROPERTY_BALANCE_TYPE),
				strFromDouble((double)Class.GetCombatStrength()),
				Class.GetScore(),
				Class.GetHullDesc().GetSize(),
				Class.GetPropertyInteger(PROPERTY_MASS),
				Class.GetPropertyInteger(PROPERTY_MANEUVER),
				Class.GetPropertyInteger(PROPERTY_THRUST),
				Class.GetPropertyInteger(PROPERTY_MAX_SPEED),
				Class.GetPropertyInteger(PROPERTY_POWER),
				strFromDouble((double)Class.GetReactorDesc().GetFuelCapacity()),

				Class.GetPropertyInteger(PROPERTY_FUEL_EFFICIENCY),
				Class.GetCargoDesc().GetCargoSpace(),
				Class.GetHullDesc().GetMass(),
				Class.GetHullValue(),
				Class.GetHullDesc().GetCargoSpace(),
				Class.GetHullDesc().GetMaxCargoSpace(),
				Class.GetHullDesc().GetArmorLimits().GetStdArmorSize(),
				Class.GetHullDesc().GetArmorLimits().GetMaxArmorSize(),
				Class.GetHullDesc().GetArmorLimits().GetMaxArmorSpeedPenalty(),
				Class.GetHullDesc().GetArmorLimits().GetMinArmorSpeedBonus(),

				Class.GetHullDesc().GetMaxDevices(),
				Class.GetHullDesc().GetMaxWeapons(),
				Class.GetHullDesc().GetMaxNonWeapons(),
				Class.GetHullDesc().GetMaxReactorPower(),
				Class.GetInteriorDesc().GetHitPoints(),
				strFromInt(Class.GetArmorDesc().GetCount()),	//	LATER
				CString(),										//	LATER
				CString(),										//	LATER
				CString(),										//	LATER
				CString());										//	LATER

		Output.Write(sRecord.GetASCIIZPointer(), sRecord.GetLength());
		}
	}

CString CExportProcess::ComposeOutputFilespec (const CString& sFilename) const
	{
	return pathAddComponent(pathAddComponent(m_Options.sOutputFolder, m_Options.sInstallation), sFilename);
	}
