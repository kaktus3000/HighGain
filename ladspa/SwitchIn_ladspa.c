/*
 * Simple switch to selectively route one of two inputs to a single output
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

#include "Switch.h"
#include "Switch_ladspa.h"

//port layout if EXPOSE_PARAMETERS is not defined
enum
{
	PORT_INPUT1 = 0,
	PORT_INPUT2,
	PORT_OUTPUT,
	PORT_CHANNEL_SELECT,
	PORT_COUNT
};

//crossover point 1ms @ 48kHz
#define DECAY_BASE 0.993f

#define INSW_NUM_CHANNELS 2

typedef struct
{
	unsigned long m_ulSampleRate;

	LADSPA_Data * m_pfInput0;
	LADSPA_Data * m_pfInput1;
	LADSPA_Data * m_pfOutput;

	LADSPA_Data m_fAmplitude;

	LADSPA_Data * m_pfChannelSelect;

	Switch m_State;
} Switch_ladspa;

void
activateSwitchIn(LADSPA_Handle inst)
{
	Switch_ladspa* pSwitch = (Switch_ladspa*) inst;
	pSwitch->m_fAmplitude = 0;
}

LADSPA_Handle
instantiateSwitchIn(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	Switch_ladspa* pSwitch;
	SAFE_ALLOC(pSwitch, Switch_ladspa);

	pSwitch->m_ulSampleRate = ulSampleRate;

	activateSwitchIn((LADSPA_Handle)pSwitch);

	return pSwitch;
}

void
connectPortToSwitchIn(LADSPA_Handle inst,
		unsigned long port,
		LADSPA_Data * pDataLocation
		)
{
	Switch_ladspa* pSwitch = (Switch_ladspa*) inst;

	switch (port)
	{
	case PORT_INPUT1:
		pSwitch->m_pfInput0 = pDataLocation;
		break;
	case PORT_INPUT2:
		pSwitch->m_pfInput1 = pDataLocation;
		break;
	case PORT_OUTPUT:
		pSwitch->m_pfOutput = pDataLocation;
		break;
	case PORT_CHANNEL_SELECT:
		pSwitch->m_pfChannelSelect = pDataLocation;
		break;

	default:
		break;
	}
}

void
runSwitchIn(LADSPA_Handle inst, unsigned long nSamples)
{
	Switch_ladspa* pSwitch = (Switch_ladspa*) inst;

	int uiSelected = (int)rintf(*pSwitch->m_pfChannelSelect);

	//clamp result
	uiSelected = CLAMP(uiSelected, 0, 1);

	float* pOutput = pSwitch->m_pfOutput;

	float* pInput0 = pSwitch->m_pfInput0;
	float* pInput1 = pSwitch->m_pfInput1;

	switchIn(&pSwitch->m_State, pInput0, pInput1, pOutput, nSamples, pSwitch->m_ulSampleRate, uiSelected);
}

void
cleanupSwitchIn(LADSPA_Handle inst)
{
	Switch_ladspa* pSwitch = (Switch_ladspa*) inst;
	free(pSwitch);
}

void
switchInInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0x5F17C8;
	pDesc->Label = strdup("Switch");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG Inbound Switch");
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
	pDescs[PORT_INPUT1] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
	pDescs[PORT_INPUT2] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
	pDescs[PORT_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
	pDescs[PORT_CHANNEL_SELECT] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pNames[PORT_INPUT1] = strdup("Input1");
	pNames[PORT_INPUT2] = strdup("Input2");
	pNames[PORT_OUTPUT] = strdup("Output");
	pNames[PORT_CHANNEL_SELECT] = strdup("Channel");

	pHints[PORT_INPUT1].HintDescriptor = 0;
	pHints[PORT_INPUT2].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;

	pHints[PORT_CHANNEL_SELECT].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_INTEGER;
	pHints[PORT_CHANNEL_SELECT].LowerBound = 0;
	pHints[PORT_CHANNEL_SELECT].UpperBound = 1;


	//set callbacks
	pDesc->instantiate = instantiateSwitchIn;
	pDesc->connect_port = connectPortToSwitchIn;
	pDesc->activate = activateSwitchIn;
	pDesc->run = runSwitchIn;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = cleanupSwitchIn;
}


