//
// Name :         WaveformBuffer.h
// Description :  Header for a buffer that can be used to display an audio waveform
//                in conjuction with WaveformWnd.cpp.
// Author :       Charles B. Owen
// Version :      See WaveformWnd.cpp
// Assumes :      Microsoft foundation class application
//

#pragma once

#include <vector>
#include <set>

//
// class CWaveformBuffer
// This is a class that buffers a segment of a waveform that can
// then be displayed using CWaveformWnd.
//

class CWaveformBuffer
{
public:
    CWaveformBuffer(void);
    ~CWaveformBuffer(void);

    void Start(int p_channels, double p_sampleRate);
    int  SetCapacity(double p_capacity) {m_capacity = p_capacity;}
    void End();

    void AddView(CWnd *p_wnd);
    void RemoveView(CWnd *p_wnd);
    void UpdateAllViews();

    // Add a frame to the buffer
    inline void Frame(short *p_frame)
    {
        if(m_cnt < m_capacitySamples)
        {
            for(int i=0;  i<m_channels;  i++)
                m_buffer[i].push_back(p_frame[i]);
            m_cnt++;
            if((m_cnt % m_redrawRate) == 0 || m_cnt == m_capacitySamples)
                UpdateAllViews();
        }
    }

    const std::vector<std::vector<short> > &GetWaveform() const {return m_buffer;}

private:
    std::vector<std::vector<short> >    m_buffer;   // The actual buffer
    std::set<CWnd *>                    m_views;    // Views attached to this buffer

    int         m_channels;         // How many channels are defined?
    double      m_sampleRate;       // What is the sample rate?
    int         m_redrawRate;       // How often during loading should we force a redraw?
    double      m_capacity;         // What is the total capacity for the butter
    int         m_capacitySamples;  // What is the capacity in samples?
    int         m_cnt;              // How many samples have been loaded in?
};
