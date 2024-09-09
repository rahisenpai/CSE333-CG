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

#include "utils.h"

#define DRAW_QUADRATIC_BEZIER 1 // Use to switch Linear and quadratic bezier curves
#define SAMPLES_PER_BEZIER 10 //Sample each Bezier curve as N=10 segments and draw as connected lines

// GLobal variables
std::vector<float> controlPoints;
std::vector<float> linearBezier;
std::vector<float> quadraticBezier;
std::vector<float> phantomPoints;
int width = 640, height = 640; 
bool controlPointsUpdated = false;
bool phantomPointsUpdated = false;
bool controlPointsFinished = false;
int selectedControlPoint = -1;
int selectedPhantomPoint = -1;

void calculatePiecewiseLinearBezier()
{
    // Since linearBezier is just a polyline, we can just copy the control points and plot.
    // However to show how a piecewise parametric curve needs to be plotted, we sample t and 
    // evaluate all linear bezier curves.
    linearBezier.assign(controlPoints.begin(), controlPoints.end());

    // linearBezier.clear();
    // int sz  = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
    // float x[2], y[2];
    // float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    // float t;
    // for(int i=0; i<(sz-3); i+=3) {
    //     x[0] = controlPoints[i];
    //     y[0] = controlPoints[i+1];
    //     x[1] = controlPoints[i+3];
    //     y[1] = controlPoints[i+4];
    //     linearBezier.push_back(x[0]);
    //     linearBezier.push_back(y[0]);
    //     linearBezier.push_back(0.0);
    //     t = 0.0;
    //     for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++)
    //     {
    //     t += delta_t;
    //     linearBezier.push_back(x[0] + t*(x[1] - x[0]));
    //     linearBezier.push_back(y[0] + t*(y[1] - y[0]));
    //     linearBezier.push_back(0.0);
    //     }
    //     // No need to add the last point for this segment, since it will be added as first point in next.
    // }
    // // However, add last point of entire piecewise curve here (i.e, the last control point)
    // linearBezier.push_back(x[1]);
    // linearBezier.push_back(y[1]);
    // linearBezier.push_back(0.0);
}

void calculatePiecewiseQuadraticBezier()
{
    // TODO
    phantomPoints.clear(); //clear phantom points
    quadraticBezier.clear(); //clear current points

    int sz  = controlPoints.size(); //contains x,y,z for each point
    float x[3], y[3]; //to store p0,p1,p2
    //t and delta_t help to make segments for the quadratic bezier curve
    float t;
    float delta_t = 1.0 / (SAMPLES_PER_BEZIER - 1.0);

    int num_points = sz/3; //number of points on canvas
    if(num_points <= 2) {
        quadraticBezier.assign(controlPoints.begin(), controlPoints.end());
        return; //special case for only 2 points
    }

    //add first point
    quadraticBezier.push_back(controlPoints[0]);
    quadraticBezier.push_back(controlPoints[1]);
    quadraticBezier.push_back(0.0);

    //we need to find first phantom point by ourselves and that will define curve ahead
    //we take the point on the perpendicular bisector of p0p2 at a distance of |p0p2|/4 from midpoint
    //p0 = {x[0], y[0]}
    x[0] = controlPoints[0]; y[0] = controlPoints[1];
    //p2 = {x[2], y[2]}
    x[2] = controlPoints[3]; y[2] = controlPoints[4];
    //midpoint = (mx, my)
    float mx = (x[0]+x[2]) / 2.0f; float my = (y[0]+y[2]) / 2.0f;
    //direction vector of p0p2
    float dx = x[2] - x[0]; float dy = y[2] - y[0];
    //length of p0p2
    float length = std::sqrt(dx*dx + dy*dy);
    //direction vector of normal to p0p2
    float perp_dx = dy / length;
    float perp_dy = -dx / length;
    //the estimated phantom point
    phantomPoints.push_back(mx + perp_dx * (length/4));
    phantomPoints.push_back(my + perp_dy * (length/4));
    phantomPoints.push_back(0.0);

    int i=0; //i is used to access user-defined control points
    int j=0; //j is used to access phantom control points
    //if num_points>2, then we will have n-1 quadratic bezier curves
    while(--num_points) {
        //p0 = {x[0], y[0]}
        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        //p2 = {x[2], y[2]}
        x[2] = controlPoints[i+3];
        y[2] = controlPoints[i+4];

        //p1 = {x[1], y[1]}
        if(j==0) { //use the phantom point we decided
            x[1] = phantomPoints[j];
            y[1] = phantomPoints[j+1];
        }
        else { //calculate the new phantom point maintaining g1 continuity
            x[1] = 2*x[0] - phantomPoints[j-3];
            y[1] = 2*y[0] - phantomPoints[j-2];
            phantomPoints.push_back(x[1]);
            phantomPoints.push_back(y[1]);
            phantomPoints.push_back(0.0);
        }

        t = 0.0;
        for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++) {
            t += delta_t;
            float dx = (1-t)*(1-t)*x[0] + 2*(1-t)*t*x[1] + t*t*x[2];
            float dy = (1-t)*(1-t)*y[0] + 2*(1-t)*t*y[1] + t*t*y[2];

            quadraticBezier.push_back(dx);
            quadraticBezier.push_back(dy);
            quadraticBezier.push_back(0.0);
        }

        //add p2 of this curve curve
        quadraticBezier.push_back(x[2]);
        quadraticBezier.push_back(y[2]);
        quadraticBezier.push_back(0.0);

        addPhantomPoints(phantomPoints, width, height);
        i += 3;
        j += 3;
    }
}

