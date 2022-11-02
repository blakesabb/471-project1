#include "stdafx.h"
#include "SynthesizerAdd.h"
#include "Notes.h"
#include <string>
#include <iostream>
#include <sstream>

CSynthesizerAdd::CSynthesizerAdd(void)
{
	CrossIn = 0.0;
	CrossOut = 0.0;
	attack = 0;
	release = 0;
	decay = 0;
	sustain = 1.0;
}


CSynthesizerAdd::~CSynthesizerAdd(void)
{
}

void CSynthesizerAdd::Start()
{
	m_sinewave.SetSampleRate(GetSampleRate());
	m_sinewave.SetDuration(m_duration);
	m_sinewave.Start();
	m_time = 0;
}


bool CSynthesizerAdd::Generate()
{
	m_sinewave.Generate();


	m_frame[0] = m_sinewave.Frame(0);
	m_frame[1] = m_sinewave.Frame(1);
	double factor = 1.0;
	double sign = -1.0;
	if (sustain > 1.0) sign = 1.0;

	if (m_time < attack) {
		factor = m_time * 1. / attack;
	}
	else if (m_time < decay) {
		factor = sign * ((1.0 - sustain) / (decay - attack)) * (m_time)+(1.0 - sign * ((1.0 - sustain) / (decay - attack)) * (attack));
	}
	else if (m_time > (m_duration - release) && release != 0) {
		if (sustain <= 0) sustain = 1.0;
		factor = m_time * -sustain / release + (sustain / release) * (m_duration);
	}
	else {
		factor = sustain;
	}

	m_frame[0] = m_frame[1] *= factor;

	// Update time
	m_time += GetSamplePeriod();

	// We return true until the time reaches the duration.
	return m_time < m_duration;
}

void CSynthesizerAdd::SetNote(CNote* note)
{

	CComPtr<IXMLDOMNamedNodeMap> attributes;
	note->Node()->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	for (int i = 0; i < len; i++)
	{
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		CComBSTR name;
		attrib->get_nodeName(&name);

		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == "duration")
		{
			value.ChangeType(VT_R8);
			SetDuration(value.dblVal);
		}
		else if (name == "note")
		{
			SetFreq(NoteToFrequency(value.bstrVal));
		}
		else if (name == "amplitudes")
		{
			std::wstring wide(value.bstrVal);
			std::string str(wide.begin(), wide.end());

			std::stringstream ss(str);
			std::string item;

			double harmonics[8] = { 0 };

			int i = 0;
			while (std::getline(ss, item, char(32))) {
				harmonics[i++] = atof(item.c_str());
			}

			SetAmplitude(harmonics[0]);
		}
		else if (name == "crossFadeIn")
		{
			value.ChangeType(VT_R8);
			SetCrossFadeIn(value.dblVal * m_duration);
		}
		else if (name == "crossFadeOut")
		{
			value.ChangeType(VT_R8);
			SetCrossFadeOut(value.dblVal * m_duration);
		}
		else if (name == "ADSR") {
			std::wstring wide(value.bstrVal);
			std::string str(wide.begin(), wide.end());

			std::stringstream ss(str);
			std::string item;

			std::getline(ss, item, char(32));
			attack = atof(item.c_str()) * m_duration;

			std::getline(ss, item, char(32));
			decay = attack + atof(item.c_str()) * m_duration;

			std::getline(ss, item, char(32));
			sustain = atof(item.c_str());

			std::getline(ss, item, char(32));
			release = atof(item.c_str()) * m_duration;
		}
		else if (name == "vibrato") {
			std::wstring wide(value.bstrVal);
			std::string str(wide.begin(), wide.end());

			std::stringstream ss(str);
			std::string item;

			std::getline(ss, item, char(32));
			m_sinewave.VibratoCycle(atof(item.c_str()));

			std::getline(ss, item, char(32));
			m_sinewave.VibratoFrequency(atof(item.c_str()));
		}

	}

}