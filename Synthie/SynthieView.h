
// SynthieView.h : interface of the CSynthieView class
//


#pragma once

#include "Progress.h"
#include "audio/wave.h"
#include "audio/DirSoundStream.h"	// Added by ClassView
#include "audio/WaveformBuffer.h"
#include "Synthesizer.h"


// CSynthieView window

class CSynthieView : public CWnd, private CProgress
{
// Construction
public:
	CSynthieView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CSynthieView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	bool m_fileoutput;
	bool m_audiooutput;
	void GenerateWriteFrame(short *p_frame);
	bool OpenGenerateFile(CWaveOut &p_wave);
	void GenerateEnd();
	bool GenerateBegin();

    // Audio destinations..
    CWaveOut        m_wave;
    CDirSoundStream m_soundstream;
    CWaveformBuffer m_waveformBuffer;

	int NumChannels() {return 2;}
	double SampleRate() {return 44100;}

	//recorded performance code
	bool ProcessBegin();
	void ProcessReadFrame(short* p_frame);
	void ProcessWriteFrame(short* p_frame);
	void ProcessEnd();
	bool OpenProcessFile(CWaveOut& p_wave);
	CDirSoundSource   m_wavein;
	//CWaveOut          m_waveout;
	bool m_apply_noise_gating = false;
	bool m_apply_dynamic_range_compression = false;
	vector<short> m_sample;

public:
	afx_msg void OnGenerateFileoutput();
	afx_msg void OnUpdateGenerateFileoutput(CCmdUI *pCmdUI);
	afx_msg void OnGenerateAudiooutput();
	afx_msg void OnUpdateGenerateAudiooutput(CCmdUI *pCmdUI);
	afx_msg void OnGenerate1000hztone();
private:
	CSynthesizer m_synthesizer;
public:
	afx_msg void OnGenerateSynthesizer();
	afx_msg void OnFileOpenscore();
	//for recorded perfromance
	afx_msg void OnFileOpenrecording();
	afx_msg void OnGeneratePlayrecording();
	afx_msg void OnGenerateApplynoisegate();
	afx_msg void OnGenerateDynamicrangecompression();
};

