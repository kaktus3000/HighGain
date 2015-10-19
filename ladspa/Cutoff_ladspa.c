/*
 * Little FIR to cut off high frequency trash above 0.2 fs
 * Attenuates about 40dB
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

#include "Cutoff.h"
#include "Cutoff_ladspa.h"

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;
	LADSPA_Data * m_apPorts[PORT_COUNT];

	Cutoff m_State;
} Cutoff_ladspa;

void
cutoffActivate(LADSPA_Handle inst)
{
	Cutoff_ladspa* pCutoff = (Cutoff_ladspa*) inst;
	memset(&(pCutoff->m_State), 0, sizeof(Cutoff));
}

LADSPA_Handle
cutoffInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	Cutoff_ladspa* pCutoff;
	SAFE_ALLOC(pCutoff, Cutoff_ladspa);
	cutoffActivate(pCutoff);

	pCutoff->m_ulSampleRate = ulSampleRate;

	return pCutoff;
}

void
cutoffPortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	Cutoff_ladspa* pCutoff = (Cutoff_ladspa*) inst;
	pCutoff->m_apPorts[port] = pData;
}

void
cutoffRun(LADSPA_Handle inst, unsigned long nSamples)
{
	Cutoff_ladspa* pCutoff = (Cutoff_ladspa*) inst;

	LADSPA_Data * pIn  = pCutoff->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pCutoff->m_apPorts[PORT_OUTPUT];

	cutOff(&(pCutoff->m_State), pIn, pOut, nSamples, (uint) pCutoff->m_ulSampleRate);
}

void
cutoffCleanup(LADSPA_Handle inst)
{
	Cutoff_ladspa* pCutoff = (Cutoff_ladspa*) inst;
	free(pCutoff);
}

void
cutoffInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0x00C07C07;
	pDesc->Label = strdup("Cutoff");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG Cutoff");
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

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;

	//set callbacks
	pDesc->instantiate = cutoffInstantiate;
	pDesc->connect_port = cutoffPortConnect;
	pDesc->activate = cutoffActivate;
	pDesc->run = cutoffRun;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = cutoffCleanup;
}

