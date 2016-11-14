# version 150 core

uniform sampler2D diffuseTex ;

uniform vec3  cameraPos ;
uniform vec4  lightColour ;
uniform vec3  lightPos ;
uniform float lightRadius;

uniform vec4  lightColour1 ;
uniform vec3  lightPos1 ;
uniform float lightRadius1 ;

in Vertex {
vec3 colour ;
vec2 texCoord ;
vec3 normal ;
vec3 worldPos ;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
vec4  diffuse     = texture (diffuseTex, IN.texCoord);

vec3  incident    = normalize (lightPos     - IN.worldPos);
float lambert     = max (0.0 , dot (incident,  IN.normal));
vec3  incident1   = normalize (lightPos1    - IN.worldPos);
float lambert1    = max (0.0 , dot (incident1, IN.normal));

float dist        = length (lightPos  - IN.worldPos);
float atten       = 1.0 - clamp (dist/lightRadius,  0.0, 1.0);
float dist1       = length (lightPos1 - IN.worldPos);
float atten1      = 1.0 - clamp (dist/lightRadius1, 0.0, 1.0);

vec3 viewDir      = normalize (cameraPos - IN.worldPos);

vec3 halfDir      = normalize (incident   + viewDir);
vec3 halfDir1     = normalize (incident1  + viewDir);

float rFactor     = max (0.0, dot(halfDir,  IN.normal));
float rFactor1    = max (0.0, dot(halfDir1,  IN.normal));
float sFactor     = pow (rFactor, 50.0);
float sFactor1    = pow (rFactor1, 50.0);


vec3 colour       = (diffuse.rgb * lightColour.rgb);
vec3 colour1      = (diffuse.rgb * lightColour1.rgb);
colour            += (lightColour.rgb * sFactor )  * 0.33;
colour1           += (lightColour.rgb * sFactor1 )  * 0.33;

gl_FragColor      = vec4(colour * atten * lambert, diffuse.a);
gl_FragColor      +=vec4(colour1 * atten1 * lambert1, diffuse.a);
gl_FragColor.rgb  +=(diffuse.rgb * lightColour.rgb) * 0.1;
}
