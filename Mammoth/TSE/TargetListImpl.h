//	TargetListImpl.h
//
//	Transcendence design classes
//	Copyright 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CTargetListSelector
	{
	public:
		CTargetListSelector (CSpaceObject &Source, const CTargetList::STargetOptions &Options) :
				m_Source(Source),
				m_Options(Options),
				m_Perception(Source.GetPerception()),
				m_iAggressorThreshold(Source.GetUniverse().GetTicks() - CSpaceObject::AGGRESSOR_THRESHOLD)
			{ }

		bool CanUseSovereignEnemyList (void) const { return (!m_Options.bIncludeMinable && !m_Options.bIncludeMissiles && !m_Options.bIncludeTargetableMissiles); }
		CSpaceObject *GetIncludePlayer (void) const { return (m_Options.bIncludePlayer ? m_Source.GetPlayerShip() : NULL); }
		Metric GetMaxRange (void) const	{ return m_Perception.GetRange(0); }

		bool Matches (CSpaceObject &Obj, Metric rDist2, CTargetList::ETargetTypes *retiType = NULL) const
			{
			if (Obj.GetCategory() == CSpaceObject::catMissile)
				{
				if (m_Perception.CanBeTargeted(&Obj, rDist2)
						&& !Obj.IsUnreal()
						&& m_Source.IsAngryAt(&Obj)
						&& Obj != m_Options.pExcludeObj)
					{
					CTargetList::ETargetTypes missileType = Obj.IsTargetableProjectile() ? CTargetList::typeTargetableMissile : CTargetList::typeMissile;
					if (retiType) *retiType = missileType;
					return true;
					}
				}
			else if (m_Options.bIncludeMinable && Obj.CanBeMined())
				{
				if ((!Obj.IsExplored() || Obj.HasMinableItem())
						&& !Obj.IsUnreal()
						&& Obj != m_Options.pExcludeObj)
					{
					if (retiType) *retiType = CTargetList::typeMinable;
					return true;
					}
				}
			else if (Obj.CanAttack())
				{
				if (m_Perception.CanBeTargeted(&Obj, rDist2)
						&& !Obj.IsUnreal()
						&& (m_Options.bIncludeNonAggressors || Obj.GetLastFireTime() > m_iAggressorThreshold)
						&& m_Source.IsAngryAt(&Obj)
						&& Obj != m_Source
						&& Obj != m_Options.pExcludeObj
						&& !Obj.IsEscortingFriendOf(&m_Source))
					{
					if (retiType) *retiType = CTargetList::typeAttacker;
					return true;
					}
				}
			else if (Obj.CanBeAttacked())
				{
				if (m_Perception.CanBeTargeted(&Obj, rDist2)
						&& !Obj.IsUnreal()
						&& m_Source.IsAngryAt(&Obj)
						&& Obj != m_Source
						&& Obj != m_Options.pExcludeObj
						&& !Obj.IsEscortingFriendOf(&m_Source))
					{
					if (retiType) *retiType = CTargetList::typeFortification;
					return true;
					}
				}

			return false;
			}

		bool MatchesCanBeTargeted (CSpaceObject &Obj, Metric rDist2, CTargetList::ETargetTypes *retiType = NULL) const
			{
			if (Obj.GetCategory() == CSpaceObject::catMissile)
				{
				if (m_Perception.CanBeTargeted(&Obj, rDist2)
						&& !Obj.IsUnreal())
					{
					if (retiType) *retiType = CTargetList::typeMissile;
					return true;
					}
				}
			else if (m_Options.bIncludeMinable && Obj.CanBeMined())
				{
				if (!Obj.IsUnreal())
					{
					if (retiType) *retiType = CTargetList::typeMinable;
					return true;
					}
				}
			else if (Obj.CanAttack())
				{
				if (m_Perception.CanBeTargeted(&Obj, rDist2)
						&& !Obj.IsUnreal()
						&& Obj != m_Source)
					{
					if (retiType) *retiType = CTargetList::typeAttacker;
					return true;
					}
				}
			else
				{
				if (m_Perception.CanBeTargeted(&Obj, rDist2)
						&& !Obj.IsUnreal()
						&& Obj != m_Source)
					{
					if (retiType) *retiType = CTargetList::typeFortification;
					return true;
					}
				}

			return false;
			}

		bool MatchesCategory (CSpaceObject &Obj) const
			{
			return (Obj.GetCategory() == CSpaceObject::catShip
						|| (m_Options.bIncludeStations && Obj.GetCategory() == CSpaceObject::catStation)
						|| (m_Options.bIncludeMinable && Obj.GetCategory() == CSpaceObject::catStation && Obj.CanBeMined())
						|| (m_Options.bIncludeMissiles && Obj.GetCategory() == CSpaceObject::catMissile)
						|| (m_Options.bIncludeTargetableMissiles && (Obj.GetCategory() == CSpaceObject::catMissile && Obj.IsTargetableProjectile())));
			}

	private:
		CSpaceObject &m_Source;
		const CTargetList::STargetOptions &m_Options;

		CPerceptionCalc m_Perception;
		int m_iAggressorThreshold = -1;
	};
