
#include "PulseColor.h"

#include <ffglex/FFGLUtilities.h>

#include "version.h"

ffglqs::PluginInstance p (
                                          PluginFactory< PulseColor >,// Create method, allows the host to create a new instance of the plugin
                                          "HV37",           // Plugin unique ID
                                          "PulseColor",         // Plugin name
                                          2,                          // API major version number
                                          1,                          // API minor version number
                                          VERSION_MAJOR,         // Plugin major version number
                                          VERSION_MINOR,         // Plugin minor version number
                                          FF_EFFECT,                       // Plugin type
                                          "Control Pulse UI color from Resolume",  // Plugin description
                                          "Joris de Jong // Hybrid Constructs"         // About
                                      );

PulseColor::PulseColor() :
	ffglqs::Effect( true ),
	socket( true ),
	dirty( false )
{
	AddHueColorParam( "PulseColor" );
}

PulseColor::~PulseColor()
{
}

FFResult PulseColor::InitGL( const FFGLViewportStruct* viewPort )
{
	return CFFGLPlugin::InitGL( viewPort );
}

FFResult PulseColor::ProcessOpenGL( ProcessOpenGLStruct* inputTextures )
{
	if ( dirty )
	{
		float h = params[ 0 ]->GetValue();
		float s = params[ 1 ]->GetValue();
		float v = params[ 2 ]->GetValue();

		float r, g, b;

		ffglex::HSVtoRGB( h, s, v, r, g, b );

		uint32_t color = ( 255 << 24 ) + ( int( r * 255 ) << 16 ) + ( int( g * 255 ) << 8 ) + int( b * 255 );

		socket.write( "255.255.255.255", 23171, &color, 4 );

		dirty = false;
	}
	return FF_FAIL;
}

FFResult PulseColor::DeInitGL()
{
	return FF_SUCCESS;
}

FFResult PulseColor::SetFloatParameter( unsigned int index, float value )
{
	//first call the superclass implementation so the params get their values updated
	auto result = Effect::SetFloatParameter( index, value );

	dirty = true;
	return result;
}
