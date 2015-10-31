/*
 * Diode Saturator Simulation - data provider
 *
 * the integer parameters are meant in a "set-once" way, since they represent the physical circuit
 *
 * (c) 2013 Benedikt Hofmeister
 */

/*
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA or point your web browser to http://www.gnu.org.
*/

#include "Saturation.h"

//#define SPAM_OUTPUT
//#define SPAM_CURVES

#ifdef SPAM_OUTPUT
#include <stdio.h>
#endif

#ifdef SPAM_OUTPUT
#include <stdio.h>
#endif

#include <stdio.h>

inline float
diodeVoltageCached(const float fLogC1, const float fOverC2, const float fLogI)
{
	const float fDiff = fLogI - fLogC1;
	return fDiff > 0 ? fDiff*fOverC2 : 0;
}

inline float
diodeCurrent(const float C1, const float C2, const float Ud)
{
	if(Ud < 0.0f)
		return 0;
	return C1 * expf(Ud * C2);
}

//diode params[0] -> C1
//diode params[1] -> C2
//cached params[0] -> log(C1)
//cached params[1] -> C2^(-1)
inline float
feedbackVoltageCached(const float Id, const float R3, float* const aafDiodeParams[2], float* const aafCachedParams[2], const uint nDiodes)
{
	float fLogId = logf(Id);

	float Ud = 0;
	uint uiDiode = 0;
	for(; uiDiode < nDiodes; uiDiode++)
		if(Id > aafDiodeParams[0][uiDiode])
			Ud += diodeVoltageCached(aafCachedParams[0][uiDiode], aafCachedParams[1][uiDiode], fLogId);

	return Ud + R3 * Id;
}

inline float
feedbackCurrentCached(const float Id, const float fOverR2, const float R3, float* const aafDiodeParams[2], float* const aafCachedParams[2], const uint nDiodes)
{
	return feedbackVoltageCached(Id, R3, aafDiodeParams, aafCachedParams, nDiodes) * fOverR2 + Id;
}

inline float
feedbackCurrentDerivativeCached(const float Id, const float fOverR2, const float R3, float* const aafDiodeParams[2], float* const aafCachedParams[2], const uint nDiodes)
{
	float dIf = 1.0f;
	float fOverId = 1.0f / Id;

	uint uiDiode = 0;
	for(; uiDiode < nDiodes; uiDiode++)
		if(Id > aafDiodeParams[0][uiDiode])
			dIf += fOverId * aafCachedParams[1][uiDiode] * fOverR2;

	return dIf;
}

//R1, R2, C1, C2
//resistance to ground / virtual ground, defining current
//feedback resistance, defining gain at zero signal
//resistance for diodes
//constants for diode, defining saturation


//parameters for diode power law
//Id = C1 * expf(C2 * Ud)
float g_acfDiodes[DIO_NUM][2] = {
	{0.000000001f, 23.0258509299f},
	{8.37391804372927E-011f, 10.0589869527f},
	{1.87381742286039E-008f, 41.865183509f},
	{4.53158363760083E-008f, 7.1955784156f},
	{3.98107170553499E-008f, 4.605170186f},
	{0.00000001f, 3.8376418217f}
};

//a minimum value for R1, R2
#define R_MIN 3.0f

