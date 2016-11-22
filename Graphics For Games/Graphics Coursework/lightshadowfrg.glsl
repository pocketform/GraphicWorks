# version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D grass;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex; 

uniform vec3  cameraPos ;
uniform vec4  lightColour ;
uniform vec3  lightPos ;
uniform float lightRadius;

in Vertex {
vec4 colour;
vec2 texCoord;
vec3 position;
vec3 normal;
vec3 tangent;
vec3 binormal;
vec3 worldPos;
vec4 shadowProj; 
} IN ;

out vec4 gl_FragColor ;

//set color
const vec3 coast = vec3(0.5f, 0.5f, 0.3f);
const vec3 top   = vec3(0.4f, 0.7f, 1.0f);

void main ( void )
{
vec4  diffuse     = mix(texture (diffuseTex, IN.texCoord), texture(grass, IN.texCoord), 0.5f);
mat3 TBN          = mat3 ( IN.tangent , IN.binormal , IN.normal );
vec3 normal       = normalize ( TBN * ( texture ( bumpTex ,
                                IN.texCoord ).rgb * 2.0 - 1.0));
 //vec3 normal   = normalize (TBN);


//set scope
float mix1        =clamp((IN.position.y - 160.0f) / 50.0f,0.0f,1.0f);
float mix2        =clamp((IN.position.y - 250.0f) / 50.0f,0.0f,1.0f);

//mix color
vec3 q            = mix(coast, diffuse.rgb, mix1);
vec3 q1           = mix(q.rgb, top, mix2);

vec3  incident    = normalize (lightPos - IN.worldPos);
//float lambert     = max (0.0 , dot (incident,  IN.normal));
float lambert     = max (0.0 , dot (incident, normal));

float dist        = length (lightPos  - IN.worldPos);
float atten       = 1.0 - clamp (dist/lightRadius,  0.0, 1.0);

vec3 viewDir      = normalize (cameraPos - IN.worldPos);
vec3 halfDir      = normalize (incident + viewDir);

float rFactor     = max (0.0, dot(halfDir, IN.normal));
float sFactor     = pow (rFactor, 50.0);

float shadow = 1.0;

if( IN.shadowProj . w > 0.0)
{ // New !
shadow = textureProj ( shadowTex , IN.shadowProj );
}

lambert           *= shadow;

vec3 colour       = (q1.rgb * lightColour.rgb);
colour            += (lightColour.rgb * sFactor)  * 0.33;

gl_FragColor      = vec4(colour * atten * lambert, diffuse.a);

gl_FragColor.rgb  +=(diffuse.rgb * lightColour.rgb) * 0.1;

}
