 #include "utils.h"

int main()
{
    // Setup window
    GLFWwindow *window = setupWindow(720, 480); // (Width, Height) PLAY AROUND WITH THIS
    ImGuiIO& io = ImGui::GetIO(); // Create IO object
    
    bool * checkbox = new bool(false);
    float sliderValue = 0.0f; // Initial value for the slider

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            // ImGui widget example
            ImGui::Begin("Widget Window"); // PLAY AROUND WITH THIS
            ImGui::Text("Welcome to ImGui with OpenGL!"); // PLAY AROUND WITH THIS
            ImGui::End();
        }

        {
            // ImGui widget example
            ImGui::Begin("Widget Window"); // PLAY AROUND WITH THIS
            // Add a slider
            ImGui::SliderFloat("Adjust Value", &sliderValue, 0.0f, 1.0f); // Slider range from 0.0 to 1.0
            ImGui::End();
        }

        {
            // ImGui widget example
            ImGui::Begin("Widget Window"); // PLAY AROUND WITH THIS
            // Add a button
            if (ImGui::Button("Click Me")) {
                // Action to take when the button is clicked
                *checkbox = !*checkbox; // Toggle the checkbox as an example action
            }
            ImGui::End();
        }

        {
            // ImGui widget example
            ImGui::Begin("Widget Window"); // PLAY AROUND WITH THIS
            // Add a progress bar
            ImGui::ProgressBar(sliderValue); // Progress bar value
            ImGui::End();
        }

        {
            // ImGui widget example
            ImGui::Begin("Widget Window"); 
            ImGui::Checkbox("Lab completed!!", checkbox); 
            ImGui::End();
        }

        // Rendering
        ImGui::Render(); // renders the ImGui frame and prepares the draw data.
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h); // retrieves the size of the framebuffer of the window.
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // PLAY AROUND WITH THIS
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // uses the OpenGL backend to render the draw data generated by ImGui onto the screen.
        glfwSwapBuffers(window); // swaps the front and back buffers of the specified window.
    }

    // Cleanup ImGui and GLFW
    cleanup(window);

    return 0;
}