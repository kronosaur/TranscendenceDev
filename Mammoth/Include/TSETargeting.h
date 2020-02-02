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


		enum ETargetTypes
			{
			typeUnknown =				0x00000000,

			typeAttacker =				0x00000001,		//	Ship or station that can attack
			typeFortification =			0x00000002,		//	Target that cannot attack
			typeMissile =				0x00000004,		//	Missile
			typeTargetableMissile =		0x00000008,		//	Targetable missile (compatibility)
			typeMinable =				0x00000010,		//	Minable asteroid
			};

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

		void Delete (CSpaceObject &Obj);
		int GetCount (void) const { return m_Targets.GetCount(); }
		CSpaceObject *GetTarget (int iIndex) const { return m_Targets[iIndex].pObj; }
		Metric GetTargetDist2 (int iIndex) const { return m_Targets.GetKey(iIndex).rDist2; }
		ETargetTypes GetTargetType (int iIndex) const { return (ETargetTypes)m_Targets[iIndex].dwType; }
		void Init (CSpaceObject &SourceObj, const STargetOptions &Options);
		bool IsEmpty (void) const { return m_pSourceObj == NULL; }
		bool NoLineOfFireCheck (void) const { return m_Options.bNoLineOfFireCheck; }
		bool NoRangeCheck (void) const { return m_Options.bNoRangeCheck; }
		void ReadFromStream (SLoadCtx &Ctx);
		void Realize (void) const;
		void WriteToStream (CSystem &System, IWriteStream &Stream) const;

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
			DWORD dwType = 0;
			CSpaceObject *pObj = NULL;
			};

		void AddTarget (const STargetPriority &Priority, ETargetTypes iType, CSpaceObject &Obj) const;
		static EPriorityClass CalcPriority (ETargetTypes iType, CSpaceObject &Obj);
		void CleanUp (void) { m_pSourceObj = NULL; Invalidate(); }
		void Invalidate (void) const { m_Targets.DeleteAll(); m_bValid = false; }

		CSpaceObject *m_pSourceObj = NULL;
		STargetOptions m_Options;

		mutable TSortMap<STargetPriority, STargetDesc> m_Targets;
		mutable bool m_bValid = false;
	};
