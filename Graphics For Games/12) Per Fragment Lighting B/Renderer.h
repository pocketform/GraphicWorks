#pragma once

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
	Light moveLight(Light mylight);

protected:
	Mesh   * heightMap;
	Camera * camera;
	Light  * light;
	Light  * mylight;
	//vector<Light>  *lights;
};
