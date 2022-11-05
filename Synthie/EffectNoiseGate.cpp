#include "stdafx.h"
#include "EffectNoiseGate.h"

EffectNoiseGate::EffectNoiseGate() {
	// initialize
}

void EffectNoiseGate::Process(double* frameIn, double* frameOut)
{
	if (m_on == 0)
	{
		frameOut[0] = frameIn[0];
		frameOut[1] = frameIn[1];
		return;
	}
	// int delaySamples = 400*44.1f;
	// Loop over the channels
	for (int c = 0; c < 2; c++)
	{
		// Add output of the queue to the current input
		double dry = m_dry * frameIn[c];
		//double wet = m_wet * queue[m_rdloc + c];
		double wet = (m_wet * frameIn[c]); //+ m_queue[m_rdloc + c]; // + m_rdloc

		if (wet < m_gate * m_gateMultiplier)
		{
			wet = 0;
		}
		frameOut[c] = dry + wet;
	}

}

void EffectNoiseGate::SetNote(CNote* note) {
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
		else if (name == "limit")
		{
			//SetFreq(NoteToFrequency(value.bstrVal));
			value.ChangeType(VT_R8);
			m_gateMultiplier = value.dblVal;
		}
		else if (name == "on")
		{
			//SetFreq(NoteToFrequency(value.bstrVal));
			value.ChangeType(VT_I4);
			m_on = value.intVal;
		}
	}

}