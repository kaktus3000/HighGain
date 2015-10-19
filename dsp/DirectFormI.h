/*
 * Straightforward direct form 1 discrete filter implementation
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

#ifndef DIRECTFORMI_H_
#define DIRECTFORMI_H_

#include "HighGain.h"

void
directFormI(float* pIn, float* pOut, const uint nSamples,
		float* bs, float* xs, const uint nBs,
		float* as, float* ys, const uint nAs,
		int* pXPos, int* pYPos);

#endif /* DIRECTFORMI_H_ */
