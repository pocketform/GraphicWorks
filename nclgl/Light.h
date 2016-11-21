#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light
{
public:
	Light() {}

	Light(Vector3 position, Vector4 colour, float radius)
	{
		this->position = position;
		this->colour   = colour;
		this->radius   = radius;
	}

	~Light(void) {};

	//void UpdateLight(float msec = 10.0f)
	//{
	//	float speed = 100.0f;

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
	//{
	//	//position += Matrix4::Translation(Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec * (speed);
	//	position.x += 1.0f * speed;
	//}																 
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))					 
	//{																 
	//	//position -= Matrix4::Translation(Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec * (speed);
	//	position.x -= 1.0f * speed;
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
	//{
	//	//position += Matrix4::Translation(Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * (speed);
	//	position.z += 1.0f * speed;
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
	//{
	//	//position -= Matrix4::Translation(Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * (speed);
	//	position.z -= 1.0f * speed;
	//}
	//}

	Vector3 GetPosition() const   { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const       { return radius; }
	void SetRadius(float val)	  { radius = val; }

	Vector4 GetColour() const	  { return colour; }
	void SetColour(Vector4 val)   { colour = val; }

protected:
	Vector3 position;
	Vector4 colour;
	float   radius;
};