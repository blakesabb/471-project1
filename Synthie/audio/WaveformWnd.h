//
// Name :         WaveformWnd.h
// Description :  Implementation of a window that can be used to display an audio waveform.
// Author :       Charles B. Owen
// Version :      See WaveformWnd.cpp
//

#pragma once

#include "WaveformBuffer.h"

// CWaveformWnd

class CWaveformWnd : public CWnd
{
	DECLARE_DYNAMIC(CWaveformWnd)

public:
	CWaveformWnd();
	virtual ~CWaveformWnd();

    void Create(CWnd *p_parent, CRect &p_rect, CWaveformBuffer *p_buffer);

protected:
	DECLARE_MESSAGE_MAP()

private:
    void DrawWaveform(CDC * pDC);
    void Resize();

    CSliderCtrl             m_scale;
    CScrollBar              m_position;
    
    int                     m_offset;
    int                     m_waveformsize;

    CWaveformBuffer   *m_buffer;
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnDestroy();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


