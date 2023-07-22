#include <iostream>
#include <cstdlib>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "texture.h"

constexpr GLuint WIDTH=512, HEIGHT=512;
constexpr GLuint TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

void error_cb(GLint error, const GLchar* description);

void createModel(GLuint& vao, GLuint& vbo, GLuint& ebo);
GLuint createTextureObject(GLuint textureWidth, GLuint textureHeight);

int main()
{
    glfwSetErrorCallback(error_cb);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App",
                                          nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    // GLFW locks the refresh rate to a default of 60 fps.
    glfwSwapInterval(0);

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
    ImGui_ImplOpenGL3_Init("#version 460");

    // Setup Dear Imgui style
    ImGui::StyleColorsDark();

    // GL_MAX_COMPUTE_WORK_GROUP_SIZE contains the maximum number of work
    // groups that can be dispatched in a single call to glDispatchCompute().
    // The indices 0, 1 and 2 correspond to the x, y and z dimensions
    // respectively.
    GLint64 workGroupCount[3];
    glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
    glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
    glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

    // GL_MAX_COMPUTE_WORK_GROUP_SIZE contains the maximum number of
    // invocatoins per dimension within a workgroup.
    GLint workGroupSize[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

    // GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS contains the maximum number of
    // invocations per workgroup, i.e., the product of the x, y and z
    // dimensions of the local size must be less than this value.
    GLint64 workGroupInvocations;
    glGetInteger64v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,
                    &workGroupInvocations);

    ShaderProgram computeProgram;
    computeProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    computeProgram.compile(); GLint timeLocation = computeProgram.getLocation("time");

    ShaderProgram renderProgram;
    renderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    renderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    renderProgram.compile();

    GLint texLocation = renderProgram.getLocation("tex");

    GLuint texture = createTextureObject(TEXTURE_WIDTH, TEXTURE_HEIGHT);

    GLuint vao, vbo, ebo;
    createModel(vao, vbo, ebo);

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    GLdouble currentTime = 0.0F;
    GLdouble deltaTime   = 0.0F;
    GLdouble oldTime     = 0.0F;
    GLdouble fps         = 0.0F;
    int counter          = 0;

    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        deltaTime   = currentTime - oldTime;
        oldTime     = currentTime;

        if (counter < 500)
        {
            ++counter;
        }
        else
        {
            fps = 1 / deltaTime;
            counter = 0;
        }

        glfwPollEvents();

        computeProgram.use();
        glUniform1f(timeLocation, currentTime);
        glDispatchCompute((unsigned int)TEXTURE_WIDTH / 10,
                          (unsigned int)TEXTURE_HEIGHT / 10, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        renderProgram.use();
        glUniform1i(texLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
        glBindVertexArray(0);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI
        ImGui::Begin("Stats:");
        ImGui::Text("GL_MAX_COMPUTE_WORK_GROUP_COUNT = (%lld, %lld, %lld)",
                    workGroupCount[0],
                    workGroupCount[1],
                    workGroupCount[2]);
        ImGui::Text("GL_MAX_COMPUTE_WORK_GROUP_SIZE = (%d, %d, %d)",
                    workGroupSize[0],
                    workGroupSize[1],
                    workGroupSize[2]);
        ImGui::Text("GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS = %lld",
                    workGroupInvocations);
        ImGui::Text("FPS = %3.2f", fps);
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

    glDeleteTextures(1, &texture);

    glfwTerminate();

    return 0;
}

void error_cb(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void createModel(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    GLfloat vertices[] =
    {
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,
    };

    GLuint elements[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

GLuint createTextureObject(GLuint textureWidth, GLuint textureHeight)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth,
                 textureHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    return texture;
}
