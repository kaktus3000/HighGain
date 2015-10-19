/*
 * The central header file. nothing fancy, just some macros and helpers
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

#ifndef HIGHGAIN_H_
#define HIGHGAIN_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)
#define CLAMP(x,min,max) ( MIN(MAX(min, x), max) )
#define FRACF(f) (f-floorf(f))

#define REVERSE_INDEX(index, size) (size - 1 - index)

#define SAFE_ALLOC(VAR, TYPE) if ( (VAR = (TYPE*) malloc(sizeof(TYPE))) == NULL) return NULL

//interpolate pafX values on the given curve pafYs.
void lerp(const float* pafX, const unsigned int nXs, float* pafResults, const float* pafYs, const float fPerUnit);

typedef unsigned int uint;
typedef float v8f_t __attribute__ ((vector_size (32), aligned(32)));

#endif /* HIGHGAIN_H_ */