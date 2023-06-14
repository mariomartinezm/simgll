#include <memory>
#include <stdexcept>
#include <wx/string.h>

#include "trianglecanvas.h"

TriangleCanvas::TriangleCanvas(wxWindow* parent, wxWindowID id,
                               const int* attribList,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name,
                               const wxPalette& palette)
    : wxGLCanvas(parent, id, attribList, pos, size, style, name, palette),
    mVbo(0), mVao(0)
{
    mContext = std::make_unique<wxGLContext>(this);
    Bind(wxEVT_PAINT, &TriangleCanvas::OnPaint, this);

    SetCurrent(*mContext);
    InitializeGLEW();
    SetupGraphics();
}

TriangleCanvas::~TriangleCanvas()
{
    SetCurrent(*mContext);
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVbo);
}

void TriangleCanvas::InitializeGLEW()
{
    glewExperimental = true;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        throw std::runtime_error(reinterpret_cast<const char*>(glewGetErrorString(status)));
    }
}

void TriangleCanvas::SetupGraphics()
{
	// define vertices
	float points[] = {
		0.0f, 0.5f,
		0.5f, -0.5f,
		-0.5f, -0.5f
	};

	// upload vertex data
	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// setup vertex array objects
	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void TriangleCanvas::OnPaint(wxPaintEvent&)
{
	SetCurrent(*mContext);

	// set background to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw the graphics
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// and display
	glFlush();
	SwapBuffers();
}
