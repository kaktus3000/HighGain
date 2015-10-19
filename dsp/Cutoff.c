/*
 * Little FIR to cut off high frequency trash above 0.2 fs
 * Attenuates about 40dB
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

#include "Cutoff.h"

void
cutOff(Cutoff* pCutoff, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate)
{
	uint uiSample = 0;
	for(; uiSample < nSamples; uiSample++)
	{
		pCutoff->m_afBuffer[pCutoff->m_uiBufferPos] = pIn[uiSample];

		//straightforward FIR implementation
		float fSum = 0;
		uint uiFilterSample = 0;
		for(; uiFilterSample < FIR_SAMPLES; uiFilterSample++)
		{
			const uint uiBufferOff = (FIR_SAMPLES + pCutoff->m_uiBufferPos - uiFilterSample) % FIR_SAMPLES;
			fSum += g_afFilter[uiFilterSample] * pCutoff->m_afBuffer[uiBufferOff];
		}

		pOut[uiSample] = CLAMP(fSum, -1.0f, 1.0f);

		//increment ring buffer
		if(++(pCutoff->m_uiBufferPos) >= FIR_SAMPLES)
			pCutoff->m_uiBufferPos = 0;
	}
}
