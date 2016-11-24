#include "Renderer.h"
#include <random>
Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);

	camera             = new Camera();
	emitter_spring     = new ParticleEmitter("yun03.png");
	emitter_fog        = new ParticleEmitter("fog02.png");
	emitter_fire       = new ParticleEmitter("fire.png");

	heightMap          = new HeightMap(TEXTUREDIR"iceland.raw");
	quad               = Mesh::GenerateQuad();
	light              = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 600.0f,
								  (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)),
								   Vector4(0.9f, 0.9f, 1.0f, 1),
								  (RAW_WIDTH * HEIGHTMAP_X) * 2);

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f,
						500.0f,
						RAW_WIDTH * HEIGHTMAP_X));

	//shader
	reflectShader  = new Shader(SHADERDIR"PerPixelVertex.glsl",
				 			    "waterFregment.glsl");
	skyboxShader   = new Shader("skyboxVertex.glsl",
				  			    "skyboxFragment.glsl");
	lightShader    = new Shader("lightShadowVertex.glsl",
					 		    "lightshadowfrg.glsl");
	textShader     = new Shader(SHADERDIR"TexturedVertex.glsl", 
								SHADERDIR"TexturedFragment.glsl");
	particleShader = new Shader("vertex.glsl",
							    "fragment.glsl",
							    "geometry.glsl");


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
	if (!particleShader->LinkProgram())
	{
		return;
	}
	if (!textShader->LinkProgram())
	{
		return;
	}
	quad      ->  SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.jpg",
			 	  			 SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap ->  SetTexture(SOIL_load_OGL_texture(
			 	  			 TEXTUREDIR"mytexture00.jpg", SOIL_LOAD_AUTO,
			 	  			 SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap ->  SetTexture2(SOIL_load_OGL_texture(TEXTUREDIR"grass02.png",
			 	  			  SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	heightMap ->  SetBumpMap(SOIL_load_OGL_texture(
			 				 TEXTUREDIR"mybumpmup.jpg", SOIL_LOAD_AUTO,
			 				 SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"FullMoonLeft2048.png",    TEXTUREDIR"FullMoonRight2048.png",
									TEXTUREDIR"FullMoonUp2048.png",      TEXTUREDIR"FullMoonDown2048.png",
									TEXTUREDIR"FullMoonFront2048.png",   TEXTUREDIR"FullMoonBack2048.png",
									SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
			  
	cubeMap2  =  SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg",  TEXTUREDIR"rusted_east.jpg",
									   TEXTUREDIR"rusted_up.jpg",    TEXTUREDIR"rusted_down.jpg",
									   TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
									   SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	myFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", 
					  SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

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
	if (!heightMap->GetTexture2())
	{
		return;
	}
	if (!heightMap->GetBumpMap())
	{
		return;
	}

	SetTextureRepeating(quad      -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetTexture(), true);
	SetTextureRepeating(heightMap -> GetTexture2(), true);
	SetTextureRepeating(heightMap -> GetBumpMap(), true);

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


	waterRotate  = 0.0f;

	//try to add a new light
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

	projMatrix   = Matrix4::Perspective(1.0f, 15000.0f,
				  (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	isDayTime    = true;// check   sky box change
	timeCounter  = 0.0f;// control sky box change

	init		 = true;
}
Renderer ::~Renderer(void)
{
	for (int i = 0; i < lights.size(); i++)
	{
		delete (lights[i]);
	}

	delete emitter_spring;
	delete emitter_fog;
	delete emitter_fire;

	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete textShader;

	delete light;

	delete myFont;

	currentShader = 0;
}
void Renderer::UpdateScene(float msec)
{
	camera             -> UpdateCamera(msec);
	viewMatrix         =  camera->BuildViewMatrix();
	waterRotate        += msec / 1000.0f;
	emitter_spring     -> Update(msec);
	emitter_fog        -> Update(msec);
	emitter_fire       -> Update(msec);
}
void Renderer::AddLight(Vector3 position, Vector4 colour, float radius)
{
	lights.push_back(new Light(position, colour, radius));
}
void Renderer::SetMoreLight()
{
	for (int i = 0; i < lights.size(); i++)
	{
		ostringstream light_color_stream;
		light_color_stream << "lightColour[" << i << "]";
		string light_color_name = light_color_stream.str();

		glUniform4f(glGetUniformLocation(currentShader->GetProgram(), light_color_name.c_str()),
			//(float*)&l[i].GetPosition());
			//(float*)(*(&lights[i])).GetColour());
			lightColour[i].x, lightColour[i].y, lightColour[i].z, lightColour[i].w);

		ostringstream light_position_stream;
		light_position_stream << "lightPos[" << i << "]";
		string light_position_name = light_position_stream.str();

		glUniform3f(glGetUniformLocation(currentShader->GetProgram(), light_position_name.c_str()),
			//(float*)(lights[i])->GetPosition());
			lightPos[i].x, lightPos[i].y, lightPos[i].z);

		ostringstream light_radius_stream;
		light_radius_stream << "lightRadius[" << i << "]";
		string light_radius_name = light_radius_stream.str();

		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), light_radius_name.c_str()),
			//lights[i]->GetRadius());
			lightRadius[i]);
	}
}
void Renderer::GetFPS(float msec)
{
	FPS = 1000.0f / msec;
}
void Renderer::GetMemory()
{
	GLint cur_avail_mem_kb = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);

	GLint memoryMB = (total_mem_kb - cur_avail_mem_kb) / 1024;
	myMemory = memoryMB;

}
void Renderer::RenderScene()
{
	//glClearColor(0, 0, 0, 1);//particle
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	DrawSkybox();

	DrawShadowScene(); // First render pass for shadow...
	DrawCombinedScene(); // Second render pass for shadow ...

	DrawParticle();

	DrawWords();

	GetMemory();

	SwapBuffers();
}
void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);

	glUniform1i(glGetUniformLocation (currentShader->GetProgram (), "cubeTex"), 10);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex2"), 11);

	if (isDayTime)
	{
		timeCounter += 0.001f;

		if (timeCounter >= 1.0f)
		{
			isDayTime = !isDayTime;
		}
	}
	else
	{
		timeCounter -= 0.001f;

		if (timeCounter <= 0.0f)
		{
			isDayTime = !isDayTime;
		}
	}
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "timeCounter"), timeCounter);

	quad->Draw();

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}
void Renderer::DrawHeightmap()
{
	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	SetMoreLight();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"grass"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"bumpTex"), 1);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
				"cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"shadowTex"), 6);

	glActiveTexture(GL_TEXTURE6);
	//glActiveTexture(GL_TEXTURE2);
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

	//glEnable(GL_BLEND);

	SetMoreLight();

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
				"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	//try to change reflection of water
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"),  10);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex2"), 11);

	if (isDayTime)
	{
		timeCounter += 0.001f;

		if (timeCounter >= 1.0f)
		{
			isDayTime = !isDayTime;
		}
	}
	else
	{
		timeCounter -= 0.001f;

		if (timeCounter <= 0.0f)
		{
			isDayTime = !isDayTime;
		}
	}
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "timeCounter"), timeCounter);



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


	quad-> Mesh::Draw();

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);

	glDisable(GL_BLEND);

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
void Renderer::DrawParticle()
{
	//std::default_random_engine generator;
	//std::uniform_real_distribution<float> dis1(-1.f, 1.f);

	glEnable(GL_BLEND);
	SetCurrentShader(particleShader);

	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "pardiffuseTex"), 0);
	
	//particle for spring
	SetShaderParticleSize(emitter_spring->GetParticleSize());
	emitter_spring -> SetParticleSize(40.0f);
	emitter_spring -> SetParticleVariance(0.9f);
	emitter_spring -> SetLaunchParticles(20);
	emitter_spring -> SetParticleLifetime(50000.0f);
	emitter_spring -> SetParticleSpeed(0.3f);
	emitter_spring -> SetDirection(Vector3(0, 1.5, 0));
	emitter_spring -> SetParticle_Particle_Gracity(0.1);


	modelMatrix = Matrix4::Translation(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f) - 450.0f,
									   200.0f,
									  (RAW_WIDTH * HEIGHTMAP_X / 2.0f) + 225.0f));

	UpdateShaderMatrices();
	glDepthMask(GL_FALSE);
	emitter_spring -> Draw();
	glDepthMask(GL_TRUE);//delate the edge of particle

	//particle fog
	SetShaderParticleSize(emitter_fog->GetParticleSize());
	emitter_fog  ->  SetParticleRate(1000.0f);
	emitter_fog  ->  SetParticleSize(600.0f);
	emitter_fog  ->  SetParticleVariance(0.9f);
	emitter_fog  ->  SetLaunchParticles(2);
	emitter_fog  ->  SetParticleLifetime(150000.0f);
	emitter_fog  ->  SetParticleSpeed(0.1f);
	emitter_fog  ->  SetDirection(Vector3(0, 0.3, 0));
	modelMatrix  =   Matrix4::Translation(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f) - 450.0f,
										  100.0f,
										 (RAW_WIDTH * HEIGHTMAP_X / 2.0f) + 225.0f));


	UpdateShaderMatrices();
	glDepthMask(GL_FALSE);
	emitter_fog -> Draw();
	glDepthMask(GL_TRUE);//delate the edge of particle

	//particle for fire
	if (fire_number != 0)
	{
		fire_number = 0;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_F))
	{
		fire_number = 50;
	}
	emitter_fire -> SetParticleRate(50.0f);
	emitter_fire -> SetParticleSize(300.0f);
	emitter_fire -> SetParticleVariance(0.9f);
	emitter_fire -> SetLaunchParticles(fire_number);
	emitter_fire -> SetParticleLifetime(2000.0f);
	emitter_fire -> SetParticleSpeed(2.0f);
	emitter_fire -> SetDirection(Vector3(0, 0.5, 0));
	modelMatrix = Matrix4::Translation(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f) - 450.0f,
									   100.0f,
									  (RAW_WIDTH * HEIGHTMAP_X / 2.0f) + 225.0f));


	//particle for fire_smoke
	UpdateShaderMatrices();
	glDepthMask(GL_FALSE);
	emitter_fire->Draw();
	glDepthMask(GL_TRUE);//delate the edge of particle

	glDisable(GL_BLEND);
}

