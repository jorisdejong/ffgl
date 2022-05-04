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

	std::shared_ptr< ffglqs::Param > animParam;
	std::shared_ptr< ffglqs::ParamText > ipParam;
	std::shared_ptr< ffglqs::ParamText > portParam;
	std::shared_ptr< ffglqs::ParamText > addressParam;

	FFResult SetFloatParameter( unsigned int index, float value ) override;
	FFResult SetTextParameter( unsigned int index, const char* value ) override;

private:
	void sendOsc();
	void setPort();

	juce::OSCSender sender;
};
