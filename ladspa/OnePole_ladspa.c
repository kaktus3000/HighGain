/*
 * one pole filters, high pass and low pass. classical matched coefficients.
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

#include "OnePole.h"
#include "OnePole_ladspa.h"

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	//characteristic frequency
	PORT_FREQUENCY,
	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;

	LADSPA_Data * m_apPorts[PORT_COUNT];

	OnePole m_State;
} OnePole_ladspa;

void
onePoleActivate(LADSPA_Handle inst)
{
	OnePole_ladspa* pOnePole = (OnePole_ladspa*) inst;
	memset(&(pOnePole->m_State), 0, sizeof(OnePole));
}

LADSPA_Handle
onePoleInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	OnePole_ladspa* pOnePole;
	SAFE_ALLOC(pOnePole, OnePole_ladspa);
	pOnePole->m_ulSampleRate = ulSampleRate;

	onePoleActivate((LADSPA_Handle)pOnePole);

	return pOnePole;
}

void
onePolePortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	OnePole_ladspa* pOnePole = (OnePole_ladspa*) inst;
	pOnePole->m_apPorts[port] = pData;
}

//run functions for the filters
void
hiPassRun(LADSPA_Handle inst, unsigned long nSamples)
{
	OnePole_ladspa* pOnePole = (OnePole_ladspa*) inst;

	//read current settings
	const float	freq = * pOnePole->m_apPorts[PORT_FREQUENCY];

	LADSPA_Data * pIn  = pOnePole->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pOnePole->m_apPorts[PORT_OUTPUT];

	highPass(&pOnePole->m_State, pIn, pOut, nSamples, pOnePole->m_ulSampleRate, freq);
}

void
loPassRun(LADSPA_Handle inst, unsigned long nSamples)
{
	OnePole_ladspa* pOnePole = (OnePole_ladspa*) inst;

	//read current settings
	const float	freq = * pOnePole->m_apPorts[PORT_FREQUENCY];

	LADSPA_Data * pIn  = pOnePole->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pOnePole->m_apPorts[PORT_OUTPUT];

	lowPass(&pOnePole->m_State, pIn, pOut, nSamples, pOnePole->m_ulSampleRate, freq);
}

void
onePoleCleanup(LADSPA_Handle inst)
{
	OnePole_ladspa* pOnePole = (OnePole_ladspa*) inst;
	free(pOnePole);
}

//common init
void
onePoleInit(LADSPA_Descriptor * pDesc)
{
	//label, name and id set by respective inits
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
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
	pDescs[PORT_FREQUENCY] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");
	pNames[PORT_FREQUENCY] = strdup("Freq");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;


	pHints[PORT_FREQUENCY].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_LOGARITHMIC;
	pHints[PORT_FREQUENCY].LowerBound = 20;
	pHints[PORT_FREQUENCY].UpperBound = 20000;

	//set callbacks
	pDesc->instantiate = onePoleInstantiate;
	pDesc->connect_port = onePolePortConnect;
	pDesc->activate = onePoleActivate;
	pDesc->run = NULL;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = onePoleCleanup;
}

//inits for the two variants
void
hiPassInit(LADSPA_Descriptor * pDesc)
{
	onePoleInit(pDesc);
	pDesc->run = hiPassRun;

	pDesc->UniqueID = 0x00000104;
	pDesc->Label = strdup("HiPassFilter");
	pDesc->Name = strdup("HG High Pass Filter");
}

void
loPassInit(LADSPA_Descriptor * pDesc)
{
	onePoleInit(pDesc);
	pDesc->run = loPassRun;

	pDesc->UniqueID = 0x00000105;
	pDesc->Label = strdup("LoPassFilter");
	pDesc->Name = strdup("HG Low Pass Filter");
}
