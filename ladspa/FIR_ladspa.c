/*
 * vectorized FIR filter. Best used for cabinet+microphone emulation
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

#include "FIR.h"
#include "FIR_ladspa.h"


enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	//just linear gain
	PORT_GAIN,
	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;
	LADSPA_Data * m_apPorts[PORT_COUNT];

	FIR m_State;
} FIR_ladspa;

void
firActivate(LADSPA_Handle inst)
{
	FIR_ladspa* pFIR = (FIR_ladspa*) inst;

	memset(&(pFIR->m_State.m_afBuffer), 0, FIR_SAMPLES_8 * sizeof(v8f_t));
	pFIR->m_State.m_uiBufferPos = 0;
}

LADSPA_Handle
firInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	FIR_ladspa* pFIR;

	SAFE_ALLOC(pFIR, FIR_ladspa);
	initializeFIR(&(pFIR->m_State));

	firActivate(pFIR);

	pFIR->m_ulSampleRate = ulSampleRate;

	return pFIR;
}

void
firPortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	FIR_ladspa* pFIR = (FIR_ladspa*) inst;
	pFIR->m_apPorts[port] = pData;
}

void
firRun(LADSPA_Handle inst, unsigned long nSamples)
{
	FIR_ladspa* pFIR = (FIR_ladspa*) inst;

	//read gain port
	const float	gain = * pFIR->m_apPorts[PORT_GAIN];

	LADSPA_Data * pIn  = pFIR->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pFIR->m_apPorts[PORT_OUTPUT];

	fir(&pFIR->m_State, pIn, pOut, nSamples, pFIR->m_ulSampleRate, 0, gain);
}

void
firCleanup(LADSPA_Handle inst)
{
	FIR* pFIR = (FIR*) inst;
	free(pFIR);
}

void
firInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0x00000103;
	pDesc->Label = strdup("FIR");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG FIR");
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

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");
	pNames[PORT_GAIN] = strdup("Gain");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;

	pHints[PORT_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1;
	pHints[PORT_GAIN].LowerBound = 0;
	pHints[PORT_GAIN].UpperBound = 5;

	//set callbacks
	pDesc->instantiate = firInstantiate;
	pDesc->connect_port = firPortConnect;
	pDesc->activate = firActivate;
	pDesc->run = firRun;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = firCleanup;
}

