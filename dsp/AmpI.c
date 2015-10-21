/*
 * Solid state amplifier simulation
 *
 * features:
 * - clean/dist channel
 * - lead channel
 * - two gain controls
 * - full tonestack
 * - physically based voltage levels
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

#include "AmpI.h"

void
ampI(AmpI* pAmp, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float pre, float dist, uint bDist, uint bLead,
		float b, float m, float t, float vol)
{
	//code on the windows os can't allocate on the stack...
	float* afTempIn = calloc(nSamples, sizeof(float));
	float* afTempOut = calloc(nSamples, sizeof(float));
	float* afClean = calloc(nSamples, sizeof(float));

	memcpy(afTempIn, pIn, sizeof(float) * nSamples);

	//input filter
	highPass(&pAmp->m_HpInput, afTempIn, afTempOut, nSamples, uiSampleRate, 165);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//1st stage
	static const EDiodeType aDiodes[] = {DIO_ZENER_2V4, DIO_SCHOTTKY};
	saturationSigned(afTempIn, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes, POT_LINEAR, 1000, 50000, 100, pre);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);
	memcpy(afClean, afTempOut, sizeof(float) * nSamples);

	//1st stage decoupling
	highPass(&pAmp->m_Hp1stDecoup, afTempIn, afTempOut, nSamples, uiSampleRate, 358);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//1st stage cutoff
	lowPass(&pAmp->m_Lp1stCutoff, afTempIn, afTempOut, nSamples, uiSampleRate, 2000);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//attenuator
	int iSample = 0;
	for(; iSample < nSamples; iSample++)
		afTempIn[iSample] *= 0.5f;

	//2nd stage
	saturationSigned(afTempIn, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes, POT_LINEAR, bLead ? 1000 : 2000, 4700, 47, 1);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//2nd stage decoupling
	highPass(&pAmp->m_Hp2ndDecoup, afTempIn, afTempOut, nSamples, uiSampleRate, 61);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//2nd stage cutoff
	lowPass(&pAmp->m_Lp2ndCutoff, afTempIn, afTempOut, nSamples, uiSampleRate, bLead ? 4000 : 2000);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//3rd stage
	saturationSigned(afTempIn, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes, POT_LINEAR, 1000, 100000, 100, dist);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//bass boost after 3rd stage
	bassBoost(&pAmp->m_BassBoost, afTempIn, afTempOut, nSamples, uiSampleRate, 2, 300);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);
	//switch clean/dist
	switchIn(&pAmp->m_Switch, afClean, afTempIn, afTempOut, nSamples, uiSampleRate, bDist ? 1 : 0);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//master tone stack
	toneStack(&pAmp->m_ToneStack, afTempIn, afTempOut, nSamples, uiSampleRate, 1, b, m, t, 9);
	memcpy(afTempIn, afTempOut, sizeof(float) * nSamples);

	//master cutoff
	cutOff(&pAmp->m_Cutoff, afTempIn, afTempOut, nSamples, uiSampleRate);

	//master volume
	iSample = 0;
	for(; iSample < nSamples; iSample++)
		afTempOut[iSample] = CLAMP(afTempOut[iSample] * 0.1f * vol / (1.0f - vol), -1.0f, 1.0f);

	memcpy(pOut, afTempOut, sizeof(float) * nSamples);

	//free temp buffers
	free(afTempIn);
	free(afTempOut);
	free(afClean);

}

