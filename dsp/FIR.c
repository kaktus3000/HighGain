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

void initializeFIR(FIR* pFIR)
{
	memset(pFIR->m_afBuffer, 0, FIR_SAMPLES_8 * sizeof(v8f_t));
	//fill fir coefficients
	//the fir is reversed here, so multiplication can be carried out sequentially
	uint uiPermutation = 0;
	for(; uiPermutation < 8; uiPermutation++)
	{
		uint uiFIRSample = 0;
		for(; uiFIRSample < FIR_SAMPLES; uiFIRSample++)
			pFIR->m_afFIR[uiPermutation * FIR_SAMPLES_8 + (uiFIRSample>>3)][uiFIRSample&0b111] =
					g_afFIR[(1 + uiPermutation + REVERSE_INDEX(uiFIRSample, FIR_SAMPLES))%FIR_SAMPLES];
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
		const uint uiPermutation = (pFIR->m_uiBufferPos)&0b111;
		//batch of current sample
		const uint uiBatchOffset = pFIR->m_uiBufferPos>>3;

		//put current sample to buffer, apply gain
		pFIR->m_afBuffer[uiBatchOffset][uiPermutation] = pIn[uiSample] * gain;

		//sub-sums of MAC operation
		v8f_t v8fSum = {0,0,0,0,0,0,0,0};

		//index to the block to use
		const uint uiPermutationOffset = FIR_SAMPLES_8 * uiPermutation;

		//multiply-accumulate FIR samples with input buffer
		uint uiBatch = 0;
		for(; uiBatch < uiBatchOffset; uiBatch++)
			v8fSum += pFIR->m_afBuffer[uiBatch] * pFIR->m_afFIR[uiBatch + FIR_SAMPLES_8 - uiBatchOffset + uiPermutationOffset];

		for(; uiBatch < FIR_SAMPLES_8; uiBatch++)
			v8fSum += pFIR->m_afBuffer[uiBatch] * pFIR->m_afFIR[uiBatch - uiBatchOffset + uiPermutationOffset];

		//accumulate sub-sums
		float fCollector = v8fSum[0];
		uint uiComponent = 1;
		for(; uiComponent < 8; uiComponent++)
			fCollector += v8fSum[uiComponent];

		//throw out a result
		pOut[uiSample] = CLAMP(fCollector, -1.0f, 1.0f);

		if(++(pFIR->m_uiBufferPos) >= FIR_SAMPLES)
			pFIR->m_uiBufferPos = 0;
	}
}
