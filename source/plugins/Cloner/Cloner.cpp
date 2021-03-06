#include "Cloner.h"
#include <ffgl/FFGLLib.h>
#include <ffglex/FFGLScopedShaderBinding.h>
#include <ffglex/FFGLScopedSamplerActivation.h>
#include <ffglex/FFGLScopedTextureBinding.h>

#define PI 3.141592654

#define paramNumClones 0
#define paramScale 1
#define paramRadius 2
#define paramFan 3

const float maxClones = 16.0f;

using namespace ffglex;

static CFFGLPluginInfo PluginInfo(
	PluginFactory< Cloner >,// Create method
	"RE02",                 // Plugin unique ID of maximum length 4.
	"Cloner Example",       // Plugin name
	2,                      // API major version number
	1,                      // API minor version number
	1,                      // Plugin major version number
	0,                      // Plugin minor version number
	FF_EFFECT,              // Plugin type
	"Hi Anton",             // Plugin description
	"Resolume FFGL Example" // About
);

static const char vertexShaderCode[] = R"(#version 410 core


layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

out vec2 uv;

void main()
{
	gl_Position = vPosition;
	uv = vUV;
}
)";

static const char fragmentShaderCode[] = R"(#version 410 core

#define PI 3.141592564

in vec2 uv;

uniform sampler2D InputTexture;
uniform vec2 MaxUV;
uniform float Aspect;

uniform float NumClones;
uniform float Scale;
uniform float Radius;
uniform float Fan;

out vec4 fragColor;

// rotation and scale in GLSL using matrices
// is really well explained here
// https://thebookofshaders.com/08/

mat2 rotate2d(float _angle)
{
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

mat2 scale(vec2 _scale){
    return mat2(_scale.x,0.0,
                0.0,_scale.y);
}

void main()
{
	
	vec4 color = vec4(0.0);
	//for loop for each clone
	const float maxClones = 16.0f;
	float fNumClones = floor( NumClones * maxClones );
	
	for( float i = 0; i < fNumClones; i++ )
	{
		float angle = 360.0f / maxClones //rotation for one clone
		* i // calculated for this clone
		* PI / 180.0f //converted to radians
		* Fan; //scaled by the fan value

		float x     = cos( angle ) * Radius / Aspect; //adjust for aspect ratio
		float y     = sin( angle ) * Radius;

		//position of our uv coordinate
		vec2 st = uv;
		
		//move to the required position
		st += vec2(x,y);
	
		//set the center to 0, 0
		st -= 0.5;

		//apply scaling
		st = scale ( vec2 ( Scale ) ) * st;

		//set the aspect ratio to 1:1
		st = scale ( vec2 ( 1.0, 1.0f / Aspect ) ) * st;

		//rotate
		st = rotate2d( angle ) * st; 

		//set the aspect ratio back
		st = scale ( vec2 ( 1.0, Aspect ) ) * st;

		//set the center back
		st += 0.5;

		//use step to see where st goes out of bounds
		vec2 bounds = step ( vec2(0.0), st ) * step ( st, vec2(1.0) ) ;
		float min = min ( bounds.x, bounds.y );

		//get the color at this uv coordinate
		vec4 cloneCol = texture( InputTexture, st * MaxUV);
	
		//make color transparent where we're out of bounds
		cloneCol *= min;
		
		//regular alpha blend
		color = color * ( 1.0 - cloneCol.a) + cloneCol;
	}
	fragColor = color;
}
)";

Cloner::Cloner() :
	maxUVLocation( -1 ),
	aspectLocation( -1 ),
	numClonesLocation( -1 ),
	scaleLocation( -1 ),
	radiusLocation( -1 ),
	fanLocation( -1 )
{
	// Input properties
	SetMinInputs( 1 );
	SetMaxInputs( 1 );

	//Params
	numClones = 1.0f;
	scale     = 0.33f;
	radius    = 0.5f;
	fan       = 1.0f;
	SetParamInfof( paramNumClones, "Clones", FF_TYPE_STANDARD );
	SetParamInfof( paramScale, "Scale", FF_TYPE_STANDARD );
	SetParamInfof( paramRadius, "Radius", FF_TYPE_STANDARD );
	SetParamInfof( paramFan, "Fan", FF_TYPE_STANDARD );
}
Cloner::~Cloner()
{
}

