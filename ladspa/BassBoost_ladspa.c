/*
 * Low Shelving bass booster, inverting op amp R3 parallel to C, R1 and R2 in series
 * (first-order filter)
 *
 * low shelf gain is positive (in dB)
 * high frequency gain is unity
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

#include "BassBoost_ladspa.h"
#include "BassBoost.h"

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	//gain of low shelf
	PORT_GAIN,
	//transition frequency
	PORT_FREQ,
	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;
	LADSPA_Data * m_apPorts[PORT_COUNT];

	BassBoost state;
} BassBoost_ladspa;

void
bassBoostActivate(LADSPA_Handle inst)
{
	BassBoost_ladspa* pBassBoost = (BassBoost_ladspa*) inst;

	//clear data, reset positions
	memset(&pBassBoost->state, 0, sizeof(BassBoost));
}

LADSPA_Handle
bassBoostInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	BassBoost_ladspa* pBassBoost;
	SAFE_ALLOC(pBassBoost, BassBoost_ladspa);
	bassBoostActivate(pBassBoost);

	pBassBoost->m_ulSampleRate = ulSampleRate;

	return pBassBoost;
}

void
bassBoostPortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	BassBoost_ladspa* pBassBoost = (BassBoost_ladspa*) inst;
	pBassBoost->m_apPorts[port] = pData;
}

void
bassBoostRun(LADSPA_Handle inst, unsigned long nSamples)
{
	BassBoost_ladspa* pBassBoost = (BassBoost_ladspa*) inst;

	//read current settings
	const float	gain = * pBassBoost->m_apPorts[PORT_GAIN];
	const float	freq = * pBassBoost->m_apPorts[PORT_FREQ];

	LADSPA_Data * pIn  = pBassBoost->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pBassBoost->m_apPorts[PORT_OUTPUT];


	bassBoost(&pBassBoost->state, pIn, pOut, (uint)nSamples, (uint)pBassBoost->m_ulSampleRate, gain, freq);
}

void
bassBoostCleanup(LADSPA_Handle inst)
{
	BassBoost_ladspa* pBassBoost = (BassBoost_ladspa*) inst;
	free(pBassBoost);
}

void
bassBoostInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0xBA55BA55;
	pDesc->Label = strdup("BassBoost_ladspa");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG BassBoost_ladspa");
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
	pDescs[PORT_FREQ] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");
	pNames[PORT_GAIN] = strdup("Gain");
	pNames[PORT_FREQ] = strdup("Freq");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;

	pHints[PORT_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_GAIN].LowerBound = 1;
	pHints[PORT_GAIN].UpperBound = 10;

	pHints[PORT_FREQ].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_LOGARITHMIC;
	pHints[PORT_FREQ].LowerBound = 50;
	pHints[PORT_FREQ].UpperBound = 1000;

	//set callbacks
	pDesc->instantiate = bassBoostInstantiate;
	pDesc->connect_port = bassBoostPortConnect;
	pDesc->activate = bassBoostActivate;
	pDesc->run = bassBoostRun;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = bassBoostCleanup;
}

