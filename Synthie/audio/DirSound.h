// DirSound.h: interface for the CDirSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRSOUND_H__8D413D16_CE21_45B0_9542_DE158E978D38__INCLUDED_)
#define AFX_DIRSOUND_H__8D413D16_CE21_45B0_9542_DE158E978D38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <mmsystem.h>
#include <amstream.h>
#include <mmstream.h>
#include <control.h>
#include <uuids.h>
#include <dsound.h>

class CDirSound  
{
public:
	bool Initialize();
	CDirSound();
	virtual ~CDirSound();

   WAVEFORMATEX *WaveFmt() {return &m_wfx;}
   IDirectSound *DirectSound() {return m_pDirectSound;}

private:
   CComPtr<IDirectSound>   m_pDirectSound;
   CComPtr<IDirectSoundBuffer> m_pPrimaryBuffer;
   WAVEFORMATEX m_wfx;
};

#endif // !defined(AFX_DIRSOUND_H__8D413D16_CE21_45B0_9542_DE158E978D38__INCLUDED_)