void Renderer::SetShaderParticleSize(float f)
{
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}
void Renderer::DrawWords()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	
	SetCurrentShader(textShader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	
	stringstream temp01;
	int fps = int(FPS);
	temp01 << fps;
	string s_myfps = temp01.str();
	


	stringstream temp02;
	temp02 << myMemory;
	string s_mymemory = temp02.str();


	/*glActiveTexture(GL_TEXTURE0);*/
	/*glBindTexture(GL_TEXTURE_2D, myFont->texture);*/

	Renderer::DrawText("My FPS is:" + s_myfps, Vector3(0, 0, 0), 16.0f);
	Renderer::DrawText("My memory is:" + s_mymemory + "MB", Vector3(0, 15, 0), 16.0f);
	Renderer::DrawText("Welcome to MO's world  ( 0 v 0)b ", Vector3(500, 1300, 1500), 80.0f, true);
	Renderer::DrawText("Push 'F' to get fire   ( 0 v 0)b ", Vector3(500, 1500, 1500), 80.0f, true);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E))
	{
		check_end = true;
		words_position = 2000.0f;
	}
	if (check_end == true)
	{
	Renderer::DrawText("Thanks for you watching m(0 V 0)m", camera->GetPosition() + Vector3(-1500, words_position, -3000), 80.0f, true);
	words_position -= 10.0f;
	}
	//Renderer::DrawText("Welcome to MO's world ( 0 _ 0)b", camera->GetPosition() + Vector3(-1500,500,-2000), 70.0f, true);

	//textureMatrix.ToIdentity();
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	glUseProgram(0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)
{
	
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *myFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective)
	{
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else
	{
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!

	
}