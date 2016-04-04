#include "FIR_vst.h"
#include <stdio.h>
#include <malloc.h>

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {return new FIR_vst (audioMaster);}

FIR_vst::FIR_vst (audioMasterCallback audioMaster)
	: AudioEffectX(audioMaster, 1, PORT_COUNT)	// 1 program
{
	setNumInputs (1);		// mono in
	setNumOutputs (1);		// mono out
	setUniqueID ('HGFI');	// identify
	canProcessReplacing ();	// supports replacing output single precision

	//initialize parameters
	setParameter(PORT_VOL,		0.5f);
	setParameter(PORT_MODEL,	0.5f);
	
	instantiateFIR(&m_State, (uint)getSampleRate());

	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
}

FIR_vst::~FIR_vst (){} //nothing to do, all data is static

void FIR_vst::setProgramName (char* name) {vst_strncpy (programName, name, kVstMaxProgNameLen);}

void FIR_vst::getProgramName (char* name) {vst_strncpy (name, programName, kVstMaxProgNameLen);}

void FIR_vst::setParameter (VstInt32 index, float value)
{
	m_afParameters[index] = value;
}

float FIR_vst::getParameter (VstInt32 index) {return m_afParameters[index];}

void FIR_vst::getParameterName (VstInt32 index, char* label)
{
	static const char* astrNames[PORT_COUNT] = {
		"volume",
		"model"
		};
	vst_strncpy (label, astrNames[index], kVstMaxParamStrLen);
}

void
FIR_vst::getParameterDisplay (VstInt32 index, char* text)
{
	if(index == PORT_VOL)
		sprintf(text, "%d", (int)(m_afParameters[PORT_VOL] * 80.0f - 60.0f) );
	else
		sprintf(text, "%d", (uint)MAX((m_afParameters[PORT_VOL] * NUM_MODELS) + 1, NUM_MODELS));
}

void
FIR_vst::getParameterLabel (VstInt32 index, char* label)
{
	if(index == PORT_VOL)
		vst_strncpy (label, "dB", kVstMaxParamStrLen);
	else
		vst_strncpy(label, "", kVstMaxParamStrLen);
}

bool
FIR_vst::getEffectName (char* name)
{
	vst_strncpy (name, "HG FIR", kVstMaxEffectNameLen);
	return true;
}

bool
FIR_vst::getProductString (char* text) {vst_strncpy (text, "High Gain", kVstMaxProductStrLen);	return true;}

bool
FIR_vst::getVendorString (char* text) {vst_strncpy (text, "GPL v3", kVstMaxVendorStrLen); return true;}

VstInt32 FIR_vst::getVendorVersion() {return 0x0001;}

void
FIR_vst::processReplacing (float** inputs, float** outputs, VstInt32 nSamples)
{
    float* pIn  =  *inputs;
    float* pOut = *outputs;

	const float	vol = powf(10.0f, (m_afParameters[PORT_VOL] * 80.0f - 60.0f) / 20.0f);
	uint model = (uint) MAX(m_afParameters[PORT_MODEL] * NUM_MODELS, NUM_MODELS - 1);

	fir(&m_State, pIn, pOut, nSamples, (uint)getSampleRate(), model, vol);
}

