//	StationImpl.h
//
//	Station Implementation Helpers.
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CChallengeRatingCalculator
	{
	public:
		static constexpr Metric BALANCE_STD_DEFENDERS_PER_STATION = 3.5;
		static constexpr Metric BALANCE_STD_HITS_TO_DESTROY = 25.0;
		static constexpr Metric MAX_HITS_ADJ = 2.0;

		static constexpr Metric MULTI_WEAPON_BONUS = 1.25;
		static constexpr Metric MULTI_WEAPON_BONUS_THRESHOLD = 0.8;

		//	Decrease expected weapon damage if we have multi-hull
		static constexpr Metric MULTI_HULL_WEAPON_DAMAGE_ADJ = 0.34;

		static constexpr Metric STD_FIRE_RATE_ADJ = 40.0;

		static Metric CalcBalance (const CStation &StationObj);
		static int CalcChallengeRating (const CShip &ShipObj);
		static int CalcChallengeRating (const CStation &StationObj);

	private:
		static constexpr Metric CHALLENGE_RATING_K1 = 1.25;
		static constexpr Metric TREASURE_BALANCE_POWER = 0.7;

		static Metric CalcBalanceHitsAdj (const CStation &StationObj, int iLevel);
		static Metric CalcDefenderStrength (const CStation &StationObj, int iLevel);
		static Metric CalcHitsToDestroy (const CStation &StationObj, int iLevel);
		static Metric CalcHitsToDestroySingle (const CStation &StationObj, int iLevel);
		static Metric CalcLevelStrength (const CShip &ShipObj, int iLevel);
		static Metric CalcLevelStrength (const CStation &StationObj, int iLevel);
		static Metric CalcWeaponStrength (const CStation &StationObj, int iLevel);

	};
