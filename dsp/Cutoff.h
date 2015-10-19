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

#ifndef CUTOFF_H_
#define CUTOFF_H_

#include "HighGain.h"

//define filter coefficients
static const float g_afFilter[10] = {0.03703052384075727f, 0.1651567481422922f, 0.3355051771420027f, 0.389393269162582f, 0.23536072856342313f, 0.00412742180227102f, -0.10977662999769633f, -0.07280804713409078f, -0.003447604814302327f, 0.019458413292761212f};

#define FIR_SAMPLES   ((uint)sizeof(g_afFilter) / (uint)sizeof(float))

typedef struct{
	//buffer for fir input history
	float m_afBuffer[FIR_SAMPLES];
	unsigned int m_uiBufferPos;
}Cutoff;

void
cutOff(Cutoff* pCutoff, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate);


#endif
