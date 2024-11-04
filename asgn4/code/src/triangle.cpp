//triangle.cpp

#include "triangle.h"
#include <glm/glm.hpp>

bool Triangle::intersect(Ray& r) const
{
    Vector3D e = r.getOrigin();
    Vector3D d = r.getDirection();

    glm::mat3 A(
        v1.X()-v2.X(), v1.X()-v3.X(), d.X(),
        v1.Y()-v2.Y(), v1.Y()-v3.Y(), d.Y(),
        v1.Z()-v2.Z(), v1.Z()-v3.Z(), d.Z()
    );
    double det_A = glm::determinant(A); //calculate determinant of A

    double beta = glm::determinant(glm::mat3(
        v1.X()-e.X(), v1.X()-v3.X(), d.X(),
        v1.Y()-e.Y(), v1.Y()-v3.Y(), d.Y(),
        v1.Z()-e.Z(), v1.Z()-v3.Z(), d.Z()
    ));
    beta /= det_A;

    double gamma = glm::determinant(glm::mat3(
        v1.X()-v2.X(), v1.X()-e.X(), d.X(),
        v1.Y()-v2.Y(), v1.Y()-e.Y(), d.Y(),
        v1.Z()-v2.Z(), v1.Z()-e.Z(), d.Z()
    ));
    gamma /= det_A;

    double t = glm::determinant(glm::mat3(
        v1.X()-v2.X(), v1.X()-v3.X(), v1.X()-e.X(),
        v1.Y()-v2.Y(), v1.Y()-v3.Y(), v1.Y()-e.Y(),
        v1.Z()-v2.Z(), v1.Z()-v3.Z(), v1.Z()-e.Z()
    ));
    t /= det_A;

    if (beta>0 && gamma>0 && (beta+gamma)<1){
        r.setParameter(t, this);
        r.setNormal(normal, this);
        return true; //intersection inside the triangle
    }
    return false; //ray hits plane outside the triangle
}