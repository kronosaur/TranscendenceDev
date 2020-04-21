//	TSESystemRandomEncounters.h
//
//	Defines classes and interfaces related to random encounters
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CRandomEncounterDesc
	{
	public:
		CRandomEncounterDesc (void) { }
		CRandomEncounterDesc (const IShipGenerator &Table, CSpaceObject &BaseObj);
		CRandomEncounterDesc (const IShipGenerator &Table, CDesignType &Type, CSovereign *pBaseSovereign = NULL);
		CRandomEncounterDesc (CSpaceObject &BaseObj);
		CRandomEncounterDesc (CDesignType &Type, CSovereign *pBaseSovereign = NULL);

		explicit operator bool() const { return (m_pTable || m_pType); }

		void Create (CSystem &System, CSpaceObject *pTarget, CSpaceObject *pGate = NULL) const;

	private:
		const IShipGenerator *m_pTable = NULL;
		CDesignType *m_pType = NULL;
		CSpaceObject *m_pBase = NULL;
		CSovereign *m_pBaseSovereign = NULL;
	};

class CRandomEncounterObjTable
	{
	public:
		CRandomEncounterDesc CalcEncounter (CSystem &System, CSpaceObject &PlayerObj) const;
		void Invalidate (void) { m_bValid = false; }

	private:
		static constexpr Metric MAX_ENCOUNTER_DIST	=			30.0 * LIGHT_MINUTE;

		struct SEntry
			{
			const IShipGenerator *pTable = NULL;
			CSpaceObject *pBase = NULL;
			int iFreq = 0;
			};

		TProbabilityTable<const SEntry *> CalcEncounterTable (CSystem &System, CSpaceObject &PlayerObj) const;
		void RealizeAllObjs (CSystem &System) const;

		mutable TArray<SEntry> m_AllObjs;
		mutable bool m_bValid = false;
	};

class CRandomEncounterTypeTable
	{
	public:
		CRandomEncounterDesc CalcEncounter (CSystem &System, CSpaceObject &PlayerObj) const;

	private:
		struct SEntry
			{
			const IShipGenerator *pTable = NULL;
			CDesignType *pType = NULL;
			CSovereign *pBaseSovereign = NULL;
			};

		void Realize (CSystem &System) const;

		mutable TProbabilityTable<SEntry> m_Table;
		mutable bool m_bValid = false;
	};
