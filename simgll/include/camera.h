#pragma once

#include <GL/gl.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "simgll_export.h"

class GLFWwindow;

namespace simgll
{
    class SIMGLL_EXPORT Camera
    {
    public:
        Camera(GLFWwindow* window, glm::vec3 position, glm::vec3 target,
               glm::vec3 up);
        ~Camera();

        glm::vec3 position() const;
        glm::vec3 target() const;

        glm::mat4x4 update(const GLfloat deltaTime, GLfloat fov = 45.0F,
                           GLfloat zNear = 0.1F, GLfloat zFar = 100.0F);

    private:
        void poll_keyboard(const GLfloat deltaTime);
        void poll_cursor();

        GLFWwindow* mWindow;

        glm::vec3 mPosition  = { 0.0f, 0.0f, 0.0f };
        glm::vec3 mTarget    = { 0.0f, 0.0f, 0.0f };
        glm::vec3 mUp        = { 0.0f, 0.0f, 0.0f };

        GLfloat   mSpeed     = { 5.0F };
        GLdouble  mMouseX    = { 0.0F };
        GLdouble  mMouseY    = { 0.0F };
        GLboolean mMouseInit = { GL_TRUE };
    };
}
