#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "ParticleEmitter.h"	//A new class!
class Renderer : public OGLRenderer
{
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();

	void DrawShadowScene(); // For shadow
	void DrawCombinedScene(); // For shadow

	void DrawParticle();

	void	SetShaderParticleSize(float f);//And a new setter

	Shader           * lightShader;
	Shader           * reflectShader;
	Shader           * skyboxShader;
	Shader           * sceneShader;// For shadow
	Shader           * shadowShader;// For shadow
	Shader           * particleShader;//For particle

	HeightMap        * heightMap;
	Mesh             * quad;
		             
	Light            * light;
	Camera           * camera;

	ParticleEmitter  * emitter_spring;	//A single particle emitter
	ParticleEmitter  * emitter_mo;      //A single particle emitter

	GLuint cubeMap;

	GLuint shadowTex;// For shadow
	GLuint shadowFBO;// For shadow

	float waterRotate;
};
