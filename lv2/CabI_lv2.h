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


#ifndef CABI_LV2_H_
#define CABI_LV2_H_

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define CAB_I_URI "https://github.com/kaktus3000/HighGain/CabI"

LV2_Handle
cabIInstantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features);

void
cabIConnect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data);

void
cabIActivate(LV2_Handle instance);


void
cabIRun(LV2_Handle instance, uint32_t nScables);


void
cabIDeactivate(LV2_Handle instance);


void
cabICleanup(LV2_Handle instance);


const void*
cabIExtension_data(const char* uri);

static const LV2_Descriptor cabIDescriptor = {
	CAB_I_URI,
	cabIInstantiate,
	cabIConnect_port,
	cabIActivate,
	cabIRun,
	cabIDeactivate,
	cabICleanup,
	cabIExtension_data
};


#endif /* CABI_LV2_H_ */
