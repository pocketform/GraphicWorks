#version 150 core

const   int num_light = 4;
uniform int lightCount;

uniform sampler2D diffuseTex ;
uniform sampler2D bumpTex ; // New !


uniform vec4  lightColour[num_light];
uniform vec3  lightPos[num_light];
uniform float lightRadius[num_light];

uniform vec3  cameraPos ;


in Vertex {
    vec3 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 tangent;  // New !
    vec3 binormal; // New !
} IN;

out vec4 gl_FragColor;

void main (void)
{
    vec4 diffuse  = texture (diffuseTex, IN.texCoord);
    mat3 TBN      = mat3 ( IN.tangent , IN.binormal , IN.normal );// New !
    vec3 normal   = normalize ( TBN * ( texture ( bumpTex ,
                                IN.texCoord ).rgb * 2.0 - 1.0));
   // vec3 normal   = normalize (TBN);

    vec3 viewDir  = normalize (cameraPos - IN.worldPos);//camera view
    int shininess = 50; //effect the power of lights

    for (int i = 0; i < lightCount; i++)
    {
        vec3  incident    = normalize (lightPos[i] - IN.worldPos);
        float lambert     = max (0.0, dot (incident, IN.normal));
        float dist        = length (lightPos[i] - IN.worldPos);
        float atten       = 1.0 - clamp (dist / lightRadius[i], 0.0, 1.0);
        vec3  halfDir     = normalize (incident - viewDir);
        float rFactor     = max (0.0, dot (halfDir, IN.normal));
        float sFactor     = pow (rFactor, shininess);
        vec3  colour      = (diffuse.rgb * lightColour[i].rgb);
        colour           += (lightColour[i].rgb * sFactor) * 0.33;
        gl_FragColor     += vec4 (colour * atten * lambert, diffuse.a);
        gl_FragColor.rgb += (diffuse.rgb * lightColour[i].rgb) * 0.001;
    }
	//gl_FragColor.rgb = normal;
}