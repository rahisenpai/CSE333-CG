/**
 * @file main.cpp
 * @brief This code renders a cuboid

 * @author Aarya Patel
 * @date September 6, 2024
 */

#include "utils.h"

//Globals
int screen_width=720, screen_height=480;
GLint vModel_uniform, vView_uniform, vProjection_uniform;
glm::mat4 modelT, viewT, projectionT;// Model, View and Projection Transformation Matrices

double oldX, oldY, currentX, currentY;
bool isDragging=false;

void createCubeObjectUniformWay(unsigned int &, unsigned int &);
void createCubeObjectAttributeWay(unsigned int &, unsigned int &);

void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &);
glm::vec3 getTrackBallVector(double x, double y);

int main()
{
    // Setup Window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO Object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // define where the shader files are located
    unsigned int shaderProgram = createProgram("./shaders/vshader_uniform.vs", "./shaders/fshader.fs");

    glUseProgram(shaderProgram);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    setupModelTransformation(shaderProgram);
    setupViewTransformation(shaderProgram);
    setupProjectionTransformation(shaderProgram);

    createCubeObjectUniformWay(shaderProgram, VAO);
    // createCubeObjectAttributeWay(shaderProgram, VAO);

    // Colors for each unique vertex of the cuboid
    // glm::vec3 vertexColors = glm::vec3(1.0f, 0.0f, 0.0f);  // Vertex 0: Red

    // --------------------- Comment this when using GL Attribute and uncomment for GL Uniforms ---------------------    
    // Set the uniform array in the shader
    int vertexColors_location = glGetUniformLocation(shaderProgram, "vertexColors");
    if (vertexColors_location == -1) {
        fprintf(stderr, "Could not bind location: vertexColors\n");
        exit(0);
    }
    // glUniform3fv(vertexColors_location, 1, glm::value_ptr(vertexColors));


    oldX = oldY = currentX = currentY = 0.0;
    int prevLeftButtonState = GLFW_RELEASE;

    float red = 1.0;
    float blue = 0.0;
    float tranformation_rate = 0.01;
    bool forward = true;

    while (!glfwWindowShouldClose(window))
    {
        if(forward) {
            red -= tranformation_rate;
            blue += tranformation_rate;
            if(red <= 0.0) {
                forward = false;
            }
        }
        else {
            blue -= tranformation_rate;
            red += tranformation_rate;
            if(blue <= 0.0) {
                forward = true;
            }
        }

        glm::vec3 vertexColors = glm::vec3(red, 0.0f, blue);
        glUniform3fv(vertexColors_location, 1, glm::value_ptr(vertexColors));

        glfwPollEvents();

        // Get current mouse position
        int leftButtonState = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
        double x,y;
        glfwGetCursorPos(window,&x,&y);
        if(leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_RELEASE){
            isDragging = true;
            currentX = oldX = x;
            currentY = oldY = y;
        }
        else if(leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_PRESS){
            currentX = x;
            currentY = y;
        }
        else if(leftButtonState == GLFW_RELEASE && prevLeftButtonState == GLFW_PRESS){
            isDragging = false;
        }

        // Rotate based on mouse drag movement
        prevLeftButtonState = leftButtonState;
        if(isDragging && (currentX !=oldX || currentY != oldY))
        {
            glm::vec3 va = getTrackBallVector(oldX, oldY);
            glm::vec3 vb = getTrackBallVector(currentX, currentY);

            float angle = acos(std::min(1.0f, glm::dot(va,vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
            glm::mat3 camera2object = glm::inverse(glm::mat3(viewT*modelT));
            glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
            modelT = glm::rotate(modelT, angle, axis_in_object_coord);
            glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

            oldX = currentX;
            oldY = currentY;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(shaderProgram);

        {
            ImGui::Begin("Information");                          
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO); 
        
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    cleanup(window);
    return 0;
}

void createCubeObjectUniformWay(unsigned int &program, unsigned int &cube_VAO)
{
    glUseProgram(program);

    //Bind Shader Variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }

    // Vertex Data
    GLfloat cube_vertices[] = {0, 0, 0,
                                10, 0, 0,
                                10, 20, 0,
                                0, 20, 0,
                                0, 0, 10,
                                10, 0, 10,
                                10, 20, 10,
                                0, 20, 10
                                };

    // Indice Order
    GLushort cube_indices[] = {0, 1, 2,
                                0, 2, 3,
                                4, 5, 6,
                                4, 6, 7,
                                0, 4, 7,
                                0, 7, 3,
                                1, 5, 6,
                                1, 6, 2,
                                0, 1, 5,
                                0, 5, 4,
                                3, 2, 6,
                                3, 6, 7
                                };

    int nVertices = 6*2*3 ; //(1 face) * (2 triangles each) * (3 vertices each)
    GLfloat *expanded_vertices = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        expanded_vertices[i*3] = cube_vertices[cube_indices[i]*3];
        expanded_vertices[i*3 + 1] = cube_vertices[cube_indices[i]*3+1];
        expanded_vertices[i*3 + 2] = cube_vertices[cube_indices[i]*3+2];
    }
    // -----------------------------------------------------

    // Generate VAO object
    glGenVertexArrays(1, &cube_VAO);
    glBindVertexArray(cube_VAO);

    // Create VBOs for the VAO
    GLuint vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_vertices;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}

void createCubeObjectAttributeWay(unsigned int &program, unsigned int &cube_VAO)
{
    glUseProgram(program);

    // Bind Shader Variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if (vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }

    int vColor_attrib = glGetAttribLocation(program, "vColor");
    if (vColor_attrib == -1) {
        fprintf(stderr, "Could not bind location: vColor\n");
        exit(0);
    }

    // Vertex Data with color attributes
    GLfloat cube_vertices[] = {
        0, 0, 0,  1, 0, 0,  // Red
        10, 0, 0,  0, 1, 0, // Green
        10, 20, 0,  0, 0, 1, // Blue
        0, 20, 0,  1, 1, 0,  // Yellow
        0, 0, 10,  1, 0, 1,  // Magenta
        10, 0, 10, 0, 1, 1, // Cyan
        10, 20, 10, 1, 1, 1, // White
        0, 20, 10, 0, 0, 0   // Black
    };

    // Indice Order
    GLushort cube_indices[] = {
        0, 1, 2, 0, 2, 3,   // Front face
        4, 5, 6, 4, 6, 7,   // Back face
        0, 4, 7, 0, 7, 3,   // Left face
        1, 5, 6, 1, 6, 2,   // Right face
        0, 1, 5, 0, 5, 4,   // Bottom face
        3, 2, 6, 3, 6, 7    // Top face
    };

    int nVertices = 6 * 2 * 3; //(1 face) * (2 triangles each) * (3 vertices each)
    GLfloat *expanded_vertices = new GLfloat[nVertices * 6];
    for (int i = 0; i < nVertices; i++) {
        expanded_vertices[i * 6] = cube_vertices[cube_indices[i] * 6];
        expanded_vertices[i * 6 + 1] = cube_vertices[cube_indices[i] * 6 + 1];
        expanded_vertices[i * 6 + 2] = cube_vertices[cube_indices[i] * 6 + 2];
        expanded_vertices[i * 6 + 3] = cube_vertices[cube_indices[i] * 6 + 3];
        expanded_vertices[i * 6 + 4] = cube_vertices[cube_indices[i] * 6 + 4];
        expanded_vertices[i * 6 + 5] = cube_vertices[cube_indices[i] * 6 + 5];
    }
    // -----------------------------------------------------

    // Generate VAO object
    glGenVertexArrays(1, &cube_VAO);
    glBindVertexArray(cube_VAO);

    // Create VBO for vertices and colors
    GLuint vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices * 6 * sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

    // Set up the color attribute pointer
    glEnableVertexAttribArray(vColor_attrib);
    glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    delete[] expanded_vertices;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind the VAO to disable changes outside this function.
}

void setupModelTransformation(unsigned int &program)
{
    //Modelling transformations (Model -> World coordinates)
    modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

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
    //Viewing transformations (World -> Camera coordinates
    //Camera at (0, 0, 100) looking down the negative Z-axis in a right handed coordinate system
    viewT = glm::lookAt(glm::vec3(40.0, -40.0, 40.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    //Pass-on the viewing matrix to the vertex shader
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
    //Projection transformation
    projectionT = glm::perspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 0.1f, 1000.0f);

    //Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if(vProjection_uniform == -1){
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}

glm::vec3 getTrackBallVector(double x, double y)
{
	glm::vec3 p = glm::vec3(2.0*x/screen_width - 1.0, 2.0*y/screen_height - 1.0, 0.0); //Normalize to [-1, +1]
	p.y = -p.y; //Invert Y since screen coordinate and OpenGL coordinates have different Y directions.

	float mag2 = p.x*p.x + p.y*p.y;
	if(mag2 <= 1.0f)
		p.z = sqrtf(1.0f - mag2);
	else
		p = glm::normalize(p); //Nearest point, close to the sides of the trackball
	return p;
}