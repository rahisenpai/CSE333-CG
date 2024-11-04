/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Ojaswa Sharma. All rights reserved.                    *
 *                                                                            *
 *  Author: Ojaswa Sharma                                                     *
 *  E-mail: ojaswa@iiitd.ac.in                                                *
 *                                                                            *
 *  This code is provided solely for the purpose of the CSE 333/533 course    *
 *  at IIIT Delhi. Unauthorized reproduction, distribution, or disclosure     *
 *  of this code, in whole or in part, without the prior written consent of   *
 *  the author is strictly prohibited.                                        *
 *                                                                            *
 *  This code is provided "as is", without warranty of any kind, express      *
 *  or implied, including but not limited to the warranties of                *
 *  merchantability, fitness for a particular purpose, and noninfringement.   *
 *                                                                            *
 ******************************************************************************/

#include "world.h"
#include "material.h"

#define MAX_DEPTH 50

Color Material::shade(const Ray& incident, const bool isSolid) const
{
    if (incident.getLevel() > MAX_DEPTH) {
        return Color(0, 0, 0); // Stop recursion at max depth
    }

    Vector3D norm = incident.getNormal();
    Vector3D viewVec = -incident.getDirection();
    viewVec.normalize();

    //ambient
    Color Ia = world->getAmbient() * ka;
    Color Id(0.0), Is(0.0);

    for(auto it : world->getLightSources()){
        Vector3D lightDir = it->getPosition() - incident.getPosition();
        lightDir.normalize();

        //generating shadows
        Ray shadowRay(incident.getPosition(), lightDir);
        bool underShadow = false;
        for (auto obj : world->getObjects()) {
            //if ray intersects another non-refractive object that is not current object
            if (obj != incident.intersected() && obj->intersect(shadowRay) && obj->getMaterial()->kt==0) {
                underShadow = true;
                break;
            }
        }
        if (underShadow) {
            continue;
        }

        //diffuse (lambertian shading)
        Id = Id + (it->getIntensity() * (kd * std::max(0.0, dotProduct(norm, lightDir))));
        //halfway vector
        Vector3D h = viewVec+lightDir;
        h.normalize();
        //specular
        Is = Is + (it->getIntensity() * (ks * std::max(0.0, pow(dotProduct(norm, h), n))));
    }

    Color reflective(0.0), refractive(0.0);
    if (kr > 0.0) { //reflection contribution
        Vector3D d = incident.getDirection();
        Vector3D reflectDir = d - 2 * dotProduct(d, norm) * norm;
        reflectDir.normalize();
        Ray reflectionRay(incident.getPosition(), reflectDir, incident.getLevel()+1);
        reflective = reflective + world->shade_ray(reflectionRay) * kr;
    }

    if (kt >0.0) { //refractive contribution
        double etaI = 1.0; //assuming the incident medium is air
        double etaT = eta; //refraction index of the material
        Vector3D d = incident.getDirection();
        Vector3D refractDir;
        Vector3D nor = norm;
        // Snell's law
        double cosI = -dotProduct(norm, d);
        if (cosI < 0.0) cosI = -cosI;
        else {
            std::swap(etaI, etaT); //
            nor = -norm;
        }
        double sinT2 = (etaI / etaT) * (etaI / etaT) * (1.0 - cosI * cosI);

        // Check for total internal reflection
        if (sinT2 > 1.0) {
            // Total internal reflection
            Vector3D reflectDir = d - 2 * dotProduct(d, nor) * nor;
            reflectDir.normalize();
            Ray reflectionRay(incident.getPosition(), reflectDir, incident.getLevel() + 1);
            reflective = reflective + world->shade_ray(reflectionRay) * kt;
        }
        else {
            // Calculate refraction direction
            double cosT = sqrt(1.0 - sinT2);
            refractDir = (etaI / etaT) * d + ((etaI / etaT) * cosI - cosT) * nor;
            refractDir.normalize();
            Ray refractionRay(incident.getPosition(), refractDir, incident.getLevel() + 1);

            double distance = (refractionRay.getPosition() - incident.getPosition()).length();
            double attenuation = std::exp(-distance * beersCoef);

            refractive = refractive + world->shade_ray(refractionRay) * kt * attenuation;
        }
    }

    Color outColor = color * (Ia + Id + Is + reflective + refractive);
    outColor.clamp();
    return outColor;
}
