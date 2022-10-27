//
// Name :         DirSound.cpp
// Description :  Implementation of the CDirSound class.
//                CDirSound is a class that creates a single DirectSound object that
//                can be used by multiple sound sources for playback.  This class should
//                be instantiated once for your application.
// Notice :       To utilize this class, you MUST call CoInitialize in your InitInstance()
//                function and call Initialize on this function after the system is stable.
//

#include "stdafx.h"
#include "DirSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirSound::CDirSound()
{
    CoInitialize(NULL);
}

CDirSound::~CDirSound()
{

}

bool CDirSound::Initialize()
{
   HRESULT hr;

   // Create the DirectSound object that we will use.
   // If this line fails, it is likely that you have not 
   // initialized COM.  Be sure you called CoInitialize or
   // CoInitializeEx before initializing this class.
   hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL);
   if(hr != S_OK)
   {
      AfxMessageBox(TEXT("Unable to initialize DirectSound"));
      return false;
   }

   // Set the cooperative level to PRIORITY so we will be
   // able to set the sample rate.
   if(m_pDirectSound->SetCooperativeLevel(AfxGetMainWnd()->m_hWnd, DSSCL_PRIORITY) != S_OK)
   {
      AfxMessageBox(TEXT("Unable to set DirectSound priority level"));
      return false;
   }

   //
   // Create the primary sound buffer
   //

   DSBUFFERDESC   dsbdesc;
   memset(&dsbdesc, 0, sizeof(dsbdesc));
   dsbdesc.dwSize = sizeof(dsbdesc);
   dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
   if(m_pDirectSound->CreateSoundBuffer(&dsbdesc, &m_pPrimaryBuffer, NULL) != S_OK)
      return false;

   //
   // Set primary buffer format
   //

   memset(&m_wfx, 0, sizeof(WAVEFORMATEX)); 
   m_wfx.wFormatTag = WAVE_FORMAT_PCM; 
   m_wfx.nChannels = 2; 
   m_wfx.nSamplesPerSec = 44100; 
   m_wfx.wBitsPerSample = 16; 
   m_wfx.nBlockAlign = m_wfx.wBitsPerSample / 8 * m_wfx.nChannels;
   m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;

   m_pPrimaryBuffer->SetFormat(&m_wfx); 
 

   return true;
}
