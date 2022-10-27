//
// Name :         WaveformWnd.cpp
// Description :  Implementation of a window that can be used to display an audio waveform.
// Author :       Charles B. Owen
// Version :      1.00  9-10-06 Initial development
// Assumes :      Microsoft foundation class application
//

#include "stdafx.h"
#include "WaveformWnd.h"

const int WSLIDERW = 25;            // Width of the slider in pixels...
const int WSCROLLBARHEIGHT = 15;    // Height of the scroll bar

IMPLEMENT_DYNAMIC(CWaveformWnd, CWnd)

CWaveformWnd::CWaveformWnd()
{
    m_buffer = NULL;        // Until we know a waveform buffer.
    m_offset = 0;
    m_waveformsize = 0;
}

CWaveformWnd::~CWaveformWnd()
{
}


BEGIN_MESSAGE_MAP(CWaveformWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_DESTROY()
    ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CWaveformWnd::Create(CWnd *p_parent, CRect &p_rect, CWaveformBuffer *p_buffer)
{
    CString winclass = AfxRegisterWndClass(0, 0, 0, 0);

    CWnd::CreateEx(0, 
        winclass, TEXT("Waveform"),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS , 
           p_rect,
           p_parent, 0);
    
    m_buffer = p_buffer;

    // Tell the buffer we are installed
    m_buffer->AddView(this);
}



//
// Name :         CWaveformWnd::OnCreate()
// Description :  This method is invoked when the window is created.  It's a good time
//                to create the child windows (controls)
//

int CWaveformWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    //
    // Create the slider for scale
    //

    CRect rect;
    GetClientRect(rect);
    rect.right = rect.left + WSLIDERW;
    rect.bottom -= WSCROLLBARHEIGHT;
    m_scale.Create(TBS_VERT | TBS_AUTOTICKS | WS_VISIBLE, rect, this, 1);
    m_scale.SetRange(0, 100);
    m_scale.SetTicFreq(10);
    m_scale.SetPos(100);

    //
    // Create the scroll bar control
    //

    GetClientRect(rect);
    rect.left += WSLIDERW;
    rect.top = rect.bottom - WSCROLLBARHEIGHT;
    m_position.Create(SBS_HORZ | WS_VISIBLE, rect, this, 2);

    return 0;
}



void CWaveformWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    DrawWaveform(&dc);
}


void CWaveformWnd::DrawWaveform(CDC * pDC)
{
    CRect rect;
    GetClientRect(rect);

    // Convert to a rect that will contain the waveform window
    rect.left += WSLIDERW + 2;
    rect.right -= 2;
    rect.bottom -= 2 + WSCROLLBARHEIGHT;

    int height = rect.Height() - 2;
    int width = rect.Width() - 2;


    CBrush blackbrush(RGB(0, 0, 0));
    pDC->FillRect(&rect, &blackbrush);

    CPen borderpen;
    borderpen.CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
    CPen divpen;
    divpen.CreatePen(PS_SOLID, 1, RGB(0, 120, 0));
    CPen tickpen;
    tickpen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    CPen wavepen;
    wavepen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));

    CPen *oldpen = pDC->SelectObject(&borderpen);

    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.right, rect.top);
    pDC->LineTo(rect.right, rect.bottom);
    pDC->LineTo(rect.left, rect.bottom);
    pDC->LineTo(rect.left, rect.top);

    if(m_buffer == NULL)
        return;

    const std::vector<std::vector<short> > &waveforms = m_buffer->GetWaveform();

    //
    // Draw lines that separate into sections
    //

    int channels = waveforms.size();

    if(channels == 0)
    {
        m_offset = 0;
        m_waveformsize = 0;

        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        si.nMin = 0;
        si.nMax = 0;
        si.nPage = 1;
        si.nPos = 0;
        m_position.SetScrollInfo(&si);
    }

    pDC->SelectObject(&divpen);

    for(int c=0;  c<channels - 1;  c++)
    {
        int center = rect.Height() * (c + 1) / channels + rect.top;

        pDC->MoveTo(rect.left, center);
        pDC->LineTo(rect.right, center);
    }

    //
    // Now draw each of the individual waveforms.
    //

    double scale = 1. / (101 - m_scale.GetPos());

    for(int c=0;  c<channels;  c++)
    {
        // Now draw the actual waveform...
        const std::vector<short> &waveform = waveforms[c];
        int waveformsize = waveform.size();


        // We only set the scroll bar for the first channel, since all will be the same
        if(c == 0)
        {
            int scaling = waveformsize <= 32767 ? 1 : (waveformsize / 32767 + 1);
            m_waveformsize = waveformsize;

            SCROLLINFO si;
            si.cbSize = sizeof(SCROLLINFO);
            si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
            si.nMin = 0;
            si.nMax = waveformsize / scaling;
            si.nPage = (int( (double)rect.Width() / scale)) / scaling;
            si.nPos = m_offset / scaling;
            m_position.SetScrollInfo(&si);
        }


        int top = rect.Height() * (c) / channels + rect.top;
        int bottom = rect.Height() * (c + 1) / channels + rect.top;
        int center = (top + bottom) / 2;
        height = bottom - top;

        //
        // Draw horizontal line through the center
        //

        pDC->SelectObject(&tickpen);

        pDC->MoveTo(rect.left, center);
        pDC->LineTo(rect.right, center);

        //
        // Draw the waveform
        //
        pDC->SelectObject(&wavepen);

        double x = rect.left + 1;

        std::vector<short>::const_iterator i=waveform.begin() + m_offset;
        int y = center - (*i * (height / 2) / 32768);
        pDC->MoveTo(int(x), y);
        i++;
        x += scale;

        for(;  i!=waveform.end() && x < rect.right - 1;  i++)
        {
            int y = center - (*i * (height / 2) / 32768);
            pDC->LineTo(int(x), y);

            x += scale;
        }
    }

    pDC->SelectObject(oldpen);
}



