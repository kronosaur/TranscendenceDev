//	ExportData.cpp
//
//	Exports Data suitable for GridWhale and other products
//	Copyright (c) 2023 Kronosaur Productions, LLC. All Rights Reserved.
//
//	We generate an output directory with the following files:
//
//	/Data
//		Currencies.csv
//		ItemTypes.csv
//		ShipClasses.csv
//		Sovereigns.csv
//		StationTypes.csv
//
//		AmmoTypes.csv
//		ArmorTypes.csv
//		CargoHoldTypes.csv
//		DriveTypes.csv
//		MiscDeviceTypes.csv
//		ReactorTypes.csv
//		ShieldTypes.csv
//		WeaponTypes.csv
//
//	/Images
//
//	ItemTypes.csv
//
//		Installation		String				"core" or some extension group
//		UNID				Integer				Unique ID in Installation
//		Type				String				"weapon", "shield", etc.
//		Name				String				Name pattern (e.g., "laser cannon(s)")
//		Level				Integer				Level of item
//		MaxLevel			Integer				Max level of item (for scalable items)
//		Value				String				Price in some currency (e.g., "1000 credit")
//		MassKg				Integer				Mass in kg
//		Frequency			String				"common", "uncommon", etc.
//		NoAppearing			String				A dice range (e.g., "2d4")
//		Image				String				"Image.png|100,100|128x128"
//		Tags				String				Comma-separated list of tags
//		Description			String				Short description
//
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
//		FuelConsumption		Float				Fuel units consumed per tick per 1/10th MW.
//		CargoSpaceT			Integer				Cargo space in CBM (cubic meters)
// 
//		HullMassT			Integer				Hull mass in metric tons
//		HullValue			String				Price in some currency (e.g., "1000 credit")
//		HullCargoSpaceT		Integer				Cargo space in CBM
//		MaxCargoSpaceT		Integer				Max cargo space in CBM
//		StdArmorMassT		Integer				Standard armor mass in CBM (no penalty)
//		MaxArmorMassT		Integer				Max armor mass in CBM
//		MaxArmorSpeedAdj	Integer				Speed penalty (1/100th of c) at max armor
//		MinArmorSpeedAdj	Integer				Speed bonus (1/100th of c) at 1/2 std armor
//		MaxDevices			Integer				Max number of devices
//		MaxWeapons			Integer				Max number of weapons
//		MaxNonWeapons		Integer				Max number of non-weapons
//		MaxReactorPower		Integer				Max reactor power (in 1/10th MW)
//		TotalInteriorHP		Integer				Total interior HP
//
//		ArmorSegments		String				Comma-separated list of armor segment angles
//		ArmorUNIDs			String				Comma-separated list of armor UNIDs
//		DeviceSlots			String				Comma-separated list of device slot descriptors
//		DeviceUNIDs			String				Comma-separated list of device UNIDs
//		AmmoUNIDs			String				Comma-separated list of ammo UNIDs and counts

#include "PreComp.h"

void ExportData (CUniverse& Universe, const CXMLElement& CmdLine)
	{
	printf("Exporting data...\n");
	}
