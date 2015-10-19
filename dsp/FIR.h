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

#ifndef FIR_H_
#define FIR_H_

#include "HighGain.h"

#define FIR_SAMPLES_8 (539 / 8 + 1)
#define FIR_SAMPLES   (FIR_SAMPLES_8 * 8)

typedef struct{
	//history buffer. vectors are filled component by component
	v8f_t m_afBuffer[FIR_SAMPLES_8];

	//buffer layout:
	//8 blocks of FIR_SAMPLES/8 coefficients. each block shifted one sample further to the future
	//after that, the blocks can be shifted by one
	v8f_t m_afFIR[FIR_SAMPLES];

	unsigned int m_uiBufferPos;
}FIR;

void initializeFIR(FIR* pFIR);

void
fir(FIR* pFIR, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		uint model, float gain);

#endif
