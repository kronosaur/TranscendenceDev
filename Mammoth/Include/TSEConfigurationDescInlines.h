//	TSEConfigurationDescInlines.h
//
//	Defines classes and interfaces for weapons
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

template <class T>
T CConfigurationDesc::CalcShots (const CVector &vSource, int iFireAngle, int iPolarity, Metric rScale) const
	{
	switch (GetType())
		{
		case CConfigurationDesc::ctDual:
			{
			//	Compute a normal perpendicular to the direction of fire

			CVector Perp = PolarToVector(iFireAngle, (g_KlicksPerPixel * DUAL_SHOT_SEPARATION));
			Perp = Perp.Perpendicular() * rScale;

			//	Create two shots

			T Result;
			Result.InsertEmpty(2);
			Result[0].vPos = vSource + Perp;
			Result[0].iDir = iFireAngle;

			Result[1].vPos = vSource - Perp;
			Result[1].iDir = iFireAngle;

			return Result;
			}

		case CConfigurationDesc::ctDualAlternating:
			{
			//	Compute a normal perpendicular to the direction of fire

			CVector Perp = PolarToVector(iFireAngle, (g_KlicksPerPixel * DUAL_SHOT_SEPARATION));
			Perp = Perp.Perpendicular() * rScale;

			//	Polarity -1 means return all shots

			if (iPolarity == -1)
				{
				T Result;
				Result.InsertEmpty(2);
				Result[0].vPos = vSource + Perp;
				Result[0].iDir = iFireAngle;

				Result[1].vPos = vSource - Perp;
				Result[1].iDir = iFireAngle;

				return Result;
				}

			//	Otherwise, alternate based on polarity

			else
				{
				//	Create a shot either from the left or from the right depending
				//	on the continuous shot variable.

				T Result;
				Result.InsertEmpty(1);
				Result[0].vPos = vSource + ((iPolarity % 2) ? Perp : -Perp);
				Result[0].iDir = iFireAngle;

				return Result;
				}

			break;
			}

		case CConfigurationDesc::ctWall:
			{
			//	Compute a normal perpendicular to the direction of fire

			CVector Perp = PolarToVector(iFireAngle, (g_KlicksPerPixel * DUAL_SHOT_SEPARATION));
			Perp = Perp.Perpendicular() * rScale;

			//	Create five shots

			T Result;
			Result.InsertEmpty(5);
			for (int i = -2; i <= 2; i++)
				{
				Result[i + 2].vPos = vSource + ((Metric)i * Perp);
				Result[i + 2].iDir = iFireAngle;
				}

			return Result;
			}

		case CConfigurationDesc::ctSpread5:
			{
			T Result;
			Result.InsertEmpty(5);

			//	Shots at +2 and -2 degrees

			Result[0].vPos = vSource;
			Result[0].iDir = (iFireAngle + 2) % 360;

			Result[1].vPos = vSource;
			Result[1].iDir = (iFireAngle + 358) % 360;

			//	Shot at 0 degrees

			Result[2].vPos = vSource;
			Result[2].iDir = iFireAngle;

			//	Shots at +5 and -5 degrees

			Result[3].vPos = vSource;
			Result[3].iDir = (iFireAngle + 5) % 360;

			Result[4].vPos = vSource;
			Result[4].iDir = (iFireAngle + 355) % 360;

			return Result;
			}

		case CConfigurationDesc::ctSpread3:
			{
			T Result;
			Result.InsertEmpty(3);

			//	Shot at 0 degrees

			Result[0].vPos = vSource;
			Result[0].iDir = iFireAngle;

			//	Shots at +5 and -5 degrees

			Result[1].vPos = vSource;
			Result[1].iDir = (iFireAngle + 5) % 360;

			Result[2].vPos = vSource;
			Result[2].iDir = (iFireAngle + 355) % 360;

			return Result;
			}

		case CConfigurationDesc::ctSpread2:
			{
			T Result;
			Result.InsertEmpty(2);

			//	Shots at +2 and -2 degrees

			Result[0].vPos = vSource;
			Result[0].iDir = (iFireAngle + 2) % 360;

			Result[1].vPos = vSource;
			Result[1].iDir = (iFireAngle + 358) % 360;

			return Result;
			}

		case CConfigurationDesc::ctCustom:
			{
			if (GetCustomConfigCount() == 0)
				return CShotArray();

			else if (IsAlternating() && iPolarity != -1)
				{
				//	Figure out which shot we're firing

				int iShot = (iPolarity % GetCustomConfigCount());

				T Result;
				Result.InsertEmpty(1);
				Result[0].vPos = vSource + GetCustomConfigPos(iShot, iFireAngle) * rScale;
				Result[0].iDir = GetCustomConfigFireAngle(iShot, iFireAngle);

				return Result;
				}
			else
				{
				T Result;
				Result.InsertEmpty(GetCustomConfigCount());

				for (int i = 0; i < Result.GetCount(); i++)
					{
					Result[i].vPos = vSource + GetCustomConfigPos(i, iFireAngle) * rScale;
					Result[i].iDir = GetCustomConfigFireAngle(i, iFireAngle);
					}

				return Result;
				}

			break;
			}

		default:
			{
			T Result;
			Result.InsertEmpty(1);
			Result[0].vPos = vSource;
			Result[0].iDir = iFireAngle;

			return Result;
			}
		}
	}
