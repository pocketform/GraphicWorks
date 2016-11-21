#include "Renderer.h"


Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	camera    = new Camera();
	heightMap = new HeightMap(TEXTUREDIR"iceland.raw");
	quad      = Mesh::GenerateQuad();
	light     = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 600.0f,
						 (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)),
						 Vector4(0.9f, 0.9f, 1.0f, 1),
						 (RAW_WIDTH * HEIGHTMAP_X) * 2);

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f,
						500.0f,
						RAW_WIDTH * HEIGHTMAP_X));

	//shader
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
							   "waterFregment.glsl");
	skyboxShader  = new Shader("skyboxVertex.glsl",
				 			   "skyboxFragment.glsl");
	lightShader   = new Shader("lightShadowVertex.glsl",
					 		   "lightshadowfrg.glsl");

	if (!reflectShader->LinkProgram())
	{
		return;
	}
	if (!lightShader->LinkProgram())
	{
		return;
	}
	if (!skyboxShader->LinkProgram())
	{
		return;
	}

	//initialize shadow
	glGenTextures(1, &shadowTex);// get a shadowtex
	glBindTexture(GL_TEXTURE_2D, shadowTex);// bind shadowtex
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 2048/*shadow width*/, 2048/*shadow height*/, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);// disbind shadowtex

	glGenFramebuffers(1, &shadowFBO);// get a shadowFBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);// bind shadowFBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						   GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);//no colour so draw none
	glReadBuffer(GL_NONE);//no colour so Read none
	glBindFramebuffer(GL_FRAMEBUFFER, 0);// disbind shadowFBO

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.jpg",
					 SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//heightMap->SetTexture(SOIL_load_OGL_texture(
	//					  TEXTUREDIR"BarrenReds.JPG", SOIL_LOAD_AUTO,
	//					  SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture(
						  TEXTUREDIR"mytexture00.jpg", SOIL_LOAD_AUTO,
						  SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//heightMap->SetBumpMap(SOIL_load_OGL_texture(
	//					  TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
	//					  SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(
						  TEXTUREDIR"mybumpmap.jpg", SOIL_LOAD_AUTO,
						  SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg",  TEXTUREDIR"rusted_east.jpg",
									TEXTUREDIR"rusted_up.jpg",    TEXTUREDIR"rusted_down.jpg",
									TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
									SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap)
	{
		return;
	}
	if (!quad->GetTexture())
	{
		return;
	}
	if (!heightMap->GetTexture())
	{
		return;
	}
	if (!heightMap->GetBumpMap())
	{
		return;
	}

	SetTextureRepeating(quad      -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetBumpMap(), true);

	init		 = true;
	waterRotate  = 0.0f;

	projMatrix   = Matrix4::Perspective(1.0f, 15000.0f,
				  (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void)
{
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;

	currentShader = 0;
}
void Renderer::UpdateScene(float msec)
{
	camera      -> UpdateCamera(msec);
	viewMatrix  =  camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();

	DrawShadowScene(); // First render pass for shadow...
	DrawCombinedScene(); // Second render pass for shadow ...

	SwapBuffers();
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}
void Renderer::DrawHeightmap()
{
	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"bumpTex"), 1);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
				"cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"shadowTex"), 3);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowTex);//bind shadowTex


	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	//textureMatrix = Matrix4::BuildViewMatrix(
	//light->GetPosition(), Vector3(0, 0, 0));

	//shadowMatrix.ToIdentity();
	//shadowMatrix = Matrix4::BuildViewMatrix (light -> GetPosition(), Vector3(0, 0, 0));

	// change for shadow
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
						,"shadowMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
						,"modelMatrix"), 1, false, *& modelMatrix.values);
	
	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
}
void Renderer::DrawWater()
{
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	//glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
	//			"cameraPos"), 1, (float *)& camera->GetPosition());

	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
	//			"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	//float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightX = (RAW_WIDTH * HEIGHTMAP_X * 2);

	float heightY = (256 * HEIGHTMAP_Y / 3.0f) + 20.0f;

	//float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z * 2);

	modelMatrix   = Matrix4::Translation(Vector3(heightX / 2, heightY, heightZ / 2)) *
				    Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
				    Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
					Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	//change for shadow
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
						,"shadowMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
						,"modelMatrix"), 1, false, *& modelMatrix.values);

	UpdateShaderMatrices();

	//try to make mirrored repeat
	glBindTexture(GL_TEXTURE_2D, quad->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, //x axis
					true ? GL_MIRRORED_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, //y axis
					true ? GL_MIRRORED_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	quad->Draw();

	glUseProgram(0);
}
void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);// bind shadowFBO

	glClear(GL_DEPTH_BUFFER_BIT);// clear area without shadow
								 // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, 2048, 2048);// keep same size

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//glColorMask(1, 1, 1, 1);

	//original 
	//SetCurrentShader(shadowShader);
	viewMatrix    = Matrix4::BuildViewMatrix(
				    light->GetPosition(), Vector3(0, 0, 0));

	shadowMatrix = biasMatrix *(projMatrix * viewMatrix);

	UpdateShaderMatrices();

	DrawWater();
	DrawHeightmap();


	//glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);// disbind shadowFBO
}
void Renderer::DrawCombinedScene()
{
	//SetCurrentShader(sceneShader);
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
	//			"diffuseTex"), 0);
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
	//			"bumpTex"), 1);
	//glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
	//			"cameraPos"), 1, (float *)& camera->GetPosition());

	//SetShaderLight(*light);


	//glBindTexture(GL_TEXTURE_2D, shadowTex);//bind shadowTex

	viewMatrix = camera -> BuildViewMatrix();
	//textureMatrix = Matrix4::BuildViewMatrix(
	//light->GetPosition(), Vector3(0, 0, 0));
	UpdateShaderMatrices();

	DrawWater();
	DrawHeightmap();


	//glUseProgram(0);
}
