#include "AmpI_vst.h"
#include <stdio.h>

AudioEffect* createEffectInstance (audioMasterCallback audioMaster) {return new AmpI_vst (audioMaster);}

AmpI_vst::AmpI_vst (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, 1, 1)	// 1 program, 1 parameter only
{
	setNumInputs (1);		// mono in
	setNumOutputs (1);		// mono out
	setUniqueID ('HGA1');	// identify
	canProcessReplacing ();	// supports replacing output single precision

	//initialize parameters
	setParameter(PORT_PRE,		0.5f);
	setParameter(PORT_DIST,		0.5f);
	setParameter(PORT_BASS,		0.5f);
	setParameter(PORT_MID,		0.5f);
	setParameter(PORT_TREBLE,	0.5f);
	setParameter(PORT_VOL,		0.5f);
	setParameter(PORT_DIST_FSW,	1.0f);
	setParameter(PORT_LEAD_FSW,	0.0f);

	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
}

AmpI_vst::~AmpI_vst () {} //nothing to do, all data is static

void AmpI_vst::setProgramName (char* name) {vst_strncpy (programName, name, kVstMaxProgNameLen);}

void AmpI_vst::getProgramName (char* name) {vst_strncpy (name, programName, kVstMaxProgNameLen);}

void AmpI_vst::setParameter (VstInt32 index, float value) {m_afParameters[index] = value;}

float AmpI_vst::getParameter (VstInt32 index) {return m_afParameters[index];}

void AmpI_vst::getParameterName (VstInt32 index, char* label)
{
	static const char* astrNames[PORT_COUNT] = {
		"pre",
		"dist",
		"bass",
		"mid",
		"treble",
		"volume",
		"dist sw",
		"lead sw"
		};
	return 
	vst_strncpy (label, astrNames[index], kVstMaxParamStrLen);
}

void
AmpI_vst::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < PORT_DIST_FSW)
		sprintf(text, "%d", (int) (m_afParameters[index] * 100.0f));
	else
		vst_strncpy (text, m_afParameters[index] > 0.5f ? "on" : "off", kVstMaxParamStrLen);
}

void
AmpI_vst::getParameterLabel (VstInt32 index, char* label)
{
	if(index < PORT_DIST_FSW)
		vst_strncpy (label, "%", kVstMaxParamStrLen);
	else
		vst_strncpy (text, "", kVstMaxParamStrLen);
}

bool
AmpI_vst::getEffectName (char* name)
{
	vst_strncpy (name, "HG AmpI", kVstMaxEffectNameLen);
	return true;
}

bool
AmpI_vst::getProductString (char* text) {vst_strncpy (text, "High Gain", kVstMaxProductStrLen);	return true;}

bool
AmpI_vst::getVendorString (char* text) {vst_strncpy (text, "GPL v3", kVstMaxVendorStrLen); return true;}

VstInt32 AmpI_vst::getVendorVersion() {return 0x0001;}

void
AmpI_vst::processReplacing (float** inputs, float** outputs, VstInt32 nSamples)
{
    float* pIn  =  *inputs;
    float* pOut = *outputs;

	const float	pre =	m_afParameters[index][PORT_PRE];
	const float	dist =	m_afParameters[index][PORT_DIST];
	const float	b =		m_afParameters[index][PORT_BASS];
	const float	m =		m_afParameters[index][PORT_MID];
	const float	t =		m_afParameters[index][PORT_TREBLE];
	const float	vol =	m_afParameters[index][PORT_VOL];

	const uint bDist =	m_afParameters[index][PORT_DIST_FSW] > 0.0f ? 1 : 0;
	const uint bLead =	m_afParameters[index][PORT_LEAD_FSW] > 0.0f ? 1 : 0;

	ampI(m_State, pIn, pOut, nSamples, getSampleRate(), pre, dist, bDist, bLead, b, m, t, vol);
}

