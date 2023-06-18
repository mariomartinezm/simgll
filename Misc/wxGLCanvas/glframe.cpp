#include "glframe.h"

#include <wx/event.h>


GLFrame::GLFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxString{})
{
    // Create the canvas and context
    wxGLAttributes dispAttrs;
    dispAttrs.PlatformDefaults().RGBA().DoubleBuffer().EndList();

    mCanvas  = new wxGLCanvas(this, dispAttrs);

    mCanvas->Bind(wxEVT_SIZE, &GLFrame::OnCanvasSize, this);
    mCanvas->Bind(wxEVT_PAINT, &GLFrame::OnCanvasPaint, this);
}

GLFrame::~GLFrame()
{
    mHelper.cleanup();
    delete mContext;
}

void GLFrame::OnCanvasSize(wxSizeEvent& event)
{
    // Context creation is done here since in Linux you should wait
    // until the canvas has a positive size and IsShownOnScreen() is
    // true
    wxGLContextAttrs cxtAttrs;
    cxtAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
    mContext = new wxGLContext(mCanvas, NULL, &cxtAttrs);

    if(!mContext->IsOK())
    {
        SetTitle("Failed to create context");
        return;
    }

    initGL();

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
}
