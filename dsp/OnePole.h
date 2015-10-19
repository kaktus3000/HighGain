/*
 * one pole filters, high pass and low pass. classical matched coefficients.
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

#ifndef ONEPOLE_H_
#define ONEPOLE_H_

#include "HighGain.h"

typedef struct{
	//stores output samples
	float fY;
}OnePole;

void
highPass(OnePole* pOnePole, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float freq);

void
lowPass(OnePole* pOnePole, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float freq);

#endif /* ONEPOLE_H_ */
