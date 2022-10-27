//
// Name :         CDirSoundSource.cpp
// Description :  This module implements input from DXSHOW movie files.
// Written By :   Charles B. Owen
// Version :      3.00 10-11-09 Completely new version
//

#include "StdAfx.h"

#include <cassert>
#include <vector>
#include <string>

#include "DirSoundSource.h"

// This fixes a problem that dxtrans.h is missing, yet is 
// included by qedit.h
#pragma include_alias( "dxtrans.h", "qedit.h" )

#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__

#include "qedit.h"

using namespace std;

// The video callback class
class CMovieCallback : public ISampleGrabberCB
{
public:
    CMovieCallback(CDirSoundSource *p_user, bool iv) : m_user(p_user), m_isVideo(iv) {}

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppv); 
    virtual HRESULT STDMETHODCALLTYPE SampleCB( double SampleTime, IMediaSample * pSample );
    virtual HRESULT STDMETHODCALLTYPE BufferCB( double dblSampleTime, BYTE * pBuffer, long iBufSize );

private:
    CDirSoundSource    *m_user;
    bool             m_isVideo;
};


/*
 *  Name :         CDirSoundSource::CDirSoundSource()
 *  Description :  Constructor for DXSHOW input module.
 */

CDirSoundSource::CDirSoundSource()
{
    CoInitialize(NULL);

    m_hasaudio = false;
    m_isopen = false;
    m_errorMsgBox = true;

    m_audioCallback = new CMovieCallback(this, false);
}


/*
 *  Name :         CDirSoundSource::~CDirSoundSource()
 *  Description :  Destructor for module.  Note that modules are not
 *                 closed, but rather destroyed.  This takes care of
 *                 closing any open file.
 */

CDirSoundSource::~CDirSoundSource(void)
{
    Close();
    delete m_audioCallback;

    CoUninitialize();
}



/*
 *  Name :         CDirSoundSource::Open()
 *  Description :  Open the media file.
 */

bool CDirSoundSource::Open(const wchar_t *p_filename)
{
    string filename;

    size_t len = wcslen(p_filename);
    vector<char> vfilename;
    vfilename.resize(len + 1);
    int cnt = WideCharToMultiByte(CP_UTF8, 0, p_filename, -1, &vfilename[0], int(len) + 1, NULL, NULL);
    filename = &vfilename[0];

    return Open(filename.c_str());
}


bool CDirSoundSource::Open(const char *p_filename)
{
    Close();

    if(!TryOpen(p_filename))
    {
        Close();        // Open failed, so return to a closed state
        if(m_errorMsgBox)
        {
            AfxMessageBox(m_errorMsg.c_str());
        }
        return false;
    }

    return true;
}

//
// Name :         CDirSoundSource::TryOpen()
// Description :  Attempt to open the stream. This is the function
//                that actually does all of the work.
//

