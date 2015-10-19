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

#ifndef BASSBOOST_H_
#define BASSBOOST_H_

#include "HighGain.h"

//number of coefficients in numerator and denominator of discrete TF
#define BB_ORDER 2

typedef struct{
	//stores output samples
	float m_afY[BB_ORDER];
	//stored input samples
	float m_afX[BB_ORDER];

	//positions in output and input buffers
	int m_iXPos;
	int m_iYPos;
}BassBoost;

void
bassBoost(BassBoost* pBB, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float fGain, float fFreq);

#endif /* BASSBOOST_H_ */
