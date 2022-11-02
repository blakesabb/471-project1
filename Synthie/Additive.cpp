#include "stdafx.h"
#include "Additive.h"


CAdditive::CAdditive(void)
{
	
	amplitude = 0;
	amplitude2 = 0;
	frequnecy = 440.0;;
	FreqVib = 0.0;
	CycleVibrato = 0.0;


}


CAdditive::~CAdditive(void)
{
}

bool CAdditive::Generate()
{
	int cur_index = index = index + 1;
	
	m_frame[0] = audio[cur_index];
	m_frame[1] = audio[cur_index];

	return true;
}

void CAdditive::Start()
{
	audio = new short[int(m_duration * GetSampleRate())];


	index = 0;


	CAdditive::GenerateWaveTable();
}



void CAdditive::GenerateWaveTable()
{

	double vibangle = 0;

	
	double sinangle = 0; 


	double time = 0; 
	


	for (int i = 0; i < int(m_duration * GetSampleRate()); i++, time += 1. / GetSampleRate())
	{

		double sample = 0;


		for (float n = 1.0; n * frequnecy <= 22050 && n <= 8.0; n++)
		{
			sample += (amplitude * amplitude2 * (sin(n * sinangle)));

		}
		
		sinangle += (2 * PI * (frequnecy + FreqVib * sin(vibangle))) / GetSampleRate();

		vibangle += (2 * PI * CycleVibrato) / GetSampleRate();

	}
}