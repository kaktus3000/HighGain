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
#include "CabI_lv2.h"

typedef enum {
        AMP_GAIN   = 0,
        AMP_INPUT  = 1,
        AMP_OUTPUT = 2
} PortIndex;

enum
{
	PORT_INPUT = 0,
	PORT_OUTPUT,
	//model select
	PORT_MODEL,
	//just linear gain
	PORT_GAIN,
	PORT_COUNT
};

typedef struct
{
	unsigned long m_ulSampleRate;
	float * m_apPorts[PORT_COUNT];

	FIR m_State;
} CabI_lv2;


LV2_Handle
cabIInstantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
	CabI_lv2* cab = (CabI_lv2*)malloc(sizeof(CabI_lv2));

	cab->m_ulSampleRate = (int)rate;

	return (LV2_Handle)cab;
}

void
cabIConnect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	CabI_lv2* cab = (CabI_lv2*)instance;

	cab->m_apPorts[port] = data;
}

void
cabIActivate(LV2_Handle instance)
{
	CabI_lv2* cab = (CabI_lv2*)instance;
	//clear data, reset positions
	memset(&cab->m_State, 0, sizeof(FIR));
}

void
cabIRun(LV2_Handle instance, uint32_t nSamples)
{
	const CabI_lv2* pCabI = (const CabI_lv2*)instance;

	float * pIn  = pCabI->m_apPorts[PORT_INPUT];
	float * pOut = pCabI->m_apPorts[PORT_OUTPUT];

	//read gain port
	const float	gain = * pCabI->m_apPorts[PORT_GAIN];

	const unsigned int model = * pCabI->m_apPorts[PORT_MODEL];


	fir(&pCabI->m_State, pIn, pOut, nSamples, pCabI->m_ulSampleRate, 0, gain);
}

void
cabIDeactivate(LV2_Handle instance)
{
}

void
cabICleanup(LV2_Handle instance)
{
	free(instance);
}

const void*
cabIExtension_data(const char* uri)
{
	return NULL;
}






