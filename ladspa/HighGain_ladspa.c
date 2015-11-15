/*
 * define .so entry and exit points
 * load plugins
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

#include <string.h>
#include <stdlib.h>

#include "OnePole_ladspa.h"
#include "Switch_ladspa.h"
#include "Saturation_ladspa.h"
#include "FIR_ladspa.h"
#include "ToneStack_ladspa.h"
#include "Cutoff_ladspa.h"
#include "BassBoost_ladspa.h"
#include "AmpI_ladspa.h"

enum EPlugs
{
	AmpI = 0,
	FIR,
#ifdef HIGHGAIN_ALL_PLUGS
	InSwitch,
	HiPass,
	LoPass,
	Saturator,
	ToneStack,
	Cutoff,
	BassBoost,
#endif
	NumPlugs
};

LADSPA_Descriptor g_aPlugInDescs[NumPlugs];

void
_init()
{
	memset(g_aPlugInDescs, 0, sizeof(LADSPA_Descriptor) * NumPlugs);

	ampIInit(g_aPlugInDescs + AmpI);
	firInit(g_aPlugInDescs + FIR);
#ifdef HIGHGAIN_ALL_PLUGS
	switchInInit(g_aPlugInDescs + InSwitch);
	hiPassInit(g_aPlugInDescs + HiPass);
	loPassInit(g_aPlugInDescs + LoPass);
	saturationInit(g_aPlugInDescs + Saturator);
	toneStackInit(g_aPlugInDescs + ToneStack);
	cutoffInit(g_aPlugInDescs + Cutoff);
	bassBoostInit(g_aPlugInDescs + BassBoost);

#endif
}

const LADSPA_Descriptor *
ladspa_descriptor(unsigned long i)
{
	if (i >= NumPlugs)
		return NULL;

	return g_aPlugInDescs + i;
}

void
_fini()
{
	//free descriptor and strings
	uint iPlug = 0;
	for(; iPlug < NumPlugs; iPlug++)
	{
		LADSPA_Descriptor * psDescriptor = g_aPlugInDescs + iPlug;
		free((char*)psDescriptor->Label);
		free((char*)psDescriptor->Name);
		free((char*)psDescriptor->Maker);
		free((char*)psDescriptor->Copyright);
		free((char*)psDescriptor->PortDescriptors);

		unsigned int iIndex = 0;
		for (; iIndex < psDescriptor->PortCount; iIndex++)
			free((char*)psDescriptor->PortNames[iIndex]);

		free((char**)psDescriptor->PortNames);
		free((char*)psDescriptor->PortRangeHints);
	}
}
