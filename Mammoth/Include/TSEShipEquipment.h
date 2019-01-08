//	TSEShipSystems.h
//
//	Defines ship equipment
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Conditions -----------------------------------------------------------------

class CConditionSet
	{
	public:
		enum ETypes
			{
			cndNone =				0x00000000,

			cndBlind =				0x00000001,		//	SRS-blind
			cndParalyzed =			0x00000002,		//	EMP
			cndRadioactive =		0x00000004,		//	Radioactive
			cndDisarmed =			0x00000008,		//	Unable to fire weapons
			cndDragged =			0x00000010,		//	Subject to drag
			cndSpinning =			0x00000020,		//	Spinning uncontrollably
			cndTimeStopped =		0x00000040,		//	Time-stopped
			cndShipScreenDisabled =	0x00000080,		//	Source cannot bring up ship screen
			cndLRSBlind =			0x00000100,		//	LRS-blind

			cndCount =				9,
			};

		enum EModifications
			{
			cndModificationUnknown =	-1,

			cndAdded =					0,
			cndRemoved =				1,
			};

		inline void Clear (ETypes iCondition) { m_dwSet &= ~iCondition; }
		inline void ClearAll (void) { m_dwSet = 0; }
		bool Diff (const CConditionSet &OldSet, TArray<ETypes> &Added, TArray<ETypes> &Removed) const;
		inline bool IsEmpty (void) const { return (m_dwSet == 0); }
		inline bool IsSet (ETypes iCondition) const { return ((m_dwSet & iCondition) ? true : false); }
		inline void ReadFromStream (SLoadCtx &Ctx) { Ctx.pStream->Read(m_dwSet); }
		inline void Set (ETypes iCondition) { m_dwSet |= iCondition; }
		void Set (const CConditionSet &Conditions);
		ICCItemPtr WriteAsCCItem (void) const;
		inline void WriteToStream (IWriteStream *pStream) const { pStream->Write(m_dwSet); }

		static ETypes ParseCondition (const CString &sCondition);

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

