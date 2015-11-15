/*
 * Implementation for sse stuff
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

#include "HighGain.h"

inline v8f_t
v8f_create(float* p)
{
#ifdef _MSC_VER
	return v8f_t(p);
#else
	return (v8f_t){
		p[0], p[1], p[2], p[3],
			p[4], p[5], p[6], p[7]
	};
#endif
}

inline void
v8f_get(float* p, v8f_t* v8f)
{
#ifdef _MSC_VER
	v8f->get(p);
#else
	p[0] = (*v8f)[0];
	p[1] = (*v8f)[1];
	p[2] = (*v8f)[2];
	p[3] = (*v8f)[3];
	p[4] = (*v8f)[4];
	p[5] = (*v8f)[5];
	p[6] = (*v8f)[6];
	p[7] = (*v8f)[7];
#endif
}
