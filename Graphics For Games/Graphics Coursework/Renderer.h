#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "ParticleEmitter.h"	
#include "TextMesh.h" 

#include <sstream>
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

class Renderer : public OGLRenderer
{
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void GetFPS(float msec);
	void GetMemory();


protected:
	//add a light to lights vector
	void AddLight(Vector3 position, Vector4 colour, float radius);
	void SetMoreLight();

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();

	void DrawShadowScene(); // For shadow
	void DrawCombinedScene(); // For shadow

	void DrawParticle();

	void DrawWords();
	void DrawText(const string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	void	SetShaderParticleSize(float f);//And a new setter

	Shader           * lightShader;
	Shader           * reflectShader;
	Shader           * skyboxShader;
	Shader           * sceneShader;// For shadow
	Shader           * shadowShader;// For shadow
	Shader           * particleShader;//For particle
	Shader           * textShader;//For text

	HeightMap        * heightMap;
	Mesh             * quad;
		             
	Light            * light;
	Camera           * camera;
	
	//particle 
	ParticleEmitter  * emitter_spring;	 //A single particle emitter
	ParticleEmitter  * emitter_fog;      //A single particle emitter
	ParticleEmitter  * emitter_fire;

	int fire_number       = 0;

	//bool  check_explotion = false;

	//skybox
	GLuint cubeMap;    //skybox01
	GLuint cubeMap2;   //skybox02
	float timeCounter; //change the picture of skybox
	bool isDayTime;    //check time for skybox

	//shadow
	GLuint shadowTex;// For shadow
	GLuint shadowFBO;// For shadow

	//tex
	float FPS;
	float myMemory;
	Font * myFont;
	bool  check_end = false;
	float words_position;

	float waterRotate;

	GLint total_mem_kb = 0;

	//multiple lights
	static const int num_light = 4;

	vector<Light*> lights;

	Vector4 lightColour[num_light];
	Vector3 lightPos[num_light];
	GLfloat lightRadius[num_light];
};
