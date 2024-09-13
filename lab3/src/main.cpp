// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

/**
 * @file main.cpp
 * @brief This code renders a cube that is locked in one view projection.

 * @author Hardik Jain
 * @date 13 Sept, 2024
 */

#include "utils.h"

#define  GLM_FORCE_RADIANS
#define  GLM_ENABLE_EXPERIMENTAL


#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

int screen_width = 640, screen_height = 640;
GLint vModel_uniform, vView_uniform, vProjection_uniform;
GLint vColor_uniform;
glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations


void createCubeObject(unsigned int &, unsigned int &);
void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &);


//global variables
bool up = true;
float del = 0.0;
    
int main(int, char**)
{
    // Setup window

    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object


    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    glUseProgram(shaderProgram);

    
    int vVertex_attrib = glGetAttribLocation(shaderProgram, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }

    int cVertex_attrib = glGetAttribLocation(shaderProgram, "vertex_colour");
    if(cVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vertex_colour\n");
        exit(0);
    }


    GLuint VAO;
    glGenVertexArrays( 1, &VAO );
    createCubeObject(shaderProgram, VAO);

    setupModelTransformation(shaderProgram);
    setupViewTransformation(shaderProgram);
    setupProjectionTransformation(shaderProgram);

    
    glEnable( GL_CULL_FACE ); // cull face
    glCullFace( GL_BACK );      // cull back face
    glFrontFace( GL_CW );           // GL_CCW for counter clock-wise
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        setupViewTransformation(shaderProgram);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(shaderProgram);

        showOptionsDialog(shaderProgram);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); 
            ImGui::SliderFloat("Adjust Value", &del, 1.0f, -1.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);



        glBindVertexArray(VAO); 
        glDrawArrays(GL_TRIANGLES, 0, 12*3);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    cleanup(window);

    return 0;
}



void createCubeObject(unsigned int &program, unsigned int &cube_VAO)
{
    glUseProgram(program);

    //Bind shader variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }
    int vColor_attrib = glGetAttribLocation(program, "vertex_colour");
    if(vColor_attrib == -1) {
        fprintf(stderr, "Could not bind location: vColor\n");
        exit(0);
    }

    //Cube data


    GLfloat cube_vertices[] = {0.125, 0.125, 0.125, -0.125, 0.125, 0.125, -0.125, -0.125, 0.125, 0.125, -0.125, 0.125, //Front
                   0.125, 0.125, -0.125, -0.125, 0.125, -0.125, -0.125, -0.125, -0.125, 0.125, -0.125, -0.125}; //Back
    GLushort cube_indices[] = {0, 2, 3, 0, 1, 2, //Front
                4, 7, 6, 4, 6, 5, //Back
                5, 2, 1, 5, 6, 2, //Left
                4, 3, 7, 4, 0, 3, //Right
                1, 0, 4, 1, 4, 5, //Top
                2, 7, 3, 2, 6, 7}; //Bottom
    GLfloat cube_colors[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1}; //Unique face colors

    //Generate VAO object
    glGenVertexArrays(1, &cube_VAO);
    glBindVertexArray(cube_VAO);

    //Create VBOs for the VAO
    //Position information (data + format)
    int nVertices = 6*2*3; //(6 faces) * (2 triangles each) * (3 vertices each)
    GLfloat *expanded_vertices = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        expanded_vertices[i*3] = cube_vertices[cube_indices[i]*3];
        expanded_vertices[i*3 + 1] = cube_vertices[cube_indices[i]*3+1];
        expanded_vertices[i*3 + 2] = cube_vertices[cube_indices[i]*3+2];
    }
    GLuint vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_vertices;

    //Color - one for each face
    GLfloat *expanded_colors = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        int color_index = i / 6;
        expanded_colors[i*3] = cube_colors[color_index*3];
        expanded_colors[i*3+1] = cube_colors[color_index*3+1];
        expanded_colors[i*3+2] = cube_colors[color_index*3+2];
    }
    GLuint color_VBO;
    glGenBuffers(1, &color_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor_attrib);
    glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_colors;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}

void setupModelTransformation(unsigned int &program)
{
    //Modelling transformations (Model -> World coordinates)
    modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates
    modelT = glm::rotate(modelT, 3.14f, glm::vec3(0.0, 1.0, 0.0));
    //Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    vModel_uniform = glGetUniformLocation(program, "vModel");
    if(vModel_uniform == -1){
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}


void setupViewTransformation(unsigned int &program)
{
    // if(up) {
    //     if(del >= 1.0) up = false;
    //     del += 0.01;
    // }
    // else {
    //     if(del <= -1.0) up = true;
    //     del -= 0.01;
    // }

    // viewT = glm::lookAt(glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //parametric equation of parabola: x=a*t*t, y=2*a*t
    viewT = glm::lookAt(glm::vec3(del*del, 2*del, 0.5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

 
    glUseProgram(program);
    vView_uniform = glGetUniformLocation(program, "vView");
    if(vView_uniform == -1){
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(unsigned int &program)
{
 
    projectionT = glm::perspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 0.1f, 1000.0f);

 
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if(vProjection_uniform == -1){
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}
