
// Synthie.h : main header file for the Synthie application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "audio/DirSound.h"	// Added by ClassView
#include <audio/DirSoundSource.h>
#include <vector>


// CSynthieApp:
// See Synthie.cpp for the implementation of this class
//

class CSynthieApp : public CWinAppEx
{
public:
	CSynthieApp();

    CDirSound *DirSound() {return &m_DirSound;}

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
    CDirSound   m_DirSound;
	//vector<short> m_sampleL;
	//vector<short> m_sampleR;
	std::vector<short> m_sample; //variable for recorder perfromance
	//code from step 3 - for playing recording
	/*void ProcessEnd();
	bool ProcessBegin();
	CDirSoundSource   m_wavein;
	CWaveOut          m_waveout;
	CWaveformBuffer m_waveformBuffer;*/

public:
	//afx_msg void OnFileOpenrecording();
	//virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

extern CSynthieApp theApp;
