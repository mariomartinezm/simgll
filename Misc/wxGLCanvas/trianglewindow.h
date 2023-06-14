#pragma once

#include <wx/wx.h>

class TriangleWindow : public wxFrame
{
public:
    TriangleWindow(wxWindow* parent, const wxString& title,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize);

    virtual ~TriangleWindow();

    TriangleWindow(const TriangleWindow& tw) = delete;
    TriangleWindow(TriangleWindow&& tw) = delete;
    TriangleWindow& operator=(const TriangleWindow& tw) = delete;
    TriangleWindow& operator=(const TriangleWindow&&) = delete;
};
