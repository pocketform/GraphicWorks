# version 150 core

uniform samplerCube cubeTex ;
uniform vec3        cameraPos ;

in Vertex 
{
vec3 normal ;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) 
{
gl_FragColor = texture ( cubeTex , normalize ( IN.normal ));
}