/*
 * Diode Saturator Simulation
 *
 * the integer parameters are meant in a "set-once" way, since they represent the physical circuit
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

#include "Saturation.h"
#include "Saturation_ladspa.h"

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	PORT_INPUT_GAIN,
	PORT_OUTPUT_GAIN,
	PORT_GAIN,
	PORT_DIODE_TYPE,
	PORT_CIRCUIT_TYPE,
	PORT_POT_TYPE,
	PORT_POT_VAL,
	PORT_SERIES_VAL,
	PORT_GROUND_VAL,
	PORT_COUNT
};

typedef enum
{
	DIODE_SCHOTTKY = 0,
	DIODE_JUNCTION,

	DIODE_ZENER_2V4_SCHOTTKY,
	DIODE_ZENER_3V0_SCHOTTKY,
	DIODE_ZENER_3V9_SCHOTTKY,

	DIODE_ZENER_2V4_JUNCTION,
	DIODE_ZENER_3V0_JUNCTION,
	DIODE_ZENER_3V9_JUNCTION,

	DIODE_LED,

	DIODE_NUM
}EDiodeCombinations;

typedef struct
{
	unsigned long m_ulSampleRate;

	LADSPA_Data * m_apPorts[PORT_COUNT];
} Saturator;

void
saturationActivate(LADSPA_Handle inst){}

LADSPA_Handle
saturationInstantiate(const LADSPA_Descriptor* desc, unsigned long ulSampleRate)
{
	Saturator* pSaturator;

	SAFE_ALLOC(pSaturator, Saturator);

	memset(pSaturator, 0xff, sizeof(Saturator));

	pSaturator->m_ulSampleRate = ulSampleRate;

	return pSaturator;
}

void
saturationPortConnect(LADSPA_Handle inst, unsigned long port, LADSPA_Data * pData)
{
	Saturator* pSaturator = (Saturator*) inst;

	pSaturator->m_apPorts[port] = pData;
}

#include "stdio.h"

void
saturationRun(LADSPA_Handle inst, unsigned long nSamples)
{
	Saturator* pSaturator = (Saturator*) inst;

	const float	gain	= * pSaturator->m_apPorts[PORT_GAIN],
				input	= * pSaturator->m_apPorts[PORT_INPUT_GAIN],
				output	= * pSaturator->m_apPorts[PORT_OUTPUT_GAIN];

	const unsigned int	diode	= CLAMP((unsigned int)* pSaturator->m_apPorts[PORT_DIODE_TYPE], 1, DIODE_NUM),
						circuit	= CLAMP((unsigned int)* pSaturator->m_apPorts[PORT_CIRCUIT_TYPE], 1, SAT_NUM),
						pot		= CLAMP((unsigned int)* pSaturator->m_apPorts[PORT_POT_TYPE], 1, POT_NUM);

	const float	potVal = * pSaturator->m_apPorts[PORT_POT_VAL],
				series = * pSaturator->m_apPorts[PORT_SERIES_VAL],
				ground = * pSaturator->m_apPorts[PORT_GROUND_VAL];

	EDiodeType aDiodes[2];
	unsigned int nDiodes = 1;

	switch(diode -1)
	{
	case DIODE_SCHOTTKY:
		*aDiodes = DIO_SCHOTTKY;
		break;
	case DIODE_JUNCTION:
		*aDiodes = DIO_JUNCTION;
		break;
	case DIODE_ZENER_2V4_SCHOTTKY:
		aDiodes[0] = DIO_ZENER_2V4;
		aDiodes[1] = DIO_SCHOTTKY;
		nDiodes = 2;
		break;
	case DIODE_ZENER_3V0_SCHOTTKY:
		aDiodes[0] = DIO_ZENER_3V0;
		aDiodes[1] = DIO_SCHOTTKY;
		nDiodes = 2;
		break;
	case DIODE_ZENER_3V9_SCHOTTKY:
		aDiodes[0] = DIO_ZENER_3V9;
		aDiodes[1] = DIO_SCHOTTKY;
		nDiodes = 2;
		break;
	case DIODE_ZENER_2V4_JUNCTION:
		aDiodes[0] = DIO_ZENER_2V4;
		aDiodes[1] = DIO_JUNCTION;
		nDiodes = 2;
		break;
	case DIODE_ZENER_3V0_JUNCTION:
		aDiodes[0] = DIO_ZENER_3V0;
		aDiodes[1] = DIO_JUNCTION;
		nDiodes = 2;
		break;
	case DIODE_ZENER_3V9_JUNCTION:
		aDiodes[0] = DIO_ZENER_3V9;
		aDiodes[1] = DIO_JUNCTION;
		nDiodes = 2;
		break;
	case DIODE_LED:
		*aDiodes = DIO_LED;
		break;
	}

	LADSPA_Data * pIn = pSaturator->m_apPorts[PORT_INPUT];
	LADSPA_Data * pOut = pSaturator->m_apPorts[PORT_OUTPUT];

	LADSPA_Data afBuffer1[nSamples], afBuffer2[nSamples];

	uint i = 0;
	for(; i < nSamples; i++)
		afBuffer1[i] = fabsf(pIn[i]) * input;

//	puts("processing");

	saturation(afBuffer1, afBuffer2, (uint)nSamples,
				circuit - 1, nDiodes, aDiodes, pot - 1,
				ground, potVal, series,
				gain);

//	puts("finished");

	i = 0;
	for(; i < nSamples; i++)
		//saturation calculated using absolute voltages
		//copy sign of input and saturation curve to output
		pOut[i] = CLAMP(copysignf(afBuffer2[i] * output, pIn[i] * afBuffer1[i]), -1.0f, 1.0f);
}

void
saturationCleanup(LADSPA_Handle inst)
{
	Saturator* pSaturator = (Saturator*) inst;
	free(pSaturator);
}

void
saturationInit(LADSPA_Descriptor * pDesc)
{
	pDesc->UniqueID = 0x00000106;
	pDesc->Label = strdup("Saturator");
	pDesc->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	pDesc->Name = strdup("HG Saturator");;
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
	pDescs[PORT_INPUT_GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_OUTPUT_GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_DIODE_TYPE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_CIRCUIT_TYPE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_POT_TYPE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_POT_VAL] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_SERIES_VAL] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	pDescs[PORT_GROUND_VAL] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

	pNames[PORT_INPUT] = strdup("Input");
	pNames[PORT_OUTPUT] = strdup("Output");
	pNames[PORT_INPUT_GAIN] = strdup("In Gain");
	pNames[PORT_OUTPUT_GAIN] = strdup("Out Gain");
	pNames[PORT_GAIN] = strdup("Gain");
	pNames[PORT_DIODE_TYPE] = strdup("Diode");
	pNames[PORT_CIRCUIT_TYPE] = strdup("Circuit");
	pNames[PORT_POT_TYPE] = strdup("Pot Type");
	pNames[PORT_POT_VAL] = strdup("Pot Val");
	pNames[PORT_SERIES_VAL] = strdup("Series Val");
	pNames[PORT_GROUND_VAL] = strdup("Ground Val");

	pHints[PORT_INPUT].HintDescriptor = 0;
	pHints[PORT_OUTPUT].HintDescriptor = 0;

	pHints[PORT_INPUT_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_INPUT_GAIN].LowerBound = 0;
	pHints[PORT_INPUT_GAIN].UpperBound = SATURATION_MAX_VOLTAGE;

	pHints[PORT_OUTPUT_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_OUTPUT_GAIN].LowerBound = 0;
	pHints[PORT_OUTPUT_GAIN].UpperBound = 1;

	pHints[PORT_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
	pHints[PORT_GAIN].LowerBound = 0;
	pHints[PORT_GAIN].UpperBound = 1;

	pHints[PORT_DIODE_TYPE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_INTEGER;
	pHints[PORT_DIODE_TYPE].LowerBound = 1;
	pHints[PORT_DIODE_TYPE].UpperBound = DIODE_NUM;

	pHints[PORT_CIRCUIT_TYPE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_INTEGER;
	pHints[PORT_CIRCUIT_TYPE].LowerBound = 1;
	pHints[PORT_CIRCUIT_TYPE].UpperBound = SAT_NUM;

	pHints[PORT_POT_TYPE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_INTEGER;
	pHints[PORT_POT_TYPE].LowerBound = 1;
	pHints[PORT_POT_TYPE].UpperBound = POT_NUM;

	pHints[PORT_POT_VAL].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_LOGARITHMIC;
	pHints[PORT_POT_VAL].LowerBound = 100;
	pHints[PORT_POT_VAL].UpperBound = 500e3;

	pHints[PORT_SERIES_VAL].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_LOGARITHMIC;
	pHints[PORT_SERIES_VAL].LowerBound = 10;
	pHints[PORT_SERIES_VAL].UpperBound = 2200;

	pHints[PORT_GROUND_VAL].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_LOGARITHMIC;
	pHints[PORT_GROUND_VAL].LowerBound = 10;
	pHints[PORT_GROUND_VAL].UpperBound = 2200;



	//set callbacks
	pDesc->instantiate = saturationInstantiate;
	pDesc->connect_port = saturationPortConnect;
	pDesc->activate = saturationActivate;
	pDesc->run = saturationRun;
	pDesc->run_adding = NULL;
	pDesc->set_run_adding_gain = NULL;
	pDesc->deactivate = NULL;
	pDesc->cleanup = saturationCleanup;
}

