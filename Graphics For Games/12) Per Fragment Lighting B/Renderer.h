#pragma once
#include <sstream>
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	//try to move light
	Light MoveLight(Light mylight);

	//add a light to lights vector
	void AddLight(Vector3 position, Vector4 colour, float radius);

protected:

	static const int num_light = 4;

	Mesh   * heightMap;
	Camera * camera;

	//multiple lights
	vector<Light*> lights;

	Light  * light;
	Light  * mylight;

	Vector4 lightColour[num_light];
	Vector3 lightPos[num_light];
	GLfloat lightRadius[num_light];
};