bool CDirSoundSource::TryOpen(const char *p_filename)
{
    HRESULT hr;
    string filename;

    filename = p_filename;

    // Until we know
    m_hasaudio = false;
    m_audiosamplerate = 0.;
    m_audioStreamEof = false;
    m_audioeof = false;
    m_seekTime = 0;
    m_audioCurrentSample = NULL;
    m_audioCurrentSamplePos = 0;

    m_audioSamples.clear();
   
    // Create the filter graph
    if (FAILED(m_graphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
        return Error(TEXT("Unable to create a DirectShow filter graph"));

    // Obtain a bunch of interfaces we will neet
    m_graphBuilder.QueryInterface(&m_mediaControl);
    m_graphBuilder.QueryInterface(&m_mediaPosition);
    m_graphBuilder.QueryInterface(&m_filterGraph);
    m_graphBuilder.QueryInterface(&m_mediaSeeking);

    m_graphBuilder.QueryInterface(&m_mediaEvent);
    m_mediaEvent->GetEventHandle( (OAEVENT*)&m_eventHandle);


    //
    // Create the audio sample grabber
    //

    if(FAILED(m_audioGrabberBF.CoCreateInstance(CLSID_SampleGrabber)))
        return Error(TEXT("Unable to create a DirectShow sample grabber"));

    CComPtr<ISampleGrabber>         audioGrabber;
    m_audioGrabberBF.QueryInterface(&audioGrabber);

    // Set the sample grabber media type
    AM_MEDIA_TYPE asg;

    ZeroMemory(&asg, sizeof(AM_MEDIA_TYPE));
    asg.majortype = MEDIATYPE_Audio;
    asg.subtype = MEDIASUBTYPE_PCM;

    audioGrabber->SetMediaType(&asg);

    if(FAILED(m_graphBuilder->AddFilter(m_audioGrabberBF, L"AudioGrabber")))
        return Error(TEXT("Unable to add the video sample grabber to the filter graph"));

    //
    // Now we render the file.  The video will connect to the sample grabber.
    // A video renderer will be created as well, which we'll have to remove.
    //

    int flen = int(filename.length());
    std::vector<WCHAR>   wPath(flen+1);
    MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, &wPath[0], flen+1);

    hr = m_graphBuilder->RenderFile(&wPath[0], NULL);
    if(hr != S_OK)
        return Error(TEXT("Unable to open video file"));

    // Determine if anything connected to our sample grabber
    if(InputConnected(m_audioGrabberBF))
        m_hasaudio = true;
    else
        m_audioGrabberBF.Release();

    if(m_hasaudio)
    {
        if(!NullRendererDownstream(m_audioGrabberBF))
            return false;

        audioGrabber->SetBufferSamples(false);

        audioGrabber->SetCallback(m_audioCallback, 0);
        
        //
        // Determine information about the audio.
        //

        audioGrabber->GetConnectedMediaType(&asg);
        WAVEFORMATEX *wf = (WAVEFORMATEX *)(asg.pbFormat);

        m_audiosamplerate = wf->nSamplesPerSec;
        m_audiosampleduration = 1./m_audiosamplerate;
        m_audiobitsize = wf->wBitsPerSample;
        m_audiochannels = wf->nChannels;
        m_audiobytesperframe = m_audiobitsize / 8 * m_audiochannels;
        m_audioSecPerByte = m_audiosampleduration / m_audiobytesperframe;
        m_audioRead.SetEvent();
    }

    //
    // We set the graph synchronization source to NULL so the
    // graph has no clock and will run as fast as possible.
    //

    CComPtr<IMediaFilter> mediaFilter;
    m_graphBuilder.QueryInterface(&mediaFilter);

    mediaFilter->SetSyncSource(NULL);

    REFTIME length;
    m_mediaPosition->get_Duration(&length);
    m_duration = length;


    m_isopen = true;
    m_mediaControl->Run();

    return true;
}

//
// Name :         CDirSoundSource::InputConnected(IBaseFilter *filter)
// Description :  Returns true if the input of a filter is connected to something.
//

bool CDirSoundSource::InputConnected(IBaseFilter *filter)
{
    // Locate the filters input pin
    CComPtr<IPin> inputPin;
    GetPin(filter, PINDIR_INPUT, 0, inputPin);
    if(inputPin == NULL)
        return false;

    CComPtr<IPin> fromPin;
    inputPin->ConnectedTo(&fromPin);
    if(fromPin == NULL)
        return false;

    return true;
}


//
// Name :         CDirSoundSource::NullRendererDownstream()
// Description :  Given a pointer to a filter in the filter graph, this function
//                removes all filters that are downstream of that filter and replaces
//                them with a Null Renderer.  This is used to
//                remove the filters automatically created by the 
//                RenderFile function.
//

bool CDirSoundSource::NullRendererDownstream(IBaseFilter *filter)
{
    HRESULT hr;

    // Delete everything downstream from this filter
    if(!DeleteDownstream(filter))
        return false;

    //
    // Locate the filters output pin
    //

    CComPtr< IPin > pGrabOut;
    hr = GetPin(filter, PINDIR_OUTPUT, 0, pGrabOut);

    //
    // Create a null renderer to follow the filter
    //

    CComPtr<IBaseFilter> nullRenderer;

    if(FAILED(nullRenderer.CoCreateInstance(CLSID_NullRenderer)))
        return Error(TEXT("Unable to create null renderer filter"));

    if(FAILED(m_graphBuilder->AddFilter(nullRenderer, L"NullRenderer")))
        return Error(TEXT("Unable to add null renderer to the filter graph"));

    //
    // Get the pins for the sample grabber and the null renderer
    //


    CComPtr< IPin > pNullIn;
    if(FAILED(GetPin(nullRenderer, PINDIR_INPUT, 0, pNullIn)))
        return Error(TEXT("Unable find the null renderer input pin."));

    if(FAILED(m_graphBuilder->Connect( pGrabOut, pNullIn )))
        return Error(TEXT("Unable find the sample grabber output pin."));

    return true;
}


//
// Name :       CDirSoundSource::DeleteDownstream()
// Description: Delete all nodes downstream of a selected node.
// Note :       Recursive.
//

bool CDirSoundSource::DeleteDownstream(IBaseFilter *filter)
{
    HRESULT hr;

    //
    // Locate the filters output pin
    //

    CComPtr< IPin > pGrabOut;
    if(GetPin(filter, PINDIR_OUTPUT, 0, pGrabOut) != S_OK || pGrabOut == NULL)
        return true;            // If no output pin

    // What is it connected to?
    CComPtr<IPin> pGrabToPin;
    hr = pGrabOut->ConnectedTo(&pGrabToPin);
    if(pGrabToPin == NULL)
        return true;            // If output pin not connected to anything

    // Determine the connected filter
    PIN_INFO pinfo;
    pGrabToPin->QueryPinInfo(&pinfo);

    IBaseFilter *filterDownstream = pinfo.pFilter;

    // Disconnect them.
    hr = m_filterGraph->Disconnect(pGrabOut);
    hr = m_filterGraph->Disconnect(pGrabToPin);

    DeleteDownstream(filterDownstream);
    hr = m_filterGraph->RemoveFilter(filterDownstream);
    return true;
}


/*
 * Name:        CDirSoundSource::GetPin()
 * Description: A helper func which get input/output pins of a filter
 */

HRESULT CDirSoundSource::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, CComPtr<IPin> &pin)
{
    CComPtr< IEnumPins > pEnum;
    pin = NULL;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;
        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                pin = pPin;
                // Found requested pin, so clear error
                hr = S_OK;
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}



