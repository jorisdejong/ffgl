
#include "ParamToOsc.h"

ffglqs::PluginInstance p = ffglqs::Effect::CreatePlugin< ParamToOsc >( { "POSC", "Param To Osc" } );

const int NUMPARAMS = 50;

ParamToOsc::ParamToOsc()
{
	AddParam( animParam = ffglqs::Param::Create( "Param", 0.0f ) );      //index 0
	AddParam( ipParam = ffglqs::ParamText::Create( "Ip", "127.0.0.1" ) );//index 1
	AddParam( portParam = ffglqs::ParamText::Create( "Port", "7000" ) ); //index 2
	for( int i = 0; i < NUMPARAMS; i++ )
	{
		std::shared_ptr< ffglqs::ParamText > addressParam = ffglqs::ParamText::Create( juce::String( "Address " + juce::String( i + 1 ) ).toRawUTF8(), "" );//index 3 to 53
		addressParams.push_back( addressParam );
		AddParam( addressParam );
	}
}

ParamToOsc::~ParamToOsc()
{
}

FFResult ParamToOsc::InitGL( const FFGLViewportStruct* viewPort )
{
	return CFFGLPlugin::InitGL( viewPort );
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
	juce::OSCBundle bundle;
	for( int i = 0; i < NUMPARAMS; i++ )
	{
		auto address = addressParams[ i ]->text;
		if( address != "" )
			bundle.addElement( juce::OSCMessage( juce::OSCAddressPattern( address ), animParam->GetValue() ) );
	}
	sender.send( bundle );

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
	//the only other textparams are addressParams, so in that case just send a message to the new address instead
	else if( !juce::String( value ).isEmpty() )
		sender.send( juce::OSCMessage( juce::OSCAddressPattern( value ), animParam->GetValue() ) );
	return result;
}

void ParamToOsc::setPort()
{
	sender.connect( ipParam->text, atoi( portParam->text.c_str() ) );
}
