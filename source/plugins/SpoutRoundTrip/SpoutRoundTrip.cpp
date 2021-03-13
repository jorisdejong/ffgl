
#include "SpoutRoundTrip.h"

static PluginInstance p = Effect::CreatePlugin< SpoutRoundTrip >( { "HV27", "Spout Round Trip" } );

static const std::string fShader = R"(
uniform sampler2D Tex;
void main()
{
	fragColor = texture( Tex, i_uv );
}

)";

SpoutRoundTrip::SpoutRoundTrip() :
	Effect( true )
{
	SetFragmentShader( fShader );

	senderName = "RoundTripOut";
	SetParamInfo( 0, "Sender Name", FF_TYPE_TEXT, senderName );
	receiverName = "RoundTripIn";
	SetParamInfo( 1, "Receiver Name", FF_TYPE_TEXT, receiverName );
}

SpoutRoundTrip::~SpoutRoundTrip()
{
}

FFResult SpoutRoundTrip::InitGL( const FFGLViewportStruct* viewPort )
{
	std::string fragmentShaderCode = CreateFragmentShader( fragmentShaderBase );
	if( !shader.Compile( vertexShaderCode, fragmentShaderCode ) )
	{
		DeInitGL();
		return FF_FAIL;
	}
	if( !quad.Initialise() )
	{
		DeInitGL();
		return FF_FAIL;
	}
	if( Init() == FF_FAIL )
	{
		DeInitGL();
		return FF_FAIL;
	}

	spoutReceiver.SetReceiverName( receiverName );
	spoutSender.SetSenderName( senderName );

	return CFFGLPlugin::InitGL( viewPort );
}

FFResult SpoutRoundTrip::Render( ProcessOpenGLStruct* inputTextures )
{
	if( inputTextures && inputTextures->numInputTextures > 0 )
	{
		FFGLTextureStruct* tex = inputTextures->inputTextures[ 0 ];
		spoutSender.SendTexture( tex->Handle, GL_TEXTURE_2D, tex->Width, tex->Height, false, inputTextures->HostFBO );
	}

	spoutReceiver.IsUpdated();
	if ( spoutReceiver.ReceiveTexture() )
	{
		ffglex::ScopedShaderBinding shaderBinding( shader.GetGLID() );
		ffglex::ScopedSamplerActivation activateSampler0( 0 );

		spoutReceiver.spout.BindSharedTexture();
		shader.Set( "Tex", 0 );
		shader.Set( "maxUV", 1.0f, 1.0f );

		quad.Draw();
		spoutReceiver.spout.UnBindSharedTexture();
	}

	
	return FF_SUCCESS;
}

FFResult SpoutRoundTrip::DeInitGL()
{
	shader.FreeGLResources();
	quad.Release();
	Clean();

	return FF_SUCCESS;
}

char* SpoutRoundTrip::GetParameterDisplay( unsigned int index )
{
	return CFFGLPlugin::GetParameterDisplay( index );
}

FFResult SpoutRoundTrip::SetTextParameter( unsigned int index, const char* text )
{
	switch( index )
	{
	case 0:
		senderName = text;
		spoutSender.SetSenderName( senderName );
		return FF_SUCCESS;
		break;
	case 1:
		receiverName = text;
		spoutReceiver.SetReceiverName( receiverName );
		return FF_SUCCESS;
		break;
	default:
		return FF_FAIL;
		break;
	}
}

char* SpoutRoundTrip::GetTextParameter( unsigned int index )
{
	switch( index )
	{
	case 0:
		return _strdup( senderName );
		break;
	case 1:
		return _strdup( receiverName );
		break;
	default:
		return "";
		break;
	}
}
