
#include "ParamToOsc.h"

ffglqs::PluginInstance p = ffglqs::Effect::CreatePlugin< ParamToOsc >( { "POSC", "Param To Osc" } );

std::string fShader = R"(
void main()
{
	fragColor = texture( inputTexture, i_uv );
}
)";

ParamToOsc::ParamToOsc()
{
	SetFragmentShader( fShader );
	AddParam( animParam = ffglqs::Param::Create( "Param", 0.0f ) );
	AddParam ( ipParam = ffglqs::ParamText::Create( "Ip", "127.0.0.1" ) );
	AddParam( portParam = ffglqs::ParamText::Create( "Port", "7000" ) );
	AddParam( addressParam = ffglqs::ParamText::Create( "Address", "/composition/selectedclip/transport/position" ) );
}

ParamToOsc::~ParamToOsc()
{
}

FFResult ParamToOsc::SetFloatParameter( unsigned int index, float value )
{
	sendOsc();
	return Effect::SetFloatParameter( index, value );
}

FFResult ParamToOsc::SetTextParameter( unsigned int index, const char* value )
{
	if( index == 1 || index == 2 )
		setPort();
	else
		sendOsc();
	return Effect::SetTextParameter( index, value );
}

void ParamToOsc::sendOsc()
{
	sender.send( juce::OSCMessage( juce::OSCAddressPattern( addressParam->text ), animParam->GetValue() ) );
}

void ParamToOsc::setPort()
{
	sender.connect( ipParam->text, atoi( portParam->text.c_str() ) );
}
