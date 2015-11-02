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

#ifndef _MSC_VER
typedef float v8f_t __attribute__ ((vector_size (32), aligned(32)));
#define V8F_ZERO {0,0,0,0,0,0,0,0}
#else
#include <xmmintrin.h>

class v8f_t
{
public:
	inline
	v8f_t()
	{
		m_vf1 = _mm_setzero_ps();
		m_vf2 = _mm_setzero_ps();
	}

	inline v8f_t
	operator+(const v8f_t& rhs)
	{
		v8f_t obj;
		const __m128 temp = _mm_add_ps(m_vf1, rhs.m_vf1);
		obj.m_vf1 = temp;

		const __m128 temp2 = _mm_add_ps(m_vf2, rhs.m_vf2);
		obj.m_vf2 = temp2;

		return obj;
	}

	inline void
	operator+=(const v8f_t& rhs)
	{
		const __m128 temp = _mm_add_ps(m_vf1, rhs.m_vf1);
		m_vf1 = temp;

		const __m128 temp2 = _mm_add_ps(m_vf2, rhs.m_vf2);
		m_vf2 = temp2;
	}

	inline v8f_t
	operator*(const v8f_t& rhs)
	{
		v8f_t obj;
		const __m128 temp = _mm_mul_ps(m_vf1, rhs.m_vf1);
		obj.m_vf1 = temp;
		
		const __m128 temp2 = _mm_mul_ps(m_vf2, rhs.m_vf2);
		obj.m_vf2 = temp2;

		return obj;
	}

	inline float&
	operator[](const int uiIndex)
	{
		if (uiIndex >= 4)
			return m_vf2.m128_f32[uiIndex - 4];
		else
			return m_vf1.m128_f32[uiIndex];
	}
private:
	__m128 m_vf1, m_vf2;
};

#define V8F_ZERO v8f_t()
#endif


#endif /* HIGHGAIN_H_ */
