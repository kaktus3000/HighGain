#ifndef AMP_I_VST_H_
#define AMP_I_VST_H_

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "AmpI.h"

class AmpI_vst : public AudioEffectX
{
public:
	AmpI_vst (audioMasterCallback audioMaster);
	~AmpI_vst ();

	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

protected:
	//state of amp model
	AmpI m_State;
	
	enum
	{
		//preamp gain
		PORT_PRE = 0,
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

	float m_afParameters[PORT_COUNT];
	
	//vst program setting name
	char programName[kVstMaxProgNameLen + 1];
};

#endif
