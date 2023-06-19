#include <iostream>
#include <cstdlib>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

constexpr GLuint WIDTH=512, HEIGHT=512;

void error_cb(GLint error, const GLchar* description);
void createGeometry(GLuint& vao, GLuint& vbo, GLuint& ebo);

int main()
{
    glfwSetErrorCallback(error_cb);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App",
                                           NULL, NULL);

    if(!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";

        exit(1);
    }

    // Setup dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup platform / renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // Setup Dear Imgui style
    ImGui::StyleColorsDark();

    GLuint renderProgram;
    ShaderProgram shaderProgram(renderProgram);
    shaderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    GLuint vao, vbo, ebo;
    createGeometry(vao, vbo, ebo);

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(renderProgram);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Render your GUI
        ImGui::Begin("Demo window");
        ImGui::Button("Hello!");
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(renderProgram);

    glfwTerminate();

    return 0;
}

void error_cb(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void createGeometry(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    std::vector<GLfloat> vertices =
    {
        // Position             // Color
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
