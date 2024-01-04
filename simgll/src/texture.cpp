#include <iostream>
#include "texture.h"

using std::cout;

GLuint simgll::createTextureObject(const char* filename)
{
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

    if (format == FIF_UNKNOWN)
    {
        cout << "Unknown image format" << filename << std::endl;

        exit(1);
    }

    FIBITMAP* bitmap = FreeImage_Load(format, filename);

    int bitsPerPixel = FreeImage_GetBPP(bitmap);

    FIBITMAP* bitmap32;
    if (bitsPerPixel == 32)
    {
        cout << "Source image has " << bitsPerPixel << " bits per pixel." << std::endl;

        bitmap32 = bitmap;
    }
    else
    {
        cout << "Source image has " << bitsPerPixel << " bits per pixel." << std::endl;

        bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
        FreeImage_Unload(bitmap);
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(bitmap32), FreeImage_GetHeight(bitmap32), 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(bitmap32));

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    FreeImage_Unload(bitmap32);

    return texture;
}