FFResult Cloner::InitGL( const FFGLViewportStruct* vp )
{
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

	//FFGL requires us to leave the context in a default state on return, so use this scoped binding to help us do that.
	ScopedShaderBinding shaderBinding( shader.GetGLID() );

	//We're never changing the sampler to use, instead during rendering we'll make sure that we're always
	//binding the texture to sampler 0.
	glUniform1i( shader.FindUniform( "inputTexture" ), 0 );

	//We need to know these uniform locations because we need to set their value each frame.
	maxUVLocation     = shader.FindUniform( "MaxUV" );
	aspectLocation    = shader.FindUniform( "Aspect" );
	numClonesLocation = shader.FindUniform( "NumClones" );
	scaleLocation     = shader.FindUniform( "Scale" );
	radiusLocation    = shader.FindUniform( "Radius" );
	fanLocation       = shader.FindUniform( "Fan" );

	//Use base-class init as success result so that it retains the viewport.
	return CFFGLPlugin::InitGL( vp );
}
FFResult Cloner::ProcessOpenGL( ProcessOpenGLStruct* pGL )
{
	if( pGL->numInputTextures < 1 )
		return FF_FAIL;

	if( pGL->inputTextures[ 0 ] == NULL )
		return FF_FAIL;

	//prevent division by zero error
	if( scale == 0.0f )
		return FF_SUCCESS;

	//FFGL requires us to leave the context in a default state on return, so use this scoped binding to help us do that.
	ScopedShaderBinding shaderBinding( shader.GetGLID() );

	FFGLTextureStruct& Texture = *( pGL->inputTextures[ 0 ] );

	//The input texture's dimension might change each frame and so might the content area.
	//We're adopting the texture's maxUV using a uniform because that way we dont have to update our vertex buffer each frame.
	FFGLTexCoords maxCoords = GetMaxGLTexCoords( Texture );
	glUniform2f( maxUVLocation, maxCoords.s, maxCoords.t );

	float aspect = Texture.Width / float( Texture.Height );
	glUniform1f( aspectLocation, aspect );

	//pass the variables to the shader
	glUniform1f( numClonesLocation, numClones );
	glUniform1f( scaleLocation, 1.0f / scale );
	glUniform1f( radiusLocation, radius );
	glUniform1f( fanLocation, fan );
	
	//The shader's sampler is always bound to sampler index 0 so that's where we need to bind the texture.
	//Again, we're using the scoped bindings to help us keep the context in a default state.
	ScopedSamplerActivation activateSampler( 0 );
	Scoped2DTextureBinding textureBinding( Texture.Handle );
	
	quad.Draw();

	return FF_SUCCESS;
}
FFResult Cloner::DeInitGL()
{
	shader.FreeGLResources();
	quad.Release();
	maxUVLocation     = -1;
	aspectLocation    = -1;
	numClonesLocation = -1;
	scaleLocation     = -1;
	radiusLocation    = -1;
	fanLocation       = -1;

	return FF_SUCCESS;
}

FFResult Cloner::SetFloatParameter( unsigned int dwIndex, float value )
{
	switch( dwIndex )
	{
	case paramNumClones:
		numClones = value;
		break;
	case paramScale:
		scale = value;
		break;
	case paramRadius:
		radius = value;
		break;
	case paramFan:
		fan = value;
		break;
	default:
		return FF_FAIL;
	}

	return FF_SUCCESS;
}

float Cloner::GetFloatParameter( unsigned int dwIndex )
{
	switch( dwIndex )
	{
	case paramNumClones:
		return numClones;
		break;
	case paramScale:
		return scale;
		break;
	case paramRadius:
		return radius;
		break;
	case paramFan:
		return fan;
		break;
	default:
		return 0.0f;
	}
}
