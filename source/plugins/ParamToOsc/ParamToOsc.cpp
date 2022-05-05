
#include "ParamToOsc.h"

ffglqs::PluginInstance p = ffglqs::Effect::CreatePlugin< ParamToOsc >( { "POSC", "Param To Osc" } );

ParamToOsc::ParamToOsc()
{
	AddParam( animParam = ffglqs::Param::Create( "Param", 0.0f ) ); //index 0
	AddParam ( ipParam = ffglqs::ParamText::Create( "Ip", "127.0.0.1" ) ); //index 1
	AddParam( portParam = ffglqs::ParamText::Create( "Port", "7000" ) ); //index 2
	AddParam( addressParam = ffglqs::ParamText::Create( "Address", "/composition/selectedclip/transport/position" ) ); //index 3
}

ParamToOsc::~ParamToOsc()
{
}

FFResult ParamToOsc::InitGL( const FFGLViewportStruct* viewPort )
{
	return CFFGLPlugin::InitGL ( viewPort );
}

FFResult ParamToOsc::ProcessOpenGL( ProcessOpenGLStruct* inputTextures )
{
	return FF_FAIL;
}

FFResult ParamToOsc::DeInitGL()
{
	return FF_SUCCESS;
}

FFResult ParamToOsc::SetFloatParameter( unsigned int index, float value )
{
	//first call the superclass implementation so the params get their values updated
	auto result = Effect::SetFloatParameter( index, value );
	//then do the sendy thingy
	sendOsc();
	return result;
}

FFResult ParamToOsc::SetTextParameter( unsigned int index, const char* value )
{
	//first call the superclass implementation so the params get their values updated
	auto result = Effect::SetTextParameter( index, value );
	//index 1 is ipParam, index 2 is portParam
	//if those are changed, update the sender
	if( index == 1 || index == 2 )
		setPort();
	//the only other textparam is addressParam, so in that case just send to the new address instead
	else 
		sendOsc();
	return result;
}

void ParamToOsc::sendOsc()
{
	sender.send( juce::OSCMessage( juce::OSCAddressPattern( addressParam->text ), animParam->GetValue() ) );
}

void ParamToOsc::setPort()
{
	sender.connect( ipParam->text, atoi( portParam->text.c_str() ) );
}
