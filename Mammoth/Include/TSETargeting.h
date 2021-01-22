//	TSETargeting.h
//
//	Transcendence design classes
//	Copyright 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CTargetList
	{
	public:
		static constexpr DWORD FLAG_INCLUDE_STATIONS =			0x00000001;
		static constexpr DWORD FLAG_INCLUDE_NON_AGGRESSORS =	0x00000002;

		enum class ETargetType
			{
			Unknown =					0x00000000,

			AttackerCompatible =		0x00000001,		//	Ship or station that can attack
			Fortification =				0x00000002,		//	Target that cannot attack
			Missile =					0x00000004,		//	Missile
			TargetableMissile =			0x00000008,		//	Targetable missile (compatibility)
			Minable =					0x00000010,		//	Minable asteroid
			AggressiveShip =			0x00000020,		//	A ship that has fired recently
			NonAggressiveShip =			0x00000040,		//	A ship that has not fired recently
			Station =					0x00000080,		//	A station that can attack
			};

		static constexpr DWORD SELECT_ALL =					0xffff;
		static constexpr DWORD SELECT_ANY_MISSILE =			(DWORD)ETargetType::Missile | (DWORD)ETargetType::TargetableMissile;
		static constexpr DWORD SELECT_ATTACKERS =			(DWORD)ETargetType::AggressiveShip | (DWORD)ETargetType::NonAggressiveShip | (DWORD)ETargetType::Station | (DWORD)ETargetType::AttackerCompatible;
		static constexpr DWORD SELECT_FORTIFICATION =		(DWORD)ETargetType::Fortification;
		static constexpr DWORD SELECT_MINABLE =				(DWORD)ETargetType::Minable;
		static constexpr DWORD SELECT_MISSILE =				(DWORD)ETargetType::Missile;
		static constexpr DWORD SELECT_TARGETABLE_MISSILE =	(DWORD)ETargetType::TargetableMissile;

		struct STargetOptions
			{
			int iMaxTargets = 10;
			Metric rMaxDist = 30.0 * LIGHT_SECOND;
			CSpaceObject *pExcludeObj = NULL;
			CSpaceObject *pPerceptionObj = NULL;		//	If NULL, same as SourceObj

			bool bIncludeMinable = false;
			bool bIncludeMissiles = false;
			bool bIncludeNonAggressors = false;
			bool bIncludePlayer = false;
			bool bIncludeSourceTarget = false;
			bool bIncludeStations = false;
			bool bIncludeTargetableMissiles = false;

			bool bNoLineOfFireCheck = false;
			bool bNoRangeCheck = false;
			};

		struct STargetResult
			{
			CSpaceObject *pObj = NULL;
			int iFireAngle = -1;
			Metric rDist2 = 0.0;
			};

		CTargetList (void)
			{ }

		CTargetList (CSpaceObject &SourceObj, const STargetOptions &Options)
			{ Init(SourceObj, Options);	}

		static ETargetType CalcType (const CSpaceObject &Obj);
		void Delete (CSpaceObject &Obj);
		CSpaceObject *FindBestTarget (DWORD dwTargetTypes = SELECT_ALL, Metric rMaxRange = -1.0) const;
		int GetCount (void) const { return m_Targets.GetCount(); }
		CSpaceObject *GetTarget (int iIndex) const { return m_Targets[iIndex].pObj; }
		Metric GetTargetDist2 (int iIndex) const { return m_Targets.GetKey(iIndex).rDist2; }
		ETargetType GetTargetType (int iIndex) const { return m_Targets[iIndex].iType; }
		void Init (CSpaceObject &SourceObj, const STargetOptions &Options);
		bool IsEmpty (void) const { return m_pSourceObj == NULL; }
		bool NoLineOfFireCheck (void) const { return m_Options.bNoLineOfFireCheck; }
		bool NoRangeCheck (void) const { return m_Options.bNoRangeCheck; }
		void ReadFromStream (SLoadCtx &Ctx);
		void Realize (void) const;
		void WriteToStream (IWriteStream &Stream) const;

	private:
		static constexpr DWORD SPECIAL_INVALID_COUNT = 0xffffffff;

		enum EPriorityClass
			{
			priorityUnknown =			-1,

			priorityCritical =			0,	//	Highest priority
			priorityPrimary =			1,	//	Player/AI current target
			prioritySecondary =			2,	//	Target of opportunity
			priorityTertiary =			3,	//	Target of opportunity that cannot attack
											//	(including minable asteroids)
			priorityLowest =			4,	//	Lowest priority
			};

		struct STargetPriority
			{
			STargetPriority (void)
				{ }

			STargetPriority (EPriorityClass iTypeArg, Metric rDist2Arg) :
					iType(iTypeArg),
					rDist2(rDist2Arg)
				{ }

			bool operator < (const STargetPriority &Value) const
				{
				if (iType < Value.iType)
					return true;
				else if (iType == Value.iType && rDist2 < Value.rDist2)
					return true;
				else
					return false;
				}

			bool operator > (const STargetPriority &Value) const
				{
				if (iType > Value.iType)
					return true;
				else if (iType == Value.iType && rDist2 > Value.rDist2)
					return true;
				else
					return false;
				}

			EPriorityClass iType = priorityUnknown;
			Metric rDist2 = 0.0;
			};

		struct STargetDesc
			{
			ETargetType iType = ETargetType::Unknown;
			CSpaceObject *pObj = NULL;
			};

		void AddTarget (const STargetPriority &Priority, ETargetType iType, CSpaceObject &Obj) const;
		static EPriorityClass CalcPriority (ETargetType iType, CSpaceObject &Obj);
		void CleanUp (void) { m_pSourceObj = NULL; Invalidate(); }
		void Invalidate (void) const { m_Targets.DeleteAll(); m_bValid = false; }

		CSpaceObject *m_pSourceObj = NULL;
		STargetOptions m_Options;

		mutable TSortMap<STargetPriority, STargetDesc> m_Targets;
		mutable bool m_bValid = false;
	};
