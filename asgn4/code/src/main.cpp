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

// Assignment 04: Raytracing

#include "imgui_setup.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
#include "material.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "lightsource.h"
#include "pointlightsource.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../depends/stb/stb_image.h"
#include "../depends/stb/stb_image_write.h"

#define RENDER_BATCH_COLUMNS 20 // Number of columns to render in a single go. Increase to gain some display/render speed!

Camera *camera;
RenderEngine *engine;

int screen_width = 800, screen_height = 600; // This is window size, used to display scaled raytraced image.
int image_width = 1920, image_height = 1080; // This is raytraced image size. Change it if needed.
GLuint texImage;

int main(int, char **)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Setup raytracer camera. This is used to spawn rays.
    Vector3D camera_position(0, 0, 10);
    Vector3D camera_target(0, 0, 0); // Looking down -Z axis
    Vector3D camera_up(0, 1, 0);
    float camera_fovy = 45;
    camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);

    // Create a world
    World *world = new World;
    world->setAmbient(Color(1));
    world->setBackground(Color(0.1, 0.3, 0.6));

    Material *m1 = new Material(world);
    m1->color = Color(0.1, 0.7, 0.0);
    m1->ka = 0.1;
    m1->kd = 0.9;
    m1->ks = 0.0;
    Material *m2 = new Material(world);
    m2->color = Color(0.7, 0.0, 0.1);
    m2->ka = 0.1;
    m2->kd = 0.9;
    m2->ks = 0.0;
    Material *m3 = new Material(world);
    m3->color = Color(1.0);
    m3->ka = 0.1;
    m3->kd = 0.9;
    m3->ks = 0.5;
    m3->n = 512;
    m3->kr = 1.0;
    Material *m4 = new Material(world);
    m4->color = Color(1.0, 1.0, 1.0);
    m4->ka = 0.1;
    m4->kd = 0.9;
    m4->ks = 0.0;
    m4->n = 500;
    m4->kr = 0.0;
    m4->kt = 1.0;
    m4->eta = 1.5;
    // m4->beersCoef = 0.00005;

    Object *sphere = new Sphere(Vector3D(1, 0, -5), 3, m1);
    world->addObject(sphere);
    Object *triangle = new Triangle(Vector3D(0, 2, 0), Vector3D(-1, 0, 0), Vector3D(1, 0, 0), m2);
    world->addObject(triangle);
    LightSource *light1 = new PointLightSource(world, Vector3D(5, 0, 20), Color(0.5, 0.5, 0.5));
    world->addLight(light1);
    LightSource *light2 = new PointLightSource(world, Vector3D(-5, 0, 20), Color(0.5, 0.5, 0.5));
    world->addLight(light2);

    // // reflection
    //  Object *sphere2 = new Sphere(Vector3D(0, -2, -6), 4, m3);
    //  world->addObject(sphere2);
    //  Object *sphere3 = new Sphere(Vector3D(7, 0, 0), 3, m2);
    //  world->addObject(sphere3);
    //  LightSource *light1 = new PointLightSource(world, Vector3D(-2, 0, 0), Color(1, 1, 1));
    //  world->addLight(light1);
    //  LightSource *light2 = new PointLightSource(world, Vector3D(2, 0, 0), Color(0.5, 0.5, 1));
    //  world->addLight(light2);

    // // refraction
    // Object *sphere = new Sphere(Vector3D(-2, -3, -10), 3, m1);
    // world->addObject(sphere);
    // Object *triangle = new Triangle(Vector3D(-2, -3, -5), Vector3D(2, -3, -7), Vector3D(2, 3, -7), m2);
    // world->addObject(triangle);
    // Object *sphere2 = new Sphere(Vector3D(0, -2, 0), 2, m4);
    // world->addObject(sphere2);
    // LightSource *light = new PointLightSource(world, Vector3D(-2, 0, 0), Color(0.5, 0.5, 0.5));
    // world->addLight(light);
    // LightSource *light2 = new PointLightSource(world, Vector3D(2, 0, 0), Color(0.5, 0.5, 1));
    // world->addLight(light2);


    engine = new RenderEngine(world, camera);

    // Initialise texture
    glGenTextures(1, &texImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool render_status;
        for (int i = 0; i < RENDER_BATCH_COLUMNS; i++)
            render_status = engine->renderLoop(); // RenderLoop() raytraces 1 column of pixels at a time.
        if (!render_status)
        {
            // Update texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texImage);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
        }

        ImGui::Begin("Lumina", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Size: %d x %d", image_width, image_height);
        if (ImGui::Button("Save"))
        {
            char filename[] = "img.png";
            stbi_write_png(filename, image_width, image_height, 3, camera->getBitmap(), 0);
        }
        // Display render view - fit to width
        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);
        float image_aspect = (float)image_width / (float)image_height;
        float frac = 0.95; // ensure no horizontal scrolling
        ImGui::Image((void *)(intptr_t)texImage, ImVec2(frac * win_w, frac * win_w / image_aspect), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &texImage);

    cleanup(window);

    return 0;
}