void
saturation(const float *pIn, float *pOut, const uint nSamples,
		const ESaturationType satType, const uint nDiodes,
		const EDiodeType* const pDiodeTypes, const EPotType potType,
		float R1, float R2, const float R3,
		const float fGainSetting)
{
#ifdef SPAM_OUTPUT
	puts("running saturation");
#endif

	float afDiodeC1[nDiodes];
	float afDiodeC2[nDiodes];
	float* const aafDiodeParams[2]={afDiodeC1, afDiodeC2};
	uint uiDiode = 0;
	for(; uiDiode < nDiodes; uiDiode++)
	{
#ifdef SPAM_OUTPUT
		printf("diode %d is type %d\n", uiDiode, pDiodeTypes[uiDiode]);
#endif

		aafDiodeParams[0][uiDiode] = g_acfDiodes[pDiodeTypes[uiDiode]][0];
		aafDiodeParams[1][uiDiode] = g_acfDiodes[pDiodeTypes[uiDiode]][1];
	}

#ifdef SPAM_OUTPUT
	printf("R1 = %f\n", R1);
#endif

#ifdef SPAM_OUTPUT
	printf("R1 = %f\n", R1);
#endif

	switch(potType)
	{
	case POT_LINEAR:
#ifdef SPAM_OUTPUT
		puts("pot is linear.");
#endif

		R2 *= fGainSetting;
		break;
	case POT_LOG:
	{	//simulate a pot with 60 dB wipe
#ifdef SPAM_OUTPUT
		puts("pot is log.");
#endif

		float fdBAttenuation = -60.0f * (1.0f - fGainSetting);
		R2 *= powf(10.0f, fdBAttenuation/20.0f);
		break;
	}
	//pot with three wires
	case POT_LIN_SPLIT:
#ifdef SPAM_OUTPUT
		puts("pot is split.");
#endif

		R1 += R2 * (1.0f-fGainSetting);

		R2 *= fGainSetting;
		break;
	default:
		break;
	}

	R2 = fmaxf(R_MIN, R2);
	R1 = fmaxf(R_MIN, R1);

	//minimum feedback current for which to solve
	//-140dB is plenty, well below A/D noise
	//float fMinFeedbackCurrent = 1e-7f * SATURATION_MAX_VOLTAGE / R1;
	float fMinFeedbackCurrent = 1e-8f * SATURATION_MAX_VOLTAGE / R1;

	//uint iters = 0;

	//calculate cached inverse resistor values
	const float fOverR1 = 1.0f / R1;
	const float fOverR2 = 1.0f / R2;
	//calculate cached diode values
	float afCached1[nDiodes];
	float afCached2[nDiodes];
	float* const aafCachedParams[2]={afCached1, afCached2};
	uiDiode = 0;
	for(; uiDiode < nDiodes; uiDiode++)
	{
		aafCachedParams[0][uiDiode] = logf(aafDiodeParams[0][uiDiode]);
		aafCachedParams[1][uiDiode] = 1.0f/aafDiodeParams[1][uiDiode];
	}

	uint uiSample = 0;
	for(; uiSample < nSamples; uiSample++)
	{
		float Ucurr = pIn[uiSample];
#ifdef SPAM_OUTPUT
		printf("processing voltage %f\n", Ucurr);
#endif

		//inverting saturation is the simplest to calculate
#ifdef SPAM_OUTPUT
		printf("calculating inverting saturation. R1=%f R2=%f R3=%f Uin=%f\n", R1, R2, R3, Ucurr);
#endif
		float If = Ucurr * fOverR1;

#ifdef SPAM_OUTPUT
		printf("number of diodes is %d; If=%.10e\n", nDiodes, If);
#endif

#ifdef SPAM_OUTPUT
		puts("finding root of equation");
#endif
		//current through diode must be somewhere between zero and total current
		//epsilon to ensure good approximation
		float fIdMax = fminf(If, diodeCurrent(aafDiodeParams[0][0], aafDiodeParams[1][0], R2 * If));
		//find the diode current by -100dB of the total feedback current
		//limit accuracy for low signal level to absolute minimum current
		//const float fEpsilon = fmaxf(If * 1e-5f, fMinFeedbackCurrent);
		const float fEpsilon = fmaxf(If * 1e-6f, fMinFeedbackCurrent);

		//minimum bound diode current
		float fIdMin = 0;

		//find root of monotonic ascending function, must be in [fIdMin : fIdMax]
		//Newton with upper and lower bounds, defaults to bisection if convergence is bad

		//feedback current for lower and upper bound
		float fIfMin = 0;
		float fIfMax = feedbackCurrentCached(fIdMax, fOverR2, R3, aafDiodeParams, aafCachedParams, nDiodes);

		//best values of search
		float Id_best = fIdMax;
		float If_best = fIfMax;

		while(fabsf(If_best - If) > fEpsilon )
		{
			//calculate guess for Id
			float Id_current;

			//start point is the bound closer to the objective
			//use newtons method, first order
			if(fabsf(fIfMax - If) > fabsf(fIfMin - If) )
			{
				const float fDMin = feedbackCurrentDerivativeCached(fIdMin, fOverR2, R3, aafDiodeParams, aafCachedParams,nDiodes);
				Id_current = fIdMin + (If - fIfMin) / fDMin;
			}
			else
			{
				const float fDMax = feedbackCurrentDerivativeCached(fIdMax, fOverR2, R3, aafDiodeParams, aafCachedParams,nDiodes);
				Id_current = fIdMax - (fIfMax - If) / fDMax;
			}

			//if out of bounds, default to bisection
			if(Id_current <= fIdMin || Id_current >= fIdMax)
				Id_current = 0.5f * (fIdMin + fIdMax);

			//calculate target function
			const float If_current = feedbackCurrentCached(Id_current, fOverR2, R3, aafDiodeParams, aafCachedParams, nDiodes);

			//check if the current value is new lower or new upper bound
			if(If_current - If < 0)
			{
				fIfMin = If_current;
				fIdMin = Id_current;
			}
			else
			{
				fIfMax = If_current;
				fIdMax = Id_current;
			}

			//break when numerically exhausted
			if(If_best == If_current || Id_best == Id_current)
				break;

			//write output
			Id_best = Id_current;
			If_best = If_current;

			//iters += 1;
		}

		float Uout = 0;
		//check which branch carries most current
		if(Id_best < If*0.5f)
		//resistance dominates -> low voltages, linear region
			Uout = - (If - Id_best) * R2;
		else
		//diodes dominate -> high voltages, nonlinear region
			Uout = -feedbackVoltageCached(Id_best, R3, aafDiodeParams, aafCachedParams, nDiodes);

#ifdef SPAM_OUTPUT
		printf("Id = %.10e; (If - Id) = %.10e Uo = %.6f\n", Id_best, If-Id_best, Uout);
#endif

		//check which one is actually used and alter value to that
		switch(satType)
		{
		case SAT_NON_INVERTING:
#ifdef SPAM_OUTPUT
			puts("saturation is non-inverting.");
#endif
			Uout = Ucurr - Uout;
			break;
		case SAT_INVERTING:
#ifdef SPAM_OUTPUT
			puts("saturation is inverting.");
#endif
			//do nothing
			break;
		case SAT_NOISE_SUPPRESSOR:
#ifdef SPAM_OUTPUT
			puts("saturation is a noise suppressor.");
			printf("R1 = %f\n", R1);
#endif
			//calculate voltage divider for input and inverting saturation
			Uout = (Uout * R1 + Ucurr * R2) / (R1 + R2);
#ifdef SPAM_CURVES
			printf("%f\t%f\n", Ucurr, Uout);
#endif
			break;
		default:
			break;
		}

		//clamp to maximum permissible voltage
		pOut[uiSample] = MIN(SATURATION_MAX_VOLTAGE, Uout);
	}
	//printf("%.10f\t%.10f\n", pIn[nSamples - 1], pOut[nSamples - 1]);
	//printf("Uin = %.10f UOut = %.10f iters = %.2f\n", pIn[nSamples - 1], pOut[nSamples - 1], (float)iters / (float)nSamples);

#ifdef SPAM_CURVES
	puts("");
#endif


#ifdef SPAM_CURVES
	if(satType == SAT_NOISE_SUPPRESSOR)
			exit(0);
#endif

	//exit(0);
}

void
saturationSigned(const float *pIn, float *pOut, const uint nSamples,
		const ESaturationType satType, const uint nDiodes,
		const EDiodeType* pDiodeTypes, const EPotType potType,
		float ground, float pot, const float series,
		const float fGainSetting)
{
	uint i = 0;

	float* afTempIn = calloc(nSamples, sizeof(float));

	for(; i < nSamples; i++)
		afTempIn[i] = fabsf(pIn[i]);

	saturation(afTempIn, pOut, nSamples,
				satType, nDiodes, pDiodeTypes, potType,
				ground, pot, series,
				fGainSetting);

	i = 0;
	for(; i < nSamples; i++)
	//saturation calculated using absolute voltages
	//copy sign of input and saturation curve to output
	//multiplication for inverting/non-inverting circuit
		pOut[i] = copysignf(pOut[i], pIn[i] * pOut[i]);


	free(afTempIn);
}
