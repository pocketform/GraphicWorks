#include "Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	camera = new Camera(0.0f, 0.0f, Vector3(
	RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));
	
	light         = NULL;
	mylight       = NULL;
	heightMap     = new HeightMap(TEXTUREDIR"terrain.raw");
	currentShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
	SHADERDIR"PerPixelFragment.glsl");
	
	 heightMap -> SetTexture(SOIL_load_OGL_texture(
	 TEXTUREDIR"BarrenReds.JPG", SOIL_LOAD_AUTO,
	 SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	 if (!currentShader -> LinkProgram() || !heightMap -> GetTexture())
	{
		 return;
	}
	
	SetTextureRepeating(heightMap -> GetTexture(), true);

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
	                           Vector4(1, 0.5, 0, 1), 
		                      (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	//add mylight
	mylight = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)),//position
		                         Vector4(0, 0, 1, 1), //color
		                        (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);//radius

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
	(float)width / (float)height, 45.0f);
	
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void)
{
	delete camera;
	delete heightMap;
	delete light;
	delete mylight;
}
void Renderer::UpdateScene(float msec)
{
	camera -> UpdateCamera(msec);
	viewMatrix = camera -> BuildViewMatrix();	
}
void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	glUseProgram(currentShader -> GetProgram());
	glUniform1i(glGetUniformLocation(currentShader  -> GetProgram(),
	"diffuseTex"), 0);
	
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(),
	"cameraPos"), 1, (float *)& camera -> GetPosition());
	
	UpdateShaderMatrices();
	SetShaderLight(*light);
	//SetShaderLight(*mylight);
	SetShaderLight1(*mylight);
	//SetShaderLight1(*mylight);
	
	heightMap -> Draw();
	
	glUseProgram(0);
	
	SwapBuffers();
}
