//triangle.h
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"

class Triangle : public Object
{
private:
	Vector3D v1, v2, v3; //vertices of triangle
	Vector3D normal;    //normal of triangle

public:
	Triangle(const Vector3D& _v1, const Vector3D& _v2, const Vector3D& _v3, Material* mat):
		Object(mat), v1(_v1), v2(_v2), v3(_v3)
	{
		normal = crossProduct(v2 - v1, v3 - v1);  //calculate normal
		normal.normalize();
		isSolid = true;
	}

	virtual bool intersect(Ray& r) const;
};
#endif
