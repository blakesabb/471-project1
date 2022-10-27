//
// Name :         WaveformBuffer.cpp
// Description :  Implementation of a buffer that can be used to display an audio waveform
//                in conjuction with WaveformWnd.cpp.
// Author :       Charles B. Owen
// Version :      See WaveformWnd.cpp
// Assumes :      Microsoft foundation class application
//

#include "stdafx.h"
#include "WaveformBuffer.h"

CWaveformBuffer::CWaveformBuffer(void)
{
    m_sampleRate = 44100.;
    m_channels = 0;
    m_capacity = 10.;
    m_capacitySamples = 0;
    m_cnt = 0;
}

CWaveformBuffer::~CWaveformBuffer(void)
{
}

void CWaveformBuffer::AddView(CWnd *p_wnd)
{
    m_views.insert(p_wnd);
}

void CWaveformBuffer::RemoveView(CWnd *p_wnd)
{
    m_views.erase(p_wnd);
}

void CWaveformBuffer::UpdateAllViews()
{
    for(std::set<CWnd *>::iterator i= m_views.begin();  i != m_views.end(); i++)
        (*i)->Invalidate();
}

//
// Name :         CWaveformBuffer::Clear(
// Description :  Clear the buffer and prepare to accumulate audio data.
//

void CWaveformBuffer::Start(int p_channels, double p_sampleRate)
{
    m_channels = p_channels;
    m_sampleRate = p_sampleRate;
    m_buffer.clear();
    m_buffer.resize(m_channels);

    m_redrawRate = int(m_sampleRate);

    // What is the capacity in actual samples?
    m_capacitySamples = int(m_sampleRate * m_capacity);

    // Sample counter
    m_cnt = 0;
}

void CWaveformBuffer::End()
{
    UpdateAllViews();
}
