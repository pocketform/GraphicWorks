#include "Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	camera        = new Camera(0.0f, 0.0f, Vector3(
		                       RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));

	heightMap     = new HeightMap(TEXTUREDIR"terrain.raw");
	currentShader = new Shader(SHADERDIR"BumpVertex.glsl",
		                       SHADERDIR"bumpfragmentmorelight.glsl");

	light         = NULL;
	mylight       = NULL;

	heightMap     -> SetTexture(SOIL_load_OGL_texture(
		                        TEXTUREDIR"BarrenReds.JPG", SOIL_LOAD_AUTO, 
		                        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
			     
	heightMap     -> SetBumpMap(SOIL_load_OGL_texture(
		                        TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		                        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!currentShader -> LinkProgram())
	{
		return;
	}
	if (!heightMap     -> GetTexture())
	{
		return;
	}
	if (!heightMap     -> GetBumpMap())
	{
		return;
	}

	SetTextureRepeating(heightMap -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetBumpMap(), true);


	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		        500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		        Vector4(1, 0.5, 0, 1), (RAW_WIDTH * HEIGHTMAP_X) / 5.0f);

	//try to add a new light
	mylight = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)),//position
		                         Vector4(0, 0, 1, 1), //color
		                        (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);//radius

	AddLight(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
			 Vector4(1, 0.5, 0, 1), 
		    (RAW_WIDTH * HEIGHTMAP_X) / 5.0f);

	AddLight(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)),//position
	         Vector4(0, 0, 1, 1), //color
	        (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);//radius

	AddLight(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 5.0f)),//position
			 Vector4(1, 0, 1, 1), //color
			(RAW_WIDTH * HEIGHTMAP_X) / 2.0f);//radius

	AddLight(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.5f)),//position
		     Vector4(0, 1, 0, 1), //color
		     (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);//radius

	for (int i = 0; i < lights.size(); i++)
	{
		lightColour[i] = lights[i]->GetColour();
		lightPos[i]    = lights[i]->GetPosition();
		lightRadius[i] = lights[i]->GetRadius();
	}

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		        (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void)
{
	delete heightMap;
	delete camera;
	delete light;
	delete mylight;

	for (int i = 0; i < lights.size(); i++)
	{
		delete (lights[i]);
	}
}
void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::AddLight(Vector3 position, Vector4 colour, float radius)
{
	lights.push_back(new Light(position, colour, radius));
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader -> GetProgram());
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		                             "diffuseTex"), 0);
	//Real-Time Lighting B
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		                             "bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		         "cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "lightCount"), lights.size());

	UpdateShaderMatrices();

	for (int i = 0; i < lights.size(); i++)
	{
		ostringstream light_color_stream;
		light_color_stream << "lightColour[" << i << "]";
		string light_color_name = light_color_stream.str();

		glUniform4f(glGetUniformLocation(currentShader->GetProgram(), light_color_name.c_str()),
			        lightColour[i].x, lightColour[i].y, lightColour[i].z, lightColour[i].w);

		ostringstream light_position_stream;
		light_position_stream << "lightPos[" << i << "]";
		string light_position_name = light_position_stream.str();

		glUniform3f(glGetUniformLocation(currentShader->GetProgram(), light_position_name.c_str()),
			        lightPos[i].x, lightPos[i].y, lightPos[i].z);

		ostringstream light_radius_stream;
		light_radius_stream << "lightRadius[" << i << "]";
		string light_radius_name = light_radius_stream.str();

		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), light_radius_name.c_str()),
			        lightRadius[i]);
	}

	//SetShaderLight(*light);
	//SetShaderLight(*mylight);
	//SetShaderLight1(*light);
	//SetShaderLight1(*mylight);

	heightMap->Draw();

	glUseProgram(0);

	SwapBuffers();
}
