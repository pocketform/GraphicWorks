# version 150 core

uniform sampler2D diffuseTex ;
uniform sampler2DShadow shadowTex; 

uniform vec3  cameraPos ;
uniform vec4  lightColour ;
uniform vec3  lightPos ;
uniform float lightRadius;

//uniform vec4  lightColour1 ;
//uniform vec3  lightPos1 ;
//uniform float lightRadius1 ;

in Vertex {
vec4 colour;
vec2 texCoord;
vec3 normal;
vec3 tangent;
vec3 binormal;
vec3 worldPos;
vec4 shadowProj; 
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
vec4  diffuse     = texture (diffuseTex, IN.texCoord);


vec3  incident    = normalize (lightPos - IN.worldPos);
float lambert     = max (0.0 , dot (incident,  IN.normal));

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

lambert          *= shadow;

vec3 colour       = (diffuse.rgb * lightColour.rgb);
colour            += (lightColour.rgb * sFactor)  * 0.33;

gl_FragColor      = vec4(colour * atten * lambert, diffuse.a);
gl_FragColor.rgb  +=(diffuse.rgb * lightColour.rgb) * 0.1;
}
