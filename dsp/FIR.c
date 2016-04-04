/*
 * vectorized FIR filter. Best used for cabinet+microphone emulation
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

#include "FIR.h"

//include coefficient vectors
#include "firs.h"

#include <stdio.h>

void instantiateFIR(FIR* pFIR, const uint uiSampleRate)
{
	printf("FIR: instantiating FIR with %d samples / s\n", uiSampleRate);
	//check which base frequency to use
	float** ppfDataSource = NULL;
	uint nSourceRate = 0;
	uint* pnSourceBufferLengths = NULL;

	switch(uiSampleRate)
	{
	case 44100:
	case 22050:
	case 88200:
		ppfDataSource = g_aafFIRs44k1;
		nSourceRate = 44100;
		pnSourceBufferLengths = g_anSamples44k1;
		break;
	default:
	//case 48000:
	//case 96000:
	//case 192000:
		ppfDataSource = g_aafFIRs48k;
		nSourceRate = 48000;
		pnSourceBufferLengths = g_anSamples48k;
		break;
	}

	double fRateScaler = (float)nSourceRate / (double)uiSampleRate;

	printf("FIR: chosen rate %d, scaler %f\n", nSourceRate, fRateScaler);

	//reserve buffer
	uint uiModel = 0;
	for(; uiModel < NUM_MODELS; uiModel++)
	{
		uint nSourceSamples = pnSourceBufferLengths[uiModel];

		uint n8Tuples = (uint)(fRateScaler * (float)nSourceSamples - 1.0f) / 8 + 1;

		pFIR->m_anHistory8Tuples[uiModel] = n8Tuples;

		uint nDestSamples = n8Tuples * 8;

		pFIR->m_apfHistory[uiModel] = 
#ifdef _MSC_VER
			(v8f_t*) _aligned_malloc(n8Tuples * sizeof(v8f_t), 16);
#else
			(v8f_t*) calloc(n8Tuples, sizeof(v8f_t));
#endif
		pFIR->m_pfFIR[uiModel] =
#ifdef _MSC_VER
			(v8f_t*) _aligned_malloc(nDestSamples * sizeof(v8f_t), 16);
#else
			(v8f_t*) calloc(nDestSamples, sizeof(v8f_t));
#endif

		//fill fir coefficients
		//the fir is reversed here, so multiplication with history data can be carried out sequentially
		uint uiPermutation = 0;
		for(; uiPermutation < 8; uiPermutation++)
		{
			uint uiFIRSample = 0;
			while (uiFIRSample < nDestSamples)
			{
				float afCoeffs[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
				const uint uiStartSample = uiFIRSample;
				for (; uiFIRSample < uiStartSample + 8 && uiFIRSample < nDestSamples; uiFIRSample++)
				{
					uint uiSourcePosUnscaled = (uiPermutation - uiFIRSample + nDestSamples) % nDestSamples;
					uint uiSourceBufferPos = (uint)((double)uiSourcePosUnscaled * fRateScaler);

					if(uiSourceBufferPos < nSourceSamples)
						afCoeffs[uiFIRSample & 0x7] = ppfDataSource[uiModel][uiSourceBufferPos];
				}

				const uint uiDestIndex = uiPermutation * n8Tuples + (uiStartSample >> 3);
				pFIR->m_pfFIR [uiModel][uiDestIndex] = v8f_create(afCoeffs);
			}
		}
	}
	memset(pFIR->m_auiBufferPos, 0, NUM_MODELS * sizeof(uint));
}

void
fir(FIR* pFIR, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		uint model, float gain)
{
	uint uiSample = 0;
	for(; uiSample < nSamples; uiSample++)
	{
		//position of current sample in history buffer
		const uint uiPermutation = (pFIR->m_auiBufferPos[model])&0x7;
		//batch of current sample
		const uint uiBatchOffset = pFIR->m_auiBufferPos[model]>>3;

		uint uiModel = 0;
		for(; uiModel < NUM_MODELS; uiModel++)
		{
			//position of current sample in history buffer
			const uint uiModelPermutation = (pFIR->m_auiBufferPos[uiModel])&0x7;
			//batch of current sample
			const uint uiModelBatchOffset = pFIR->m_auiBufferPos[uiModel]>>3;

			//put current sample to buffer, apply gain
			float afBuffer[8];
			v8f_get(afBuffer, &pFIR->m_apfHistory[uiModel][uiModelBatchOffset]);

			afBuffer[uiModelPermutation] = pIn[uiSample] * gain;
			pFIR->m_apfHistory[uiModel][uiModelBatchOffset] = v8f_create(afBuffer);

			if(++(pFIR->m_auiBufferPos[uiModel]) >= pFIR->m_anHistory8Tuples[uiModel] * 8)
				pFIR->m_auiBufferPos[uiModel] = 0;
		}
		//sub-sums of MAC operation
		v8f_t v8fSum = V8F_ZERO;

		//index to the block to use
		const uint uiPermutationOffset = pFIR->m_anHistory8Tuples[model] * uiPermutation;

		const uint FIR_SAMPLES_8 = pFIR->m_anHistory8Tuples[model];

		//multiply-accumulate FIR samples with input buffer
		uint uiBatch = 0;
		for(; uiBatch < uiBatchOffset; uiBatch++)
			v8fSum += pFIR->m_apfHistory[model][uiBatch] * (pFIR->m_pfFIR[model][uiBatch + FIR_SAMPLES_8 - uiBatchOffset + uiPermutationOffset]);

		for(; uiBatch < FIR_SAMPLES_8; uiBatch++)
			v8fSum += pFIR->m_apfHistory[model][uiBatch] * (pFIR->m_pfFIR[model][uiBatch - uiBatchOffset + uiPermutationOffset]);

		//accumulate sub-sums
		float afResults[8];
		v8f_get(afResults, &v8fSum);

		float fCollector = afResults[0];
		uint uiComponent = 1;
		for(; uiComponent < 8; uiComponent++)
			fCollector += afResults[uiComponent];

		//throw out a result
		pOut[uiSample] = CLAMP(fCollector, -1.0f, 1.0f);
	}
}
