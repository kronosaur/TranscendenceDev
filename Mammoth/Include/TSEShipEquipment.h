//	TSEShipSystems.h
//
//	Defines ship equipment
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Conditions -----------------------------------------------------------------

enum class ECondition
	{
	none =					0x00000000,

	blind =					0x00000001,		//	SRS-blind
	paralyzed =				0x00000002,		//	EMP
	radioactive =			0x00000004,		//	Radioactive
	disarmed =				0x00000008,		//	Unable to fire weapons
	dragged =				0x00000010,		//	Subject to drag
	spinning =				0x00000020,		//	Spinning uncontrollably
	timeStopped =			0x00000040,		//	Time-stopped
	shipScreenDisabled =	0x00000080,		//	Source cannot bring up ship screen
	LRSBlind =				0x00000100,		//	LRS-blind
	shieldBlocked =			0x00000200,		//	Shields disabled

	count =					10,
	};

enum class EConditionChange
	{
	unknown =				-1,

	added =					0,
	removed =				1,
	};

class CConditionSet
	{
	public:
		void Clear (ECondition iCondition) { m_dwSet &= ~(DWORD)iCondition; }
		void ClearAll (void) { m_dwSet = 0; }
		bool Diff (const CConditionSet &OldSet, TArray<ECondition> &Added, TArray<ECondition> &Removed) const;
		bool IsEmpty (void) const { return (m_dwSet == 0); }
		bool IsSet (ECondition iCondition) const { return ((m_dwSet & (DWORD)iCondition) ? true : false); }
		void ReadFromStream (SLoadCtx &Ctx) { Ctx.pStream->Read(m_dwSet); }
		void Set (ECondition iCondition) { m_dwSet |= (DWORD)iCondition; }
		void Set (const CConditionSet &Conditions);
		ICCItemPtr WriteAsCCItem (void) const;
		void WriteToStream (IWriteStream *pStream) const { pStream->Write(m_dwSet); }

		static ECondition ParseCondition (const CString &sCondition);

	private:
		DWORD m_dwSet = 0;
	};

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
	ablFriendlyFireLock =		7,		//	Can't missile/auto lock on friendlies
	ablTradingComputer =		8,		//	Trading computer
	ablProtectWingmen =			9,		//	Do not hit wingmen/autons/etc.
	ablMiningComputer =			10,		//	Mining computer

	ablCount =					11,
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
		void Clear (Abilities iAbility) { m_dwSet &= ~GetFlag(iAbility); }
		void ClearAll (void) { m_dwSet = 0; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		bool IsEmpty (void) const { return (m_dwSet == 0); }
		bool IsSet (Abilities iAbility) const { return ((m_dwSet & GetFlag(iAbility)) ? true : false); }
		void ReadFromStream (SLoadCtx &Ctx) { Ctx.pStream->Read(m_dwSet); }
		void Set (Abilities iAbility) { m_dwSet |= GetFlag(iAbility); }
		void Set (const CAbilitySet &Abilities);
		void WriteToStream (IWriteStream *pStream) const { pStream->Write(m_dwSet); }

	private:
		DWORD GetFlag (Abilities iAbility) const { return (1 << iAbility); }

		DWORD m_dwSet = 0;
	};

