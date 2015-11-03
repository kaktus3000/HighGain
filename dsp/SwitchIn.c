/*
 * Simple switch to selectively route one of two inputs to a single output
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

#include "Switch.h"

//crossover point 1ms @ 48kHz
#define DECAY_BASE 0.007f

void
switchIn(Switch* pSwitch, float* pfIn1, float* pfIn2, float* pOut, const uint nSamples, const uint uiSampleRate,
		uint uiChannel)
{
	//scale decay base with sample freq
	const float fDecay = 1.0f - DECAY_BASE * 48000.0f / (float)uiSampleRate;

	unsigned int uiSample = 0;
	for(; uiSample < nSamples; uiSample++)
	{
		//channel1: get amplitude up
		if(uiChannel)
			pSwitch->m_fAmplitude = 1.0f - (1.0f - pSwitch->m_fAmplitude)*fDecay;
		else
			//channel 0: amplitude down
			pSwitch->m_fAmplitude *= fDecay;

		pOut[uiSample] = pSwitch->m_fAmplitude *  pfIn2[uiSample]+
				(1.0f-pSwitch->m_fAmplitude) * pfIn1[uiSample];
	}
}

