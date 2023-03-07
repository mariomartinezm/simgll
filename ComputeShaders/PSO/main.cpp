#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

constexpr GLuint WIDTH            = 512;
constexpr GLuint HEIGHT           = 512;
constexpr GLint NUM_ITER          = 10000;
constexpr GLfloat PSO_UPDATE_TIME = 0.016F;
constexpr float PI                = 3.1415926535F;

enum
{
    WORKGROUP_SIZE  = 16,
    NUM_WORKGROUPS  = 3,
    SWARM_SIZE      = (NUM_WORKGROUPS * WORKGROUP_SIZE)
};

struct Particle
{
    glm::vec3 position;
    GLuint: 32;
    glm::vec3 velocity;
    GLuint: 32;
    glm::vec3 bestPosition;
    float fitness;
};

// Test functions
GLfloat goldsteinPrice(glm::vec2 p);
GLfloat rastrigin3D(const glm::vec3& p);

glm::vec3 getBestPosition(Particle* p);

GLvoid error_callback(GLint error, const GLchar* description);

int main()
{
    glfwSetErrorCallback(error_callback);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App",
                                          nullptr, nullptr);

    if(!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW error: " << glewGetErrorString(status) << "\n";

        glfwTerminate();
        exit(1);
    }

    // Setup DearImGui contxt
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup platform / renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    // Setup Dear Imgui style
    ImGui::StyleColorsDark();

    Camera camera;
    camera.position = { 0.0f, 0.0f,  3.0f };
    camera.target   = { 0.0f, 0.0f, -1.0f };
    camera.up       = { 0.0f, 1.0f,  0.0f };

    glfwSetWindowUserPointer(window, (GLvoid*)&camera);

    // Create render and compute programs
    ShaderProgram renderProgram;
    renderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    renderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    renderProgram.compile();

    GLint mvpLocation = glGetUniformLocation(renderProgram.name(), "mvp");

    ShaderProgram psoProgram;
    psoProgram.addShader("pso.glsl", GL_COMPUTE_SHADER);
    psoProgram.compile();

    GLint omegaLocation   = glGetUniformLocation(psoProgram.name(), "omega");
    GLint cpuSeedLocation = glGetUniformLocation(psoProgram.name(), "cpuSeed");

    // Create swarm buffers
    GLuint psoBuffers[2];
    glGenBuffers(2, psoBuffers);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, psoBuffers[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, SWARM_SIZE * sizeof(Particle),
                 nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, psoBuffers[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, SWARM_SIZE * sizeof(Particle),
                 nullptr, GL_DYNAMIC_COPY);

    // Initialize PRNG
    std::random_device rd;
    unsigned seed;

    // Check if the implementation provides a usable random_device
    if (0 != rd.entropy())
    {
        seed = rd();
    }
    else
    {
        // No random device available, seed using the system clock
        seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    }

    std::mt19937 engine(seed);
    std::uniform_real_distribution<> dist(-2.0F, 2.0F);

    // Get a pointer to the first buffer so we can initialize its contents
    glBindBuffer(GL_ARRAY_BUFFER, psoBuffers[0]);
    Particle* p = reinterpret_cast<Particle*>(
        glMapBufferRange(GL_ARRAY_BUFFER, 0, SWARM_SIZE * sizeof(Particle),
                         GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

    // Initialize position, velocity and fitness
    auto f = goldsteinPrice;

    for(int i = 0; i < SWARM_SIZE; ++i)
    {
        p[i].position.x = dist(engine);
        p[i].position.y = dist(engine);
        p[i].position.z = 0.0F;

        p[i].velocity.x = dist(engine);
        p[i].velocity.y = dist(engine);
        p[i].velocity.z = 0.0F;

        p[i].bestPosition = p[i].position;

        p[i].fitness = f(p[i].position);
    }

    glm::vec3 bestPosition = getBestPosition(p);
    std::cout << bestPosition.x << " " << bestPosition.y << " " << bestPosition.z << "\n";

    GLint bestPositionLocation = glGetUniformLocation(psoProgram.name(), "bestPosition");

    glUnmapBuffer(GL_ARRAY_BUFFER);

    const glm::vec3 particleGeometry[] =
    {
        glm::vec3(0.0F, 0.0F, 0.0F)
    };

    GLuint geometryBuffer;
    glGenBuffers(1, &geometryBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleGeometry),
                 particleGeometry, GL_STATIC_DRAW);

    GLuint renderVaos[2];
    glGenVertexArrays(2, renderVaos);

    // Setup data for rendering
    for(int i = 0; i < 2; ++i)
    {
        glBindVertexArray(renderVaos[i]);

        glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, psoBuffers[i]);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                              nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                              (GLvoid*)sizeof(glm::vec4));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0f);

    GLfloat currentTime = 0.0F;
    GLfloat oldTime     = 0.0F;
    GLfloat deltaTime   = 0.0F;
    GLfloat totalTime   = 0.0f;

    GLint i = 0;
    GLuint frameIndex = 0;
    GLfloat omega = 0.9F;

    while(!glfwWindowShouldClose(window))
    {
        currentTime = (GLfloat) glfwGetTime();
        deltaTime   = currentTime - oldTime;
        oldTime     = currentTime;
        totalTime  += deltaTime;

        if(totalTime >= PSO_UPDATE_TIME && i < NUM_ITER)
        {
            glUseProgram(psoProgram.name());

            glUniform1f(omegaLocation, omega);
            glUniform3f(bestPositionLocation, bestPosition.x, bestPosition.y, bestPosition.z);
            glUniform1ui(cpuSeedLocation, rand());

            // Bind buffers for compute shader
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, psoBuffers[frameIndex]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, psoBuffers[frameIndex ^ 1]);

            glDispatchCompute(NUM_WORKGROUPS, 1, 1);

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

            glBindBuffer(GL_ARRAY_BUFFER, psoBuffers[frameIndex ^ 1]);
            p = reinterpret_cast<Particle*>(glMapBufferRange(GL_ARRAY_BUFFER,
                                                             0, SWARM_SIZE *
                                                             sizeof(Particle),
                                                             GL_MAP_READ_BIT));

            glm::vec3 currentBestPosition = getBestPosition(p);

            if(f(currentBestPosition) < f(bestPosition))
            {
                bestPosition = currentBestPosition;
            }

            glUnmapBuffer(GL_ARRAY_BUFFER);

            omega -= (0.9F - 0.4F) / NUM_ITER;
            totalTime = 0.0F;
            frameIndex ^= 1;
            i++;
        }

        glfwPollEvents();
        poll_keyboard(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(renderProgram.name());

        // Render your GUI
        ImGui::Begin("PSO");

        ImGui::Text("Iteration = %d", i);
        ImGui::Text("Omega = %f", omega);
        ImGui::Text("Best Position = (%f, %f, %f)",
                    bestPosition.x, bestPosition.y, bestPosition.z);
        ImGui::Text("Best Fitness = %f", f(bestPosition));

        auto newTarget  = camera.position + camera.target;
        auto modelView  = glm::lookAt(camera.position, newTarget, camera.up);
        auto projection = glm::perspective(45.0f, float(WIDTH) / HEIGHT, 0.1f,
                                           100.0f);
        auto mvp        = projection * modelView;

        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

        glBindVertexArray(renderVaos[frameIndex]);
        glDrawArraysInstanced(GL_POINTS, 0, 1, SWARM_SIZE);

        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

glm::vec3 getBestPosition(Particle* p)
{
    glm::vec3 bestPosition = p[0].position;

    for(int i = 1; i < SWARM_SIZE; i++)
    {
        if(p[i].fitness < p[i - 1].fitness)
        {
            bestPosition = p[i].position;
        }
    }

    return bestPosition;
}

GLvoid error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << "\n";
    exit(1);
}

// Global minimum = f(0, -1) = 3
// Search domain = -2.0 <= pi <= 2.0
GLfloat goldsteinPrice(glm::vec2 p)
{
    return (1  + (p.x + p.y + 1) * (p.x + p.y + 1) *
           (19 - 14 * p.x + 3 * p.x * p.x - 14 * p.y + 6 * p.x * p.y + 3 * p.y * p.y)) *
           (30 + (2 * p.x - 3 * p.y) * (2 * p.x - 3 * p.y) *
           (18 - 32 * p.x + 12 * p.x * p.x + 48 * p.y - 36 * p.x * p.y + 27 * p.y * p.y));
}

// Global minimum = f(0, ..., 0) = 0
// Search domain  = -5.12 <= pi <= 5.12
GLfloat rastrigin3D(const glm::vec3& p)
{
    constexpr int a = 10;
    float s = a * 3.0F;

    s += p.x * p.x - a * cosf(2 * PI * p.x);
    s += p.y * p.y - a * cosf(2 * PI * p.y);
    s += p.z * p.z - a * cosf(2 * PI * p.z);

    return s;
}
