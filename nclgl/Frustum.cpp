#include"Frustum.h"

bool Frustum::InsideFrustum(SceneNode &n)
{
	for (int p = 0; p < 6; p++)
	{
		if (!planes[p].SphereInPlane(n.GetWorldTransform().GetPositionVector(),n.GetBoundingRadius()))
		{
			return false;
		}
	}
	return true;
}

void Frustum::FromMatrix(const Matrix4 &mat)
{
	Vector3 xaxis = Vector3(mat.values[0], mat.values[4], mat.values[8]);
	Vector3 yaxis = Vector3(mat.values[1], mat.values[5], mat.values[9]);
	Vector3 zaxis = Vector3(mat.values[2], mat.values[6], mat.values[10]);
	Vector3 waxis = Vector3(mat.values[3], mat.values[7], mat.values[11]);

	//RIGHT
	planes[0] = Plane(waxis - xaxis,
		             (mat.values[15] - mat.values[12]), true);

	planes[1] = Plane(waxis + xaxis,
		(mat.values[15] + mat.values[12]), true);

	planes[2] = Plane(waxis + yaxis,
		(mat.values[15] + mat.values[13]), true);

	planes[3] = Plane(waxis - yaxis,
		(mat.values[15] - mat.values[13]), true);

	planes[4] = Plane(waxis - zaxis,
		(mat.values[15] - mat.values[14]), true);

	planes[5] = Plane(waxis + zaxis,
		(mat.values[15] + mat.values[14]), true);
}