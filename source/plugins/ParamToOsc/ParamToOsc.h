#pragma once

#include <FFGLSDK.h>
#include <ffglquickstart/FFGLEffect.h>

#include <juce_core/juce_core.h>
#include <juce_osc/juce_osc.h>

class ParamToOsc : public ffglqs::Effect
{
public:
	ParamToOsc();
	~ParamToOsc();

	//override the default FFGL implementations so we can strip some of the crud that comes with inheriting from Effect
	//I do however want its implementations of the param getter and setters
	FFResult InitGL( const FFGLViewportStruct* viewPort ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* inputTextures ) override;
	FFResult DeInitGL() override;

	std::shared_ptr< ffglqs::Param > animParam;
	std::shared_ptr< ffglqs::ParamText > ipParam;
	std::shared_ptr< ffglqs::ParamText > portParam;
	std::vector< std::shared_ptr< ffglqs::ParamText > > addressParams;
	
	//we use these as callbacks to know when a param has been changed in Resolume
	FFResult SetFloatParameter( unsigned int index, float value ) override;
	FFResult SetTextParameter( unsigned int index, const char* value ) override;

private:
	void setPort();

	juce::OSCSender sender;
};
