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

void inistantiateFIR(FIR* pFIR, const uint uiSampleRate)
{
	//check which base frequency to use
	float** ppfDataSource = NULL;
	uint nSourceRate = 0;
	float* pnSourceBufferLengths = NULL;

	switch(uiSampleRate)
	{
	case 44100:
	case 22050:
	case 88200:
		ppfDataSource = g_aafFIRs44k1;
		nSourceRate = 44100;
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

	double fRateScaler = (double)uiSampleRate / (float)nSourceRate;

	float fNormalizer = 1.0f;
	if(fRateScaler > 1.0f)
		fNormalizer = fRateScaler;

	//reserve buffer
	uint uiModel = 0;
	for(; uiModel < NUM_MODELS; uiModel++)
	{
		uint nSourceSamples = pnSourceBufferLengths[uiModel];

		uint n8Tuples = (uint)(fRateScaler * (float)nSourceSamples - 1.0f) / 8 + 1;

		pFIR->m_anBuffer8Tuples[uiModel] = n8Tuples;

		uint nDestSamples = n8Tuples * 8;

		pFIR->m_apfBuffers[uiModel] = calloc(n8Tuples, sizeof(v8f_t));
		pFIR->m_pfFIR[uiModel] = (v8f_t*) calloc(nDestSamples, sizeof(v8f_t));
		//memset(pFIR->m_afBuffer, 0, FIR_SAMPLES_8 * sizeof(v8f_t));

		//fill fir coefficients
		//the fir is reversed here, so multiplication can be carried out sequentially
		uint uiPermutation = 0;
		for(; uiPermutation < 8; uiPermutation++)
		{
			uint uiFIRSample = 0;
			while (uiFIRSample < nSourceSamples)
			{
				float afCoeffs[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
				const uint uiStartSample = uiFIRSample;
				for (; uiFIRSample < uiStartSample + 8 && uiFIRSample < nSourceSamples; uiFIRSample++)
				{
					const uint uiBufferOff = (1 + uiPermutation + REVERSE_INDEX(uiFIRSample, nSourceSamples)) % nSourceSamples;
					afCoeffs[uiFIRSample & 0x7] = ppfDataSource[uiModel][uiBufferOff] * fNormalizer;
				}

				const uint uiDestIndex = uiPermutation * n8Tuples + ((uint)((double)uiStartSample * fRateScaler) >> 3);
				pFIR->m_pfFIR [uiModel][uiDestIndex] = v8f_create(afCoeffs);
			}
		}
	}
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
			v8f_get(afBuffer, &pFIR->m_apfBuffers[uiModel][uiModelBatchOffset]);
			afBuffer[uiModelPermutation] = pIn[uiSample] * gain;
			pFIR->m_apfBuffers[uiModel][uiModelBatchOffset] = v8f_create(afBuffer);

			if(++(pFIR->m_auiBufferPos[uiModel]) >= pFIR->m_anBuffer8Tuples[uiModel] * 8)
				pFIR->m_auiBufferPos[uiModel] = 0;
		}

		//sub-sums of MAC operation
		v8f_t v8fSum = V8F_ZERO;

		//index to the block to use
		const uint uiPermutationOffset = pFIR->m_anBuffer8Tuples[model] * uiPermutation;

		const uint FIR_SAMPLES_8 = pFIR->m_anBuffer8Tuples[model];

		//multiply-accumulate FIR samples with input buffer
		uint uiBatch = 0;
		for(; uiBatch < uiBatchOffset; uiBatch++)
			v8fSum += pFIR->m_apfBuffers[model][uiBatch] * (pFIR->m_pfFIR[model][uiBatch + FIR_SAMPLES_8 - uiBatchOffset + uiPermutationOffset]);

		for(; uiBatch < FIR_SAMPLES_8; uiBatch++)
			v8fSum += pFIR->m_apfBuffers[model][uiBatch] * (pFIR->m_pfFIR[model][uiBatch - uiBatchOffset + uiPermutationOffset]);

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
