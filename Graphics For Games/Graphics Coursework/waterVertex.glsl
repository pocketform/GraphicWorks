# version 150 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal; // New Attribute !
in vec2 texCoord;

out Vertex 
{
vec4 colour;
vec2 texCoord;
vec3 normal;
vec3 worldPos;
vec4 shadowProj; 
} OUT;

void main ( void ) 
{
mat3 normalMatrix = transpose (inverse ( mat3 ( modelMatrix )));

OUT.colour        = colour;
OUT.texCoord      = (textureMatrix * vec4 ( texCoord , 0.0 , 1.0)).xy;

OUT.normal        = normalize (normalMatrix * normalize ( normal ));

OUT.shadowProj    = ( shadowMatrix* vec4 (position +(normal * 1.5) ,1));

OUT.worldPos      = (modelMatrix * vec4 (position ,1)).xyz;
gl_Position       = (projMatrix * viewMatrix * modelMatrix ) *
                      vec4 (position , 1.0);
}
