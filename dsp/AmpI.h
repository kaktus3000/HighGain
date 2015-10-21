/*
 * Solid state amplifier simulation
 *
 * features:
 * - clean/dist channel
 * - lead channel
 * - two gain controls
 * - full tonestack
 * - physically based voltage levels
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

#ifndef AMP_I_H_
#define AMP_I_H_

#include "HighGain.h"

#include "OnePole.h"
#include "Saturation.h"
#include "Switch.h"
#include "BassBoost.h"
#include "ToneStack.h"
#include "Cutoff.h"

typedef struct{
	//input filter
	OnePole m_HpInput;
	//1st stage decoupling
	OnePole m_Hp1stDecoup;
	//1st stage cutoff
	OnePole m_Lp1stCutoff;
	//2nd stage decoupling
	OnePole m_Hp2ndDecoup;
	//2nd stage cutoff
	OnePole m_Lp2ndCutoff;
	//bass boost after 3rd stage
	BassBoost m_BassBoost;
	//switch clean/dist
	Switch m_Switch;
	//master tone stack
	ToneStack m_ToneStack;
	//master cutoff
	Cutoff m_Cutoff;
}AmpI;

void
ampI(AmpI* pAmp, float* pIn, float* pOut, const uint nSamples, const uint uiSampleRate,
		float pre, float dist, uint bDist, uint bLead,
		float b, float m, float t, float vol);

#endif /* AMP_I_H_ */
