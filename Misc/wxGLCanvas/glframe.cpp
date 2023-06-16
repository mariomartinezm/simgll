#include "glframe.h"
#include <wx/event.h>
#include <wx/msw/glcanvas.h>

GLFrame::GLFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxString{})
{
    // Create the canvas and context
    wxGLAttributes dispAttrs;
    dispAttrs.PlatformDefaults().RGBA().DoubleBuffer().EndList();

    wxGLContextAttrs cxtAttrs;
    cxtAttrs.PlatformDefaults().CoreProfile().OGLVersion(4, 5).EndList();

    mCanvas  = new wxGLCanvas(this, dispAttrs);
    mContext = new wxGLContext(mCanvas, NULL, &cxtAttrs);

    if(!mContext->IsOK())
    {
        SetTitle("Failed to create context");
        return;
    }

    initGL();
    //mCanvas->Bind(wxEVT_CREATE, [this](wxWindowCreateEvent&){ initGL(); });
}

GLFrame::~GLFrame()
{
    mHelper.cleanup();
    delete mContext;
}

void GLFrame::OnCanvasSize(wxSizeEvent& event)
{
    wxSize sz = event.GetSize();
    mHelper.setSize(sz.GetWidth(), sz.GetHeight());
    event.Skip();
}

void GLFrame::OnCanvasPaint(wxPaintEvent&)
{
    wxPaintDC dc(mCanvas);

    mHelper.render();
    mCanvas->SwapBuffers();
}

void GLFrame::initGL()
{
    // First call SetCurrent or GL initialization will fail
    mContext->SetCurrent(*mCanvas);

    // Initialize GLEW
    bool glewInitialized = mHelper.initGlew();

    if(!glewInitialized)
    {
        SetTitle("Failed to initialize GLEW");
        return;
    }

    SetTitle("Context and GLEW initialized");

    // Initialize the triangle data
    mHelper.initData();

    // Bind event handlers for the canvas. Binding was delayed until OpenGL was initialized
    // because these handlers will need to call OpenGL functions
    mCanvas->Bind(wxEVT_SIZE, &GLFrame::OnCanvasSize, this);
    mCanvas->Bind(wxEVT_PAINT, &GLFrame::OnCanvasPaint, this);
}
