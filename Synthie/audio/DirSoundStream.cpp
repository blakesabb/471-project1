//
// Name :         DirSoundStream.cpp
// Description :  Implementation of CDirSoundStream
//                This class implements an MP-3 player that uses DirectShow and DirectSound
//                to play an audio selection continuously, looping when done.  Full control 
//                of the Pan position is available.
// Note :         There must be a single instantiation of CDirSound that is initialized.  A
//                pointer to that object can be passed to a constructor, Open, or the
//                SetDirSound function.
//

#include "stdafx.h"
#include <math.h>

#include "DirSoundStream.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDirSoundStream, CObject)

UINT CDirSoundStreamThreadProc( LPVOID pParam );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirSoundStream::CDirSoundStream()
{
    m_pDirSound = NULL;
    Initialize();
}

CDirSoundStream::CDirSoundStream(CDirSound *p_DirSound) : m_pDirSound(p_DirSound)
{
    Initialize();
}


void CDirSoundStream::Initialize()
{
    m_isopen = false;
    m_numchannels = 2;
    m_samplerate = 44100;
    m_audiocnt = 0;
	m_bufferduration = 2.0;
}


CDirSoundStream::~CDirSoundStream()
{
    if(m_isopen)
        Close();
}

//
// Name :         CDirSoundStream::Open()
// Description :  Open a stream for playback using DirectSound.
//                This creates a secondary buffer and manages the 
//                feeding of audio into that secondary buffer.
//

bool CDirSoundStream::Open(CDirSound *p_DirSound)
{
    if(p_DirSound)
        SetDirSound(p_DirSound);

    if(m_pDirSound == NULL)
        return false;

    // Initialize the DirectSound system
    if(!InitializeDirectSound())
        return false;

    m_isopen = TRUE;

    // Ensure the event is reset before we start the thread
    m_delaysync.ResetEvent();

    // This class creates a thread for audio playback.  Begin the thread.
    AfxBeginThread(CDirSoundStreamThreadProc, this, THREAD_PRIORITY_HIGHEST);

    // Initially stopped until we get some data into it
    m_pSoundBuffer->Stop();
    m_bufferplaying = false;

    // And start the secondary buffer playing
    // m_pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

    m_threaddone.ResetEvent();
    m_delaysync.SetEvent();

    return true;
}

//
// Name :         CDirSoundStream::InitializeDirectSound()
// Description :  Initialize the DirectSound secondary buffer that
//                we will be using in this application.  Also create
//                the position events we'll use to synchronize the filling
//                of the secondary buffer.
//

bool CDirSoundStream::InitializeDirectSound()
{
   //
   // Create a secondary buffer
   //

   DSBUFFERDESC dsbdesc;
   memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
   dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
   dsbdesc.dwFlags = 
            DSBCAPS_GETCURRENTPOSITION2   // Always a good idea
            | DSBCAPS_GLOBALFOCUS         // Allows background playing
            | DSBCAPS_CTRLPOSITIONNOTIFY  // Needed for notification
            | DSBCAPS_CTRLPAN             // Allow for panning
            | DSBCAPS_CTRLVOLUME;         // Allow volume control
 
   // The size of the buffer is arbitrary, but should be at least
   // two seconds, to keep data writes well ahead of the play
   // position.

   //
   // Set secondary buffer format
   //

   WAVEFORMATEX wfx;

   memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
   wfx.wFormatTag = WAVE_FORMAT_PCM; 
   wfx.nChannels = m_numchannels; 
   wfx.nSamplesPerSec = m_samplerate; 
   wfx.wBitsPerSample = 16; 
   wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
   wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

   m_buffersize = int(wfx.nAvgBytesPerSec * m_bufferduration); 
   m_bufferwrloc = 0;
   m_bufferrdloc = 0;
   m_bufferfilled = 0;

   dsbdesc.dwBufferBytes = m_buffersize;
   dsbdesc.lpwfxFormat = &wfx;
 
   if(m_pDirSound->DirectSound()->CreateSoundBuffer(&dsbdesc, &m_pSoundBuffer, NULL) != S_OK)
      return false;

   return true;
}


//
// Name :         CDirSoundStream::Close()
// Description :  Close a currently open playback stream.
//

bool CDirSoundStream::Close()
{
   if(!m_isopen)
      return false;

   // Pad the end with 500ms of silence.
   short silence[2] = {0, 0};
   for(int i=0;  i<m_samplerate / 2;  i++)
      WriteFrame(silence);

   // This ensures we will finish playing...
   while(m_bufferplaying)
   {
      Sleep(100);
   }

   // Stop the playback
   m_pSoundBuffer->Stop();

   // End the thread
   // I set the m_threadactive flag to false to end the loop in the thread.
   // Then I set the notify events so it will immediately stop waiting
   // for playback that will never end.  Finally, I wait for an event
   // that signifies that the thread is done.
   m_threadactive = false;

	CSingleLock dnlock(&m_threaddone);
	dnlock.Lock();

   // No more buffers needed.
   m_pSoundBuffer.Release();

   m_isopen = false;

   return true;
}


