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

#include "ToneStack.h"
#include "DirectFormI.h"
#include "toneStacks.h"

void
toneStack(ToneStack* pToneStack, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float gain, float b, float m, float t, uint uiType)
{
	//calculate coefficients
	double a0, a1, a2, a3;
	double b0, b1, b2, b3;
	double T = 1.0 / (double)uiSampleRate;

	toneStackCoefficients(
			&a0, &a1, &a2, &a3,
			&b0, &b1, &b2, &b3,
			uiType, b,m,t, T);

	float bs[TS_ORDER] = {(float)b0 * gain, (float)b1 * gain, (float)b2 * gain, (float)b3 * gain},
		as[TS_ORDER] = {(float)a0, (float)a1, (float)a2, (float)a3};

	//apply filter to signal
	directFormI(pIn, pOut, (uint)nSamples,
			bs, pToneStack->m_afX, TS_ORDER,
			as, pToneStack->m_afY, TS_ORDER,
			&(pToneStack->m_iXPos), &(pToneStack->m_iYPos));

}

uint getNumToneStacks()
{
	return NUM_TONESTACKS;
}
