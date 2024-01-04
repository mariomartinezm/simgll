#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>

#include "camera.h"

simgll::Camera::Camera(GLFWwindow* window,
               glm::vec3 position,
               glm::vec3 target,
               glm::vec3 up) :
    mWindow(window),
    mPosition(position),
    mTarget(target),
    mUp(up)
{
}

simgll::Camera::~Camera()
{
}

glm::vec3 simgll::Camera::position() const
{
    return mPosition;
}

glm::vec3 simgll::Camera::target() const
{
    return mTarget;
}

void simgll::Camera::poll_keyboard(const GLfloat deltaTime)
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

void simgll::Camera::poll_cursor()
{
    GLdouble xPos, yPos;

    glfwGetCursorPos(mWindow, &xPos, &yPos);

    if(mMouseInit)
    {
        mMouseX = xPos;
        mMouseY = yPos;

        mMouseInit = GL_FALSE;
    }

    GLfloat offsetX       = (GLfloat)(mMouseX - xPos);
    GLfloat offsetY       = (GLfloat)(mMouseY - yPos);
    mMouseX               = (GLfloat)xPos;
    mMouseY               = (GLfloat)yPos;

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

glm::mat4x4 simgll::Camera::update(const GLfloat deltaTime, GLfloat fov,
                                   GLfloat zNear, GLfloat zFar)
{
    poll_keyboard(deltaTime);
    poll_cursor();

    auto view = glm::lookAt(mPosition, mPosition + mTarget, mUp);

    GLint width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);

    auto proj = glm::perspective(fov, (GLfloat)(width) / height, zNear, zFar);

    return proj * view;
}
