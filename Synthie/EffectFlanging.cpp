#include "stdafx.h"
#include "EffectFlanging.h"
#include <vector>


EffectFlanging::EffectFlanging() {
	// initialize
	m_queue.resize(m_sampleRate * 5);        // Queue size is in samples and is the max possible delay
}

void EffectFlanging::Process(double* frameIn, double* frameOut)
{
	int delayLength = 735;
	double decay = .9;
	// int delaySamples = 400*44.1f;
	// Loop over the channels
	for (int c = 0; c < 2; c++)
	{
		m_queue[m_wrloc + c] += frameIn[c] * decay;
		// Add output of the queue to the current input
		double dry = m_dry * frameIn[c];
		//double wet = m_wet * queue[m_rdloc + c];
		double wet = (m_wet * frameIn[c] + m_queue[m_wrloc + c] * m_amplitude); //+ m_queue[m_rdloc + c]; // + m_rdloc
		if (m_wrloc > delayLength)
			double wet = (m_wet * frameIn[c] + m_queue[m_wrloc - delayLength + c] * m_amplitude); //+ m_queue[m_rdloc + c]; // + m_rdloc
		frameOut[c] = dry + wet;

	}
	if (delayLength > 44100)
	{
		m_goUp = false;
		delayLength = 44100;
	}
	else if (delayLength <= 0)
		m_goUp = true;

	if (m_goUp == true)
	{
		delayLength += 735;
	}
	else
	{
		delayLength -= 735;
	}

	m_wrloc = (m_wrloc + 2) % m_queue.size();
	/*
	// Loop over the channels
	for (int c = 0; c < 2; c++)
	{
		double tremolo_amp = m_amplitude + 0.55 * sin(m_time * 2 * 3.14 * 6);

		// Add output of the queue to the current input
		double dry = m_dry * frameIn[c];
		//double wet = m_wet * queue[m_rdloc + c];
		double wet = m_wet * frameIn[c] * tremolo_amp; // + m_rdloc
		frameOut[c] = dry + wet;
	}
	
	if (m_time > 360)
		m_goUp = false;
	else if (m_time <= 0)
		m_goUp = true;

	if (m_goUp == true)
		m_time++;
	else
		m_time--;
	*/
}

void EffectFlanging::SetNote(CNote* note) {
	//m_note = note;

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

		if (name == "wet")
		{
			value.ChangeType(VT_R8);
			// SetDuration(value.dblVal); // play the note for the duration in terms of seconds
			//m_ar.SetDuration(value.dblVal * (NUM_SECS_IN_MINUTE / m_bpm));
			m_wet = value.dblVal;

		}
		else if (name == "dry")
		{
			//SetFreq(NoteToFrequency(value.bstrVal));
			value.ChangeType(VT_R8);
			m_dry = value.dblVal;
		}
		else if (name == "amplitude")
		{
			//SetFreq(NoteToFrequency(value.bstrVal));
			value.ChangeType(VT_R8);
			m_amplitude = value.dblVal;
		}
	}

}