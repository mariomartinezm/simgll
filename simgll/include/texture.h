#pragma once

#include <GL/glew.h>
#include <FreeImage.h>

#include "simgll_export.h"

namespace simgll
{
    SIMGLL_EXPORT GLuint createTextureObject(const char* filename);
}
