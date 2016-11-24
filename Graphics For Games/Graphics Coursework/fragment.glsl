#version 150 core

uniform sampler2D pardiffuseTex;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	gl_FragColor = IN.colour* texture(pardiffuseTex, IN.texCoord);
}
