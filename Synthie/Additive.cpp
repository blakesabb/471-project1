#include "stdafx.h"
#include "Additive.h"


short catchOverflow(double d)
{
	if (d < -32768)
		return -32768;
	else if (d > 32767)
		return 32767;

	return short(d);
}

CAdditive::CAdditive(void)
{
	m_vibratoFreq = 0.0;
	m_vibratoRate = 0.0;
	m_amplitude = 1000.0;
	m_amp[0] = 1.0;
	for (int i = 1; i <= 7; i++) m_amp[i] = 0.0;
	m_freq = 440.0;

}


CAdditive::~CAdditive(void)
{
	delete[] audio;
	audio = NULL;
}

void CAdditive::Start()
{
	audio = new short[int(m_duration * GetSampleRate())];
	index = 0;
	CAdditive::GenerateWaveTable();
}

bool CAdditive::Generate()
{
	m_frame[1] = m_frame[0] = audio[index++];

	return true;
}

void CAdditive::GenerateWaveTable()
{

	double radians1 = 0; // For the sine wave
	double radians2 = 0; // For the vibrato wave

	double time = 0; int i;
	for (i = 0; i < int(m_duration * GetSampleRate()); i++, time += 1. / GetSampleRate())
	{

		double sample = 0;
		for (float n = 1.0; n * m_freq <= 22050 && n <= 8.0; n++)
			sample += (m_amplitude * m_amp[int(n - 1)] * (sin(n * radians1)));

		radians2 += (2 * PI * m_vibratoRate) / GetSampleRate();
		radians1 += (2 * PI * (m_freq + m_vibratoFreq * sin(radians2))) / GetSampleRate();


		audio[i] = catchOverflow(sample);
	}
}