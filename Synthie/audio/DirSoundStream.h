// DirSoundStream.h: interface for the CDirSoundStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRSoundStream_H__C453D8B1_8641_4E8C_83D3_9BDB9B1CD06B__INCLUDED_)
#define AFX_DIRSoundStream_H__C453D8B1_8641_4E8C_83D3_9BDB9B1CD06B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

#include "DirSound.h"

const int DSNUMNOTIFY = 2;

class CDirSoundStream : public CObject
{
public:
   CDirSoundStream();
	CDirSoundStream(CDirSound *p_DirSound);
	virtual ~CDirSoundStream();

   void SetDirSound(CDirSound *p_DirSound) {m_pDirSound = p_DirSound;}

	bool Open(CDirSound *p_DirSound=NULL);
	void WriteFrame(short *p_audio);
   bool Close();

	void SetVolume(double p_gain);
	void SetPan(double p_pan);

   void SetChannels(int c) {m_numchannels = c;}
   void SetSampleRate(int s) {m_samplerate = s;}

   void SetBufferDuration(double d) {m_bufferduration = d;}

   UINT ThreadLoop();

protected:
	DECLARE_DYNCREATE(CDirSoundStream)

private:
	void Initialize();
	bool InitializeDirectSound();

	int            m_samplerate;
	int            m_numchannels;
	bool           m_isopen;
	volatile bool  m_threadactive;
	double			m_bufferduration;

   int            m_buffersize;
   volatile DWORD m_bufferwrloc;
   volatile DWORD m_bufferrdloc;
   volatile int   m_bufferfilled;
   bool           m_bufferplaying;

   CCriticalSection m_mutex;
   CEvent         m_threaddone;
   CEvent         m_delaysync;

   CDirSound     *m_pDirSound;
   CComPtr<IDirectSoundBuffer> m_pSoundBuffer;

   // Audio data accumulator
   short          m_audio[2000];
   int            m_audiocnt;
};

#endif // !defined(AFX_DIRSoundStream_H__C453D8B1_8641_4E8C_83D3_9BDB9B1CD06B__INCLUDED_)
