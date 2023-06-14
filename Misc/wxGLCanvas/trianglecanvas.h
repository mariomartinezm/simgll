#pragma once

#include <memory>
#include <GL/glew.h>
#include <wx/glcanvas.h>

class TriangleCanvas : public wxGLCanvas
{
public:
    TriangleCanvas(wxWindow* parent, wxWindowID id = wxID_ANY,
                   const int* attribList = 0,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0L,
                   const wxString& name = L"GLCanvas",
                   const wxPalette& palette = wxNullPalette);

    virtual ~TriangleCanvas();

    TriangleCanvas(const TriangleCanvas& tc) = delete;
    TriangleCanvas(TriangleCanvas&& tc) = delete;
    TriangleCanvas& operator=(const TriangleCanvas& tc) = delete;
    TriangleCanvas& operator=(TriangleCanvas&& tc) = delete;

private:
    void InitializeGLEW();
    void SetupGraphics();
    void OnPaint(wxPaintEvent& event);

    std::unique_ptr<wxGLContext> mContext;
    GLuint mVbo;
    GLuint mVao;
};
