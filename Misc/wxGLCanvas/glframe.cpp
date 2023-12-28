#include "glframe.h"
#include <wx/event.h>
#include <wx/msgdlg.h>


GLFrame::GLFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxString{})
{
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    // Create the canvas and context
    wxGLAttributes dispAttrs;
    dispAttrs.PlatformDefaults().RGBA().DoubleBuffer().EndList();

    mCanvas  = new wxGLCanvas(this, dispAttrs);
    mStaticTextFPS = new wxStaticText(this, -1, "My text.", wxDefaultPosition,
                                      wxSize(100, 60));

    mCanvas->Bind(wxEVT_SIZE, &GLFrame::OnCanvasSize, this);
    mCanvas->Bind(wxEVT_PAINT, &GLFrame::OnCanvasPaint, this);
    mCanvas->Bind(wxEVT_IDLE, &GLFrame::OnIdle, this);

    topSizer->Add(mCanvas, 1, wxEXPAND);
    topSizer->Add(mStaticTextFPS,
                  0,
                  wxEXPAND);
    SetSizerAndFit(topSizer);
}

GLFrame::~GLFrame()
{
    mHelper.cleanup();
    delete mContext;
}

void GLFrame::OnCanvasSize(wxSizeEvent& event)
{
    event.Skip();

    if (!IsShownOnScreen())
    {
        return;
    }

    // Context creation is done here since in Linux you should wait
    // until the canvas has a positive size and IsShownOnScreen() is
    // true
    if(!mIsInitialized)
    {
        wxGLContextAttrs cxtAttrs;
        cxtAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
        mContext = new wxGLContext(mCanvas, NULL, &cxtAttrs);

        if(!mContext->IsOK())
        {
            SetTitle("Failed to create context");
            return;
        }

        initGL();
    }

    wxSize sz = event.GetSize();
    mHelper.setSize(sz.GetWidth(), sz.GetHeight());

    end = std::chrono::high_resolution_clock::now();
}

void GLFrame::OnCanvasPaint(wxPaintEvent&)
{
    start   = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fms = start - end;
    elapsed = fms.count();
    end     = start;

    if (counter < 500)
    {
        counter++;
    }
    else
    {
        mStaticTextFPS->SetLabelText(wxString::Format(wxT("%f"), elapsed));
        counter = 0;
    }

    wxPaintDC dc(mCanvas);

    mHelper.render();
    mCanvas->SwapBuffers();
}

void GLFrame::OnIdle(wxIdleEvent& evt)
{
    start   = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fms = start - end;
    elapsed = fms.count();
    end     = start;

    if (counter < 500)
    {
        counter++;
    }
    else
    {
        mStaticTextFPS->SetLabelText(wxString::Format(wxT("%f"), elapsed));
        counter = 0;
    }

    mHelper.render();
    evt.RequestMore();
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

    mIsInitialized = true;
}
