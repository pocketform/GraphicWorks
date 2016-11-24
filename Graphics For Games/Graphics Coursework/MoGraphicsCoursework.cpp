#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Renderer.h"

int main()
{
	Window w("Mo's CourseWork", 800, 600, false);
	if (!w.HasInitialised())
	{
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised())
	{
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		float time = w.GetTimer() -> GetTimedMS();
		renderer.UpdateScene(time);
		renderer.GetFPS(time);
		renderer.RenderScene();
	}

	return 0;
}