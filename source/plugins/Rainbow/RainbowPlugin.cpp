
#include "RainbowPlugin.h"

static PluginInstance p = Effect::CreatePlugin< RainbowPlugin >( { PLUGIN_ID, "Rainbow" } );

static const std::string fShader = R"(

uniform float HuePhase;

vec3 hue2rgb(float hue){
    hue=fract(hue);
    return clamp(vec3(
        abs(hue*6.-3.)-1.,
        2.-abs(hue*6.-2.),
        2.-abs(hue*6.-4.)
    ), 0.0, 1.0);
}

void main() {
	vec4 tex = texture(inputTexture, i_uv);
    
    float x = i_uv.x;
    x = Mirror ? abs ( x - 0.5 ) * 2.0 : x;
    x = fract ( x - HuePhase );
    vec3 rgb = hue2rgb ( x );
    fragColor = tex * vec4( rgb, 1.0 );
}

)";

RainbowPlugin::RainbowPlugin() : 
    phase ( 0.0f )
{
    AddParam( scrollSpeedParam = Param::Create( "ScrollSpeed", 0.25f ) );
    AddParam( mirrorParam = ParamBool::Create( "Mirror", true ));
	SetFragmentShader( fShader );
}

RainbowPlugin::~RainbowPlugin()
{
}

FFResult RainbowPlugin::Render(ProcessOpenGLStruct* pGL)
{
    phase += scrollSpeedParam->GetValue() * deltaTime;
    shader.Set( "HuePhase", phase );
    shader.Set( "Mirror", mirrorParam->GetValue() > 0.5f );
    auto result = Effect::Render(pGL);
    return result;
}
