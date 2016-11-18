#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
//shadow 
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
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

	void DrawMesh(); // For shadow
	void DrawFloor(); // For shadow
	void DrawShadowScene(); // For shadow
	void DrawCombinedScene(); // For shadow

	Shader    * lightShader;
	Shader    * reflectShader;
	Shader    * skyboxShader;
		      
	Shader    * sceneShader;// For shadow
	Shader    * shadowShader;// For shadow
	MD5FileData * hellData;//tex shadow
	MD5Node     * hellNode;//tex shadow
	Mesh        * floor;//tex shadow

	HeightMap * heightMap;
	Mesh      * quad;
		      
	Light     * light;
	Camera    * camera;

	GLuint cubeMap;

	GLuint shadowTex;// For shadow
	GLuint shadowFBO;// For shadow

	float waterRotate;
};
