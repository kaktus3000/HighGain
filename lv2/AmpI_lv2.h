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


#ifndef AMPI_LV2_H_
#define AMPI_LV2_H_

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define AMP_I_URI "https://github.com/kaktus3000/HighGain/AmpI"

LV2_Handle
ampIInstantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features);

void
ampIConnect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data);

void
ampIActivate(LV2_Handle instance);


void
ampIRun(LV2_Handle instance, uint32_t nSamples);


void
ampIDeactivate(LV2_Handle instance);


void
ampICleanup(LV2_Handle instance);


const void*
ampIExtension_data(const char* uri);

static const LV2_Descriptor ampIDescriptor = {
	AMP_I_URI,
	ampIInstantiate,
	ampIConnect_port,
	ampIActivate,
	ampIRun,
	ampIDeactivate,
	ampICleanup,
	ampIExtension_data
};


#endif /* AMPI_LV2_H_ */
