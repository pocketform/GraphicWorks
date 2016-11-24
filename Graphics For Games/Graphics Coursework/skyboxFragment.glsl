# version 150 core

uniform samplerCube cubeTex ;
uniform samplerCube cubeTex2;
uniform vec3        cameraPos ;

uniform float timeCounter;

in Vertex 
{
vec3 normal ;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) 
{
gl_FragColor = mix (
 texture ( cubeTex , normalize ( IN.normal )),
 texture ( cubeTex2 , normalize ( IN.normal )),
 timeCounter
);
}