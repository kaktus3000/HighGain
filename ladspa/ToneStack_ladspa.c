/*
 * simulate several configurations of Fender-style tonestacks
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

#include "ToneStack.h"
#include "ToneStack_ladspa.h"

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	//just some linear gain
	PORT_GAIN,
	//setting of bass pot
	PORT_BASS,
	//setting of mid pot
	PORT_MID,
	//setting of treble pot
	PORT_TREBLE,
	//index of models availiable
	PORT_TYPE,
	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;
	LADSPA_Data * m_apPorts[PORT_COUNT];

	ToneStack m_State;
} ToneStack_ladspa;

void
toneStackActivate(LADSPA_Handle inst)
{
	ToneStack_ladspa* pToneStack = (ToneStack_ladspa*) inst;

	//clear data, reset positions
	memset(&pToneStack->m_State, 0, sizeof(ToneStack));
}

LADSPA_Handle
toneStackInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	ToneStack_ladspa* pToneStack;
	SAFE_ALLOC(pToneStack, ToneStack_ladspa);
	toneStackActivate(pToneStack);

	pToneStack->m_ulSampleRate = ulSampleRate;

	return pToneStack;
}

void
toneStackPortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	ToneStack_ladspa* pToneStack = (ToneStack_ladspa*) inst;
	pToneStack->m_apPorts[port] = pData;
}

void
toneStackRun(LADSPA_Handle inst, unsigned long nSamples)
{
	ToneStack_ladspa* pToneStack = (ToneStack_ladspa*) inst;

	//read current settings
	const float	gain = * pToneStack->m_apPorts[PORT_GAIN];
	const float	b = * pToneStack->m_apPorts[PORT_BASS];
	const float	m = * pToneStack->m_apPorts[PORT_MID];
	const float	t = * pToneStack->m_apPorts[PORT_TREBLE];

	const uint uiType = (uint)(* pToneStack->m_apPorts[PORT_TYPE]) - 1;

	LADSPA_Data * pIn  = pToneStack->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pToneStack->m_apPorts[PORT_OUTPUT];


	toneStack(&pToneStack->m_State, pIn, pOut, nSamples, pToneStack->m_ulSampleRate, gain, b, m, t, uiType);
}

void
toneStackCleanup(LADSPA_Handle inst)
{
	ToneStack_ladspa* pToneStack = (ToneStack_ladspa*) inst;
	free(pToneStack);
}

void
toneStackInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0x00000108;
	pDesc->Label = strdup("ToneStack");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG ToneStack");
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
	pDescs[PORT_GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_BASS] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_MID] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_TREBLE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_TYPE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");
	pNames[PORT_GAIN] = strdup("Gain");
	pNames[PORT_BASS] = strdup("Bass");
	pNames[PORT_MID] = strdup("Mid");
	pNames[PORT_TREBLE] = strdup("Treble");
	pNames[PORT_TYPE] = strdup("Type");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;

	pHints[PORT_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1;
	pHints[PORT_GAIN].LowerBound = 0;
	pHints[PORT_GAIN].UpperBound = 10;

	pHints[PORT_BASS].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_BASS].LowerBound = 0;
	pHints[PORT_BASS].UpperBound = 1;

	pHints[PORT_MID].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_MID].LowerBound = 0;
	pHints[PORT_MID].UpperBound = 1;

	pHints[PORT_TREBLE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_TREBLE].LowerBound = 0;
	pHints[PORT_TREBLE].UpperBound = 1;

	pHints[PORT_TYPE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_INTEGER;
	pHints[PORT_TYPE].LowerBound = 1;
	pHints[PORT_TYPE].UpperBound = getNumToneStacks();

	//set callbacks
	pDesc->instantiate = toneStackInstantiate;
	pDesc->connect_port = toneStackPortConnect;
	pDesc->activate = toneStackActivate;
	pDesc->run = toneStackRun;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = toneStackCleanup;
}

