/*
 *  Name :         DirSoundSource.h
 *  Description :  Header file for implementation of DXSHOW-based media file input.
 *  Version :      See DirSoundSource.cpp
 */

#pragma once

#ifndef _DIRSOUNDSOURCE_H
#define _DIRSOUNDSOURCE_H

#include <vector>
#include <list>
#include <string>
#include <Afxmt.h>

// The DirectX and Windows headers
#include <dshow.h>

namespace std
{
#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif
}

// class CDirSoundSource
// Input module for DXSHOW files.

class CDirSoundSource
{
public:
    CDirSoundSource();
    ~CDirSoundSource();
    friend class CMovieCallback;

    bool Open(const char *p_filename);
    bool Open(const WCHAR *p_filename);
    bool Close();
    bool IsOpen() const {return m_isopen;}
    bool HasAudio() const {return m_hasaudio && !m_audioeof;}

    bool ReadFrame(short *frame);
    bool ReadStereoFrame(short *p_frame)
    {
        if(!ReadFrame(p_frame)) return false;
        if(m_audiochannels == 1)
            p_frame[1] = p_frame[0];
        return true;
    }

    void Rewind();

    double SampleRate() const {return m_audiosamplerate;}
    int NumChannels() const {return m_audiochannels;}
    double GetDuration() const {return m_duration;}
    int NumSampleFrames() const {return int(m_duration * m_audiosamplerate);}

    void SetErrorMsgBox(bool s) {m_errorMsgBox = s;}
    const std::tstring &ErrorMsg() const {return m_errorMsg;}

private:
    bool TryOpen(const char *p_filename);
    void NewAudioSample(IMediaSample *s);
    bool Error(const TCHAR *msg);

    HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, CComPtr<IPin> &pin);
    bool NullRendererDownstream(IBaseFilter *filter);
    bool DeleteDownstream(IBaseFilter *filter);
    void CheckEvents();
    bool InputConnected(IBaseFilter *filter);

    struct Sample
    {
        std::vector<BYTE>   m_data;
        double              m_cur;
        double              m_end;
    };

    // Error management
    std::tstring                    m_errorMsg;
    bool                            m_errorMsgBox;  // Messasge box on error

    // General graph objects
    CComPtr<IGraphBuilder>          m_graphBuilder;
    CComPtr<IFilterGraph>           m_filterGraph;
    CComPtr<IMediaControl>          m_mediaControl;
    CComPtr<IMediaPosition>         m_mediaPosition;
    CComPtr<IMediaEvent>            m_mediaEvent;
    CComPtr<IMediaSeeking>          m_mediaSeeking;
    HANDLE                          m_eventHandle;

    bool                            m_isopen;
    double                          m_duration;    // Stream duration
    double                          m_seekTime;     // Where we seeked to

    //
    // Audio objects
    //

    CComPtr<IBaseFilter>            m_audioGrabberBF;

    bool                            m_hasaudio;             // Audio data available
    CMovieCallback                 *m_audioCallback;
    CEvent                          m_videoRead;

    std::list<Sample>               m_audioSamples;
    CCriticalSection                m_audioSamplesLock;

    Sample                         *m_audioCurrentSample;
    int                             m_audioCurrentSamplePos;

    double                          m_audiosampleduration;  // Inverse of sample rate
    int                             m_audiochannels;        // Number of channels of audio in file
    double                          m_audiosamplerate;      // Sample rate of any audio stream
    int                             m_audiobitsize;         // Audio sample size in bits
    bool                            m_audioStreamEof;       // End of the audio stream has been reached
    bool                            m_audioeof;             // At end of file for audio
    int                             m_audiobytesperframe;
    double                          m_audioSecPerByte;      // Seconds per byte of audio
    CEvent                          m_audioRead;

};


#endif

