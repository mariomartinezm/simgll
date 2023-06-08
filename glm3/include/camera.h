#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>

struct Camera
{
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 target   = { 0.0f, 0.0f, 0.0f };
    glm::vec3 up       = { 0.0f, 0.0f, 0.0f };

    float  speed     = 5.0f;
    double mouseX    = 0.0;
    double mouseY    = 0.0;
    bool   mouseInit = true;
};

void poll_keyboard(GLFWwindow* window, GLfloat deltaTime)
{
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(!camera)
    {
        return;
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera->position = camera->position +
                camera->speed * deltaTime * camera->target;
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera->position = camera->position -
                camera->speed * deltaTime * camera->target;
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec3 sideVector = glm::cross(camera->target, camera->up);
        sideVector = glm::normalize(sideVector);

        camera->position = camera->position -
                camera->speed * deltaTime * sideVector;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec3 sideVector = glm::cross(camera->target, camera->up);
        sideVector = glm::normalize(sideVector);

        camera->position = camera->position +
                camera->speed * deltaTime * sideVector;
    }
}

void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos)
{
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

    if(!camera)
    {
        return;
    }

    if(camera->mouseInit)
    {
        camera->mouseX = xPos;
        camera->mouseY = yPos;

        camera->mouseInit = false;
    }

    GLfloat offsetX = (float)(camera->mouseX - xPos);
    GLfloat offsetY = (float)(camera->mouseY - yPos);

    camera->mouseX = (float)xPos;
    camera->mouseY = (float)yPos;

    float horizontalAngle = offsetX / 200.0f;
    float verticalAngle   = offsetY / 200.0f;

    glm::mat4x4 rotation = glm::rotate(horizontalAngle, glm::vec3{0.0, 1.0f, 0.0f});
    glm::vec4 newTarget = rotation * glm::vec4(camera->target.x, camera->target.y, camera->target.z, 0.0f);
    camera->target = glm::normalize(newTarget);

    glm::vec3 hAxis = glm::normalize(glm::cross(camera->up, camera->target));
    rotation = glm::rotate(verticalAngle, hAxis);
    newTarget = rotation * glm::vec4(camera->target.x, camera->target.y, camera->target.z, 0.0f);
    camera->target = newTarget;
}


#endif // INPUT_H
