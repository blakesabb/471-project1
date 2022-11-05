
// SynthieView.cpp : implementation of the CSynthieView class
//

#include "stdafx.h"
#include "Synthie.h"
#include "SynthieView.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

short RangeBound(double d)
{
    if(d < -32768)
        return -32768;
    else if(d > 32767)
        return 32767;

    return (short)d;
}

// CSynthieView

CSynthieView::CSynthieView()
{
    m_audiooutput = true;
    m_fileoutput = false;
	m_synthesizer.SetNumChannels(NumChannels());
	m_synthesizer.SetSampleRate(SampleRate());
}

CSynthieView::~CSynthieView()
{
}


BEGIN_MESSAGE_MAP(CSynthieView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_GENERATE_FILEOUTPUT, &CSynthieView::OnGenerateFileoutput)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_FILEOUTPUT, &CSynthieView::OnUpdateGenerateFileoutput)
	ON_COMMAND(ID_GENERATE_AUDIOOUTPUT, &CSynthieView::OnGenerateAudiooutput)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_AUDIOOUTPUT, &CSynthieView::OnUpdateGenerateAudiooutput)
	ON_COMMAND(ID_GENERATE_1000HZTONE, &CSynthieView::OnGenerate1000hztone)
	ON_COMMAND(ID_GENERATE_SYNTHESIZER, &CSynthieView::OnGenerateSynthesizer)
	ON_COMMAND(ID_FILE_OPENSCORE, &CSynthieView::OnFileOpenscore)
	ON_COMMAND(ID_FILE_OPENRECORDING, &CSynthieView::OnFileOpenrecording)
	ON_COMMAND(ID_GENERATE_PLAYRECORDING, &CSynthieView::OnGeneratePlayrecording)
	ON_COMMAND(ID_GENERATE_APPLYNOISEGATE, &CSynthieView::OnGenerateApplynoisegate)
	ON_COMMAND(ID_GENERATE_DYNAMICRANGECOMPRESSION, &CSynthieView::OnGenerateDynamicrangecompression)
END_MESSAGE_MAP()



// CSynthieView message handlers

BOOL CSynthieView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), 
		NULL);

	return TRUE;
}

void CSynthieView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}


void CSynthieView::OnGenerate1000hztone()
{
	// Call to open the generator output
	if(!GenerateBegin())
		return;

	short audio[2];

	double freq = 1000;
	double duration = 5;

	for(double time=0.;  time < duration;  time += 1. / SampleRate())
	{                 
		audio[0] = short(3200 * sin(time * 2 * PI * freq));
		audio[1] = audio[0];

		GenerateWriteFrame(audio);

		// The progress control
		if(ProgressAbortCheck())
			break;
	}


	// Call to close the generator output
	GenerateEnd();
}




//
// Name :        CSynthieView::GenerateBegin()
// Description : This function opens an audio file for output as
//               m_wave.  Be sure to call EndGenerate() when done.
// Returns :     true if successful...
//

bool CSynthieView::GenerateBegin()
{
	// 
	// Waveform storage
	//

	m_waveformBuffer.Start(NumChannels(), SampleRate());

	if(m_fileoutput)
	{
	  if(!OpenGenerateFile(m_wave))
		 return false;
	}

	ProgressBegin(this);

	if(m_audiooutput)
	{
	  m_soundstream.SetChannels(NumChannels());
	  m_soundstream.SetSampleRate(int(SampleRate()));

	  m_soundstream.Open(((CSynthieApp *)AfxGetApp())->DirSound());
	}

	return true;
}

//
// Name :        CSynthieView::GenerateWriteFrame()
// Description : Write a frame of output to the current generation device.
//

void CSynthieView::GenerateWriteFrame(short *p_frame)
{
    m_waveformBuffer.Frame(p_frame);

    if(m_fileoutput)
        m_wave.WriteFrame(p_frame);

    if(m_audiooutput)
        m_soundstream.WriteFrame(p_frame);
}


//
// Name :        CSynthieView::GenerateEnd()
// Description : End the generation process.
//

void CSynthieView::GenerateEnd()
{
    m_waveformBuffer.End();

    if(m_fileoutput)
        m_wave.close();

    if(m_audiooutput)
        m_soundstream.Close();

    ProgressEnd(this);
}

//
// Name :        CSynthieView::OpenGenerateFile()
// Description : This function opens the audio file for output.
// Returns :     true if successful...
//

bool CSynthieView::OpenGenerateFile(CWaveOut &p_wave)
{
   p_wave.NumChannels(NumChannels());
   p_wave.SampleRate(SampleRate());

	static WCHAR BASED_CODE szFilter[] = L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(FALSE, L".wav", NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
      return false;

   p_wave.open(dlg.GetPathName());
   if(p_wave.fail())
      return false;

   return true;
}

void CSynthieView::OnGenerateFileoutput()
{
	m_fileoutput = !m_fileoutput;
}

void CSynthieView::OnUpdateGenerateFileoutput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_fileoutput);	
}


void CSynthieView::OnGenerateAudiooutput()
{
	m_audiooutput = !m_audiooutput;
}

