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

#include "AmpI.h"
#include "AmpI_ladspa.h"

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	//preamp gain
	PORT_PRE,
	//distortion gain
	PORT_DIST,
	//setting of bass pot
	PORT_BASS,
	//setting of mid pot
	PORT_MID,
	//setting of treble pot
	PORT_TREBLE,
	//gain
	PORT_VOL,
	//dist footswitch
	PORT_DIST_FSW,
	//lead footswitch
	PORT_LEAD_FSW,

	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;
	LADSPA_Data * m_apPorts[PORT_COUNT];

	AmpI m_State;
} AmpI_ladspa;

void
ampIActivate(LADSPA_Handle inst)
{
	AmpI_ladspa* pAmpI = (AmpI_ladspa*) inst;

	//clear data, reset positions
	memset(&pAmpI->m_State, 0, sizeof(AmpI));
}

LADSPA_Handle
ampIInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	AmpI_ladspa* pAmpI;
	SAFE_ALLOC(pAmpI, AmpI_ladspa);
	ampIActivate(pAmpI);

	pAmpI->m_ulSampleRate = ulSampleRate;

	return pAmpI;
}

void
ampIPortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	AmpI_ladspa* pAmpI = (AmpI_ladspa*) inst;
	pAmpI->m_apPorts[port] = pData;
}

void
ampIRun(LADSPA_Handle inst, unsigned long nSamples)
{
	AmpI_ladspa* pAmpI = (AmpI_ladspa*) inst;

	//read current settings
	const float	pre = * pAmpI->m_apPorts[PORT_PRE];
	const float	dist = * pAmpI->m_apPorts[PORT_DIST];
	const float	b = * pAmpI->m_apPorts[PORT_BASS];
	const float	m = * pAmpI->m_apPorts[PORT_MID];
	const float	t = * pAmpI->m_apPorts[PORT_TREBLE];
	const float	vol = * pAmpI->m_apPorts[PORT_VOL];

	const uint bDist = * pAmpI->m_apPorts[PORT_DIST_FSW] > 0.0f ? 1 : 0;
	const uint bLead = * pAmpI->m_apPorts[PORT_LEAD_FSW] > 0.0f ? 1 : 0;

	LADSPA_Data * pIn  = pAmpI->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pAmpI->m_apPorts[PORT_OUTPUT];


	ampI(&pAmpI->m_State, pIn, pOut, nSamples, pAmpI->m_ulSampleRate, pre, dist, bDist, bLead, b, m, t, vol);
}

void
ampICleanup(LADSPA_Handle inst)
{
	AmpI_ladspa* pAmpI = (AmpI_ladspa*) inst;
	free(pAmpI);
}

void
ampIInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0x00000100;
	pDesc->Label = strdup("AmpI");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG AmpI");
	pDesc->Maker = strdup("Benedikt Hofmeister");
	pDesc->Copyright = strdup("GPLv3");

	//count modules and ports
	pDesc->PortCount = PORT_COUNT;

	LADSPA_PortDescriptor* pDescs =
		(LADSPA_PortDescriptor *)calloc(pDesc->PortCount, sizeof(LADSPA_PortDescriptor));
	pDesc->PortDescriptors = pDescs;

	char ** pNames =
		(char **)calloc(pDesc->PortCount, sizeof(char *));
	pDesc->PortNames = (const char * const *) pNames;

	LADSPA_PortRangeHint * pHints =
		(LADSPA_PortRangeHint *) calloc(pDesc->PortCount, sizeof(LADSPA_PortRangeHint));
	pDesc->PortRangeHints =pHints;

	//define in and out
	pDescs[PORT_INPUT] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
	pDescs[PORT_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;

	pDescs[PORT_PRE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_DIST] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_BASS] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_MID] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_TREBLE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_VOL] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pDescs[PORT_DIST_FSW] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_LEAD_FSW] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");

	pNames[PORT_PRE] = strdup("Pre");
	pNames[PORT_DIST] = strdup("Dist");
	pNames[PORT_BASS] = strdup("Bass");
	pNames[PORT_MID] = strdup("Mid");
	pNames[PORT_TREBLE] = strdup("Treble");
	pNames[PORT_VOL] = strdup("Vol");

	pNames[PORT_DIST_FSW] = strdup("Dist Fsw");
	pNames[PORT_LEAD_FSW] = strdup("Lead Fsw");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;


	pHints[PORT_PRE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_PRE].LowerBound = 0;
	pHints[PORT_PRE].UpperBound = 1;

	pHints[PORT_DIST].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_DIST].LowerBound = 0;
	pHints[PORT_DIST].UpperBound = 1;

	pHints[PORT_BASS].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_BASS].LowerBound = 0;
	pHints[PORT_BASS].UpperBound = 1;

	pHints[PORT_MID].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_MID].LowerBound = 0;
	pHints[PORT_MID].UpperBound = 1;

	pHints[PORT_TREBLE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_TREBLE].LowerBound = 0;
	pHints[PORT_TREBLE].UpperBound = 1;

	pHints[PORT_VOL].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_VOL].LowerBound = 0;
	pHints[PORT_VOL].UpperBound = 1;


	pHints[PORT_DIST_FSW].HintDescriptor = LADSPA_HINT_TOGGLED | LADSPA_HINT_DEFAULT_0;
	pHints[PORT_LEAD_FSW].HintDescriptor = LADSPA_HINT_TOGGLED | LADSPA_HINT_DEFAULT_0;

	//set callbacks
	pDesc->instantiate = ampIInstantiate;
	pDesc->connect_port = ampIPortConnect;
	pDesc->activate = ampIActivate;
	pDesc->run = ampIRun;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = ampICleanup;
}