void reCalculatePiecewiseQuadraticBezier() {
    if(selectedPhantomPoint == -1) return;
    int npts  = phantomPoints.size()/3;
    float x[3], y[3];
    float px = phantomPoints[3*selectedPhantomPoint];
    float py = phantomPoints[3*selectedPhantomPoint+1];
    std::vector<float> temp(phantomPoints);

    phantomPoints.clear(); //clear phantom points
    quadraticBezier.clear(); //clear current points

    //rectify previous phantom points except if first phantom is selected
    //because it has no previous phantom points, idx-- handles this
    int idx = selectedPhantomPoint;
    while(idx > 0) {
        //user-defined control point
        x[1] = controlPoints[3*idx];
        y[1] = controlPoints[3*idx+1];
        //phantom point
        x[2] = temp[3*idx];
        y[2] = temp[3*idx+1];
        //new point to maintain collinearity and G1 continuity
        x[0] = 2*x[1] - x[2];
        y[0] = 2*y[1] - y[2];
        //update phantom points in reverse order
        phantomPoints.insert(phantomPoints.begin(), 0.0);
        phantomPoints.insert(phantomPoints.begin(), y[0]);
        phantomPoints.insert(phantomPoints.begin(), x[0]);
        idx--;
    }

    //add updated point by the user
    phantomPoints.push_back(px);
    phantomPoints.push_back(py);
    phantomPoints.push_back(0.0);

    //rectify next phantom points except if last phantom is selected
    idx = selectedPhantomPoint;
    while(idx < npts-1) {
        //user-defined control point
        x[1] = controlPoints[3*idx+3];
        y[1] = controlPoints[3*idx+4];
        //phantom point
        x[0] = phantomPoints[3*idx];
        y[0] = phantomPoints[3*idx+1];
        //new point to maintain collinearity and G1 continuity
        x[2] = 2*x[1] - x[0];
        y[2] = 2*y[1] - y[0];
        phantomPoints.push_back(x[2]);
        phantomPoints.push_back(y[2]);
        phantomPoints.push_back(0.0);
        idx++;
    }

    addPhantomPoints(phantomPoints, width, height);

    int sz = controlPoints.size();
    //t and delta_t help to make segments for the quadratic bezier curve
    float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    float t;

    int num_points = sz/3; //number of points on canvas
    if(num_points <= 2) {
        quadraticBezier.assign(controlPoints.begin(), controlPoints.end());
        return; //special case for only 2 points
    }

    //add first point
    quadraticBezier.push_back(controlPoints[0]);
    quadraticBezier.push_back(controlPoints[1]);
    quadraticBezier.push_back(0.0);

    for(int i=0; i<(sz-3); i+=3) {
        //p0 = {x[0], y[0]}
        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        //p2 = {x[2], y[2]}
        x[2] = controlPoints[i+3];
        y[2] = controlPoints[i+4];
        //p1 = {x[1], y[1]}
        x[1] = phantomPoints[i];
        y[1] = phantomPoints[i+1];

        t = 0.0;
        for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++) {
            t += delta_t;
            float dx = (1-t)*(1-t)*x[0] + 2*(1-t)*t*x[1] + t*t*x[2];
            float dy = (1-t)*(1-t)*y[0] + 2*(1-t)*t*y[1] + t*t*y[2];

            quadraticBezier.push_back(dx);
            quadraticBezier.push_back(dy);
            quadraticBezier.push_back(0.0);
        }

        //add p2 of this curve curve
        quadraticBezier.push_back(x[2]);
        quadraticBezier.push_back(y[2]);
        quadraticBezier.push_back(0.0);
    }
}

