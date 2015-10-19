/*
 * Diode Saturator Simulation - data provider
 *
 * the integer parameters are meant in a "set-once" way, since they represent the physical circuit
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

#ifndef SATURATION_SOFT_H_
#define SATURATION_SOFT_H_

#include "HighGain.h"

typedef enum
{
	//for a non-inverting topology
	SAT_NON_INVERTING = 0,
	SAT_INVERTING,
	SAT_NOISE_SUPPRESSOR,
	SAT_NUM
}ESaturationType;

//various typical diode types
typedef enum
{
	DIO_SCHOTTKY = 0,
	DIO_JUNCTION,
	DIO_ZENER_2V4,
	DIO_ZENER_3V0,
	DIO_ZENER_3V9,
	DIO_LED,
	DIO_NUM
}EDiodeType;

typedef enum
{
	//pots with two pins shorted
	POT_LINEAR = 0,
	POT_LOG,
	//pot with three wires
	POT_LIN_SPLIT,
	POT_NUM
}EPotType;

//maximum output voltage
#define SATURATION_MAX_VOLTAGE 3.8f

void
saturation(const float *pIn, float *pOut, const uint nSamples,
		const ESaturationType satType, const uint nDiodes,
		const EDiodeType* pDiodeTypes, const EPotType potType,
		float R1, float R2, const float R3,
		const float fGainSetting);

#endif /* SATURATION_SOFT_H_ */