//
// Name :        CDirSoundSource::Close()
// Description : Close the open file.
//

bool CDirSoundSource::Close()
{
    if(!m_isopen)
        return false;

    m_isopen = false;

    if(m_mediaControl != NULL)
    {
        m_audioRead.SetEvent();   // Release any threads that may be waiting
        m_videoRead.SetEvent();   // Release any threads that may be waiting
        m_mediaControl->Stop();

        // Second time just in case
        m_audioRead.SetEvent();   // Release any threads that may be waiting
        m_videoRead.SetEvent();   // Release any threads that may be waiting
    }

    m_graphBuilder.Release();
    m_filterGraph.Release();
    m_mediaControl.Release();
    m_mediaPosition.Release();
    m_mediaEvent.Release();
    m_mediaSeeking.Release();
    m_audioGrabberBF.Release();

    m_audioSamples.clear();

    m_hasaudio = false;

    return true;
}




//
// Name :        CDirSoundSource::ReadFrame()
// Description : Read an single audio frame.
// Returns :     true if audio available.
//               false if no audio or end of file.
//

bool CDirSoundSource::ReadFrame(short *frame)
{
    CheckEvents();

    // Do we have a current sample?
    if(m_audioCurrentSample == NULL)
    {
        // No, see if we can get one.
        while(true)
        {
            // Lock the queue of audio sample blocks
            m_audioSamplesLock.Lock();

            // If the queue is empty, we need to wait for
            // more audio samples if any more are coming
            if(m_audioSamples.empty())
            {
                // Are we done?
                if(m_audioeof)
                {
                    // No more are coming
                    frame[0] = frame[1] = 0;
                    m_audioSamplesLock.Unlock();
                    return false;
                }
                else
                {
                    // Unlock and wait
                    m_audioSamplesLock.Unlock();
                    Sleep(10);
                    continue;
                }
            }

            m_audioCurrentSample = &m_audioSamples.front();
            m_audioCurrentSamplePos = 0;

            m_audioSamplesLock.Unlock();
            break;
        }
    }

    //
    // Get the data
    //

    int c = 0;

	if(m_audiobitsize == 16)
	{
		for(c=0;  c<m_audiochannels; c++)
		{
			frame[c] = *((short *)(&(m_audioCurrentSample->m_data[m_audioCurrentSamplePos])));  
			m_audioCurrentSamplePos += 2;
		}
	}
	else
	{
		for(c=0;  c<m_audiochannels; c++)
		{
			frame[c] = (*((char *)(&(m_audioCurrentSample->m_data[m_audioCurrentSamplePos]))) + 128) * 256;  
			m_audioCurrentSamplePos ++;
		}	

	}


		//for(int i=0;  i<m_numchannels;  i++)
		//{
		//	p_frame[i] = (*((char *)m_localbufferpos) + 128) * 256;
		//	m_localbufferpos++;
		//}

    for( ; c<2;  c++)
    {
        frame[c] = 0;
    }

    //
    // Have we consumed the frame?
    //

    if(m_audioCurrentSamplePos >= int(m_audioCurrentSample->m_data.size()))
    {
        m_audioSamplesLock.Lock();
        m_audioSamples.pop_front();

        if(m_audioSamples.empty())
        {
            // No more at this time
            m_audioCurrentSample = NULL;
        }
        else
        {
            m_audioCurrentSample = &m_audioSamples.front();
        }

        m_audioCurrentSamplePos = 0;

        m_audioSamplesLock.Unlock();
    }

    return true;
}