int main(int, char* argv[])
{
    GLFWwindow* window = setupWindow(width, height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
	glUseProgram(shaderProgram);

    // Create VBOs, VAOs
    unsigned int VBO_controlPoints, VBO_linearBezier, VBO_quadraticBezier, VBO_phantomPoints;
    unsigned int VAO_controlPoints, VAO_linearBezier, VAO_quadraticBezier, VAO_phantomPoints;
    glGenBuffers(1, &VBO_controlPoints);
    glGenVertexArrays(1, &VAO_controlPoints);
    glGenBuffers(1, &VBO_linearBezier);
    glGenVertexArrays(1, &VAO_linearBezier);
    //TODO:
    //generating VAO and VBO for phantom points
    glGenBuffers(1, &VBO_phantomPoints);
    glGenVertexArrays(1, &VAO_phantomPoints);
    //generating VAO and VBO for quadratic bezier curve
    glGenBuffers(1, &VBO_quadraticBezier);
    glGenVertexArrays(1, &VAO_quadraticBezier);

    int button_status = 0;

    //Display loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        showOptionsDialog(controlPoints, io);
        ImGui::Render();

        // Add a new point on mouse click
        float x,y ;
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);


            if(!ImGui::IsAnyItemActive()) {
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    x = io.MousePos.x;
                    y = io.MousePos.y;
                    if(!controlPointsFinished) { // Add points
                        addControlPoint(controlPoints, x, y, width, height);
                        controlPointsUpdated = true;
                    } else { // Select point
                        searchNearestControlPoint(x, y);
                        searchNearestPhantomPoint(x, y);
                    } 
                }
                
                if(ImGui::IsMouseDragging(ImGuiMouseButton_Left) && controlPointsFinished) { // Edit points
                    if(selectedControlPoint >=0) {
                        x = io.MousePos.x;
                        y = io.MousePos.y;
                        editControlPoint(controlPoints, x, y, width, height);
                        controlPointsUpdated = true;
                    }
                    if(selectedPhantomPoint >=0) {
                        x = io.MousePos.x;
                        y = io.MousePos.y;
                        editPhantomPoint(phantomPoints, x, y, width, height);
                        phantomPointsUpdated = true;
                    }
                }
                
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)) { // Stop adding points
                    controlPointsFinished = true;
                }
            }

        if(controlPointsUpdated) {
            // Update VAO/VBO for control points (since we added a new point)
            glBindVertexArray(VAO_controlPoints);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_controlPoints);
            glBufferData(GL_ARRAY_BUFFER, controlPoints.size()*sizeof(GLfloat), &controlPoints[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise linear Bezier curve (since we added a new point)
            calculatePiecewiseLinearBezier();
            glBindVertexArray(VAO_linearBezier);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_linearBezier);
            glBufferData(GL_ARRAY_BUFFER, linearBezier.size()*sizeof(GLfloat), &linearBezier[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise quadratic Bezier curve
            // TODO:
            //get the points for the curves
            calculatePiecewiseQuadraticBezier();
            //binding the VAO
            glBindVertexArray(VAO_quadraticBezier);
            //binding the VBO and copying vertex data
            glBindBuffer(GL_ARRAY_BUFFER, VBO_quadraticBezier);
            glBufferData(GL_ARRAY_BUFFER, quadraticBezier.size()*sizeof(GLfloat), &quadraticBezier[0], GL_DYNAMIC_DRAW);
            //configure vertex attribute pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            //update buffers for phantom points
            glBindVertexArray(VAO_phantomPoints);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_phantomPoints);
            glBufferData(GL_ARRAY_BUFFER, phantomPoints.size()*sizeof(GLfloat), &phantomPoints[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        
            controlPointsUpdated = false; // Finish all VAO/VBO updates before setting this to false.
        }

        //different computation if we update the phantom points
        if(phantomPointsUpdated) {
            reCalculatePiecewiseQuadraticBezier();
            //binding the VAO
            glBindVertexArray(VAO_quadraticBezier);
            //binding the VBO and copying vertex data
            glBindBuffer(GL_ARRAY_BUFFER, VBO_quadraticBezier);
            glBufferData(GL_ARRAY_BUFFER, quadraticBezier.size()*sizeof(GLfloat), &quadraticBezier[0], GL_DYNAMIC_DRAW);
            //configure vertex attribute pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            //update buffers for phantom points
            glBindVertexArray(VAO_phantomPoints);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_phantomPoints);
            glBufferData(GL_ARRAY_BUFFER, phantomPoints.size()*sizeof(GLfloat), &phantomPoints[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            phantomPointsUpdated = false;
        }

        glUseProgram(shaderProgram);

        // Draw control points
        glBindVertexArray(VAO_controlPoints);
        glDrawArrays(GL_POINTS, 0, controlPoints.size()/3); // Draw points

#if DRAW_QUADRATIC_BEZIER
        // TODO:
        //draw quadratic Bezier
        //binding the VAO for quadratic bezier curve
        glBindVertexArray(VAO_quadraticBezier);
        //draw segments of the bezier curve
        glDrawArrays(GL_LINE_STRIP, 0, quadraticBezier.size()/3);

        if(controlPointsFinished) {
            //draw phantom control points
            glBindVertexArray(VAO_phantomPoints);
            glDrawArrays(GL_POINTS, 0, phantomPoints.size()/3);
            //draw tangents at joins
            glBindVertexArray(VAO_phantomPoints);
            glDrawArrays(GL_LINE_STRIP, 0, phantomPoints.size()/3);
        }

#else
        // Draw linear Bezier
        glBindVertexArray(VAO_linearBezier);
        glDrawArrays(GL_LINE_STRIP, 0, linearBezier.size()/3); //Draw lines

#endif
        glUseProgram(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    // Delete VBO buffers
    glDeleteBuffers(1, &VBO_controlPoints);
    glDeleteBuffers(1, &VBO_linearBezier);
    //TODO:
    //delete vbo buffer for quadratic bezier curve and phantom points
    glDeleteBuffers(1, &VBO_quadraticBezier);
    glDeleteBuffers(1, &VBO_phantomPoints);

    // Cleanup
    cleanup(window);
    return 0;
}
