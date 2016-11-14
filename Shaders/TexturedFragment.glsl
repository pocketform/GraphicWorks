#version 150
uniform sampler2D diffuseTex;
uniform sampler2D myText;
in Vertex {
	vec2 texCoord;
        vec4 colour;
	vec3 position;
} IN;

out vec4 gl_FragColor;
const vec3 shadow=vec3(0,0,0);
const vec3 water=vec3(0,0.5,0.8);
const vec3 snow=vec3(1,1,1);
const vec3 what=vec3(0.2,0.5,0.3);

void main(void){
	//gl_FragColor = texture(diffuseTex, IN.texCoord)+texture(myText, IN.texCoord);

	//float mix1=clamp((IN.position.y-50.0f)/100.0f,0.0f,1.0f);
	//float mix1=clamp((IN.position.y-50.0f)/150.0f,0.0f,1.0f);

	//float mix2=clamp((IN.position.y-110.0f)/150.0f,0.0f,1.0f);
	//float mix2=clamp((IN.position.y-150.0f)/100.0f,0.0f,1.0f);

	//float mix3=clamp((IN.position.y)/100.0f,0.0f,1.0f);
       // float mix4=clamp((IN.position.y-50.0f)/70.0f,0.0f,1.0f);

	//vec4 min=texture(diffuseTex,IN.texCoord);
	gl_FragColor = texture(diffuseTex, IN.texCoord);

	//vec3 q =mix(shadow,min.rgb,mix1);
	//vec3 q1=mix(q,snow,mix2);
	//vec3 q2=mix(water,q1,mix3);
        //vec3 q3=mix(q2,what,mix4);

        //gl_FragColor = gl_FragColor*IN.colour;
	//gl_FragColor.rgb=q2;
}