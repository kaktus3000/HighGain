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

#ifndef BASSBOOST_LADSPA_H_
#define BASSBOOST_LADSPA_H_

#include <ladspa.h>

void
bassBoostInit(LADSPA_Descriptor * pDesc);

#endif /* BASSBOOST_LADSPA_H_ */
