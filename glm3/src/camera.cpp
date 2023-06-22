#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>

#include "camera.h"

Camera::Camera(GLFWwindow* window,
               GLint mvpUniformLocation,
               glm::vec3 position,
               glm::vec3 target,
               glm::vec3 up) :
    mWindow(window),
    mMvpUniformLocation(mvpUniformLocation),
    mPosition(position),
    mTarget(target),
    mUp(up)
{
}

Camera::~Camera()
{
}

void Camera::poll_keyboard(const GLfloat deltaTime)
{
    if(glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(mWindow, GL_TRUE);
    }

    if(glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        mPosition = mPosition + mSpeed * deltaTime * mTarget;
    }

    if(glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        mPosition = mPosition - mSpeed * deltaTime * mTarget;
    }

    if(glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec3 sideVector = glm::cross(mTarget, mUp);
        sideVector = glm::normalize(sideVector);

        mPosition = mPosition - mSpeed * deltaTime * sideVector;
    }

    if(glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec3 sideVector = glm::cross(mTarget, mUp);
        sideVector = glm::normalize(sideVector);

        mPosition = mPosition + mSpeed * deltaTime * sideVector;
    }
}

void Camera::poll_cursor()
{
    GLdouble xPos, yPos;

    glfwGetCursorPos(mWindow, &xPos, &yPos);

    if(mMouseInit)
    {
        mMouseX = xPos;
        mMouseY = yPos;

        mMouseInit = GL_FALSE;
    }

    GLfloat offsetX       = (float)(mMouseX - xPos);
    GLfloat offsetY       = (float)(mMouseY - yPos);
    mMouseX               = (float)xPos;
    mMouseY               = (float)yPos;

    float horizontalAngle = offsetX / 200.0f;
    float verticalAngle   = offsetY / 200.0f;

    glm::mat4x4 rotation = glm::rotate(horizontalAngle, glm::vec3{0.0, 1.0f, 0.0f});
    glm::vec4 newTarget  = rotation * glm::vec4(mTarget.x, mTarget.y, mTarget.z, 0.0f);
    mTarget              = glm::normalize(newTarget);

    glm::vec3 hAxis = glm::normalize(glm::cross(mUp, mTarget));
    rotation        = glm::rotate(verticalAngle, hAxis);
    newTarget       = rotation * glm::vec4(mTarget.x, mTarget.y, mTarget.z, 0.0f);
    mTarget         = newTarget;
}

void Camera::update(const GLfloat deltaTime)
{
    poll_keyboard(deltaTime);
    poll_cursor();

    auto mv_matrix   = glm::lookAt(mPosition, mPosition + mTarget, mUp);

    GLint width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);

    auto proj_matrix = glm::perspective(45.0F,
                                        (GLfloat)(width) / height,
                                        0.1F, 100.0F);

    auto mvp = proj_matrix * mv_matrix;
    glUniformMatrix4fv(mMvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);
}
