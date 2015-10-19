/*
 * simulate several configurations of Fender-style tonestacks
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

#ifndef TONESTACK_H_
#define TONESTACK_H_

#include "HighGain.h"

//number of coefficients in numerator and denominator of discrete TF
#define TS_ORDER 4

typedef struct{
	//stores output samples
	float m_afY[TS_ORDER];
	//stored input samples
	float m_afX[TS_ORDER];

	//positions in output and input buffers
	int m_iXPos;
	int m_iYPos;
}ToneStack;

void
toneStack(ToneStack* pToneStack, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float gain, float b, float m, float t, uint uiType);

uint getNumToneStacks();

#endif /* TONESTACK_H_ */
