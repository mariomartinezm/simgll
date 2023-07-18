#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "camera.h"

constexpr GLuint WIDTH  = 512;
constexpr GLuint HEIGHT = 512;

enum
{
    WORKGROUP_SIZE  = 256,
    NUM_WORKGROUPS  = 64,
    FLOCK_SIZE      = (NUM_WORKGROUPS * WORKGROUP_SIZE)
};

struct flock_member
{
    glm::vec3 position;
    GLuint: 32;
    glm::vec3 velocity;
    GLuint: 32;
};

GLvoid error_callback(GLint error, const GLchar* description);
GLvoid createQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        glfwTerminate();
        exit(1);
    }

    Camera camera(window,
                  glm::vec3{ 0.0f, 0.5f, -400.0f },
                  glm::vec3{ 0.0f, 0.0f,    1.0f },
                  glm::vec3{ 0.0f, 1.0f,    0.0f });

    GLuint flock_update_program;
    ShaderProgram shaderProgram(flock_update_program);
    shaderProgram.addShader("flocking_cs.glsl", GL_COMPUTE_SHADER);
    shaderProgram.compile();

    GLint goalLocation = glGetUniformLocation(flock_update_program, "goal");

    GLuint flock_render_program;
    shaderProgram.setName(flock_render_program);
    shaderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    GLint mvpLocation = glGetUniformLocation(flock_render_program, "mvp");

    GLuint flock_buffers[2];
    glGenBuffers(2, flock_buffers);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffers[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffers[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), nullptr, GL_DYNAMIC_COPY);

    // This is position and normal data for a paper airplane
    static const glm::vec3 geometry[] =
    {
        // Positions
        glm::vec3(-5.0F, 1.0F, 0.0F),
        glm::vec3(-1.0F, 1.5F, 0.0F),
        glm::vec3(-1.0F, 1.5F, 7.0F),
        glm::vec3(0.0F, 0.0F, 0.0F),
        glm::vec3(0.0F, 0.0F, 10.0F),
        glm::vec3(1.0F, 1.5F, 0.0F),
        glm::vec3(1.0F, 1.5F, 7.0F),
        glm::vec3(5.0F, 1.0F, 0.0F),

        // Normals
        glm::vec3(0.0F),
        glm::vec3(0.0F),
        glm::vec3(0.107F,  -0.859F,  0.00F),
        glm::vec3(0.832F,   0.554F,  0.00F),
        glm::vec3(-0.59F,  -0.395F,  0.00F),
        glm::vec3(-0.832F,  0.554F,  0.00F),
        glm::vec3(0.295F,  -0.196F,  0.00F),
        glm::vec3(0.124F,   0.992F,  0.00F),
    };

    GLuint geometry_buffer;
    glGenBuffers(1, &geometry_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

    GLuint flock_render_vaos[2];
    glGenVertexArrays(2, flock_render_vaos);

    for(int i = 0; i < 2; i++)
    {
        glBindVertexArray(flock_render_vaos[i]);

        glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(8 * sizeof(glm::vec3)));

        glBindBuffer(GL_ARRAY_BUFFER, flock_buffers[i]);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), nullptr);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), (GLvoid*)sizeof(glm::vec4));
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
    }

    glBindBuffer(GL_ARRAY_BUFFER, flock_buffers[0]);
    flock_member* ptr = reinterpret_cast<flock_member*>(
                glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                 FLOCK_SIZE * sizeof(flock_member),
                                 GL_MAP_WRITE_BIT |
                                 GL_MAP_INVALIDATE_BUFFER_BIT));

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for(int i = 0; i < FLOCK_SIZE; i++)
    {
        ptr[i].position.x = (dist(engine) - 0.5f) * 300.0f;
        ptr[i].position.y = (dist(engine) - 0.5f) * 300.0f;
        ptr[i].position.z = (dist(engine) - 0.5f) * 300.0f;

        ptr[i].velocity.x = dist(engine) - 0.5f;
        ptr[i].velocity.y = dist(engine) - 0.5f;
        ptr[i].velocity.z = dist(engine) - 0.5f;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    GLfloat startTime = 0.0F;
    GLfloat oldTime   = 0.0F;
    GLfloat deltaTime = 0.0F;

    GLuint frameIndex = 0;

    while(!glfwWindowShouldClose(window))
    {
        startTime = (GLfloat)glfwGetTime();
        deltaTime = startTime - oldTime;
        oldTime   = startTime;

        glfwPollEvents();

        static const float black[] = { 0.0F, 0.0F, 0.0F, 1.0F };
        static const float one = 1.0F;

        glUseProgram(flock_update_program);

        glm::vec3 goal = glm::vec3(sinf(deltaTime * 0.34f),
                                   cosf(deltaTime * 0.29f),
                                   sinf(deltaTime * 0.12f) * cosf(deltaTime * 0.5f));

        goal = goal * glm::vec3(35.0f, 25.0f, 60.0f);

        glUniform3f(goalLocation, goal.x, goal.y, goal.z);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flock_buffers[frameIndex]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flock_buffers[frameIndex ^ 1]);

        glDispatchCompute(NUM_WORKGROUPS, 1, 1);

        glViewport(0, 0, WIDTH, HEIGHT);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);

        glUseProgram(flock_render_program);
        auto mvp = camera.update(deltaTime, 45.0F, 0.1F, 3000.0F);

        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

        glBindVertexArray(flock_render_vaos[frameIndex]);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, FLOCK_SIZE);

        frameIndex ^= 1;

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

GLvoid error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << "\n";
    exit(1);
}
