
#include "WipePlugin.h"

#include <iostream>

static PluginInstance p = Effect::CreatePlugin< WipePlugin >( { PLUGIN_ID, "Wipe" } );

static const std::string fShader = R"(
#define PI 3.141592654

uniform float Phase;
uniform float Width;
uniform float Angle;
uniform bool Mirror;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

void main() {
	vec2 st = i_uv;

	st -= vec2(0.5);
	st.x = mix( st.x, abs ( st.x ), Mirror );
    st = rotate2d( Angle * 2.0 * PI  ) * st;
    st += vec2(0.5);
    
    //calculate angle adjustment
	float adjust = -(pow(( fract( Angle * 4.0 ) - 0.5 ) * 2.0, 2.0)) + 1.0;
	adjust = 1.0 + adjust * .41; //why .41 why

	float mWidth = Width * 2.0;
	float mPhase = (Phase - 0.5) * -( 1.0 + mWidth ); 

	float edge = st.x + mPhase * adjust;
	float lb = 0.5 - mWidth * 0.5;
	float hb = 0.5 + mWidth * 0.5;

	float trail = smoothstep( lb, hb, edge ) ;
	trail *= step ( edge, hb );

	vec4 tex = texture(inputTexture, i_uv);
    fragColor = tex * vec4( trail );

}

)";

WipePlugin::WipePlugin()
{
	SetFragmentShader( fShader );
	AddParam( trigger = ParamTrigger::Create( "Trigger" ) );
	//AddParam( Param::Create( "Phase" ) );
	AddParam( speed = Param::Create( "Speed", 1.0f ) );
	AddParam( width = Param::Create( "Width", 1.0f ) );
	AddParam( angle = Param::Create( "Angle" ) );
	AddParam( mirror = ParamBool::Create( "Mirror", false ) );

	for ( int i = 0; i < 10; i++ )
	{
		Wipe wipe;
		wipe.phase = 1.0f;
		/*wipe.speed = speed->GetValue();
		wipe.width = width->GetValue();
		wipe.angle = angle->GetValue();*/
		wipes.push_back( wipe );
	}
}

WipePlugin::~WipePlugin()
{
}

void WipePlugin::Update()
{
	for ( Wipe& wipe : wipes )
	{
		if ( wipe.phase < 1.0f )
			wipe.phase += wipe.speed * deltaTime / ( 1.0f + width->GetValue() );
		else
			wipe.phase = 1.0f;
	}
	if ( trigger->GetValue() > 0.5f )
	{
		std::cout << "wipe" << std::endl;
		//find the first empty float
		for ( Wipe& wipe : wipes )
		{
			if ( wipe.phase == 1.0f )
			{
				wipe.phase = 0.0f;
				wipe.speed = speed->GetValue();
				wipe.width = width->GetValue();
				wipe.angle = angle->GetValue();
				wipe.mirror = mirror->GetValue() > 0.5f;
				break;
			}
		}
	}
	
}

FFResult WipePlugin::Render( ProcessOpenGLStruct* pGL )
{
	if ( pGL->numInputTextures < 1 )
		return FF_FAIL;
	if ( pGL->inputTextures[ 0 ] == NULL )
		return FF_FAIL;

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

	//Activate our shader using the scoped binding so that we'll restore the context state when we're done.
	ffglex::ScopedShaderBinding shaderBinding( shader.GetGLID() );
	//Use the scoped bindings to ensure that the context will be returned in it's default state after we're done rendering.
	ffglex::ScopedSamplerActivation activateSampler0( 0 );
	ffglex::Scoped2DTextureBinding textureBinding0( pGL->inputTextures[ 0 ]->Handle );

	shader.Set( "inputTexture", 0 );
	FFGLTexCoords maxCoords = GetMaxGLTexCoords( *pGL->inputTextures[ 0 ] );
	shader.Set( "maxUV", maxCoords.s, maxCoords.t );
	for ( const auto& wipe : wipes )
	{
		if ( wipe.phase < 1.0f )
		{
			shader.Set( "Phase", wipe.phase );
			shader.Set( "Width", wipe.width );
			shader.Set( "Angle", wipe.angle );
			shader.Set( "Mirror", wipe.mirror );
			quad.Draw();
		}
	}
	//quad.Draw();
	glBlendFunc( GL_ONE, GL_ZERO );
	glDisable( GL_BLEND );
	return FF_SUCCESS;
}
