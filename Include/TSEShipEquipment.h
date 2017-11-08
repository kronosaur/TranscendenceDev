//	TSEShipSystems.h
//
//	Defines ship equipment
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Equipment (Abilities) ------------------------------------------------------
//
//	See AbilityTable in ConstantsUtilities.cpp

enum Abilities
	{
	ablUnknown =				-1,

	ablShortRangeScanner =		0,		//	Main viewscreen
	ablLongRangeScanner =		1,		//	LRS
	ablSystemMap =				2,		//	System map display
	ablAutopilot =				3,		//	Autopilot
	ablExtendedScanner =		4,		//	Extended marks on viewscreen
	ablTargetingSystem =		5,		//	Targeting computer
	ablGalacticMap =			6,		//	Galactic map display
	ablFriendlyFireLock =		7,
	ablTradingComputer =		8,		//	Trading computer

	ablCount =					9,
	};

enum AbilityModifications
	{
	ablModificationUnknown =	-1,

	ablInstall =				0,		//	Install the ability
	ablRemove =					1,		//	Remove the ability (if installed)
	ablDamage =					2,		//	Damage the ability (if installed)
	ablRepair =					3,		//	Repair the ability (if damaged)
	};

enum AbilityModificationOptions
	{
	ablOptionUnknown =			0x00000000,

	ablOptionNoMessage =		0x00000001,	//	Do not show a message to player
	};

enum AbilityStatus
	{
	ablStatusUnknown =			-1,

	ablUninstalled =			0,		//	>0 means that is installed (though it could be damaged)
	ablInstalled =				1,
	ablDamaged =				2,
	};

class CAbilitySet
	{
	public:
		inline void Clear (Abilities iAbility) { m_dwSet &= ~GetFlag(iAbility); }
		inline void ClearAll (void) { m_dwSet = 0; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return (m_dwSet == 0); }
		inline bool IsSet (Abilities iAbility) const { return ((m_dwSet & GetFlag(iAbility)) ? true : false); }
		inline void ReadFromStream (SLoadCtx &Ctx) { Ctx.pStream->Read(m_dwSet); }
		inline void Set (Abilities iAbility) { m_dwSet |= GetFlag(iAbility); }
		void Set (const CAbilitySet &Abilities);
		inline void WriteToStream (IWriteStream *pStream) const { pStream->Write(m_dwSet); }

	private:
		inline DWORD GetFlag (Abilities iAbility) const { return (1 << iAbility); }

		DWORD m_dwSet = 0;
	};

