#include "stdafx.h"
#include "PianoSampWave.h"
#include "Note.h"
#include <cmath>

CPianoSampWave::CPianoSampWave(void)
{
	m_sample = 0;
	m_pedalCount = -1;

}


CPianoSampWave::~CPianoSampWave(void)
{
	

}

void CPianoSampWave::Start()
{
	m_sample = 1820; //london bridge
	tmp = 0;
	m_rate = m_freq / 261.626;

}


bool CPianoSampWave::Generate()
{

	double internote = fmod(m_sample, 1);
	double lininterpolation_samp = ((1 - internote) * m_samplewave->at(int(m_sample))) + (internote * m_samplewave->at(int(m_sample) + 1));

	m_frame[0] = lininterpolation_samp / 32768;

	if (m_pedalCount > -1)
	{
		m_frame[0] = m_frame[0] + (double(m_pedalwave->at(m_pedalCount)) / 32768.0);
		m_pedalCount++;
		if (m_pedalCount == m_pedalwave->size())
			m_pedalCount = -1;
	}
	m_frame[1] = m_frame[0];
	m_sample += m_rate;
	if (m_sample > 54002)
		m_sample = 53834 + (m_sample - 54002); //This makes sure the different notes loop over the sample
	return true;
}

void CPianoSampWave::Pedal(std::vector<short>* pWave)
{
	m_pedalwave = pWave;
	m_pedalCount = 0;
}