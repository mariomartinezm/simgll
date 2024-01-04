#pragma once

#include <iostream>
#include <GL/glew.h>

#include "simgll_export.h"

namespace simgll
{
    SIMGLL_EXPORT void debug_callback(GLenum source, GLenum type, GLuint id,
                                      GLenum severity, GLsizei length, GLchar
                                      const* message, void const* user_param);
}
