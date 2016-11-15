# version 150 core

uniform sampler2D diffuseTex ;
uniform sampler2D bumpTex ; // New !

uniform vec3 cameraPos ;
uniform vec4 lightColour ;
uniform vec3 lightPos ;
uniform float lightRadius ;

uniform vec4  lightColour1 ;
uniform vec3  lightPos1 ;
uniform float lightRadius1 ;

in Vertex {
vec3 colour ;
vec2 texCoord ;
vec3 normal ;
vec3 tangent ; // New !
vec3 binormal ; // New !
vec3 worldPos ;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
//TBN
vec4 diffuse      = texture ( diffuseTex , IN.texCoord );// New !
mat3 TBN          = mat3 ( IN.tangent , IN.binormal , IN.normal );// New !
vec3 normal       = normalize ( TBN * ( texture ( bumpTex ,
                           IN.texCoord ).rgb * 2.0 - 1.0));

vec3 incident     = normalize ( lightPos - IN.worldPos );
float lambert     = max (0.0 , dot ( incident , normal )); // Different !
vec3  incident1   = normalize (lightPos1    - IN.worldPos);
float lambert1    = max (0.0 , dot (incident1, IN.normal));

float dist        = length ( lightPos - IN.worldPos );
float atten       = 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);
float dist1       = length (lightPos1 - IN.worldPos);
float atten1      = 1.0 - clamp (dist/lightRadius1, 0.0, 1.0);

vec3 viewDir      = normalize ( cameraPos - IN.worldPos );

vec3 halfDir      = normalize ( incident + viewDir );
vec3 halfDir1     = normalize (incident1  + viewDir);

float rFactor     = max (0.0 , dot ( halfDir , normal )); // Different !
float sFactor     = pow ( rFactor , 33.0 );
float rFactor1    = max (0.0, dot(halfDir1,  IN.normal));
float sFactor1    = pow (rFactor1, 50.0);

vec3 colour       =  ( diffuse.rgb * lightColour . rgb );
colour            += ( lightColour.rgb * sFactor ) * 0.33;
vec3 colour1      =  (diffuse.rgb * lightColour1.rgb);
colour1           += (lightColour.rgb * sFactor1 )  * 0.33;

gl_FragColor      = vec4 ( colour * atten * lambert , diffuse.a );
gl_FragColor      +=vec4(colour1 * atten1 * lambert1, diffuse.a);
gl_FragColor.rgb  +=  ( diffuse.rgb * lightColour.rgb ) * 0.1;


}
