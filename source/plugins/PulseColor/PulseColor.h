#pragma once

#include <FFGLSDK.h>
#include <ffglquickstart/FFGLEffect.h>

#include <juce_core/juce_core.h>

class PulseColor : 
	public ffglqs::Effect
{
public:
	PulseColor();
	~PulseColor();

	//override the default FFGL implementations so we can strip some of the crud that comes with inheriting from Effect
	//I do however want its implementations of the param getter and setters
	FFResult InitGL( const FFGLViewportStruct* viewPort ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* inputTextures ) override;
	FFResult DeInitGL() override;

	
	//we use these as callbacks to know when a param has been changed in Resolume
	FFResult SetFloatParameter( unsigned int index, float value ) override;
	
private:
	juce::DatagramSocket socket;

	bool dirty;
};