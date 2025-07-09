
#include "RainbowPlugin.h"

#include "version.h"

ffglqs::PluginInstance p (
                                          PluginFactory< RainbowPlugin >,// Create method, allows the host to create a new instance of the plugin
                                          "HV18",           // Plugin unique ID
                                          "Rainbow",         // Plugin name
                                          2,                          // API major version number
                                          1,                          // API minor version number
                                          VERSION_MAJOR,         // Plugin major version number
                                          VERSION_MINOR,         // Plugin minor version number
                                          FF_EFFECT,                       // Plugin type
                                          "Screw your hue and saturation, there is only rainbooooow",  // Plugin description
                                          "Joris de Jong // Hybrid Constructs"         // About
                                      );

static const std::string fShader = R"(

uniform float HuePhase;
uniform bool Mirror;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec4 tex = texture(inputTexture, i_uv);
    if ( tex.a > 0 )
        tex.rgb /= tex.a;
    
    float x = i_uv.x;
    x = Mirror ? abs ( x - 0.5 ) * 2.0 : x;
    x = fract ( x - HuePhase );

    vec3 hsv = rgb2hsv( tex.rgb );
    hsv.rg = vec2( x, 1.0 );
    tex.rgb = hsv2rgb ( hsv ) * tex.a;
    fragColor = tex;
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
