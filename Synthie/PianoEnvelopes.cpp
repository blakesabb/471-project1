#include "stdafx.h"
#include "PianoEnvelopes.h"


CPianoEnvelopes::CPianoEnvelopes(void)
{
	
	m_attack = 0.01;
	m_release = 0.001;
	m_duration = 0.1;
	m_source = NULL;
	m_sustain = false;
	m_pedalPressed = false;
	g_bin = 0;
}

CPianoEnvelopes::~CPianoEnvelopes(void)
{
}


void CPianoEnvelopes::Start()
{
	m_time = 0;
}


bool CPianoEnvelopes::Generate()
{
	double decay = GetSamplePeriod() / 4;
	if (m_sustain == true)
	{
		m_duration = 30;
		decay = GetSamplePeriod() / 50;
		m_pedalPressed = true;


	}
	else if (m_pedalPressed == true && !m_sustain)
	{
		m_time = m_duration - 0.3;
		m_pedalPressed = false;
	}


	// further developed attack and release 
	double gain = 1;
	if (m_duration > 0)
	{
		if (m_time < m_attack)
		{
			gain = m_time / m_attack;
		}
		else if (m_time > (m_duration - m_release) && m_time < m_duration)
		{
			gain = 1 - (m_time - (m_duration - m_release)) / (m_duration - (m_duration - m_release));
		}

		else if (m_time > 1 && m_time < m_duration)
		{
			if (g_bin >= 0)
				gain = g_bin - decay;
			else
				gain = g_bin + decay;
		}

	}

	// phase 2 silence
	if (m_time >= m_duration)
	{
		
		m_frame[0] = m_frame[1] = 0;
		return false;
	}
	g_bin = gain;
	
	// phase 3 play instrument
	if (m_source != NULL)
	{
		m_frame[0] = m_source->Frame(0) * gain;
		m_frame[1] = m_source->Frame(1) * gain;
	}

	
	//phase 4 update the time

	m_time += GetSamplePeriod();
	return true;

}