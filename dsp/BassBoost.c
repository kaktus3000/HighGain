/*
 * Low Shelving bass booster, inverting op amp R3 parallel to C, R1 and R2 in series
 * (first-order filter)
 *
 * low shelf gain is positive (in dB)
 * high frequency gain is unity
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

#include "BassBoost.h"
#include "DirectFormI.h"

void
bassBoost(BassBoost* pBB, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float gain, float freq)
{
	//base resistor 1k, seems to be realistic
	double R1=1000.0;
	double R2=R1;
	double R3 = R1*gain-R2;

	//equation found analytically
	double C = sqrt(-(R3*R3+2*R2*R3+R2*R2-gain*R1*R1)/(R2*R2-gain*R1*R1))/(2.0*M_PI*freq*R3);

	//limit C to 2 pF
	C = MAX(C, 2e-12);

	//find poles and zeroes of continuous TF
	double s_z = -(R2+R3)/(R2*R3*C);
	double s_p = -1.0/(R3*C);

	//find discrete coefficients
	double a0, a1;
	double b0, b1;
	double T = 1.0 / (double)uiSampleRate;

	b0=1;
	a0=1;

	b1 = - exp(s_z * T);
	a1 = - exp(s_p * T);

	float bs[BB_ORDER] = {(float)b0, (float)b1},
		as[BB_ORDER] = {(float)a0, (float)a1};

	//apply filter to signal
	directFormI(pIn, pOut, (uint)nSamples,
			bs, pBB->m_afX, BB_ORDER,
			as, pBB->m_afY, BB_ORDER,
			&(pBB->m_iXPos), &(pBB->m_iYPos));
}