//
// Name :        CDirSoundStream::WriteFrame()
// Description : Write into the direct sound buffer.
//

void CDirSoundStream::WriteFrame(short *p_audio)
{
   //
   // I accumulate data into a small buffer so we 
   // can move a larger chunk at a time to the 
   // audio buffer.
   //

   if(m_numchannels == 2)
   {
      m_audio[m_audiocnt++] = p_audio[0];
      m_audio[m_audiocnt++] = p_audio[1];
   }
   else
   {
      m_audio[m_audiocnt++] = p_audio[0];
   }

   if(m_audiocnt >= 1996)
   {
      CSingleLock lock(&m_mutex);
      lock.Lock();

      // Do we have enough space to store this?
      while((m_buffersize - m_bufferfilled) < (m_audiocnt * 2 + 5000))
      {
         lock.Unlock();
         Sleep(250);
         lock.Lock();
      }

      // We are ready to move a chunk of data, now.

      short *pBuffer1;
      DWORD size1;
      short *pBuffer2;
      DWORD size2;
      m_pSoundBuffer->Lock(m_bufferwrloc, m_audiocnt * 2, (LPVOID *)(&pBuffer1), &size1, (LPVOID *)(&pBuffer2), &size2, 0);

      memcpy(pBuffer1, (char *)m_audio, size1);
      memcpy(pBuffer2, (char *)m_audio + size1, size2);
      m_audiocnt = 0;

      m_bufferwrloc = (m_bufferwrloc + size1 + size2) % m_buffersize;
      m_bufferfilled += size1 + size2;

      m_pSoundBuffer->Unlock(pBuffer1, size1, pBuffer2, size2);

      if(!m_bufferplaying && m_bufferfilled > 10000)
      {
         m_pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
         m_bufferplaying = true;
      }
   }
}



//
// Name :         CDirSoundStream::SetPan()
// Description :  Set the pan position for a sound.  
// Parameters :   p_pan - -1 is a full left pan, 1 is a full right pan.  0 is centered.
//                This function implements logarithmic panning.
//

void CDirSoundStream::SetPan(double p_pan)
{
   m_pSoundBuffer->SetPan(int(p_pan * 10000));
}


//
// Name :         CDirSoundStream::SetVolume()
// Description :  Set the current sound volume.  
// Parameters :   p_gain - 0 to 1.0, where 1.0 is full volume.  
//                This function implements logarithmic volume controls.
//

void CDirSoundStream::SetVolume(double p_gain)
{
   m_pSoundBuffer->SetVolume(int(p_gain * (DSBVOLUME_MAX - DSBVOLUME_MIN) + DSBVOLUME_MIN));
}





//
// Name :         CDirSoundStreamThreadProc()
// Description :  This is the controlling procedure for the thread.  It's called 
//                by thread creation.
//

UINT CDirSoundStreamThreadProc( LPVOID pParam )
{
	CoInitialize(NULL);

    CDirSoundStream* pObject = (CDirSoundStream *)pParam;

    if (pObject == NULL ||
        !pObject->IsKindOf(RUNTIME_CLASS(CDirSoundStream)))
    return 1;   // if pObject is not valid

    // And call the code in the driver
    return pObject->ThreadLoop();
}


//
// Name :         CDirSoundStream::ThreadLoop()
// Description :  This is the actual in-class thread loop.
//

UINT CDirSoundStream::ThreadLoop()
{
    m_threadactive = TRUE;

    // Wait for event indicating the thread can start
    CSingleLock slock(&m_delaysync);
    slock.Lock();

    while(true)
	{
        Sleep(10);
        if(!m_threadactive)
            break;

        DWORD playpos;
        m_pSoundBuffer->GetCurrentPosition(&playpos, NULL);

        CSingleLock lock(&m_mutex);
        lock.Lock();

        // How much as played since we last checked?
        DWORD played = ((playpos + m_buffersize) - m_bufferrdloc) % m_buffersize;

        m_bufferrdloc = playpos;
        m_bufferfilled -= played;

        // Handle situation where we are running out of data
        // Less than 333ms available...
        if(m_bufferfilled < (m_samplerate / 3) && m_bufferplaying)
        {
            m_bufferplaying = false;
            m_pSoundBuffer->Stop();
        }
	}

	// Signal to the foreground process that the thread is done.
	m_threaddone.SetEvent();

    return 0;
}



