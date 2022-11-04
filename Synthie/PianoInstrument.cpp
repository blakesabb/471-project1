#include "stdafx.h"
#include "PianoInstrument.h"



CPianoInstrument::CPianoInstrument(void)
{
	m_duration = 0.1;
}


CPianoInstrument::~CPianoInstrument(void)
{
	
}

void CPianoInstrument::Start()
{
    m_pianonote.SetSampleRate(GetSampleRate());
    m_pianonote.Start();
    m_time = 0;

    m_envelope.SetSource(&m_pianonote);
    m_envelope.SetSampleRate(GetSampleRate());
    m_envelope.Start();

}

bool CPianoInstrument::Generate()
{
    m_pianonote.Generate();
    bool valid = m_envelope.Generate();

    
    m_frame[0] = m_envelope.Frame(0);
    m_frame[1] = m_envelope.Frame(1);

    m_time += GetSamplePeriod();
    return valid;
}

void CPianoInstrument::SetNote(CNote* note)
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
		// change here to implement attack and release
		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == "duration")
		{
			value.ChangeType(VT_R8);
			// SetDuration(value.dblVal); // play the note for the duration in terms of seconds
			SetDuration(value.dblVal );

		}
		else if (name == "note")
		{
			SetFreq(NoteToFrequency(value.bstrVal));
		}
		else if (name == "attack")
		{
			value.ChangeType(VT_R8);
			m_envelope.SetAttack(value.dblVal);
		}
		else if (name == "release")
		{
			value.ChangeType(VT_R8);
			m_envelope.SetRelease(value.dblVal);
		}
	}
}