void CWaveformWnd::OnMove(int x, int y)
{
    CWnd::OnMove(x, y);
    Resize();
}

void CWaveformWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    Resize();
}

void CWaveformWnd::Resize()
{
    CRect rect;
    GetClientRect(rect);
    rect.right = rect.left + WSLIDERW;
    rect.bottom -= WSCROLLBARHEIGHT;
    m_scale.MoveWindow(&rect);

    GetClientRect(rect);
    rect.left += WSLIDERW;
    rect.top = rect.bottom - WSCROLLBARHEIGHT;
    m_position.MoveWindow(&rect);
}

void CWaveformWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if(pScrollBar == (CScrollBar *)&m_scale)
    {
        Invalidate();
        return;
    }

    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWaveformWnd::OnDestroy()
{
    if(m_buffer)
        m_buffer->RemoveView(this);
    m_buffer = NULL;

    CWnd::OnDestroy();
}

void CWaveformWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if(pScrollBar == &m_position)
    {
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        m_position.GetScrollInfo(&si);

        CRect rect;
        GetClientRect(&rect);
        rect.left += WSLIDERW;

        int scaling = m_waveformsize <= 32767 ? 1 : (m_waveformsize / 32767 + 1);

        int step = (rect.Width() > 0 ? si.nPage / rect.Width() : 1) * scaling;
        if(step < 1)
            step = 1;

        switch(nSBCode)
        {
        case SB_LEFT:      // Scroll to far left.
	        m_offset = 0;
	        break;

        case SB_RIGHT:      // Scroll to far right.
	        m_offset = m_waveformsize - step;
	        break;

        case SB_ENDSCROLL:   // End scroll.
	        break;

        case SB_LINELEFT:      // Scroll left.
	        m_offset -= step;
	        break;

        case SB_LINERIGHT:   // Scroll right.
	        m_offset += step;
	        break;

        case SB_PAGELEFT:    // Scroll one page left.
	        m_offset -= (int) si.nPage * scaling;
	        break;

        case SB_PAGERIGHT:      // Scroll one page right
	        m_offset += (int) si.nPage * scaling;
	        break;

        case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
	        m_offset = nPos * scaling;      // of the scroll box at the end of the drag operation.
	        break;

        case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
	        m_offset = nPos * scaling;     // position that the scroll box has been dragged to.
	        break;
        }

        if(m_offset < 0)
            m_offset = 0;
        if(m_offset > m_waveformsize)
            m_offset = m_waveformsize;

        m_position.SetScrollPos(m_offset / scaling);
        Invalidate();
    }

    CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}