void CSynthieView::OnUpdateGenerateAudiooutput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_audiooutput);	
}



void CSynthieView::OnGenerateSynthesizer()
{
	// Call to open the generator output
	if (!GenerateBegin())
		return;

	m_synthesizer.Start();
	short audio[2];
	double frame[2];

	while (m_synthesizer.Generate(frame))
	{
		audio[0] = RangeBound(frame[0] * 32767);
		audio[1] = RangeBound(frame[1] * 32767);

		GenerateWriteFrame(audio);

		// The progress control
		if (ProgressAbortCheck())
			break;
	}

	// Call to close the generator output
	GenerateEnd();
}


void CSynthieView::OnFileOpenscore()
{
	static WCHAR BASED_CODE szFilter[] = L"Score files (*.score)|*.score|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, L".score", NULL, 0, szFilter, NULL);
	if (dlg.DoModal() != IDOK)
		return;

	m_synthesizer.OpenScore(dlg.GetPathName());
}

//recorded performance code
void CSynthieView::OnFileOpenrecording()
{
	CDirSoundSource audioin;

	if (!audioin.Open("Happy Birthday.wav"))
	{

	}

	int numframes = audioin.NumSampleFrames();
	m_sample.resize(numframes);

	for (int i = 0; i < numframes; i++)
	{
		short audio[2];

		audioin.ReadFrame(audio);

		m_sample[i] = audio[0];
	}
}

void CSynthieView::ProcessReadFrame(short* p_frame)
{
	m_wavein.ReadFrame(p_frame);
	//m_soundstream.ReadFrame()
}

void CSynthieView::ProcessWriteFrame(short* p_frame)
{
	m_waveformBuffer.Frame(p_frame);

	if (m_fileoutput)
		//m_waveout.WriteFrame(p_frame);
		m_wave.WriteFrame(p_frame);

	if (m_audiooutput)
		m_soundstream.WriteFrame(p_frame);
}

void CSynthieView::ProcessEnd()
{
	m_waveformBuffer.End();

	if (m_fileoutput)
		//m_waveout.close();
		m_wave.close();

	if (m_audiooutput)
		m_soundstream.Close();

	ProgressEnd(this);

}

bool CSynthieView::OpenProcessFile(CWaveOut& p_wave)
{
	p_wave.NumChannels(m_synthesizer.GetNumChannels());
	p_wave.SampleRate(m_synthesizer.GetSampleRate());

	static WCHAR BASED_CODE szFilter[] = L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(FALSE, L".wav", NULL, 0, szFilter, NULL);
	if (dlg.DoModal() != IDOK)
		return false;

	p_wave.open(dlg.GetPathName());
	if (p_wave.fail())
		return false;

	return true;
}

bool CSynthieView::ProcessBegin()
{
	m_wavein.Rewind();

	// 
	// Waveform storage
	//

	m_waveformBuffer.Start(NumChannels(), SampleRate());

	if (m_fileoutput)
	{
		//if (!OpenProcessFile(m_waveout))
		if (!OpenProcessFile(m_wave))
			return false;
	}

	ProgressBegin(this);

	if (m_audiooutput)
	{
		m_soundstream.SetChannels(NumChannels());
		m_soundstream.SetSampleRate(int(SampleRate()));

		m_soundstream.Open(((CSynthieApp*)AfxGetApp())->DirSound());
	}

	return true;
}

void CSynthieView::OnGeneratePlayrecording()
{
	// Call to open the processing output
	if (!ProcessBegin())
		return;

	short audio[2];

	for (int i = 0; i < m_sample.size(); i++)
	{
		//ProcessReadFrame(audio);

		//maybe fix later
		double m_amplitude = 1;

		//Base dry recording 
		if (!m_apply_noise_gating && !m_apply_dynamic_range_compression) {
			audio[0] = short(m_sample[i] * m_amplitude);
			audio[1] = short(m_sample[i] * m_amplitude);
		}

		//When Noise Gating Effect is Applied  - get rid of breath sounds
		if (m_apply_noise_gating) {
			if (m_sample[i] < 300 && m_sample[i] > -300) {
				audio[0] = 0;
				audio[1] = 0;
			}
			else {
				audio[0] = short(m_sample[i] * m_amplitude);
				audio[1] = short(m_sample[i] * m_amplitude);
			}
		}

		//Applying the dynamic Compression
		if (m_apply_dynamic_range_compression) {
			if (m_sample[i] > 12000) {

				audio[0] = 12000;
				audio[1] = 12000;
			}
			else if (m_sample[i] < -12000) {

				audio[0] = 12000;
				audio[1] = 12000;
			}
			else {
				audio[0] = short(m_sample[i] * m_amplitude);
				audio[1] = short(m_sample[i] * m_amplitude);
			}
		}
		ProcessWriteFrame(audio);
	}
	// Call to close the generator output
	ProcessEnd();
}

void CSynthieView::OnGenerateApplynoisegate()
{
	m_apply_noise_gating = !m_apply_noise_gating;
}


void CSynthieView::OnGenerateDynamicrangecompression()
{
	m_apply_dynamic_range_compression = !m_apply_dynamic_range_compression;
}