void CDirSoundSource::Rewind()
{
    if(!m_isopen)
        return;

    // First we stop everything.  This is like a partial close here
    m_isopen = false;

    if(m_mediaControl != NULL)
    {
        m_videoRead.SetEvent();
        m_audioRead.SetEvent();
        m_mediaControl->Stop();
        m_videoRead.SetEvent();
        m_audioRead.SetEvent();
    }

    Sleep(1000);

	m_audioSamplesLock.Lock();

    // Clear all of the queues
    m_audioSamples.clear();
    m_audioCurrentSample = NULL;
    m_audioCurrentSamplePos = 0;

    // Get a seeking interface
    CComPtr<IMediaSeeking> mediaSeeking;
    m_filterGraph.QueryInterface(&mediaSeeking);

    REFERENCE_TIME reftime = (REFERENCE_TIME)(0);
    HRESULT hr = mediaSeeking->SetPositions(&reftime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

    m_audioStreamEof = false;
    m_audioeof = false;


    // Back active again
    m_mediaControl->Run();
    m_isopen = true;

	m_audioSamplesLock.Unlock();
}



void CDirSoundSource::CheckEvents()
{
    if(WaitForSingleObject(m_eventHandle, 0) == WAIT_OBJECT_0)
    {
        long eventCode, param1, param2;
        m_mediaEvent->GetEvent(&eventCode, &param1, &param2, 0);
        if(eventCode == EC_COMPLETE)
        {
            m_audioStreamEof = true;
        }

        m_mediaEvent->FreeEventParams(eventCode, param1, param2);
    }
}





ULONG CMovieCallback::AddRef() {return 2;}
ULONG CMovieCallback::Release() {return 1;}

//
// Name :        CMovieCallback::QueryInterface()
// Description : Return a pointer to a specific interface for this class.
//

HRESULT CMovieCallback::QueryInterface(REFIID riid, void ** ppv) 
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}    
	return ResultFromScode(E_NOINTERFACE);
}

HRESULT CMovieCallback::SampleCB( double SampleTime, IMediaSample * pSample ) 
{ 
    m_user->NewAudioSample(pSample);
    return 0; 
};

HRESULT CMovieCallback::BufferCB( double dblSampleTime, BYTE * pBuffer, long iBufSize )  {return 0;}


void CDirSoundSource::NewAudioSample(IMediaSample *s)
{ 
    m_audioSamplesLock.Lock();

    if(!m_isopen)
    {
        m_audioSamplesLock.Unlock();
        return;
    }

    long size = s->GetActualDataLength();
    BYTE *ptr;
    s->GetPointer(&ptr);

    m_audioSamples.push_back(Sample());
    Sample &samp = m_audioSamples.back();

    REFERENCE_TIME cur, end;
    s->GetTime(&cur, &end);
    samp.m_cur = m_seekTime + cur * 0.0000001;
    samp.m_end = m_seekTime + end * 0.0000001;

    samp.m_data.resize(size);
    memcpy(&samp.m_data[0], ptr, size);

    m_audioSamplesLock.Unlock();
}

//
// Name :         CDirSoundSource::Error(const TCHAR *msg)
// Description :  Sets the error string and returns false (error value)
//

bool CDirSoundSource::Error(const TCHAR *msg)
{
    m_errorMsg = msg;
    return false;
}