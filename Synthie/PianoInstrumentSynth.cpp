#include "stdafx.h"

#include "PianoInstrumentSynth.h"
#include "audio/DirSoundSource.h"

CPianoInstrumentSynth::CPianoInstrumentSynth(void)
{
	m_sustain = false;
	m_dynam = 0.34;

	//debugging code
	//LoadLoudPiano("MiddleC");
	//PianoLoad("MiddleC", m_loudwave);
	//LoadSoftPiano("MiddleCSoft");
	//PianoLoad("MiddleCSoft", m_softwave);
	//PedalPress("PianoPedalPress");
	//PedalRelease("PianoPedalRelease");
}

CPianoInstrumentSynth::~CPianoInstrumentSynth(void)
{

}

CPianoInstrument* CPianoInstrumentSynth::CreateInstrument()
{
	m_instrument = new CPianoInstrument();

	m_instrument->Sustain(m_sustain);
	m_instrument->GetPlayer()->SetSamples(&m_wave, (int)m_wave.size());

	return m_instrument;
}

bool CPianoInstrumentSynth::LoadSoftPiano(const char *pianofile) 
{
	// step 1 clear
	m_softwave.clear();
	// step 2 access file
	CDirSoundSource m_file;

	if (!m_file.Open(pianofile)) {
		CString err = L"Read file error";
		err += pianofile;
		AfxMessageBox(err);
		return false;
	}

	for (int f = 0; f < m_file.NumSampleFrames(); f++) {
		short frame[2];
		m_file.ReadFrame(frame);
		m_softwave.push_back(frame[0]);
	}
	// step 4 stop
	m_file.Close();
	return true;
}

bool CPianoInstrumentSynth::LoadLoudPiano(const char* pianofile)
{
	m_loudwave.clear();
	CDirSoundSource m_file;

	if (!m_file.Open(pianofile)) {
		CString err = L"Read file error";
		err += pianofile;
		AfxMessageBox(err);
		return false;
	}

	for (int f = 0; f < m_file.NumSampleFrames(); f++) {
		short frame[2];
		m_file.ReadFrame(frame);
		m_loudwave.push_back(frame[0]);
	}
	// step 4 stop
	m_file.Close();
	return true;
}

bool CPianoInstrumentSynth::PianoLoad(const char* pianofile, std::vector<short>& table)
{
	table.clear();
	CDirSoundSource audioin;
	
	if (!audioin.Open(pianofile)) {
		CString err = L"Read file error";
		err += pianofile;
		AfxMessageBox(err);
		return false;
	}
	int numframes = audioin.NumSampleFrames();
	table.resize(numframes);

	for (int f = 0; f < numframes; f++) {
		short frame[2];
		audioin.ReadFrame(frame);
		//table.push_back(frame[0]);
		table[f] = frame[0];
	}
	// step 4 stop
	audioin.Close();
	return true;
}

void CPianoInstrumentSynth::Interpolate(double dyna) {
	// start interpolation
	m_wave.clear();
	// apply interpolation equation btw loud and soft W/ DYNAM
	for (unsigned int w = 0; w < m_loudwave.size(); w++) {
		                              //apply equation
		m_wave.push_back(short(dyna * m_loudwave.at(w) + (1 - dyna) * m_softwave.at(w)));
	}
}

bool CPianoInstrumentSynth::PedalPress(const char* press_file) {
	// step 1 clear
	m_pedalPress.clear();
	// step 2 access file
	CDirSoundSource m_file;
	if (!m_file.Open(press_file)) {
		CString err = L"Read file error";
		err += press_file;
		AfxMessageBox(err);
		return false;
	}
	// step 3 the pedal has been pressed
	for (int f = 0; f < m_file.NumSampleFrames(); f++) {
		short frame[2];
		m_file.ReadFrame(frame);
		m_pedalPress.push_back(frame[0]/6);
	}
	// step 4 stop
	m_file.Close();
	return true;

}


bool CPianoInstrumentSynth::PedalRelease(const char* release_file) {
	// step 1 clear
	m_pedalReleased.clear();
	// step 2 access file
	CDirSoundSource m_file;

	if (!m_file.Open(release_file)) {
		CString err = L"Read file error";
		err += release_file;
		AfxMessageBox(err);
		return false;
	}
	// step 3 the pedal has been pressed
	for (int f = 0; f < m_file.NumSampleFrames(); f++) {
		short frame[2];
		m_file.ReadFrame(frame);
		m_pedalPress.push_back(frame[0]*58);
	}
	// step 4 stop
	m_file.Close();
	return true;

}

void CPianoInstrumentSynth::SetNote(CNote* note)
{
	// Get a list of all attribute nodes and the
	// length of that list
	CComPtr<IXMLDOMNamedNodeMap> attributes;
	note->Node()->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	// Loop over the list of attributes
	for (int i = 0; i < len; i++)
	{
		// Get attribute i
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		// Get the name of the attribute
		CComBSTR name;
		attrib->get_nodeName(&name);

		// Get the value of the attribute.  A CComVariant is a variable
		// that can have any type. It loads the attribute value as a
		// string (UNICODE), but we can then change it to an integer 
		// (VT_I4) or double (VT_R8) using the ChangeType function 
		// and then read its integer or double value from a member variable.
		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == "pedal")
		{
			CComBSTR val;
			val = value.bstrVal;
			if (val == "press")
			{
				m_sustain = true;
				m_instrument->GetPlayer()->Pedal(&m_pedalPress);
			}
			else if (val == "release")
			{
				m_sustain = false;
				m_instrument->GetPlayer()->Pedal(&m_pedalReleased);
			}

		}
		else if (name == "dynamic")
		{
			value.ChangeType(VT_R8);
			m_dynam = value.dblVal;
			if (m_dynam < 0) {
				m_dynam = 0;
			}
			else if (m_dynam > 1) {
				m_dynam = 1;
			}
			else {
				m_dynam = m_dynam;
			}
			Interpolate(m_dynam);
		}
	}

}