# version 150 core

//for multiple light
//const   int num_light = 4;
//uniform int lightCount;

//uniform vec4  lightColour[num_light];
//uniform vec3  lightPos[num_light];
//uniform float lightRadius[num_light];

uniform sampler2D   diffuseTex;
//uniform sampler2DShadow shadowTex;//  for shadow
uniform samplerCube cubeTex;
uniform samplerCube cubeTex2;

uniform vec4  lightColour;
uniform vec3  lightPos;
uniform vec3  cameraPos;
uniform float lightRadius;

uniform float timeCounter;//change skybox

in Vertex 
{
vec4 colour;
vec2 texCoord;
vec3 normal;
vec3 worldPos;
//vec4 shadowProj; //for shadow
} IN;

out vec4 gl_FragColor;


void main ( void ) 
{
vec4  diffuse    = texture ( diffuseTex , IN.texCoord ) * IN.colour ;

vec3  incident   = normalize ( IN . worldPos - cameraPos );
float lambert    = max (0.0 , dot (incident,  IN.normal));

float dist       = length ( lightPos - IN.worldPos );
//float atten      = 1.0 - clamp ( dist / lightRadius , 0.2 , 0.8);
float atten      = 1.0 - clamp ( dist / lightRadius , 0.2 , 0.6);


atten = clamp(atten, 0.1f, 1.0f);

vec4  reflection = texture ( cubeTex ,
			     reflect (incident, normalize( IN.normal )));

      reflection = mix(
		   texture(cubeTex , reflect(incident, normalize(IN.normal))), 
		   texture(cubeTex2, reflect(incident, normalize(IN.normal))),
 		   timeCounter);
 
//try for shadow
//float shadow = 1.0;
//if( IN.shadowProj . w > 0.0)
//{ // New !
//shadow = textureProj ( shadowTex , IN.shadowProj );
//}
//lambert          *= shadow;

gl_FragColor = (lightColour * diffuse * atten )*( diffuse + reflection );
//gl-FragColor.rgb = diffuse;
gl_FragColor +=( diffuse + reflection ) * 0.1;
//gl_FragColor.rgb = vec3(atten,atten,atten);

//gl_FragColor = reflection;

//gl_FragColor.a = 0.6f; // control alpha
}
