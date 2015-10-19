/*
 * define some configurations for Fender-type tonestack and calculate filter coefficients
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


#ifndef TONESTACKS_H_
#define TONESTACKS_H_

//define resistor and capacitor values (5 resistors, 3 caps)
double g_afToneStacks[][8] = {
								{250.e3, 250.e3, 10.e3, 100.e3,   1.e6, 250.e-12, .100e-6, .047e-6},//mesa boogie mark 4
								{250.e3, 1.00e6, 25.e3,  47.e3,   1.e6, 570.e-12, .022e-6, .022e-6},//Peavey 5150
								{250.e3, 1.00e6, 25.e3,  56.e3,   1.e6, 250.e-12, .020e-6, .020e-6},//Fender Bassman
								{250.e3, 250.e3, 25.e3, 100.e3,   1.e6, 250.e-12, .022e-6, .022e-6},//Fender Blues Junior
								{250.e3, 250.e3, 25.e3, 100.e3,   1.e6, 270.e-12, .100e-6, .022e-6},//Fender Princeton
								{220.e3, 1.00e6, 25.e3,  33.e3, 517.e3, 470.e-12, .022e-6, .022e-6},//marshall
								{250.e3, 1.00e6, 20.e3,  68.e3, 690.e3,   1.e-09, .047e-6, .047e-6},//engl fireball
								{250.e3, 1.00e6, 25.e3,  56.e3, 690.e3, 220.e-12, .022e-6, .022e-6},//hughes and kettner triamp clean
								{250.e3, 1.00e6, 25.e3,  33.e3, 690.e3, 470.e-12, .022e-6, .022e-6},//hughes and kettner triamp crunch
								{ 10.e3, 10.0e3, 1.0e3,  3.9e3, 39.0e3,  10.e-09,  2.2e-6, .470e-6},//proposed amp
							};

#define NUM_TONESTACKS ((uint)sizeof(g_afToneStacks) / (uint)sizeof(double[8]))

//calculate discrete coefficients for fender-style tonestack
//derivation based on Yeh06, but added resistance of gain pot

//based on calculating continuous model and applying bilinear transform
//solved using maxima
void
toneStackCoefficients(double* pa0, double* pa1, double* pa2, double* pa3,
							double* pb0, double* pb1, double* pb2, double* pb3,
							unsigned int uiToneStack,
							double b, double m, double t, double T)
{
	double a0, a1, a2, a3;
	double b0, b1, b2, b3;

	//read component values

	double
		R1 = g_afToneStacks[uiToneStack][0],
		R2 = g_afToneStacks[uiToneStack][1],
		R3 = g_afToneStacks[uiToneStack][2],
		R4 = g_afToneStacks[uiToneStack][3],
		R5 = g_afToneStacks[uiToneStack][4],
		C1 = g_afToneStacks[uiToneStack][5],
		C2 = g_afToneStacks[uiToneStack][6],
		C3 = g_afToneStacks[uiToneStack][7];

	//crunch some numbers
	b3=-2*R5
	         *(C1*R1*t*T*T+C3*m*R3*T*T+C2*R3*T*T+C1*R3*T*T+b*C2*R2*T*T+b*C1*R2*T*T
	                      -2*C1*C3*R1*R4*t*T-2*C1*C2*R1*R4*t*T-2*C1*C3*R3*R4*T
	                      -2*C1*C2*R3*R4*T-2*b*C1*C3*R2*R4*T-2*b*C1*C2*R2*R4*T
	                      +2*C2*C3*m*m*R3*R3*T+2*C1*C3*m*m*R3*R3*T-2*C2*C3*m*R3*R3*T
	                      -2*C1*C3*m*R3*R3*T-2*b*C2*C3*m*R2*R3*T
	                      -2*b*C1*C3*m*R2*R3*T-2*C1*C3*m*R1*R3*T-2*C1*C2*R1*R3*T
	                      -2*b*C1*C2*R1*R2*T-4*C1*C2*C3*m*R1*R3*R4*t
	                      +4*C1*C2*C3*R1*R3*R4*t+4*b*C1*C2*C3*R1*R2*R4*t
	                      -4*C1*C2*C3*m*m*R3*R3*R4+4*C1*C2*C3*m*R3*R3*R4
	                      +4*b*C1*C2*C3*m*R2*R3*R4-4*C1*C2*C3*m*m*R1*R3*R3
	                      +4*C1*C2*C3*m*R1*R3*R3+4*b*C1*C2*C3*m*R1*R2*R3);

	b2=-2*R5
	         *(C1*R1*t*T*T+C3*m*R3*T*T+C2*R3*T*T+C1*R3*T*T+b*C2*R2*T*T+b*C1*R2*T*T
	                      +2*C1*C3*R1*R4*t*T+2*C1*C2*R1*R4*t*T+2*C1*C3*R3*R4*T
	                      +2*C1*C2*R3*R4*T+2*b*C1*C3*R2*R4*T+2*b*C1*C2*R2*R4*T
	                      -2*C2*C3*m*m*R3*R3*T-2*C1*C3*m*m*R3*R3*T+2*C2*C3*m*R3*R3*T
	                      +2*C1*C3*m*R3*R3*T+2*b*C2*C3*m*R2*R3*T
	                      +2*b*C1*C3*m*R2*R3*T+2*C1*C3*m*R1*R3*T+2*C1*C2*R1*R3*T
	                      +2*b*C1*C2*R1*R2*T+12*C1*C2*C3*m*R1*R3*R4*t
	                      -12*C1*C2*C3*R1*R3*R4*t-12*b*C1*C2*C3*R1*R2*R4*t
	                      +12*C1*C2*C3*m*m*R3*R3*R4-12*C1*C2*C3*m*R3*R3*R4
	                      -12*b*C1*C2*C3*m*R2*R3*R4+12*C1*C2*C3*m*m*R1*R3*R3
	                      -12*C1*C2*C3*m*R1*R3*R3-12*b*C1*C2*C3*m*R1*R2*R3);

	b1=2*R5
	        *(C1*R1*t*T*T+C3*m*R3*T*T+C2*R3*T*T+C1*R3*T*T+b*C2*R2*T*T+b*C1*R2*T*T
	                     -2*C1*C3*R1*R4*t*T-2*C1*C2*R1*R4*t*T-2*C1*C3*R3*R4*T
	                     -2*C1*C2*R3*R4*T-2*b*C1*C3*R2*R4*T-2*b*C1*C2*R2*R4*T
	                     +2*C2*C3*m*m*R3*R3*T+2*C1*C3*m*m*R3*R3*T-2*C2*C3*m*R3*R3*T
	                     -2*C1*C3*m*R3*R3*T-2*b*C2*C3*m*R2*R3*T-2*b*C1*C3*m*R2*R3*T
	                     -2*C1*C3*m*R1*R3*T-2*C1*C2*R1*R3*T-2*b*C1*C2*R1*R2*T
	                     +12*C1*C2*C3*m*R1*R3*R4*t-12*C1*C2*C3*R1*R3*R4*t
	                     -12*b*C1*C2*C3*R1*R2*R4*t+12*C1*C2*C3*m*m*R3*R3*R4
	                     -12*C1*C2*C3*m*R3*R3*R4-12*b*C1*C2*C3*m*R2*R3*R4
	                     +12*C1*C2*C3*m*m*R1*R3*R3-12*C1*C2*C3*m*R1*R3*R3
	                     -12*b*C1*C2*C3*m*R1*R2*R3);

	b0=2*R5
	        *(C1*R1*t*T*T+C3*m*R3*T*T+C2*R3*T*T+C1*R3*T*T+b*C2*R2*T*T+b*C1*R2*T*T
	                     +2*C1*C3*R1*R4*t*T+2*C1*C2*R1*R4*t*T+2*C1*C3*R3*R4*T
	                     +2*C1*C2*R3*R4*T+2*b*C1*C3*R2*R4*T+2*b*C1*C2*R2*R4*T
	                     -2*C2*C3*m*m*R3*R3*T-2*C1*C3*m*m*R3*R3*T+2*C2*C3*m*R3*R3*T
	                     +2*C1*C3*m*R3*R3*T+2*b*C2*C3*m*R2*R3*T+2*b*C1*C3*m*R2*R3*T
	                     +2*C1*C3*m*R1*R3*T+2*C1*C2*R1*R3*T+2*b*C1*C2*R1*R2*T
	                     -4*C1*C2*C3*m*R1*R3*R4*t+4*C1*C2*C3*R1*R3*R4*t
	                     +4*b*C1*C2*C3*R1*R2*R4*t-4*C1*C2*C3*m*m*R3*R3*R4
	                     +4*C1*C2*C3*m*R3*R3*R4+4*b*C1*C2*C3*m*R2*R3*R4
	                     -4*C1*C2*C3*m*m*R1*R3*R3+4*C1*C2*C3*m*R1*R3*R3
	                     +4*b*C1*C2*C3*m*R1*R2*R3);

	a3=R1*t*T*T*T+R5*T*T*T+R3*T*T*T+b*R2*T*T*T+2*C1*R1*R1*t*t*T*T-2*C3*R1*R4*t*T*T
	               -2*C2*R1*R4*t*T*T-2*C3*m*R1*R3*t*T*T-2*C2*R1*R3*t*T*T
	               +2*C1*R1*R3*t*T*T-2*b*C2*R1*R2*t*T*T+2*b*C1*R1*R2*t*T*T
	               -2*C1*R1*R1*t*T*T-2*C3*R4*R5*T*T-2*C2*R4*R5*T*T-2*C3*m*R3*R5*T*T
	               -2*C2*R3*R5*T*T-2*C1*R3*R5*T*T-2*b*C2*R2*R5*T*T
	               -2*b*C1*R2*R5*T*T-2*C1*R1*R5*T*T-2*C3*R3*R4*T*T-2*C2*R3*R4*T*T
	               -2*b*C3*R2*R4*T*T-2*b*C2*R2*R4*T*T+2*C3*m*m*R3*R3*T*T
	               -2*C3*m*R3*R3*T*T-2*b*C3*m*R2*R3*T*T-2*C1*R1*R3*T*T
	               -2*b*C1*R1*R2*T*T-4*C1*C3*R1*R1*R4*t*t*T-4*C1*C2*R1*R1*R4*t*t*T
	               -4*C1*C3*m*R1*R1*R3*t*t*T-4*C1*C2*R1*R1*R3*t*t*T
	               -4*b*C1*C2*R1*R1*R2*t*t*T-4*C2*C3*m*R1*R3*R4*t*T
	               +4*C2*C3*R1*R3*R4*t*T-4*C1*C3*R1*R3*R4*t*T-4*C1*C2*R1*R3*R4*t*T
	               +4*b*C2*C3*R1*R2*R4*t*T-4*b*C1*C3*R1*R2*R4*t*T
	               -4*b*C1*C2*R1*R2*R4*t*T+4*C1*C3*R1*R1*R4*t*T+4*C1*C2*R1*R1*R4*t*T
	               -4*C2*C3*m*m*R1*R3*R3*t*T+4*C1*C3*m*m*R1*R3*R3*t*T
	               +4*C2*C3*m*R1*R3*R3*t*T-4*C1*C3*m*R1*R3*R3*t*T
	               +4*b*C2*C3*m*R1*R2*R3*t*T-4*b*C1*C3*m*R1*R2*R3*t*T
	               +4*C1*C3*m*R1*R1*R3*t*T+4*C1*C2*R1*R1*R3*t*T
	               +4*b*C1*C2*R1*R1*R2*t*T-4*C2*C3*m*R3*R4*R5*T+4*C2*C3*R3*R4*R5*T
	               +4*C1*C3*R3*R4*R5*T+4*C1*C2*R3*R4*R5*T+4*b*C2*C3*R2*R4*R5*T
	               +4*b*C1*C3*R2*R4*R5*T+4*b*C1*C2*R2*R4*R5*T+4*C1*C3*R1*R4*R5*T
	               +4*C1*C2*R1*R4*R5*T-4*C2*C3*m*m*R3*R3*R5*T-4*C1*C3*m*m*R3*R3*R5*T
	               +4*C2*C3*m*R3*R3*R5*T+4*C1*C3*m*R3*R3*R5*T+4*b*C2*C3*m*R2*R3*R5*T
	               +4*b*C1*C3*m*R2*R3*R5*T+4*C1*C3*m*R1*R3*R5*T+4*C1*C2*R1*R3*R5*T
	               +4*b*C1*C2*R1*R2*R5*T-4*C2*C3*m*m*R3*R3*R4*T+4*C2*C3*m*R3*R3*R4*T
	               +4*b*C2*C3*m*R2*R3*R4*T+4*C1*C3*R1*R3*R4*T+4*C1*C2*R1*R3*R4*T
	               +4*b*C1*C3*R1*R2*R4*T+4*b*C1*C2*R1*R2*R4*T
	               -4*C1*C3*m*m*R1*R3*R3*T+4*C1*C3*m*R1*R3*R3*T
	               +4*b*C1*C3*m*R1*R2*R3*T-8*C1*C2*C3*m*R1*R1*R3*R4*t*t
	               +8*C1*C2*C3*R1*R1*R3*R4*t*t+8*b*C1*C2*C3*R1*R1*R2*R4*t*t
	               -8*C1*C2*C3*m*m*R1*R1*R3*R3*t*t+8*C1*C2*C3*m*R1*R1*R3*R3*t*t
	               +8*b*C1*C2*C3*m*R1*R1*R2*R3*t*t-8*C1*C2*C3*m*m*R1*R3*R3*R4*t
	               +8*C1*C2*C3*m*R1*R3*R3*R4*t+8*b*C1*C2*C3*m*R1*R2*R3*R4*t
	               +8*C1*C2*C3*m*R1*R1*R3*R4*t-8*C1*C2*C3*R1*R1*R3*R4*t
	               -8*b*C1*C2*C3*R1*R1*R2*R4*t+8*C1*C2*C3*m*m*R1*R1*R3*R3*t
	               -8*C1*C2*C3*m*R1*R1*R3*R3*t-8*b*C1*C2*C3*m*R1*R1*R2*R3*t
	               +8*C1*C2*C3*m*m*R3*R3*R4*R5-8*C1*C2*C3*m*R3*R3*R4*R5
	               -8*b*C1*C2*C3*m*R2*R3*R4*R5+8*C1*C2*C3*m*R1*R3*R4*R5
	               -8*C1*C2*C3*R1*R3*R4*R5-8*b*C1*C2*C3*R1*R2*R4*R5
	               +8*C1*C2*C3*m*m*R1*R3*R3*R5-8*C1*C2*C3*m*R1*R3*R3*R5
	               -8*b*C1*C2*C3*m*R1*R2*R3*R5+8*C1*C2*C3*m*m*R1*R3*R3*R4
	               -8*C1*C2*C3*m*R1*R3*R3*R4-8*b*C1*C2*C3*m*R1*R2*R3*R4;

	a2=3*R1*t*T*T*T+3*R5*T*T*T+3*R3*T*T*T+3*b*R2*T*T*T+2*C1*R1*R1*t*t*T*T
	                 -2*C3*R1*R4*t*T*T-2*C2*R1*R4*t*T*T-2*C3*m*R1*R3*t*T*T
	                 -2*C2*R1*R3*t*T*T+2*C1*R1*R3*t*T*T-2*b*C2*R1*R2*t*T*T
	                 +2*b*C1*R1*R2*t*T*T-2*C1*R1*R1*t*T*T-2*C3*R4*R5*T*T
	                 -2*C2*R4*R5*T*T-2*C3*m*R3*R5*T*T-2*C2*R3*R5*T*T
	                 -2*C1*R3*R5*T*T-2*b*C2*R2*R5*T*T-2*b*C1*R2*R5*T*T
	                 -2*C1*R1*R5*T*T-2*C3*R3*R4*T*T-2*C2*R3*R4*T*T
	                 -2*b*C3*R2*R4*T*T-2*b*C2*R2*R4*T*T+2*C3*m*m*R3*R3*T*T
	                 -2*C3*m*R3*R3*T*T-2*b*C3*m*R2*R3*T*T-2*C1*R1*R3*T*T
	                 -2*b*C1*R1*R2*T*T+4*C1*C3*R1*R1*R4*t*t*T+4*C1*C2*R1*R1*R4*t*t*T
	                 +4*C1*C3*m*R1*R1*R3*t*t*T+4*C1*C2*R1*R1*R3*t*t*T
	                 +4*b*C1*C2*R1*R1*R2*t*t*T+4*C2*C3*m*R1*R3*R4*t*T
	                 -4*C2*C3*R1*R3*R4*t*T+4*C1*C3*R1*R3*R4*t*T
	                 +4*C1*C2*R1*R3*R4*t*T-4*b*C2*C3*R1*R2*R4*t*T
	                 +4*b*C1*C3*R1*R2*R4*t*T+4*b*C1*C2*R1*R2*R4*t*T
	                 -4*C1*C3*R1*R1*R4*t*T-4*C1*C2*R1*R1*R4*t*T
	                 +4*C2*C3*m*m*R1*R3*R3*t*T-4*C1*C3*m*m*R1*R3*R3*t*T
	                 -4*C2*C3*m*R1*R3*R3*t*T+4*C1*C3*m*R1*R3*R3*t*T
	                 -4*b*C2*C3*m*R1*R2*R3*t*T+4*b*C1*C3*m*R1*R2*R3*t*T
	                 -4*C1*C3*m*R1*R1*R3*t*T-4*C1*C2*R1*R1*R3*t*T
	                 -4*b*C1*C2*R1*R1*R2*t*T+4*C2*C3*m*R3*R4*R5*T
	                 -4*C2*C3*R3*R4*R5*T-4*C1*C3*R3*R4*R5*T-4*C1*C2*R3*R4*R5*T
	                 -4*b*C2*C3*R2*R4*R5*T-4*b*C1*C3*R2*R4*R5*T
	                 -4*b*C1*C2*R2*R4*R5*T-4*C1*C3*R1*R4*R5*T-4*C1*C2*R1*R4*R5*T
	                 +4*C2*C3*m*m*R3*R3*R5*T+4*C1*C3*m*m*R3*R3*R5*T
	                 -4*C2*C3*m*R3*R3*R5*T-4*C1*C3*m*R3*R3*R5*T
	                 -4*b*C2*C3*m*R2*R3*R5*T-4*b*C1*C3*m*R2*R3*R5*T
	                 -4*C1*C3*m*R1*R3*R5*T-4*C1*C2*R1*R3*R5*T-4*b*C1*C2*R1*R2*R5*T
	                 +4*C2*C3*m*m*R3*R3*R4*T-4*C2*C3*m*R3*R3*R4*T
	                 -4*b*C2*C3*m*R2*R3*R4*T-4*C1*C3*R1*R3*R4*T-4*C1*C2*R1*R3*R4*T
	                 -4*b*C1*C3*R1*R2*R4*T-4*b*C1*C2*R1*R2*R4*T
	                 +4*C1*C3*m*m*R1*R3*R3*T-4*C1*C3*m*R1*R3*R3*T
	                 -4*b*C1*C3*m*R1*R2*R3*T+24*C1*C2*C3*m*R1*R1*R3*R4*t*t
	                 -24*C1*C2*C3*R1*R1*R3*R4*t*t-24*b*C1*C2*C3*R1*R1*R2*R4*t*t
	                 +24*C1*C2*C3*m*m*R1*R1*R3*R3*t*t-24*C1*C2*C3*m*R1*R1*R3*R3*t*t
	                 -24*b*C1*C2*C3*m*R1*R1*R2*R3*t*t+24*C1*C2*C3*m*m*R1*R3*R3*R4*t
	                 -24*C1*C2*C3*m*R1*R3*R3*R4*t-24*b*C1*C2*C3*m*R1*R2*R3*R4*t
	                 -24*C1*C2*C3*m*R1*R1*R3*R4*t+24*C1*C2*C3*R1*R1*R3*R4*t
	                 +24*b*C1*C2*C3*R1*R1*R2*R4*t-24*C1*C2*C3*m*m*R1*R1*R3*R3*t
	                 +24*C1*C2*C3*m*R1*R1*R3*R3*t+24*b*C1*C2*C3*m*R1*R1*R2*R3*t
	                 -24*C1*C2*C3*m*m*R3*R3*R4*R5+24*C1*C2*C3*m*R3*R3*R4*R5
	                 +24*b*C1*C2*C3*m*R2*R3*R4*R5-24*C1*C2*C3*m*R1*R3*R4*R5
	                 +24*C1*C2*C3*R1*R3*R4*R5+24*b*C1*C2*C3*R1*R2*R4*R5
	                 -24*C1*C2*C3*m*m*R1*R3*R3*R5+24*C1*C2*C3*m*R1*R3*R3*R5
	                 +24*b*C1*C2*C3*m*R1*R2*R3*R5-24*C1*C2*C3*m*m*R1*R3*R3*R4
	                 +24*C1*C2*C3*m*R1*R3*R3*R4+24*b*C1*C2*C3*m*R1*R2*R3*R4;

	a1=3*R1*t*T*T*T+3*R5*T*T*T+3*R3*T*T*T+3*b*R2*T*T*T-2*C1*R1*R1*t*t*T*T
	                 +2*C3*R1*R4*t*T*T+2*C2*R1*R4*t*T*T+2*C3*m*R1*R3*t*T*T
	                 +2*C2*R1*R3*t*T*T-2*C1*R1*R3*t*T*T+2*b*C2*R1*R2*t*T*T
	                 -2*b*C1*R1*R2*t*T*T+2*C1*R1*R1*t*T*T+2*C3*R4*R5*T*T
	                 +2*C2*R4*R5*T*T+2*C3*m*R3*R5*T*T+2*C2*R3*R5*T*T
	                 +2*C1*R3*R5*T*T+2*b*C2*R2*R5*T*T+2*b*C1*R2*R5*T*T
	                 +2*C1*R1*R5*T*T+2*C3*R3*R4*T*T+2*C2*R3*R4*T*T
	                 +2*b*C3*R2*R4*T*T+2*b*C2*R2*R4*T*T-2*C3*m*m*R3*R3*T*T
	                 +2*C3*m*R3*R3*T*T+2*b*C3*m*R2*R3*T*T+2*C1*R1*R3*T*T
	                 +2*b*C1*R1*R2*T*T+4*C1*C3*R1*R1*R4*t*t*T+4*C1*C2*R1*R1*R4*t*t*T
	                 +4*C1*C3*m*R1*R1*R3*t*t*T+4*C1*C2*R1*R1*R3*t*t*T
	                 +4*b*C1*C2*R1*R1*R2*t*t*T+4*C2*C3*m*R1*R3*R4*t*T
	                 -4*C2*C3*R1*R3*R4*t*T+4*C1*C3*R1*R3*R4*t*T
	                 +4*C1*C2*R1*R3*R4*t*T-4*b*C2*C3*R1*R2*R4*t*T
	                 +4*b*C1*C3*R1*R2*R4*t*T+4*b*C1*C2*R1*R2*R4*t*T
	                 -4*C1*C3*R1*R1*R4*t*T-4*C1*C2*R1*R1*R4*t*T
	                 +4*C2*C3*m*m*R1*R3*R3*t*T-4*C1*C3*m*m*R1*R3*R3*t*T
	                 -4*C2*C3*m*R1*R3*R3*t*T+4*C1*C3*m*R1*R3*R3*t*T
	                 -4*b*C2*C3*m*R1*R2*R3*t*T+4*b*C1*C3*m*R1*R2*R3*t*T
	                 -4*C1*C3*m*R1*R1*R3*t*T-4*C1*C2*R1*R1*R3*t*T
	                 -4*b*C1*C2*R1*R1*R2*t*T+4*C2*C3*m*R3*R4*R5*T
	                 -4*C2*C3*R3*R4*R5*T-4*C1*C3*R3*R4*R5*T-4*C1*C2*R3*R4*R5*T
	                 -4*b*C2*C3*R2*R4*R5*T-4*b*C1*C3*R2*R4*R5*T
	                 -4*b*C1*C2*R2*R4*R5*T-4*C1*C3*R1*R4*R5*T-4*C1*C2*R1*R4*R5*T
	                 +4*C2*C3*m*m*R3*R3*R5*T+4*C1*C3*m*m*R3*R3*R5*T
	                 -4*C2*C3*m*R3*R3*R5*T-4*C1*C3*m*R3*R3*R5*T
	                 -4*b*C2*C3*m*R2*R3*R5*T-4*b*C1*C3*m*R2*R3*R5*T
	                 -4*C1*C3*m*R1*R3*R5*T-4*C1*C2*R1*R3*R5*T-4*b*C1*C2*R1*R2*R5*T
	                 +4*C2*C3*m*m*R3*R3*R4*T-4*C2*C3*m*R3*R3*R4*T
	                 -4*b*C2*C3*m*R2*R3*R4*T-4*C1*C3*R1*R3*R4*T-4*C1*C2*R1*R3*R4*T
	                 -4*b*C1*C3*R1*R2*R4*T-4*b*C1*C2*R1*R2*R4*T
	                 +4*C1*C3*m*m*R1*R3*R3*T-4*C1*C3*m*R1*R3*R3*T
	                 -4*b*C1*C3*m*R1*R2*R3*T-24*C1*C2*C3*m*R1*R1*R3*R4*t*t
	                 +24*C1*C2*C3*R1*R1*R3*R4*t*t+24*b*C1*C2*C3*R1*R1*R2*R4*t*t
	                 -24*C1*C2*C3*m*m*R1*R1*R3*R3*t*t+24*C1*C2*C3*m*R1*R1*R3*R3*t*t
	                 +24*b*C1*C2*C3*m*R1*R1*R2*R3*t*t-24*C1*C2*C3*m*m*R1*R3*R3*R4*t
	                 +24*C1*C2*C3*m*R1*R3*R3*R4*t+24*b*C1*C2*C3*m*R1*R2*R3*R4*t
	                 +24*C1*C2*C3*m*R1*R1*R3*R4*t-24*C1*C2*C3*R1*R1*R3*R4*t
	                 -24*b*C1*C2*C3*R1*R1*R2*R4*t+24*C1*C2*C3*m*m*R1*R1*R3*R3*t
	                 -24*C1*C2*C3*m*R1*R1*R3*R3*t-24*b*C1*C2*C3*m*R1*R1*R2*R3*t
	                 +24*C1*C2*C3*m*m*R3*R3*R4*R5-24*C1*C2*C3*m*R3*R3*R4*R5
	                 -24*b*C1*C2*C3*m*R2*R3*R4*R5+24*C1*C2*C3*m*R1*R3*R4*R5
	                 -24*C1*C2*C3*R1*R3*R4*R5-24*b*C1*C2*C3*R1*R2*R4*R5
	                 +24*C1*C2*C3*m*m*R1*R3*R3*R5-24*C1*C2*C3*m*R1*R3*R3*R5
	                 -24*b*C1*C2*C3*m*R1*R2*R3*R5+24*C1*C2*C3*m*m*R1*R3*R3*R4
	                 -24*C1*C2*C3*m*R1*R3*R3*R4-24*b*C1*C2*C3*m*R1*R2*R3*R4;

	a0=R1*t*T*T*T+R5*T*T*T+R3*T*T*T+b*R2*T*T*T-2*C1*R1*R1*t*t*T*T+2*C3*R1*R4*t*T*T
	               +2*C2*R1*R4*t*T*T+2*C3*m*R1*R3*t*T*T+2*C2*R1*R3*t*T*T
	               -2*C1*R1*R3*t*T*T+2*b*C2*R1*R2*t*T*T-2*b*C1*R1*R2*t*T*T
	               +2*C1*R1*R1*t*T*T+2*C3*R4*R5*T*T+2*C2*R4*R5*T*T+2*C3*m*R3*R5*T*T
	               +2*C2*R3*R5*T*T+2*C1*R3*R5*T*T+2*b*C2*R2*R5*T*T
	               +2*b*C1*R2*R5*T*T+2*C1*R1*R5*T*T+2*C3*R3*R4*T*T+2*C2*R3*R4*T*T
	               +2*b*C3*R2*R4*T*T+2*b*C2*R2*R4*T*T-2*C3*m*m*R3*R3*T*T
	               +2*C3*m*R3*R3*T*T+2*b*C3*m*R2*R3*T*T+2*C1*R1*R3*T*T
	               +2*b*C1*R1*R2*T*T-4*C1*C3*R1*R1*R4*t*t*T-4*C1*C2*R1*R1*R4*t*t*T
	               -4*C1*C3*m*R1*R1*R3*t*t*T-4*C1*C2*R1*R1*R3*t*t*T
	               -4*b*C1*C2*R1*R1*R2*t*t*T-4*C2*C3*m*R1*R3*R4*t*T
	               +4*C2*C3*R1*R3*R4*t*T-4*C1*C3*R1*R3*R4*t*T-4*C1*C2*R1*R3*R4*t*T
	               +4*b*C2*C3*R1*R2*R4*t*T-4*b*C1*C3*R1*R2*R4*t*T
	               -4*b*C1*C2*R1*R2*R4*t*T+4*C1*C3*R1*R1*R4*t*T+4*C1*C2*R1*R1*R4*t*T
	               -4*C2*C3*m*m*R1*R3*R3*t*T+4*C1*C3*m*m*R1*R3*R3*t*T
	               +4*C2*C3*m*R1*R3*R3*t*T-4*C1*C3*m*R1*R3*R3*t*T
	               +4*b*C2*C3*m*R1*R2*R3*t*T-4*b*C1*C3*m*R1*R2*R3*t*T
	               +4*C1*C3*m*R1*R1*R3*t*T+4*C1*C2*R1*R1*R3*t*T
	               +4*b*C1*C2*R1*R1*R2*t*T-4*C2*C3*m*R3*R4*R5*T+4*C2*C3*R3*R4*R5*T
	               +4*C1*C3*R3*R4*R5*T+4*C1*C2*R3*R4*R5*T+4*b*C2*C3*R2*R4*R5*T
	               +4*b*C1*C3*R2*R4*R5*T+4*b*C1*C2*R2*R4*R5*T+4*C1*C3*R1*R4*R5*T
	               +4*C1*C2*R1*R4*R5*T-4*C2*C3*m*m*R3*R3*R5*T-4*C1*C3*m*m*R3*R3*R5*T
	               +4*C2*C3*m*R3*R3*R5*T+4*C1*C3*m*R3*R3*R5*T+4*b*C2*C3*m*R2*R3*R5*T
	               +4*b*C1*C3*m*R2*R3*R5*T+4*C1*C3*m*R1*R3*R5*T+4*C1*C2*R1*R3*R5*T
	               +4*b*C1*C2*R1*R2*R5*T-4*C2*C3*m*m*R3*R3*R4*T+4*C2*C3*m*R3*R3*R4*T
	               +4*b*C2*C3*m*R2*R3*R4*T+4*C1*C3*R1*R3*R4*T+4*C1*C2*R1*R3*R4*T
	               +4*b*C1*C3*R1*R2*R4*T+4*b*C1*C2*R1*R2*R4*T
	               -4*C1*C3*m*m*R1*R3*R3*T+4*C1*C3*m*R1*R3*R3*T
	               +4*b*C1*C3*m*R1*R2*R3*T+8*C1*C2*C3*m*R1*R1*R3*R4*t*t
	               -8*C1*C2*C3*R1*R1*R3*R4*t*t-8*b*C1*C2*C3*R1*R1*R2*R4*t*t
	               +8*C1*C2*C3*m*m*R1*R1*R3*R3*t*t-8*C1*C2*C3*m*R1*R1*R3*R3*t*t
	               -8*b*C1*C2*C3*m*R1*R1*R2*R3*t*t+8*C1*C2*C3*m*m*R1*R3*R3*R4*t
	               -8*C1*C2*C3*m*R1*R3*R3*R4*t-8*b*C1*C2*C3*m*R1*R2*R3*R4*t
	               -8*C1*C2*C3*m*R1*R1*R3*R4*t+8*C1*C2*C3*R1*R1*R3*R4*t
	               +8*b*C1*C2*C3*R1*R1*R2*R4*t-8*C1*C2*C3*m*m*R1*R1*R3*R3*t
	               +8*C1*C2*C3*m*R1*R1*R3*R3*t+8*b*C1*C2*C3*m*R1*R1*R2*R3*t
	               -8*C1*C2*C3*m*m*R3*R3*R4*R5+8*C1*C2*C3*m*R3*R3*R4*R5
	               +8*b*C1*C2*C3*m*R2*R3*R4*R5-8*C1*C2*C3*m*R1*R3*R4*R5
	               +8*C1*C2*C3*R1*R3*R4*R5+8*b*C1*C2*C3*R1*R2*R4*R5
	               -8*C1*C2*C3*m*m*R1*R3*R3*R5+8*C1*C2*C3*m*R1*R3*R3*R5
	               +8*b*C1*C2*C3*m*R1*R2*R3*R5-8*C1*C2*C3*m*m*R1*R3*R3*R4
	               +8*C1*C2*C3*m*R1*R3*R3*R4+8*b*C1*C2*C3*m*R1*R2*R3*R4;

	//normalize filter to a0 = 1

	a1 /= a0;
	a2 /= a0;
	a3 /= a0;

	b0 /= a0;
	b1 /= a0;
	b2 /= a0;
	b3 /= a0;

	a0 = 1.0;

	//output to caller

	*pa0 = a0;
	*pa1 = a1;
	*pa2 = a2;
	*pa3 = a3;

	*pb0 = b0;
	*pb1 = b1;
	*pb2 = b2;
	*pb3 = b3;
}



#endif /* TONESTACKS_H_ */
