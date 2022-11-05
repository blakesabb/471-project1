#include "stdafx.h"
#include "CWaveTable.h"
#include "Notes.h"


CWaveTable::CWaveTable()
{

}

CWaveTable::CWaveTable(float bpm)
{
    m_bpm = bpm;
}



CWaveTable::~CWaveTable()
{
}


void CWaveTable::Start()
{
    m_sinewave.SetSampleRate(GetSampleRate());
    m_sinewave.Start();
    m_time = 0;
}

bool CWaveTable::Generate()
{
    // Tell the component to generate an audio sample
    m_sinewave.Generate();

    // Read the component's sample and make it our resulting frame.




    // R : !!!! m_release is the time before ending !!!!!
    if (m_time > (m_duration - m_release))
    {
        m_frame[0] = m_amplitute * m_sinewave.Frame(0) * (m_attack) / (m_decay) * (m_duration - m_time) / (m_release);
        m_frame[1] = m_amplitute * m_sinewave.Frame(1) * (m_attack) / (m_decay) * (m_duration - m_time) / (m_release);
    }

    // A 
    else if (m_time < m_attack)
    {
        m_frame[0] = m_amplitute * m_sinewave.Frame(0) * m_time / m_attack;
        m_frame[1] = m_amplitute * m_sinewave.Frame(1) * m_time / m_attack;
    }
    // D
    else if (m_time < m_decay) {
        m_frame[0] = m_amplitute * m_sinewave.Frame(0) * (m_decay - m_time + m_attack) / (m_decay);
        m_frame[1] = m_amplitute * m_sinewave.Frame(1) * (m_decay - m_time + m_attack) / (m_decay);
    }
    // S
    else {
        m_frame[0] = m_amplitute *  m_sinewave.Frame(0) * (m_attack) / (m_decay);
        m_frame[1] = m_amplitute *  m_sinewave.Frame(1) * (m_attack) / (m_decay);
    }



    // Update time
    m_time += GetSamplePeriod();

    // We return true until the time reaches the duration.
    return m_time < m_duration;
}

void CWaveTable::SetNote(CNote* note)
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

        if (name == "duration")
        {
            value.ChangeType(VT_R8);
            SetDuration(value.dblVal);
        }
        else if (name == "note")
        {
            SetFreq(wtToFrequency(value.bstrVal));

            // Store the frequency for Pitch and Attack
            m_freq = m_sinewave.GetFreq();
        }

        // For Envolope: ADSR

        // Attack
        else if (name == "A")
        {
            // Set the Attack Value
            value.ChangeType(VT_R8);
            m_attack = value.dblVal;
        }
        //  Decay
        else if (name == "D")
        {
            // Set the Decay Value
            value.ChangeType(VT_R8);
            m_decay = value.dblVal;
        }
        // Sustain
        else if (name == "S")
        {
            // Set the SetSustain Value
            value.ChangeType(VT_R8);
            m_sustain = value.dblVal;
        }
        // Release
        else if (name == "R")
        {
            // Set the Release Value
            value.ChangeType(VT_R8);
            m_release = value.dblVal;
        }

        // Pitch
        else if (name == "Pitch")
        {
            // Set how many times to speed up
            value.ChangeType(VT_I4);
            int speed = value.intVal;

            SetFreq(m_freq * speed);
        }

        // Attack And Sustain Wave
        else if (name == "Attack")
        {
            value.ChangeType(VT_R8);
            m_attack = m_decay = value.dblVal;
            m_attack = m_decay = value.dblVal;
            m_release = m_duration - m_attack;

            // Making the beat more clearly 
            m_amplitute = 2;
            SetFreq(m_freq * 1.03);
        }
        else if (name == "Sustain")
        {
            value.ChangeType(VT_R8);
            m_release = value.dblVal;
        }

        // Table Across Fadding

        // For Attacking
        else if (name == "CrossFadeIn")
        {
            value.ChangeType(VT_R8);
            m_attack = value.dblVal;
            m_decay = (m_duration - m_attack) / 5 + m_attack;
        }

        // For Releasing
        else if (name == "CrossFadeOut")
        {
            value.ChangeType(VT_R8);
            m_release = value.dblVal;
        }

    }
}