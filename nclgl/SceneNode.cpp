#include"SceneNode.h"

SceneNode::SceneNode(Mesh * mesh, Vector4 colour)
{
	//Scene Management
	parent			= NULL;
	this->mesh		= mesh;
	worldTransform	= Matrix4();
	transform		= Matrix4();
	modelScale		= Vector3(1, 1, 1);
	this->colour	= colour;
	children		= vector<SceneNode*>();

	distanceFromCamera = 0.0f;
	boundingRadius     = 1.0f;
}

SceneNode ::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}

	children.clear();
}

void SceneNode::AddChild(SceneNode * s)
{
	s->parent = this;
	children.push_back(s);
}

void SceneNode::Draw(const OGLRenderer & r)
//void SceneNode::Draw()
{
	if (mesh) { mesh->Draw(); }
}

void SceneNode::Update(float msec)
{
	if (parent)
	{ // This node has a parent ...
		worldTransform = parent->worldTransform * transform;
	}
	else
	{ // Root node , world transform is local transform !
		worldTransform = transform;
	}
	for (vector < SceneNode * >::iterator i = children.begin();
	i != children.end(); ++i)
	{
		(*i)->Update(msec);
	}
}
