
// Synthie.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "Synthie.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSynthieApp

BEGIN_MESSAGE_MAP(CSynthieApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSynthieApp::OnAppAbout)
END_MESSAGE_MAP()

//ON_COMMAND(ID_FILE_OPENRECORDING, &CSynthieApp::OnFileOpenrecording)


// CSynthieApp construction

CSynthieApp::CSynthieApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSynthieApp object

CSynthieApp theApp;


// CSynthieApp initialization

BOOL CSynthieApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


    // Initialize our DirectSound interface class.  This must be
    // done before we dispatch any command line options since
    // that is when we create new windows.
    if(!m_DirSound.Initialize())
        return false;





	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}


// CSynthieApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CSynthieApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSynthieApp customization load/save methods

void CSynthieApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CSynthieApp::LoadCustomState()
{
}

void CSynthieApp::SaveCustomState()
{
}

// CSynthieApp message handlers

//code from step 3 for playing recording
/*
bool CSynthieApp::ProcessBegin()
{
	m_wavein.Rewind();

	// 
	// Waveform storage
	//

	m_waveformBuffer.Start(NumChannels(), SampleRate());

	if (m_fileoutput)
	{
		if (!OpenProcessFile(m_waveout))
			return false;
	}

	ProgressBegin(this);

	if (m_audiooutput)
	{
		m_soundstream.SetChannels(NumChannels());
		m_soundstream.SetSampleRate(int(SampleRate()));

		m_soundstream.Open(((CAudioProcessApp*)AfxGetApp())->DirSound());
	}

	return true;
}


//
// Name :        CAudioProcessDoc::ProcessReadFrame()
// Description : Read a frame of input from the current audio file.
//

void CAudioProcessDoc::ProcessReadFrame(short* p_frame)
{
	m_wavein.ReadFrame(p_frame);
}


//
// Name :        CAudioProcessDoc::ProcessWriteFrame()
// Description : Write a frame of output to the current generation device.
//

void CAudioProcessDoc::ProcessWriteFrame(short* p_frame)
{
	m_waveformBuffer.Frame(p_frame);

	if (m_fileoutput)
		m_waveout.WriteFrame(p_frame);

	if (m_audiooutput)
		m_soundstream.WriteFrame(p_frame);
}


//
// Name :        CAudioProcessDoc::ProcessEnd()
// Description : End the generation process.
//

void CAudioProcessDoc::ProcessEnd()
{
	m_waveformBuffer.End();

	if (m_fileoutput)
		m_waveout.close();

	if (m_audiooutput)
		m_soundstream.Close();

	ProgressEnd(this);


}
*/

/*
void CSynthieApp::OnFileOpenrecording()
{
	CDirSoundSource audioin;

	if (!audioin.Open("Octave.wav"))

	{

		// Deal with the error
	}

	int numframes = audioin.NumSampleFrames();
	m_sample.resize(numframes);
	//m_sampleL.resize(numframes);
	//m_sampleR.resize(numframes);

	for (int i = 0; i < numframes; i++)
	{
		short audio[2];

		audioin.ReadFrame(audio);

		m_sample[i] = audio[0];
		//m_sampleL[i] = audio[0];
		//m_sampleR[i] = audio[0];
	}
}*/

	//code for playing the sample

	/*if (!ProcessBegin())
		return;

	short audio[2];

	for (int i = 0; i < SampleFrames(); i++)
	{
		ProcessReadFrame(audio);

		audio[0] = short(audio[0] * m_amplitude);
		audio[1] = short(audio[1] * m_amplitude);

		ProcessWriteFrame(audio);

		// The progress control
		if (!ProcessProgress(double(i) / SampleFrames()))
			break;
	}


	// Call to close the generator output
	ProcessEnd();

}*/


/*void CSynthieApp::OnFileOpenrecording()
{
	static WCHAR BASED_CODE szFilter[] = L"Audio Files (*.wav;*.mp3)|*.wav; *.mp3|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, 0, szFilter, NULL);
	if (dlg.DoModal() != IDOK)
		return;

	POSITION position = GetFirstDocTemplatePosition(); //Gets the position of the first document template in the application.
	//NULL if the list is empty.
	GetNextDocTemplate(position); //position cannot be null here after GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = GetNextDocTemplate(position);
	pTemplate->OpenDocumentFile(dlg.GetPathName());
}

CDocument* CSynthieApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	POSITION position = GetFirstDocTemplatePosition();
	GetNextDocTemplate(position);
	CDocTemplate* pTemplate = GetNextDocTemplate(position);
	return pTemplate->OpenDocumentFile(lpszFileName);
}*/
