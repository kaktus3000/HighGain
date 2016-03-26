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
#include "AmpI_lv2.h"

typedef enum {
        AMP_GAIN   = 0,
        AMP_INPUT  = 1,
        AMP_OUTPUT = 2
} PortIndex;


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
	float * m_apPorts[PORT_COUNT];

	AmpI m_State;
} AmpI_lv2;


LV2_Handle
ampIInstantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
	AmpI_lv2* amp = (AmpI_lv2*)malloc(sizeof(AmpI_lv2));

	amp->m_ulSampleRate = (int)rate;

	return (LV2_Handle)amp;
}

void
ampIConnect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	AmpI_lv2* amp = (AmpI_lv2*)instance;

	amp->m_apPorts[port] = data;
}

void
ampIActivate(LV2_Handle instance)
{
	AmpI_lv2* amp = (AmpI_lv2*)instance;
	//clear data, reset positions
	memset(&amp->m_State, 0, sizeof(AmpI));
}

void
ampIRun(LV2_Handle instance, uint32_t nSamples)
{
	const AmpI_lv2* pAmpI = (const AmpI_lv2*)instance;

	//read current settings
	const float	pre = * pAmpI->m_apPorts[PORT_PRE];
	const float	dist = * pAmpI->m_apPorts[PORT_DIST];
	const float	b = * pAmpI->m_apPorts[PORT_BASS];
	const float	m = * pAmpI->m_apPorts[PORT_MID];
	const float	t = * pAmpI->m_apPorts[PORT_TREBLE];
	const float	vol = * pAmpI->m_apPorts[PORT_VOL];

	const uint bDist = * pAmpI->m_apPorts[PORT_DIST_FSW] > 0.0f ? 1 : 0;
	const uint bLead = * pAmpI->m_apPorts[PORT_LEAD_FSW] > 0.0f ? 1 : 0;

	float * pIn  = pAmpI->m_apPorts[PORT_INPUT];
	float * pOut = pAmpI->m_apPorts[PORT_OUTPUT];


	ampI(&pAmpI->m_State, pIn, pOut, nSamples, pAmpI->m_ulSampleRate, pre, dist, bDist, bLead, b, m, t, vol);
}

void
ampIDeactivate(LV2_Handle instance)
{
}

void
ampICleanup(LV2_Handle instance)
{
	free(instance);
}

const void*
ampIExtension_data(const char* uri)
{
	return NULL;
}






