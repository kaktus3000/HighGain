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

#include "OnePole.h"

float getParam(float* d, float freq, uint uiSampleRate)
{
	float f0 = freq / (float)uiSampleRate;
	return expf(-2.0f * (float)M_PI * f0);
}

//run functions for the filters
void
highPass(OnePole* pOnePole, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float freq)
{
	float d = getParam(&d, freq, uiSampleRate);

	uint i = 0;
	for(; i < nSamples; i++)
	{
		const float fIn = pIn[i];
		pOnePole->fY = ((1-d) * pIn[i] + d * pOnePole->fY);

		pOut[i] = fIn - pOnePole->fY;
	}
}

void
lowPass(OnePole* pOnePole, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float freq)
{
	float d = getParam(&d, freq, uiSampleRate);

	uint i = 0;
	for(; i < nSamples; i++)
	{
		pOnePole->fY = ((1-d) * pIn[i] + d * pOnePole->fY);

		pOut[i] = pOnePole->fY;
	}
}
