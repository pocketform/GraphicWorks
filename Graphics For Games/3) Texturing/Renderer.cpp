#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();

	triangle->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg",
		                                       SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	if (!triangle->GetTexture())
	{
		return;
	}

	triangle->SetTexture2(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",
		                                        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	if (!triangle->GetTexture2())
	{
		return;
	}

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		                       SHADERDIR"texturedfragment.glsl");

	if (!currentShader->LinkProgram())
	{
		return;
	}

	init = true;

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, 1, -1);

	filtering = true;
	repeating = false;

}

Renderer ::~Renderer(void)
{
	delete triangle;
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"myText"), 1);
	triangle->Draw();

	glUseProgram(0);
	SwapBuffers();
}

void Renderer::UpdateTextureMatrix(float value)
{
	Matrix4 pushPos  = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 popPos   = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(1, 1, 1));
	textureMatrix = pushPos * rotation * popPos;
}

void Renderer::ToggleRepeating()
{
	repeating = !repeating;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture2());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, //x axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, //y axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::ToggleFiltering()
{
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture2());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
