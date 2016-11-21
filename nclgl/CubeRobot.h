#pragma once
#include"SceneNode.h"
#include"OBJMesh.h"

class CubeRobot : public SceneNode
{
public:
	CubeRobot(void);
	~CubeRobot(void) {};
	virtual void Update(float msec);
	static  void CreateCube()
	{
		OBJMesh * m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		cube = m;
	}

	static Mesh* GetCube() {
		return cube;
	}

	static void DeleteCube() {
		if (cube)
		{
			delete cube;
			cube = NULL;
		}
	}

protected:
	static Mesh * cube;
	SceneNode   * body;
	SceneNode   * head;
	SceneNode   * leftArm;
	SceneNode   * rightArm;
	SceneNode   * leftLeg;
	SceneNode   * rightLeg;
};
