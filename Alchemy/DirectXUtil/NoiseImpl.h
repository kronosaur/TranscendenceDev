//	NoiseImpl.h
//
//	Some internal definitions common across image files.
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

//	Noise ----------------------------------------------------------------------

#pragma once

const int PERM_TABLE_SIZE =					256;	//	Must be power of 2
const int PERM_TABLE_MASK =					(PERM_TABLE_SIZE - 1);

extern BYTE g_perm[PERM_TABLE_SIZE];
extern Metric g_gradient[PERM_TABLE_SIZE * 3];

inline Metric RANDF (void) { return (Metric)::rand() / (Metric)RAND_MAX; }
inline Metric LERP (Metric t, Metric x0, Metric x1) { return (x0 + t * (x1 - x0)); }
inline Metric SMOOTHSTEP (Metric x) { return x * x * (3.0f - 2.0f * x); }

inline BYTE PERM (DWORD x) { return g_perm[x & PERM_TABLE_MASK]; }
inline BYTE INDEX2D (int x, int y) { return PERM(x + PERM(y)); }
inline BYTE INDEX3D (int x, int y, int z) { return PERM(x + PERM(y + PERM(z))); }
inline Metric RANDVECTOR2D (int x, int y, Metric fx, Metric fy)
	{
	Metric *pVec = &g_gradient[INDEX2D(x, y) * 3];
	return pVec[0] * fx + pVec[1] * fy;
	}
inline Metric RANDVECTOR3D (int x, int y, int z, Metric fx, Metric fy, Metric fz)
	{
	Metric *pVec = &g_gradient[INDEX3D(x, y, z) * 3];
	return pVec[0] * fx + pVec[1] * fy + pVec[2] * fz;
	}

inline Metric PERIODICRANDVECTOR3D (int iPeriod, int x, int y, int z, Metric fx, Metric fy, Metric fz)
	{
	Metric *pVec = &g_gradient[INDEX3D(x, y, z % iPeriod) * 3];
	return pVec[0] * fx + pVec[1] * fy + pVec[2] * fz;
	}

//	Stochastic Opacity ---------------------------------------------------------

const int STOCHASTIC_DIM =							16;
const int STOCHASTIC_SIZE =							STOCHASTIC_DIM * STOCHASTIC_DIM;
extern BYTE STOCHASTIC_OPACITY[STOCHASTIC_SIZE][256];

void InitStochasticTable (void);

//	Helpers --------------------------------------------------------------------

class CCloudGenerator
	{
	public:
		CCloudGenerator (int iScale);

		Metric GetMaxValue (void) const { return m_rMaxValue; }
		Metric GetNext (void);
		void NextY (void);
		void ResetX (int x);
		void ResetY (int y);

	private:
		struct SFreq
			{
			SFreq (void) : pNoise(NULL)
				{ }

			~SFreq (void)
				{
				if (pNoise)
					delete pNoise;
				}

			int iFreq;
			Metric rAmplitude;
			CNoiseGenerator *pNoise;
			SNoisePos x;
			SNoisePos y;
			};

		int m_iScale;
		TArray<SFreq> m_Frequencies;

		Metric m_rMaxValue;
	};
